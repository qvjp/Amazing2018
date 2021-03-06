#include "amzjson.h"
#include <assert.h> /* assert() */
#include <errno.h>  /* errno, ERANGE */
#include <math.h>   /* HUGE_VAL */
#include <stdlib.h> /* NULL, malloc(), realloc(), free(), strtod() */
#include <string.h> /* memcpy() */

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++; } while(0)
#define PUTC(c, ch) do { *(char*)amz_context_push(c, sizeof(char)) = (ch); } while(0)
#define STRING_ERROR(ret) do { c->top = head; return ret; } while (0)

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

static const char* amz_parse_hex4(const char* p, unsigned* u) {
  int i;
  *u = 0;
  for (i = 0; i < 4; i++) {
    char ch = *p++;
    *u <<= 4;
    if      (ch >= '0' && ch <= '9') *u |= ch - '0';
    else if (ch >= 'A' && ch <= 'F') *u |= ch - ('A' - 10);
    else if (ch >= 'a' && ch <= 'f') *u |= ch - ('a' - 10);
    else return NULL;
  }
  return p;
}

static void amz_encode_utf8(amz_context* c, unsigned u) {
  if (u <= 0x7F) 
    PUTC(c, u & 0xFF);
  else if (u <= 0x7FF) {
    PUTC(c, 0xC0 | ((u >> 6) & 0xFF));
    PUTC(c, 0x80 | ( u       & 0x3F));
  }
  else if (u <= 0xFFFF) {
    PUTC(c, 0xE0 | ((u >> 12) & 0xFF));
    PUTC(c, 0x80 | ((u >>  6) & 0x3F));
    PUTC(c, 0x80 | ( u        & 0x3F));
  }
  else {
    assert(u <= 0x10FFFF);
    PUTC(c, 0xF0 | ((u >> 18) & 0xFF));
    PUTC(c, 0x80 | ((u >> 12) & 0x3F));
    PUTC(c, 0x80 | ((u >>  6) & 0x3F));
    PUTC(c, 0x80 | ( u        & 0x3F));
   }
}

static int amz_parse_string_raw(amz_context* c, char** str, size_t* len) {
  unsigned u, u2;
  size_t head = c->top;
  const char* p;
  EXPECT(c, '\"');
  p = c->json;
  for(;;) {
    char ch = *p++;
    switch (ch) {
      case '\"':
        *len = c->top - head;
        *str = amz_context_pop(c, *len);
        c->json = p;
        return AMZ_PARSE_OK;
      case '\0':
        STRING_ERROR(AMZ_PARSE_MISS_QUOTATION_MARK);
      case '\\':
        switch (*p++) {
          case 'u':
            if (!(p = amz_parse_hex4(p, &u)))
              STRING_ERROR(AMZ_PARSE_INVALID_UNICODE_HEX);
            if (u >= 0xD800 && u <= 0xDBFF) { /* surrogate pair */
              if (*p++ != '\\')
                STRING_ERROR(AMZ_PARSE_INVALID_UNICODE_SURROGATE);
              if (*p++ != 'u')
                STRING_ERROR(AMZ_PARSE_INVALID_UNICODE_SURROGATE);
              if (!(p = amz_parse_hex4(p, &u2)))
                STRING_ERROR(AMZ_PARSE_INVALID_UNICODE_HEX);
              if (u2 < 0xDC00 || u2 > 0xDFFF)
                STRING_ERROR(AMZ_PARSE_INVALID_UNICODE_SURROGATE);
              u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
            }
            amz_encode_utf8(c, u);
            break;
          case '\"': PUTC(c, '\"'); break;
          case '\\': PUTC(c, '\\'); break;
          case '/':  PUTC(c, '/');  break;
          case 'b':  PUTC(c, '\b'); break;
          case 'f':  PUTC(c, '\f'); break;
          case 'n':  PUTC(c, '\n'); break;
          case 'r':  PUTC(c, '\r'); break;
          case 't':  PUTC(c, '\t'); break;
          default:
            STRING_ERROR(AMZ_PARSE_INVALID_STRING_ESCAPE);
        }
        break;
      default:
        if ((unsigned char)ch < 0x20) {
          STRING_ERROR(AMZ_PARSE_INVALID_STRING_CHAR);
        }
      PUTC(c, ch);
    }
  }
}

static int amz_parse_string(amz_context* c, amz_value* v) {
  int ret;
  char* s;
  size_t len;
  if ((ret = amz_parse_string_raw(c, &s, &len)) == AMZ_PARSE_OK)
    amz_set_string(v, s, len);
  return ret;
}

static int amz_parse_value(amz_context* c, amz_value* v); /* 向前声明 */

/* array = %x5B ws [ value *( ws %x2C ws value ) ] ws %x5D */
static int amz_parse_array(amz_context* c, amz_value* v) {
  size_t i, size = 0;
  int ret;
  EXPECT(c, '[');
  amz_parse_whitespace(c);
  if (*c->json == ']') {
    c->json++;
    v->type = AMZ_ARRAY;
    v->u.a.size = 0;
    v->u.a.e = NULL;
    return AMZ_PARSE_OK;
  }
  for (;;) {
    amz_value e;
    amz_init(&e);
    if ((ret = amz_parse_value(c, &e)) != AMZ_PARSE_OK)
      break;
    memcpy(amz_context_push(c, sizeof(amz_value)), &e, sizeof(amz_value));
    size++;
    amz_parse_whitespace(c);
    if (*c->json == ',')
    {
      c->json++;
      amz_parse_whitespace(c);
    }
    else if (*c->json == ']') {
      c->json++;
      v->type = AMZ_ARRAY;
      v->u.a.size = size;
      size *= sizeof(amz_value);
      memcpy(v->u.a.e = (amz_value*)malloc(size), amz_context_pop(c, size), size);
      return AMZ_PARSE_OK;
    }
    else
    {
      ret = AMZ_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
      break;
    }
  }
  for (i = 0; i < size; i++)
      amz_free((amz_value*)amz_context_pop(c, sizeof(amz_value)));
  return ret;
}

