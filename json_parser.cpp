#include "json_parser.h"
#include "Jsonobj.h"
#include <sstream>
#include <iostream>

namespace RootType
{
    enum RootType
    {
        START,  // 文本开始
        ARRAY,  // 数组
        OBJECT, // 对象
        STRING, // 字符串
        NUMBER, // 数字
        KEY,    // 关键字
    };
}

// 获取utf8字节个数
static int byte_size(char ch)
{
    if(static_cast<unsigned char>(ch) >= 0xfc)
    { return 6; }
    if(static_cast<unsigned char>(ch) >= 0xf8)
    { return 5; }
    if(static_cast<unsigned char>(ch) >= 0xf0)
    { return 4; }
    if(static_cast<unsigned char>(ch) >= 0xe0)
    { return 3; }
    if(static_cast<unsigned char>(ch) >= 0xc0)
    { return 2; }
    return 1;
}

static JsonList parse_list(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr);
static JsonObject parse_object(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr);
static JsonString parse_string(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr);
static JsonNumber parse_number(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr);
static JObj&& parse_key(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr);

// 开始
static RootType::RootType parse_start(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr)
{
    while(it != end)
    {
        switch (*it)
        {
        case '{': return RootType::OBJECT;
        case '[': return RootType::ARRAY;
        case ' ': break;
        case '\t': break;
        case '\n': ++line; break;
        case '\"': return RootType::STRING;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': return RootType::NUMBER;
        case 'n':
        case 'f':
        case 't': return RootType::KEY;
        default:
            pr.code = JsonParser::ErrorCode::TYPE_INVALID;
            pr.line = line;
            return RootType::START;
        }
        ++it;
    }
    return RootType::START;
}

// 关键字
static JObj&& parse_key(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr)
{
    switch (*it)
    {
    case 'n':
        if(*++it == 'u' && *++it == 'l' && *++it == 'l')
        {
            return std::move(JObj());
        }
        break;
    case 't':
        if(*++it == 'r' && *++it == 'u' && *++it == 'e')
        {
            JsonBool jb;
            jb->set(true);
            return std::move(jb);
        }
        break;
    case 'f':
        if(*++it == 'a' && *++it == 'l' && *++it == 's' && *++it == 'e')
        {
            JsonBool jb;
            jb->set(false);
            return std::move(jb);
        }
        break;
    default:
        break;
    }
    pr.code = JsonParser::ErrorCode::VALUE_INVALID;
    pr.line = line;
    return std::move(JObj());
}

// 字符串
static JsonString parse_string(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr)
{
    JsonString js;
    while(it != end)
    {
        int size = byte_size(*it);
        // ascii
        if(size == 1)
        {
            // 转义字符
            if(*it == '\\')
            {
                js->append(*it);

                if(++it == end)
                {
                    pr.code = JsonParser::ErrorCode::ESCAPE_INVALID;
                    pr.line = line;
                    return js;
                }

                if(*it == '"' || *it == '\'' || *it == '?' || *it == 'b' ||
                    *it == '\\' || *it == 'f' || *it == 'n' || *it == 'r' || *it == 't')
                {
                    js->append(*it);
                }
                else
                {
                    pr.code = JsonParser::ErrorCode::ESCAPE_INVALID;
                    pr.line = line;
                    return js;
                }
            }
            // 结束了
            else if(*it == '"')
            {
                break;
            }
            else if(*it >= 32)
            {
                js->append(*it);
            }
            else
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return js;
            }
            ++it;
        }
        else
        {
            while(size-- > 0)
            {
                js->append(*it);
                if(++it == end)
                {
                    pr.code = JsonParser::ErrorCode::NO_STRING_END;
                    pr.line = line;
                    return js;
                }
            }
        }
    }
    return js;
}

// 数字
static JsonNumber parse_number(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr)
{
    JsonNumber result;
    std::string num_str;
    bool have_dot = false;
    bool is_dot = false;
    while(it != end)
    {
        if(!num_str.empty() && num_str[0] == '0' && *it >= '0' && *it <= '9')
        {
            pr.code = JsonParser::ErrorCode::NUMBER_BEG_ERROR;
            pr.line = line;
            return result;
        }

        if(!num_str.empty() && (*it == ',' || *it == ' ' || *it == '\n'))
        {
            break;
        }
        if((*it < '0' || *it > '9') && (*it != '.' || (*it == '.' && have_dot)))
        {
            pr.code = JsonParser::ErrorCode::NUMBER_ERROR;
            pr.line = line;
            return result;
        }
        if(*it == '.')
        {
            is_dot = true;
            have_dot = true;
        }
        else
        {
            is_dot = false;
        }
        num_str += *it++;
    }
    if(is_dot)
    {
        pr.code = JsonParser::ErrorCode::NUMBER_ERROR;
        pr.line = line;
        return result;
    }
    if(*it == '\n')
    {
        ++line;
    }
    else if(*it == ',')
    {
        --it;
    }
    double num;
    std::stringstream ss;
    ss << num_str;
    ss >> num;
    result->set(num);
    return result;
}

