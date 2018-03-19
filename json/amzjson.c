#include "amzjson.h"
#include <assert.h> /* assert() */
#include <errno.h>  /* errno, ERANGE */
#include <math.h>   /* HUGE_VAL */
#include <stdlib.h> /* NULL, malloc(), realloc(), free(), strtod() */
#include <string.h> /* memcpy() */

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++; } while(0)
#define PUTC(c, ch) do { *(char*)amz_context_push(c, sizeof(char)) = (ch); } while(0)

#define ISDIGIT(ch)      ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)  ((ch) >= '1' && (ch) <= '9')

#ifndef AMZ_PARSE_STACK_INIT_SIZE
#define AMZ_PARSE_STACK_INIT_SIZE 256
#endif

typedef struct {
  const char* json;
  char* stack;
  size_t size, top;
} amz_context;

static void* amz_context_push(amz_context* c, size_t size) {
  void* ret;
  assert(size > 0);
  if (c->top + size >= c->size) {
    if (c->size == 0)
      c->size = AMZ_PARSE_STACK_INIT_SIZE;
    while (c->top + size >= c->size)
    c->size += c->size >> 1;
    c->stack = (char*)realloc(c->stack, c->size);
  }
  ret = c->stack + c->top;
  c->top += size;
  return ret;
}

static void* amz_context_pop(amz_context* c, size_t size) {
  assert(c->top >= size);
  return c->stack + (c->top -= size);
}

/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void amz_parse_whitespace(amz_context* c) {
  const char* p = c->json;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    p++;
  c->json = p;
}

/* 字面量（ture false null）*/
static int amz_parse_literal(amz_context* c, amz_value* v, const char* literal, amz_type type) {
  size_t i;
  EXPECT(c, literal[0]);
  for (i = 0; literal[i + 1]; i++)
  if (c->json[i] != literal[i + 1])
    return AMZ_PARSE_INVALID_VALUE;
  c->json += i;
  v->type = type;
  return AMZ_PARSE_OK;
}

/* number = [ "-" ] int [ frac ] [ exp ] */
/* 自行判断+strtod() */
static int amz_parse_number(amz_context* c, amz_value* v) {
  const char* p = c->json;
  /* 负号 */
  if (*p == '-') p++;
  /* 整数 */
  if (*p == '0') p++;
  else {
    if (!ISDIGIT1TO9(*p))
      return AMZ_PARSE_INVALID_VALUE;
    for (p++; ISDIGIT(*p); p++);
  }
  /* 小数 */
  if (*p == '.') {
    p++;
    if (!ISDIGIT(*p))
      return AMZ_PARSE_INVALID_VALUE;
    for (p++; ISDIGIT(*p); p++);
  }
  /* 指数 */
  if (*p == 'e' || *p == 'E') {
    p++;
    if (*p == '+' || *p == '-') p++;
    if (!ISDIGIT(*p))
      return AMZ_PARSE_INVALID_VALUE;
    for (p++; ISDIGIT(*p); p++);
  }
  errno = 0;
  v->u.n = strtod(c->json, NULL);
  if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
    return AMZ_PARSE_NUMBER_TOO_BIG;
  v->u.n = strtod(c->json, NULL);
  v->type = AMZ_NUMBER;
  c->json = p;
  return AMZ_PARSE_OK;
}

static int amz_parse_string(amz_context* c, amz_value* v) {
  size_t head = c->top, len;
  const char* p;
  EXPECT(c, '\"');
  p = c->json;
  for(;;) {
    char ch = *p++;
    switch (ch) {
      case '\"':
        len = c->top - head;
        amz_set_string(v, (const char*)amz_context_pop(c, len), len);
        c->json = p;
        return AMZ_PARSE_OK;
      case '\0':
        c->top = head;
        return AMZ_PARSE_MISS_QUOTATION_MARK;
      case '\\':
        switch (*p++) {
          case '\"': PUTC(c, '\"'); break;
          case '\\': PUTC(c, '\\'); break;
          case '/':  PUTC(c, '/');  break;
          case 'b':  PUTC(c, '\b'); break;
          case 'f':  PUTC(c, '\f'); break;
          case 'n':  PUTC(c, '\n'); break;
          case 'r':  PUTC(c, '\r'); break;
          case 't':  PUTC(c, '\t'); break;
          default:
          c->top = head;
          return AMZ_PARSE_INVALID_STRING_ESCAPE;
        }
      break;
      default:
        if ((unsigned char)ch < 0x20) {
          c->top = head;
          return AMZ_PARSE_INVALID_STRING_CHAR;
        }
      PUTC(c, ch);
    }
  }
}

/* value = null / false / true / number*/
static int amz_parse_value(amz_context* c, amz_value* v) {
  switch (*c->json) {
    case 't':  return amz_parse_literal(c, v, "true", AMZ_TRUE);
    case 'f':  return amz_parse_literal(c, v, "false", AMZ_FALSE);
    case 'n':  return amz_parse_literal(c, v, "null", AMZ_NULL);
    default:   return amz_parse_number(c, v);
    case '"':  return amz_parse_string(c, v);
    case '\0': return AMZ_PARSE_EXPECT_VALUE;
  }
}

/* JSON-text = ws value ws */
int amz_parse(amz_value* v, const char* json) {
  amz_context c;
  int ret;
  assert(v != NULL);
  c.json = json;
  c.stack = NULL;
  c.size = c.top = 0;
  amz_init(v);
  amz_parse_whitespace(&c);
  if ((ret = amz_parse_value(&c, v)) == AMZ_PARSE_OK) {
    amz_parse_whitespace(&c);
    if (*c.json != '\0') {
      v->type = AMZ_NULL;
      ret = AMZ_PARSE_ROOT_NOT_SINGULAR;
    }
  }
  assert(c.top == 0);
  free(c.stack);
  return ret;
}

amz_type amz_get_type(const amz_value* v) {
  assert(v != NULL);
  return v->type;
}

int amz_get_boolean(const amz_value* v) {
  assert(v != NULL && (v->type == AMZ_TRUE || v->type == AMZ_FALSE));
  return v->type == AMZ_TRUE;
}

void amz_set_boolean(amz_value* v, int b) {
  amz_free(v);
  v->type = b ? AMZ_TRUE : AMZ_FALSE;
}

double amz_get_number(const amz_value* v) {
  assert(v != NULL && v->type == AMZ_NUMBER);
  return v->u.n;
}

void amz_set_number(amz_value* v, double n) {
  amz_free(v);
  v->u.n = n;
  v->type = AMZ_NUMBER;
}

const char* amz_get_string(const amz_value* v) {
  assert(v != NULL && v->type == AMZ_STRING);
  return v->u.s.s;
}

size_t amz_get_string_length(const amz_value* v) {
  assert(v != NULL && v->type == AMZ_STRING);
  return v->u.s.len;
}

void amz_set_string(amz_value* v, const char* s, size_t len) {
  assert(v != NULL && (s != NULL || len == 0));
  amz_free(v);
  v->u.s.s = (char*)malloc(len + 1);
  memcpy(v->u.s.s, s, len);
  v->u.s.s[len] = '\0';
  v->u.s.len = len;
  v->type = AMZ_STRING;
}

void amz_free(amz_value* v) {
  assert(v != NULL);
  if (v->type == AMZ_STRING)
    free(v->u.s.s);
  v->type = AMZ_NULL;
}
