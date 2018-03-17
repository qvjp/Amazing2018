#ifndef AMZJSON_H__
#define AMZJSON_H__

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

/* JSON数据结构 */
typedef struct {
  double n;
  amz_type type;
} amz_value;

/* 返回值类型 */
enum {
  AMZ_PARSE_OK = 0,
  AMZ_PARSE_EXPECT_VALUE,
  AMZ_PARSE_INVALID_VALUE,
  AMZ_PARSE_ROOT_NOT_SINGULAR,
  AMZ_PARSE_NUMBER_TOO_BIG
};

/* 解析JSON */
int amz_parse(amz_value* v, const char* json);

/* 访问结果 */
amz_type amz_get_type(const amz_value* v);
double amz_get_number(const amz_value* v);

#endif /* AMZJSON_H__ */
