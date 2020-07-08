#include "json_parser.h"
#include "jsonobj.h"

#include <sstream>

// 保存
static void tostring(unsigned &, std::string &, const JObj &);
static void tostring(unsigned &, std::string &, const JsonBool &);
static void tostring(unsigned &, std::string &, const JsonString &);
static void tostring(unsigned &, std::string &, const JsonNumber &);
static void tostring(unsigned &, std::string &, const JsonList &, int, const char *);
static void tostring(unsigned &, std::string &, const JsonObject &, int, const char *);

static void dispatch_type(unsigned &floor_size, std::string &buff, const JObj &jo, int indent, const char *newl)
{
    switch (jo->type())
    {
    case JsonObjectType::BOOL:
        tostring(floor_size, buff, jo.bool_cast());
        break;
    case JsonObjectType::STRING:
        tostring(floor_size, buff, jo.string_cast());
        break;
    case JsonObjectType::NUMBER:
        tostring(floor_size, buff, jo.number_cast());
        break;
    case JsonObjectType::LIST:
        tostring(floor_size, buff, jo.list_cast(), indent, newl);
        break;
    case JsonObjectType::OBJECT:
        tostring(floor_size, buff, jo.object_cast(), indent, newl);
        break;
    case JsonObjectType::EMPTY:
        tostring(floor_size, buff, jo);
        break;
    default: break;
    }
}

static void put_indent(std::string &buff, unsigned floor_size, int indent)
{
    int len = floor_size * indent;
    while(len-- > 0)
    {
        buff += ' ';
    }
}

static void tostring(unsigned &, std::string &buff, const JObj &jb)
{
    buff += "null";
}

static void tostring(unsigned &, std::string &buff, const JsonBool &jb)
{
    if(jb->value() == true)
    {
        buff += "true";
    }
    else
    {
        buff += "false";
    }
    
    // buff += jb->value() ? "true" : "false";
}

static void tostring(unsigned &, std::string &buff, const JsonString &js)
{
    buff += '"';
    buff += js->value();
    buff += '"';
}

static void tostring(unsigned &, std::string &buff, const JsonNumber &jn)
{
    std::ostringstream oss;
    oss << jn->value();
    buff += oss.str();
}

static void tostring(unsigned &floor_size, std::string &buff, const JsonList &jl, int indent, const char *newl)
{
    ++floor_size;

    if(!jl->empty())
    {
        buff += '[';
        buff += newl;
        put_indent(buff, floor_size, indent);
    }

    __JsonList::const_iterator it = jl->cbegin();
    while(it != jl->cend())
    {
        dispatch_type(floor_size, buff, *it, indent, newl);
        if(++it != jl->cend())
        {
            buff += ',';
        }
    }

    --floor_size;
    if(!jl->empty())
    {
        buff += newl;
        put_indent(buff, floor_size, indent);
        buff += ']';
    }
}

static void tostring(unsigned &floor_size, std::string &buff, const JsonObject &jl, int indent, const char *newl)
{
    const __JsonObject::key_list_type &keys = jl->keys();

    ++floor_size;

    if(!keys.empty())
    {
        buff += '{';
        buff += newl;
        put_indent(buff, floor_size, indent);
    }

    for(__JsonObject::key_list_type::const_iterator it = keys.begin(); it != keys.end();)
    {
        const JObj &obj = jl->value(*it, JObj());
        buff += '"';
        buff += *it;
        buff += '"';
        buff += ':';
        dispatch_type(floor_size, buff, obj, indent, newl);
        if(++it != keys.end())
        {
            buff += ',';
            buff += newl;
            put_indent(buff, floor_size, indent);   
        }
    }

    --floor_size;

    if(!keys.empty())
    {
        buff += newl;
        put_indent(buff, floor_size, indent);
        buff += '}';
    }
}

/* 将json结构保存为字符串
 * @obj  __in__  json结构
 * @ret  __out__ 转换结果
 */
std::string JsonParser::to_string(const JObj &obj, int indent, const char *newl)
{
    unsigned floor_size = 0;
    std::string result;
    dispatch_type(floor_size, result, obj, indent, newl);
    return result;
}