static int amz_parse_object(amz_context* c, amz_value* v) {
  size_t i, size;
  amz_member m;
  int ret;
  EXPECT(c, '{');
  amz_parse_whitespace(c);
  if (*c->json == '}') {
    c->json++;
    v->type = AMZ_OBJECT;
    v->u.o.m = 0;
    v->u.o.size = 0;
    return AMZ_PARSE_OK;
  }
  m.k = NULL;
  size = 0;
  for (;;) {
    char* str;
    amz_init(&m.v);
    /* 解析key */
    if (*c->json != '"') {
      ret = AMZ_PARSE_MISS_KEY;
      break;
    }
    if ((ret = amz_parse_string_raw(c, &str, &m.klen)) != AMZ_PARSE_OK)
        break;
    memcpy(m.k = (char*)malloc(m.klen + 1), str, m.klen);
    m.k[m.klen] = '\0';
    /* 解析ws colon ws */
    amz_parse_whitespace(c);
    if (*c->json != ':') {
      ret = AMZ_PARSE_MISS_COLON;
      break;
    }
    c->json++;
    amz_parse_whitespace(c);
    /* 解析value */
    if ((ret = amz_parse_value(c, &m.v)) != AMZ_PARSE_OK)
        break;
    memcpy(amz_context_push(c, sizeof(amz_member)), &m, sizeof(amz_member));
    size++;
    m.k = NULL;
    /* 解析ws [comma | right-curly-brace] ws */
    amz_parse_whitespace(c);
    if (*c->json == ',') {
      c->json++;
      amz_parse_whitespace(c);
    }
    else if (*c->json == '}') {
      size_t s = sizeof(amz_member) * size;
      c->json++;
      v->type = AMZ_OBJECT;
      v->u.o.size = size;
      memcpy(v->u.o.m = (amz_member*)malloc(s), amz_context_pop(c, s), s);
      return AMZ_PARSE_OK;
    }
    else {
      ret = AMZ_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
      break;
    }
  }
  /* Pop and free members on the stack */
  free(m.k);
  for (i = 0; i < size; i++) {
    amz_member* m = (amz_member*)amz_context_pop(c, sizeof(amz_member));
    free(m->k);
    amz_free(&m->v);
  }
  v->type = AMZ_NULL;
  return ret;
}

/* value = null / false / true / number*/
static int amz_parse_value(amz_context* c, amz_value* v) {
  switch (*c->json) {
    case 't':  return amz_parse_literal(c, v, "true", AMZ_TRUE);
    case 'f':  return amz_parse_literal(c, v, "false", AMZ_FALSE);
    case 'n':  return amz_parse_literal(c, v, "null", AMZ_NULL);
    default:   return amz_parse_number(c, v);
    case '"':  return amz_parse_string(c, v);
    case '[':  return amz_parse_array(c, v);
    case '{':  return amz_parse_object(c, v);
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

void amz_free(amz_value* v) {
  size_t i;
  assert(v != NULL);
  switch (v->type) {
    case AMZ_STRING:
      free(v->u.s.s);
      break;
    case AMZ_ARRAY:
      for (i = 0; i < v->u.a.size; i++)
        amz_free(&v->u.a.e[i]);
      free(v->u.a.e);
      break;
    case AMZ_OBJECT:
      for (i = 0; i < v->u.o.size; i++) {
        free(v->u.o.m[i].k);
        amz_free(&v->u.o.m[i].v);
      }
      free(v->u.o.m);
      break;
    default: break;
  }
  v->type = AMZ_NULL;
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

size_t amz_get_array_size(const amz_value* v) {
  assert(v != NULL && v->type == AMZ_ARRAY);
  return v->u.a.size;
}

amz_value* amz_get_array_element(const amz_value* v, size_t index) {
  assert(v != NULL && v->type == AMZ_ARRAY);
  assert(index < v->u.a.size);
  return &v->u.a.e[index];
}

size_t amz_get_object_size(const amz_value* v) {
  assert(v != NULL && v->type == AMZ_OBJECT);
  return v->u.o.size;
}

const char* amz_get_object_key(const amz_value* v, size_t index) {
  assert(v != NULL && v->type == AMZ_OBJECT);
  assert(index < v->u.o.size);
  return v->u.o.m[index].k;
}

size_t amz_get_object_key_length(const amz_value* v, size_t index) {
 assert(v != NULL && v->type == AMZ_OBJECT);
 assert(index < v->u.o.size);
 return v->u.o.m[index].klen;
}

amz_value* amz_get_object_value(const amz_value* v, size_t index) {
  assert(v != NULL && v->type == AMZ_OBJECT);
  assert(index < v->u.o.size);
  return &v->u.o.m[index].v;
}
