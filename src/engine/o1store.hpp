#pragma once
//
// implements a O(1) store of objects
//
// * type: object type. 'type' must contain public field 'type **alloc_ptr'
// * instance_count: number of preallocated objects
// * store_id: id used when printing to identify the o1store
// * return_nullptr_when_no_free_instance_available: if false aborts with
//   printout when no free slot available
// * thread_safe: true to synchronize 'allocate_instance' and 'free_instance'
// * instance_size_B: custom size of object instance used to fit largest object
//   in an object hierarchy
//

template <typename type, size_t const instance_count,
          unsigned const store_id = 0,
          bool const return_nullptr_when_no_free_instance_available = false,
          bool const thread_safe = false, size_t const instance_size_B = 0>
class o1store final {
  type *all_ = nullptr;
  type **free_bgn_ = nullptr;
  type **free_ptr_ = nullptr;
  type **free_end_ = nullptr;
  type **alloc_bgn_ = nullptr;
  type **alloc_ptr_ = nullptr;
  type **del_bgn_ = nullptr;
  type **del_ptr_ = nullptr;
  type **del_end_ = nullptr;
  std::atomic_flag lock = ATOMIC_FLAG_INIT;

public:
  o1store() {
    if (instance_size_B) {
      all_ = static_cast<type *>(calloc(instance_count, instance_size_B));
    } else {
      all_ = static_cast<type *>(calloc(instance_count, sizeof(type)));
    }
    free_ptr_ = free_bgn_ =
        static_cast<type **>(calloc(instance_count, sizeof(type *)));
    alloc_ptr_ = alloc_bgn_ =
        static_cast<type **>(calloc(instance_count, sizeof(type *)));
    del_ptr_ = del_bgn_ =
        static_cast<type **>(calloc(instance_count, sizeof(type *)));

    if (!all_ or !free_bgn_ or !alloc_bgn_ or !del_bgn_) {
      throw glos_exception{
          std::format("store {}: cannot allocate arrays", store_id)};
    }

    free_end_ = free_bgn_ + instance_count;
    del_end_ = del_bgn_ + instance_count;

    // write pointers to instances in the 'free' list
    type *all_it = all_;
    for (type **free_it = free_bgn_; free_it < free_end_; ++free_it) {
      *free_it = all_it;
      if (instance_size_B) {
        all_it = reinterpret_cast<type *>(reinterpret_cast<char *>(all_it) +
                                          instance_size_B);
      } else {
        ++all_it;
      }
    }
  }

  ~o1store() {
    free(all_);
    free(alloc_bgn_);
    free(free_bgn_);
    free(del_bgn_);
  }

  // allocates an instance
  // returns nullptr if instance could not be allocated
  auto allocate_instance() -> type * {
    if (thread_safe) {
      acquire_lock();
    }
    if (free_ptr_ >= free_end_) {
      if (thread_safe) {
        release_lock();
      }
      if (return_nullptr_when_no_free_instance_available) {
        return nullptr;
      } else {
        throw glos_exception{
            std::format("store {}: out of free instances. consider increasing "
                        "the size of the store.",
                        store_id)};
      }
    }
    type *inst = *free_ptr_;
    ++free_ptr_;
    *alloc_ptr_ = inst;
    inst->alloc_ptr = alloc_ptr_;
    ++alloc_ptr_;
    if (thread_safe) {
      release_lock();
    }
    return inst;
  }

  // adds instance to list of instances to be freed with 'apply_free()'
  void free_instance(type *inst) {
    if (thread_safe) {
      acquire_lock();
    }

    if (o1store_check_free_limits) {
      if (del_ptr_ >= del_end_) {
        throw glos_exception{std::format("store {}: free overrun", store_id)};
      }
    }

    if (o1store_check_double_free) {
      for (type **it = del_bgn_; it < del_ptr_; ++it) {
        if (*it == inst) {
          throw glos_exception{std::format("store {}: double free", store_id)};
        }
      }
    }

    *del_ptr_ = inst;
    ++del_ptr_;

    if (thread_safe) {
      release_lock();
    }
  }

  // deallocates the instances that have been freed
  void apply_free() {
    for (type **it = del_bgn_; it < del_ptr_; ++it) {
      type *inst_deleted = *it;
      alloc_ptr_--;
      type *inst_to_move = *alloc_ptr_;
      inst_to_move->alloc_ptr = inst_deleted->alloc_ptr;
      *(inst_deleted->alloc_ptr) = inst_to_move;
      free_ptr_--;
      *free_ptr_ = inst_deleted;
      inst_deleted->~object();
    }
    del_ptr_ = del_bgn_;
  }

  // returns list of allocated instances
  inline auto allocated_list() const -> type ** { return alloc_bgn_; }

  // returns length of list of allocated instances
  inline auto allocated_list_len() const -> size_t {
    return size_t(alloc_ptr_ - alloc_bgn_);
  }

  // returns one past the end of allocated instances list
  inline auto allocated_list_end() const -> type ** { return alloc_ptr_; }

  // returns the list with all preallocated instances
  inline auto all_list() const -> type * { return all_; }

  // returns the length of 'all' list
  constexpr auto all_list_len() const -> size_t { return instance_count; }

  // returns instance at index 'ix' from 'all' list
  inline auto instance(size_t ix) const -> type * {
    if (!instance_size_B) {
      return &all_[ix];
    }
    // note: if instance size is specified do pointer shenanigans
    return reinterpret_cast<type *>(reinterpret_cast<char *>(all_) +
                                    instance_size_B * ix);
  }

  // returns the size of allocated heap memory in bytes
  constexpr auto allocated_data_size_B() const -> size_t {
    return instance_size_B ? (instance_count * instance_size_B +
                              3 * instance_count * sizeof(type *))
                           : (instance_count * sizeof(type) +
                              3 * instance_count * sizeof(type *));
  }

  inline void acquire_lock() {
    while (lock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { lock.clear(std::memory_order_release); }
};
