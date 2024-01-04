#pragma once
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct token {
  const char *begin;
  const char *content;
  const char *content_end;
  const char *end;
} token;

#define token_def                                                              \
  (token) { NULL, NULL, NULL, NULL }

inline static unsigned token_size(token const *t) {
  return (unsigned)(t->content_end - t->content);
}

inline static int token_starts_with(token const *t, char const *str) {
  return strncmp(str, t->content, strlen(str)) == 0;
}

inline static int token_equals(token const *t, char const *str) {
  const char *p = t->content; //? stdlib
  while (1) {
    if (p == t->content_end) {
      if (!*str)
        return 1;
      return 0;
    }
    if (*p != *str)
      return 0;
    p++;
    str++;
  }
}

inline static float token_get_float(token const *t) {
  float f = (float)atof(t->content); //? assuming file ends with whitespace
  return f;
}

inline static int token_get_int(token const *t) {
  int i = atoi(t->content); //?  assuming file ends with whitespace
  return i;
}

inline static unsigned token_get_uint(token const *t) {
  int i = atoi(t->content); //?  assuming file ends with whitespace, error?
  if (i < 0) {
    printf("%s:%d: unexpected int\n", __FILE__, __LINE__);
    std::abort();
  }
  return (unsigned)i;
}

inline static token token_next(char const **s) {
  const char *p = *s;
  token t;
  t.begin = p;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    p++;
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
    p++;
  }
  t.content_end = p;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    p++;
  }
  *s = t.end = p;
  return t;
}

inline static token token_from_string_additional_delim(char const *s,
                                                       char delim) {
  const char *p = s;
  token t;
  t.begin = s;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    p++;
  }
  t.content = p;
  while (1) {
    if (!*p)
      break;
    if (isspace(*p))
      break;
    if (*p == delim) {
      p++;
      t.end = t.content_end = p;
      return t;
    }
    p++;
  }
  t.content_end = p;
  while (1) {
    if (!*p)
      break;
    if (!isspace(*p))
      break;
    p++;
  }
  t.end = p;
  return t;
}

inline static char const *scan_to_including_newline(char const *p) {
  while (1) {
    if (!*p)
      return p;
    if (*p == '\n') {
      p++;
      return p;
    }
    p++;
  }
}

