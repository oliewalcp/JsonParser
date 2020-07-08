#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>

class JObj;

struct ParseResult
{
    unsigned long long code : 8;
    unsigned long long line : 56;
};

struct JsonParser
{
    enum ErrorCode
    {
        SUCCESS, // 成功
        NO_LIST_END, // 列表不完整
        NO_OBJECT_END, // 对象不完整
        TYPE_INVALID, // 类型不支持
        VALUE_INVALID,// 错误值
        ESCAPE_INVALID, // 转义字符错误
        NO_STRING_END, // 字符串不完整
        NUMBER_BEG_ERROR,  // 数字错误
        NUMBER_ERROR,  // 数字错误
        MISSING_COLON, // 冒号丢失
        REDUNDANT_COMMA, // 多余的逗号
        MISSING_COMMA, // 逗号丢失
        MISSING_VALUE, // 值缺失
    };
    /* json解析
     * @js   __in__  json字符串
     * @ret  __out__ 解析结果  type: JsonParserError
     */
    static JObj parse(const std::string &js, ParseResult &ret);

    /* 将json结构保存为字符串
     * @obj  __in__  json结构
     */
    static std::string to_string(const JObj &obj, int indent = 4, const char *newl = "\n");

    static std::string result_msg(const ParseResult &pr);
};

#endif // JSON_PARSER_H