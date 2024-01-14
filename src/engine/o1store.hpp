#pragma once
//
// implements a O(1) store of objects
//
// * Type is object type. 'Type' must contain public field 'Type **alloc_ptr'
// * Size is number of preallocated objects
// * StoreId is for debugging
// * InstanceSizeInBytes is custom size of instance
//   used to fit largest object in an object hierarchy
//
// note. no destructor since life-time is program life-time
//

template <typename Type, size_t const InstancesCount,
          unsigned const StoreId = 0,
          bool const return_nullptr_when_no_free_instance_available = false,
          size_t const InstanceSizeInBytes = 0>
class o1store final {
  Type *all_ = nullptr;
  Type **free_bgn_ = nullptr;
  Type **free_ptr_ = nullptr;
  Type **free_end_ = nullptr;
  Type **alloc_bgn_ = nullptr;
  Type **alloc_ptr_ = nullptr;
  Type **del_bgn_ = nullptr;
  Type **del_ptr_ = nullptr;
  Type **del_end_ = nullptr;
  std::atomic_flag lock = ATOMIC_FLAG_INIT;

public:
  o1store() {
    if (InstanceSizeInBytes) {
      all_ = static_cast<Type *>(calloc(InstancesCount, InstanceSizeInBytes));
    } else {
      all_ = static_cast<Type *>(calloc(InstancesCount, sizeof(Type)));
    }
    free_ptr_ = free_bgn_ =
        static_cast<Type **>(calloc(InstancesCount, sizeof(Type *)));
    alloc_ptr_ = alloc_bgn_ =
        static_cast<Type **>(calloc(InstancesCount, sizeof(Type *)));
    del_ptr_ = del_bgn_ =
        static_cast<Type **>(calloc(InstancesCount, sizeof(Type *)));

    if (!all_ or !free_bgn_ or !alloc_bgn_ or !del_bgn_) {
      fprintf(stderr, "\n%s:%d: store %u: cannot allocate arrays\n", __FILE__,
              __LINE__, StoreId);
      fflush(stderr);
      std::abort();
    }

    free_end_ = free_bgn_ + InstancesCount;
    del_end_ = del_bgn_ + InstancesCount;

    // write pointers to instances in the 'free' list
    Type *all_it = all_;
    for (Type **free_it = free_bgn_; free_it < free_end_; ++free_it) {
      *free_it = all_it;
      if (InstanceSizeInBytes) {
        all_it = reinterpret_cast<Type *>(reinterpret_cast<char *>(all_it) +
                                          InstanceSizeInBytes);
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
  auto allocate_instance() -> Type * {
    if (threaded_o1store) {
      acquire_lock();
    }
    if (free_ptr_ >= free_end_) {
      if (threaded_o1store) {
        release_lock();
      }
      if (return_nullptr_when_no_free_instance_available) {
        return nullptr;
      } else {
        fprintf(stderr,
                "\n%s:%d: store %u: out of free instances. consider increasing "
                "the size of the store.\n",
                __FILE__, __LINE__, StoreId);
        fflush(stderr);
        std::abort();
      }
    }
    Type *inst = *free_ptr_;
    ++free_ptr_;
    *alloc_ptr_ = inst;
    inst->alloc_ptr = alloc_ptr_;
    ++alloc_ptr_;
    if (threaded_o1store) {
      release_lock();
    }
    return inst;
  }

  // adds instance to list of instances to be freed with 'apply_free()'
  void free_instance(Type *inst) {
    if (threaded_o1store) {
      acquire_lock();
    }

    if (o1store_check_free_limits) {
      if (del_ptr_ >= del_end_) {
        fprintf(stderr, "\n%s:%d: store %u: free overrun\n", __FILE__, __LINE__,
                StoreId);
        fflush(stderr);
        std::abort();
      }
    }

    if (o1store_check_double_free) {
      for (Type **it = del_bgn_; it < del_ptr_; ++it) {
        if (*it == inst) {
          fprintf(stderr, "\n%s:%d: store %u: double free\n", __FILE__,
                  __LINE__, StoreId);
          fflush(stderr);
          std::abort();
        }
      }
    }

    *del_ptr_ = inst;
    ++del_ptr_;

    if (threaded_o1store) {
      release_lock();
    }
  }

  // deallocates the instances that have been freed
  void apply_free() {
    for (Type **it = del_bgn_; it < del_ptr_; ++it) {
      Type *inst_deleted = *it;
      alloc_ptr_--;
      Type *inst_to_move = *alloc_ptr_;
      inst_to_move->alloc_ptr = inst_deleted->alloc_ptr;
      *(inst_deleted->alloc_ptr) = inst_to_move;
      free_ptr_--;
      *free_ptr_ = inst_deleted;
      inst_deleted->~object();
    }
    del_ptr_ = del_bgn_;
  }

  // returns list of allocated instances
  inline auto allocated_list() const -> Type ** { return alloc_bgn_; }

  // returns length of list of allocated instances
  inline auto allocated_list_len() const -> size_t {
    return size_t(alloc_ptr_ - alloc_bgn_);
  }

  // returns one past the end of allocated instances list
  inline auto allocated_list_end() const -> Type ** { return alloc_ptr_; }

  // returns the list with all preallocated instances
  inline auto all_list() const -> Type * { return all_; }

  // returns the length of 'all' list
  constexpr auto all_list_len() const -> size_t { return InstancesCount; }

  // returns instance at index 'ix' from 'all' list
  inline auto instance(size_t ix) const -> Type * {
    if (!InstanceSizeInBytes) {
      return &all_[ix];
    }
    // note. if instance size is specified do pointer shenanigans
    return reinterpret_cast<Type *>(reinterpret_cast<char *>(all_) +
                                    InstanceSizeInBytes * ix);
  }

  // returns the size of allocated heap memory in bytes
  constexpr auto allocated_data_size_B() const -> size_t {
    return InstanceSizeInBytes ? (InstancesCount * InstanceSizeInBytes +
                                  3 * InstancesCount * sizeof(Type *))
                               : (InstancesCount * sizeof(Type) +
                                  3 * InstancesCount * sizeof(Type *));
  }

  inline void acquire_lock() {
    while (lock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { lock.clear(std::memory_order_release); }
};