// 列表
static JsonList parse_list(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr)
{
    JsonList result;
    bool meet_comma = false;
    while(it != end)
    {
        if(*it == '"')
        {
            if(meet_comma && result->empty())
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
            result->append(parse_string(++it, end, line, pr));
            meet_comma = false;
        }
        else if(*it == '{')
        {
            if(meet_comma && result->empty())
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
            result->append(parse_object(++it, end, line, pr));
            meet_comma = false;
        }
        else if(*it >= '0' && *it <= '9')
        {
            if(meet_comma && result->empty())
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
            result->append(parse_number(it, end, line, pr));
            meet_comma = false;
        }
        else if(*it == '[')
        {
            result->append(parse_list(it, end, line, pr));
            meet_comma = false;
        }
        else if(*it == 'n' || *it == 't' || *it == 'f')
        {
            result->append(parse_key(it, end, line, pr));
            meet_comma = false;
        }
        else if(*it == '\n')
        {
            ++line;
        }
        else if(*it == ']')
        {
            if(meet_comma)
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
            }
            return result;
        }
        else if(*it == ',')
        {
            if(meet_comma)
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
            meet_comma = true;
        }
        else if(*it != ' ')
        {
            pr.code = JsonParser::ErrorCode::TYPE_INVALID;
            pr.line = line;
            return result;
        }
        if(pr.code != JsonParser::ErrorCode::SUCCESS)
        {
            return result;
        }
        ++it;
    }
    pr.code = JsonParser::ErrorCode::NO_LIST_END;
    pr.line = line;
    return result;
}

// 对象
static JsonObject parse_object(std::string::const_iterator &it, std::string::const_iterator end,
    unsigned int &line, ParseResult &pr)
{
    JsonObject result;
    std::string key_name;
    bool meet_colon = false, meet_comma = false;
    while(it != end)
    {
        if(*it == '}')
        {
            return result;
        }
        if(*it == '"')
        {
            if(!result->empty() && !meet_comma && key_name.empty())
            {
                pr.code = JsonParser::ErrorCode::MISSING_COMMA;
                pr.line = line;
                return result;
            }
            JsonString js = parse_string(++it, end, line, pr);
            if(pr.code != JsonParser::ErrorCode::SUCCESS)
            {
                return result;
            }
            if(key_name.empty())
            {
                key_name = js->value();
            }
            else if(!meet_colon)
            {
                pr.code = JsonParser::ErrorCode::MISSING_COLON;
                pr.line = line;
                return result;
            }
            else
            {
                result->set_value(key_name, std::move(js));
                meet_colon = false;
                key_name.clear();
            }
            meet_comma = false;
        }
        else if(*it == ':')
        {
            if(key_name.empty())
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
            meet_colon = true;
        }
        else if(*it == ' ' || *it == '\t')
        { }
        else if(*it == ',')
        {
            if(meet_colon)
            {
                pr.code = JsonParser::ErrorCode::MISSING_VALUE;
                pr.line = line;
                return result;
            }
            if(result->empty() || !key_name.empty())
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
            meet_comma = true;
        }
        else if(*it == 'n' || *it == 'f' || *it == 't')
        {
            if(key_name.empty())
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
            if(!meet_colon)
            {
                pr.code = JsonParser::ErrorCode::MISSING_COLON;
                pr.line = line;
                return result;
            }
            else
            {
                JObj &&j = parse_key(it, end, line, pr);
                if(pr.code != JsonParser::ErrorCode::SUCCESS)
                {
                    return result;
                }
                result->set_value(key_name, std::move(j));
                meet_colon = false;
                key_name.clear();
            }
        }
        else if(*it == '\n')
        {
            ++line;
        }
        else if(*it == '{')
        {
            if(meet_colon && !key_name.empty())
            {
                JsonObject jo = parse_object(++it, end, line, pr);
                if(pr.code != JsonParser::ErrorCode::SUCCESS)
                {
                    return result;
                }
                result->set_value(key_name, std::move(jo));
                key_name.clear();
                meet_colon = false;
            }
            else
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
        }
        else if(*it == '[')
        {
            if(!key_name.empty() && meet_colon)
            {
                JsonList jl= parse_list(++it, end, line, pr);
                if(pr.code != JsonParser::ErrorCode::SUCCESS)
                {
                    return result;
                }
                result->set_value(key_name, std::move(jl));
                key_name.clear();
                meet_colon = false;
            }
            else
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
        }
        else if(*it >= '0' && *it <= '9')
        {
            if(!key_name.empty() && meet_colon)
            {
                JsonNumber jn = parse_number(it, end, line, pr);
                if(pr.code != JsonParser::ErrorCode::SUCCESS)
                {
                    return result;
                }
                result->set_value(key_name, std::move(jn));
                key_name.clear();
                meet_colon = false;
            }
            else
            {
                pr.code = JsonParser::ErrorCode::TYPE_INVALID;
                pr.line = line;
                return result;
            }
        }
        else
        {
            pr.code = JsonParser::ErrorCode::TYPE_INVALID;
            pr.line = line;
            return result;
        }
        ++it;
    }
    pr.code = JsonParser::ErrorCode::NO_OBJECT_END;
    pr.line = line;
    return result;
}

