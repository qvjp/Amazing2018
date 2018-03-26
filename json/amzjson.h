#ifndef AMZJSON_H__
#define AMZJSON_H__

#include <stddef.h> /* size_t */

/* 声明JSON中的数据类型 */
typedef enum {
  AMZ_NULL,
  AMZ_FALSE,
  AMZ_TRUE,
  AMZ_NUMBER,
  AMZ_STRING,
  AMZ_ARRAY,
  AMZ_OBJECT
} amz_type;

typedef struct amz_value amz_value;

/* JSON数据结构 */
struct amz_value {
  union {
    struct { amz_value* e; size_t size; }a; /* array */
    struct { char* s; size_t len; }s;       /* string */
    double n;                               /* number */
  }u;
  amz_type type;
};

/* 返回值类型 */
enum {
  AMZ_PARSE_OK = 0,
  AMZ_PARSE_EXPECT_VALUE,
  AMZ_PARSE_INVALID_VALUE,
  AMZ_PARSE_ROOT_NOT_SINGULAR,
  AMZ_PARSE_NUMBER_TOO_BIG,
  AMZ_PARSE_MISS_QUOTATION_MARK,
  AMZ_PARSE_INVALID_STRING_ESCAPE,
  AMZ_PARSE_INVALID_STRING_CHAR,
  AMZ_PARSE_INVALID_UNICODE_HEX,
  AMZ_PARSE_INVALID_UNICODE_SURROGATE,
  AMZ_PARSE_MISS_COMMA_OR_SQUARE_BRACKET
};

/* 初始化类型 */
#define amz_init(v) do { (v)->type = AMZ_NULL; } while(0)

/* 解析JSON */
int amz_parse(amz_value* v, const char* json);

void amz_free(amz_value* v);

/* 访问结果 */
amz_type amz_get_type(const amz_value* v);

#define amz_set_null(v) amz_free(v)

int amz_get_boolean(const amz_value* v);
void amz_set_boolean(amz_value* v, int b);

double amz_get_number(const amz_value* v);
void amz_set_number(amz_value* v, double b);

const char* amz_get_string(const amz_value* v);
size_t amz_get_string_length(const amz_value* v);
void amz_set_string(amz_value* v, const char* s, size_t len);

size_t amz_get_array_size(const amz_value* v);
amz_value* amz_get_array_element(const amz_value* v, size_t index);

#endif /* AMZJSON_H__ */
