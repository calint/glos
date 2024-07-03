#pragma once

struct token {
  char const *begin = nullptr;
  char const *content = nullptr;
  char const *content_end = nullptr;
  char const *end = nullptr;
};

static inline auto token_size(token const *t) -> uint32_t {
  return uint32_t(t->content_end - t->content);
}

static inline auto token_starts_with(token const *t, char const *str) -> bool {
  return strncmp(str, t->content, strlen(str)) == 0;
}

static inline auto token_equals(token const *t, char const *str) -> bool {
  char const *p = t->content;
  while (true) {
    if (p == t->content_end) {
      return not *str;
    }
    if (*p != *str) {
      return false;
    }
    ++p;
    ++str;
  }
}

static inline auto token_get_float(token const *t) -> float {
  return float(atof(t->content)); //? assuming file ends with whitespace
}

static inline auto token_get_uint(token const *t) -> uint32_t {
  int const i = atoi(t->content);
  //?  assuming file ends with whitespace, error?
  if (i < 0) {
    throw glos_exception{std::format("unexpected signed int: {}", i)};
  }
  return uint32_t(i);
}

static inline auto token_next(char const **s) -> token {
  char const *p = *s;
  token t{};
  t.begin = p;
  // read empty space
  while (true) {
    if (*p == '\0' or not isspace(*p)) {
      break;
    }
    ++p;
  }
  // read token
  t.content = p;
  while (true) {
    if (*p == '\0' or isspace(*p)) {
      break;
    }
    ++p;
  }
  t.content_end = p;
  // read trailing empty space
  while (true) {
    if (*p == '\0' or not isspace(*p)) {
      break;
    }
    ++p;
  }
  *s = t.end = p;
  return t;
}

static inline auto token_from_string_additional_delim(char const *s,
                                                      char delim) -> token {
  const char *p = s;
  token t;
  t.begin = s;
  while (true) {
    if (*p == '\0' or not isspace(*p)) {
      break;
    }
    ++p;
  }
  t.content = p;
  while (true) {
    if (*p == '\0' or isspace(*p)) {
      break;
    }
    if (*p == delim) {
      ++p;
      t.end = t.content_end = p;
      return t;
    }
    ++p;
  }
  t.content_end = p;
  while (true) {
    if (*p == '\0' or not isspace(*p)) {
      break;
    }
    ++p;
  }
  t.end = p;
  return t;
}

static inline auto token_to_including_newline(char const *p) -> char const * {
  while (true) {
    if (*p == '\0') {
      return p;
    }
    if (*p == '\n') {
      ++p;
      return p;
    }
    ++p;
  }
}
