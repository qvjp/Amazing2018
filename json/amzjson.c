#include "amzjson.h"
#include <assert.h> /* assert() */
#include <errno.h>  /* errno, ERANGE */
#include <math.h>   /* HUGE_VAL */
#include <stdlib.h> /* NULL, strtod() */

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)      ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)  ((ch) >= '1' && (ch) <= '9')

typedef struct {
  const char* json;
} amz_context;

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
  v->n = strtod(c->json, NULL);
  if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
    return AMZ_PARSE_NUMBER_TOO_BIG;
  v->n = strtod(c->json, NULL);
  v->type = AMZ_NUMBER;
  c->json = p;
  return AMZ_PARSE_OK;
}

/* value = null / false / true / number*/
static int amz_parse_value(amz_context* c, amz_value* v) {
  switch (*c->json) {
    case 't':  return amz_parse_literal(c, v, "true", AMZ_TRUE);
    case 'f':  return amz_parse_literal(c, v, "false", AMZ_FALSE);
    case 'n':  return amz_parse_literal(c, v, "null", AMZ_NULL);
    default:   return amz_parse_number(c, v);
    case '\0': return AMZ_PARSE_EXPECT_VALUE;
  }
}

/* JSON-text = ws value ws */
int amz_parse(amz_value* v, const char* json) {
  amz_context c;
  int ret;
  assert(v != NULL);
  c.json = json;
  v->type = AMZ_NULL;
  amz_parse_whitespace(&c);
  if ((ret = amz_parse_value(&c, v)) == AMZ_PARSE_OK) {
    amz_parse_whitespace(&c);
    if (*c.json != '\0') {
      v->type = AMZ_NULL;
      ret = AMZ_PARSE_ROOT_NOT_SINGULAR;
	}
  }
  return ret;
}

amz_type amz_get_type(const amz_value* v) {
  assert(v != NULL);
  return v->type;
}

double amz_get_number(const amz_value* v) {
    assert(v != NULL && v->type == AMZ_NUMBER);
    return v->n;
}
