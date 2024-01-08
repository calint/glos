#pragma once

typedef struct token {
  const char *begin;
  const char *content;
  const char *content_end;
  const char *end;
} token;

#define token_def                                                              \
  (token) { NULL, NULL, NULL, NULL }

static inline auto token_size(token const *t) -> unsigned {
  return (unsigned)(t->content_end - t->content);
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

static inline auto token_get_int(token const *t) -> int {
  return atoi(t->content); //?  assuming file ends with whitespace
}

static inline auto token_get_uint(token const *t) -> unsigned {
  int const i = atoi(t->content);
  //?  assuming file ends with whitespace, error?
  if (i < 0) {
    fprintf(stderr, "\n%s:%d: unexpected int\n", __FILE__, __LINE__);
    fflush(stderr);
    std::abort();
  }
  return (unsigned)i;
}

static inline auto token_next(char const **s) -> token {
  const char *p = *s;
  token t;
  t.begin = p;
  while (true) {
    if (not *p or not isspace(*p)) {
      break;
    }
    ++p;
  }
  t.content = p;
  while (true) {
    if (not *p or isspace(*p) or *p == ':' or *p == '{' or *p == '}' or
        *p == ',' or *p == '=' or *p == ';' or *p == '(' or *p == ')' or
        *p == '!' or *p == '>' or *p == '<') {
      break;
    }
    ++p;
  }
  t.content_end = p;
  while (true) {
    if (not *p or not isspace(*p)) {
      break;
    }
    ++p;
  }
  *s = t.end = p;
  return t;
}

static inline auto token_from_string_additional_delim(char const *s, char delim)
    -> token {
  const char *p = s;
  token t;
  t.begin = s;
  while (true) {
    if (not *p or not isspace(*p)) {
      break;
    }
    ++p;
  }
  t.content = p;
  while (true) {
    if (not *p or isspace(*p)) {
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
    if (not *p or not isspace(*p)) {
      break;
    }
    ++p;
  }
  t.end = p;
  return t;
}

static inline auto scan_to_including_newline(char const *p) -> char const * {
  while (true) {
    if (not *p) {
      return p;
    }
    if (*p == '\n') {
      ++p;
      return p;
    }
    ++p;
  }
}
