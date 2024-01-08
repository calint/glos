#pragma once

typedef struct token {
  const char *begin;
  const char *content;
  const char *content_end;
  const char *end;
} token;

#define token_def                                                              \
  (token) { NULL, NULL, NULL, NULL }

static inline unsigned token_size(token const *t) {
  return (unsigned)(t->content_end - t->content);
}

static inline bool token_starts_with(token const *t, char const *str) {
  return strncmp(str, t->content, strlen(str)) == 0;
}

static inline bool token_equals(token const *t, char const *str) {
  char const *p = t->content;
  while (true) {
    if (p == t->content_end) {
      if (!*str) {
        return true;
      }
      return false;
    }
    if (*p != *str) {
      return false;
    }
    ++p;
    ++str;
  }
}

static inline float token_get_float(token const *t) {
  return (float)atof(t->content); //? assuming file ends with whitespace
}

static inline int token_get_int(token const *t) {
  return atoi(t->content); //?  assuming file ends with whitespace
}

static inline unsigned token_get_uint(token const *t) {
  int i = atoi(t->content); //?  assuming file ends with whitespace, error?
  if (i < 0) {
    fprintf(stderr, "\n%s:%d: unexpected int\n", __FILE__, __LINE__);
    fflush(stderr);
    std::abort();
  }
  return (unsigned)i;
}

static inline token token_next(char const **s) {
  const char *p = *s;
  token t;
  t.begin = p;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    ++p;
  }
  t.content = p;
  while (1) {
    if (!*p)
      break;
    if (isspace(*p))
      break;
    if (*p == ':')
      break;
    if (*p == '{')
      break;
    if (*p == '}')
      break;
    if (*p == ',')
      break;
    if (*p == '=')
      break;
    if (*p == ';')
      break;
    if (*p == '(')
      break;
    if (*p == ')')
      break;
    if (*p == '!')
      break;
    if (*p == '>')
      break;
    if (*p == '<')
      break;
    ++p;
  }
  t.content_end = p;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    ++p;
  }
  *s = t.end = p;
  return t;
}

static inline token token_from_string_additional_delim(char const *s,
                                                       char delim) {
  const char *p = s;
  token t;
  t.begin = s;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    ++p;
  }
  t.content = p;
  while (1) {
    if (!*p)
      break;
    if (isspace(*p))
      break;
    if (*p == delim) {
      ++p;
      t.end = t.content_end = p;
      return t;
    }
    ++p;
  }
  t.content_end = p;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    ++p;
  }
  t.end = p;
  return t;
}

static inline char const *scan_to_including_newline(char const *p) {
  while (1) {
    if (!*p)
      return p;
    if (*p == '\n') {
      ++p;
      return p;
    }
    ++p;
  }
}