/* json解析
 * @js   __in__  json字符串
 * @ret  __out__ 解析结果  type: JsonParserError
 */
JObj JsonParser::parse(const std::string &js, ParseResult &ret)
{
    ret.code = JsonParser::ErrorCode::SUCCESS;
    RootType::RootType state = RootType::START;
    JObj result;
    unsigned int line_count = 1;
    std::string::const_iterator it, end = js.end();

    for(it = js.begin(); it != end; ++it)
    {
        switch (state)
        {
        case RootType::START:
            state = parse_start(it, end, line_count, ret);
            break;
        case RootType::ARRAY:
        {
            JObj &&obj = parse_list(it, end, line_count, ret);
            result.swap(obj);
        } break;
        case RootType::OBJECT:
        {
            JObj &&obj = parse_object(it, end, line_count, ret);
            result.swap(obj);
        } break;
        case RootType::NUMBER:
        {
            JObj &&obj = parse_number(it, end, line_count, ret);
            result.swap(obj);
        } break;
        case RootType::KEY:
        {
            JObj &&obj = parse_key(it, end, line_count, ret);
            result.swap(obj);
        } break;
        case RootType::STRING:
        {
            JObj &&obj = parse_string(it, end, line_count, ret);
            result.swap(obj);
        } break;
        }
        if(ret.code != JsonParser::ErrorCode::SUCCESS)
        {
            break;
        }
    }
    return result;
}

std::string JsonParser::result_msg(const ParseResult &pr)
{
    std::string msg;
    switch (pr.code)
    {
    case JsonParser::ErrorCode::SUCCESS:
        msg = "success!";
        break;
    case JsonParser::ErrorCode::NO_LIST_END:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": there is not end at list!";
    } break;
    case JsonParser::ErrorCode::NO_OBJECT_END:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": there is not end at object!";
    } break;
    case JsonParser::ErrorCode::TYPE_INVALID:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": invalid type!";
    } break;
    case JsonParser::ErrorCode::VALUE_INVALID:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": invalid value! only support STRING, NUMBER, null, true, false!";
    } break;
    case JsonParser::ErrorCode::ESCAPE_INVALID:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": invalid escape character!";
    } break;
    case JsonParser::ErrorCode::NO_STRING_END:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": there is no end!";
    } break;
    case JsonParser::ErrorCode::NUMBER_BEG_ERROR:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": number can only begin with [1-9]";
    } break;
    case JsonParser::ErrorCode::NUMBER_ERROR:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": number error!";
    } break;
    case JsonParser::ErrorCode::MISSING_COLON:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": a colon maybe missing!";
    } break;
    case JsonParser::ErrorCode::REDUNDANT_COMMA:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": a comma maybe redundant!";
    } break;
    case JsonParser::ErrorCode::MISSING_COMMA:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": a comma maybe missing!";
    } break;
    case JsonParser::ErrorCode::MISSING_VALUE:
    {
        std::ostringstream oss;
        oss << pr.line;
        msg = "line ";
        msg += oss.str();
        msg += ": a value maybe missing!";
    } break;
    default: break;
    }
    return msg;
}
