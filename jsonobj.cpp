#include "jsonobj.h"

JObj::JObj() : _M_pointer(new __JObj())
{
}

JObj::JObj(const JObj &jo) : _M_pointer(nullptr)
{
    switch (jo._M_pointer->type())
    {
    case JsonObjectType::STRING: _M_pointer = new __JsonString(*reinterpret_cast<const __JsonString*>(jo._M_pointer)); break;
    case JsonObjectType::NUMBER: _M_pointer = new __JsonNumber(*reinterpret_cast<const __JsonNumber*>(jo._M_pointer)); break;
    case JsonObjectType::LIST: _M_pointer = new __JsonList(*reinterpret_cast<const __JsonList*>(jo._M_pointer)); break;
    case JsonObjectType::OBJECT: _M_pointer = new __JsonObject(*reinterpret_cast<const __JsonObject*>(jo._M_pointer)); break;
    case JsonObjectType::BOOL: _M_pointer = new __JsonBool(*reinterpret_cast<const __JsonBool*>(jo._M_pointer)); break;
    default: _M_pointer = new __JObj(*reinterpret_cast<const __JObj*>(jo._M_pointer)); break;
    }
}

JObj::JObj(JObj &&jo) : _M_pointer(nullptr)
{
    swap(jo);
}

JObj::~JObj()
{
    if(_M_pointer != nullptr)
    {
        delete _M_pointer;
        _M_pointer = nullptr;
    }
}

JObj& JObj::operator=(const JObj &jo)
{
    if(_M_pointer != nullptr)
    {
        delete _M_pointer;
        _M_pointer = nullptr;
    }
    switch (jo._M_pointer->type())
    {
    case JsonObjectType::STRING: _M_pointer = new __JsonString(*reinterpret_cast<const __JsonString*>(jo._M_pointer)); break;
    case JsonObjectType::NUMBER: _M_pointer = new __JsonNumber(*reinterpret_cast<const __JsonNumber*>(jo._M_pointer)); break;
    case JsonObjectType::LIST: _M_pointer = new __JsonList(*reinterpret_cast<const __JsonList*>(jo._M_pointer)); break;
    case JsonObjectType::OBJECT: _M_pointer = new __JsonObject(*reinterpret_cast<const __JsonObject*>(jo._M_pointer)); break;
    case JsonObjectType::BOOL: _M_pointer = new __JsonBool(*reinterpret_cast<const __JsonBool*>(jo._M_pointer)); break;
    default: _M_pointer = new __JObj(*reinterpret_cast<const __JObj*>(jo._M_pointer));  break;
    }
    return *this;
}

JObj& JObj::operator=(JObj &&jo)
{
    swap(jo);
    return *this;
}

void JObj::swap(JObj &jo)
{
    __JObj *tmp = _M_pointer;
    _M_pointer = jo._M_pointer;
    jo._M_pointer = tmp;
}

JsonString::JsonString()
{
    _M_pointer = new __JsonString();
}

JsonNumber::JsonNumber()
{
    _M_pointer = new __JsonNumber();
}

JsonList::JsonList()
{
    _M_pointer = new __JsonList();
}

JsonObject::JsonObject()
{
    _M_pointer = new __JsonObject();
}

JsonBool::JsonBool()
{
    _M_pointer = new __JsonBool();
}
