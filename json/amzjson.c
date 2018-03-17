#include "amzjson.h"
#include <assert.h> /* assert() */
#include <stdlib.h> /* NULL */

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++; } while(0)

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

/* null = "null" */
static int amz_parse_null(amz_context* c, amz_value* v) {
  EXPECT(c, 'n');
  if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
    return AMZ_PARSE_INVALID_VALUE;
  c->json += 3;
  v->type = AMZ_NULL;
  return AMZ_PARSE_OK;
}

/* true = "true */
static int amz_parse_true(amz_context* c, amz_value* v) {
  EXPECT(c, 't');
  if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
    return AMZ_PARSE_INVALID_VALUE;
  c->json += 3;
  v->type = AMZ_TRUE;
  return AMZ_PARSE_OK;
}

/* false = "false" */
static int amz_parse_false(amz_context* c, amz_value* v) {
  EXPECT(c, 'f');
  if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
    return AMZ_PARSE_INVALID_VALUE;
  c->json += 4;
  v->type = AMZ_FALSE;
  return AMZ_PARSE_OK;
}

/* value = null / false / true */
static int amz_parse_value(amz_context* c, amz_value* v) {
  switch (*c->json) {
    case 't':  return amz_parse_true(c, v);
    case 'f':  return amz_parse_false(c, v);
    case 'n':  return amz_parse_null(c, v);
    case '\0': return AMZ_PARSE_EXPECT_VALUE;
    default:   return AMZ_PARSE_INVALID_VALUE;
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
	if (*c.json != '\0')
	  ret = AMZ_PARSE_ROOT_NOT_SINGULAR;
  }
  return ret;
}

amz_type amz_get_type(const amz_value* v) {
  assert(v != NULL);
  return v->type;
}
