#include "json_impl.h"
#include "jsonobj.h"

__JObj::~__JObj() { }
__JsonString::~__JsonString() { }
__JsonNumber::~__JsonNumber() { }
__JsonList::~__JsonList() { }
__JsonObject::~__JsonObject() { }
__JsonBool::~__JsonBool() { }

void __JsonList::clear_empty()
{
    if(empty())
    {
        return;
    }

    std::size_t index = _M_data.size();
    while(index-- > 0)
    {
        if(_M_data[index]->type() == JsonObjectType::EMPTY)
        {
            _M_data.erase(_M_data.begin() + index);
        }
    }
}

void __JsonObject::clear_empty() 
{
    std::list<std::string>::iterator it = _M_order.begin();
    while(it != _M_order.end())
    {
        iterator found = _M_data.find(*it);
        if(found != _M_data.end())
        {
            if(found->second->type() == JsonObjectType::EMPTY)
            {
                _M_order.erase(it++);
                _M_data.erase(found);
            }
            else 
            {
                ++it;
            }
        }
        else 
        {
            _M_order.erase(it++);
        }
    }
}

void __JsonString::append(const JsonString &js)
{ _M_data += js->_M_data; }

void __JsonList::append_list(const JsonList &jl)
{ _M_data.insert(_M_data.end(), jl->begin(), jl->end()); }

void __JsonList::append_list(JsonList &&jl)
{ _M_data.insert(_M_data.end(), jl->begin(), jl->end()); }

JObj& __JsonList::operator[](unsigned index)
{ return _M_data[index]; }

JObj& __JsonList::value(unsigned index)
{ return _M_data[index]; }

JObj& __JsonObject::operator[](const std::string &key)
{ 
    _M_order.push_back(key);
    return _M_data[key]; 
}

const JObj& __JsonObject::value(const std::string &name, const JObj &def_v) const 
{
    const_iterator found = _M_data.find(name);
    return found == _M_data.end() ? def_v : found->second;
}

JObj& __JsonObject::value(const std::string &key) 
{ 
    std::map<std::string, JObj>::iterator found = _M_data.find(key);
    if(found == _M_data.end())
    {
        _M_order.push_back(key);
        return _M_data[key]; 
    }
    return found->second;
}

void __JsonObject::set_value(const std::string &key, const JObj &obj)
{ 
    std::map<std::string, JObj>::iterator found = _M_data.find(key);
    if(found == _M_data.end())
    {
        _M_order.push_back(key);
        _M_data.insert(std::pair<std::string, JObj>(key, obj)); 
    }
    else 
    {
        found->second = obj;
    }
}

void __JsonObject::set_value(const std::string &key, JObj &&obj)
{
    if(_M_data.find(key) == _M_data.end())
    {
        _M_order.push_back(key);
    }
    _M_data.insert(std::pair<std::string, JObj>(key, std::move(obj)));
}

void __JsonObject::remove(const std::string &key)
{ _M_data[key] = std::move(JObj()); }
