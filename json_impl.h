#ifndef JSON_IMPL_H
#define JSON_IMPL_H

#include <map>
#include <string>
#include <vector>
#include <list>

namespace JsonObjectType
{
    enum JsonObjectType
    {
        EMPTY,
        STRING,
        NUMBER,
        LIST,
        OBJECT,
        BOOL
    };
}

class JsonString;
class JsonNumber;
class JsonList;
class JsonObject;
class JsonBool;
class JObj;

class __JsonString;
class __JsonNumber;
class __JsonList;
class __JsonObject;
class __JsonBool;
class __JObj;

class __JObj 
{ 
private:
    friend class JObj;

private:
    JsonObjectType::JsonObjectType _M_type;

protected:
    __JObj() : _M_type(JsonObjectType::EMPTY) {}
    __JObj(JsonObjectType::JsonObjectType type) : _M_type(type) {}
    __JObj(__JObj &&__j) : _M_type(__j._M_type) {}
    __JObj(const __JObj &__j) : _M_type(__j._M_type) {}
    virtual ~__JObj();

public:
    virtual JsonObjectType::JsonObjectType type() const 
    { return _M_type; }
};

class __JsonString : public __JObj
{
private:
    friend class JsonString;
    friend class JObj;

private:
    std::string _M_data;

    __JsonString() : __JObj(JsonObjectType::STRING) { }
    __JsonString(std::string v) : __JObj(JsonObjectType::STRING), _M_data(v) { }
    __JsonString(const __JsonString &__js) : __JObj(JsonObjectType::STRING), _M_data(__js._M_data) { }
    __JsonString(__JsonString &&__js) : __JObj(JsonObjectType::STRING), _M_data(std::move(__js._M_data)) { }
    virtual ~__JsonString();
public:

    inline unsigned length() const 
    { return static_cast<unsigned>(_M_data.size()); }
    
    inline const std::string& value() const
    { return _M_data; }

    inline void set(const std::string &v) 
    { _M_data = v; }

    inline void set(std::string &&v)
    { _M_data.swap(v); }

    inline void append(const __JsonString *js)
    { _M_data += js->_M_data; }

    inline void append(const JsonString &js);

    inline void append(const std::string &str)
    { _M_data += str; }

    inline void append(const char ch)
    { _M_data += ch; }

    inline void clear()
    { _M_data.clear(); }
};

class __JsonNumber : public __JObj
{
private:
    friend class JsonNumber;
    friend class JObj;

private:
    double _M_data;

    __JsonNumber() : __JObj(JsonObjectType::NUMBER), _M_data(0) { }
    __JsonNumber(double v) : __JObj(JsonObjectType::NUMBER), _M_data(v) { }
    __JsonNumber(const __JsonNumber &__jn) : __JObj(JsonObjectType::NUMBER), _M_data(__jn._M_data) { }
    __JsonNumber(__JsonNumber &&__jn) : __JObj(JsonObjectType::NUMBER), _M_data(__jn._M_data) { }
    virtual ~__JsonNumber();
public:

    inline double value() const
    { return _M_data; }

    inline void set(double v)
    { _M_data = v; }
};

class __JsonList : public __JObj
{
private:
    friend class JsonList;
    friend class JObj;

private:
    std::vector<JObj> _M_data;

    __JsonList() : __JObj(JsonObjectType::LIST) { }
    __JsonList(const __JsonList &__jl) : __JObj(JsonObjectType::LIST), _M_data(__jl._M_data) { }
    __JsonList(__JsonList &&__jl) : __JObj(JsonObjectType::LIST), _M_data(std::move(__jl._M_data)) { }
    virtual ~__JsonList();
public:
    typedef std::vector<JObj>::iterator iterator;
    typedef std::vector<JObj>::const_iterator const_iterator;

    // 获取指定位置的成员
    inline JObj& operator[](unsigned index);

    inline JObj& value(unsigned index);

    inline unsigned size() const
    { return static_cast<unsigned>(_M_data.size()); }

    // 拼接列表
    inline void append_list(const JsonList &jl);

    inline void append_list(JsonList &&jl);

    // 拼接成员
    inline void append(const JObj &j)
    { _M_data.push_back(j); }

    inline void append(JObj &&j)
    { _M_data.push_back(std::move(j)); }

    // 将空成员全部删除
    void clear_empty();

    inline bool empty() const 
    { return _M_data.empty(); }

    inline void clear() 
    { _M_data.clear(); }

    inline iterator begin()
    { return _M_data.begin(); }

    inline iterator end()
    { return _M_data.end(); }

    inline const_iterator begin() const
    { return _M_data.begin(); }
    
    inline const_iterator end() const
    { return _M_data.end(); }

    inline const_iterator cbegin() const
    { return _M_data.begin(); }
    
    inline const_iterator cend() const
    { return _M_data.end(); }
};

// json对象
class __JsonObject : public __JObj
{
private:
    friend class JsonObject;
    friend class JObj;

private:
    std::map<std::string, JObj> _M_data;
    std::list<std::string> _M_order;

    __JsonObject() : __JObj(JsonObjectType::OBJECT) { }
    __JsonObject(const __JsonObject &__jo)
     : __JObj(JsonObjectType::OBJECT), _M_data(__jo._M_data), _M_order(__jo._M_order)
    { }
    __JsonObject(__JsonObject &&__jo)
     : __JObj(JsonObjectType::OBJECT), _M_data(std::move(__jo._M_data)), _M_order(std::move(__jo._M_order))
    { }
    virtual ~__JsonObject();
public:
    typedef std::map<std::string, JObj>::iterator iterator;
    typedef std::map<std::string, JObj>::const_iterator const_iterator;
    typedef std::list<std::string> key_list_type;

    inline JObj& operator[](const std::string &key);

    /* 获取成员值
     * @name   成员名称
     * @def_v  默认值，当不存在时返回该值
     */
    const JObj& value(const std::string &name, const JObj &def_v) const;

    /* 设置成员
     * @key  成员名称
     * @obj  成员值
     */
    void set_value(const std::string &key, const JObj &obj);

    void set_value(const std::string &key, JObj &&obj);

    /* 删除成员，将对应成员标记为空，不移除，不改变size()
     * @key  成员名称
     */
    void remove(const std::string &key);

    /* 将空成员全部删除
     * 调用后，将可能改变size()
     */
    void clear_empty();

    // 获取成员数量，包含成员值为空的
    inline unsigned size() const
    { return static_cast<unsigned>(_M_data.size()); }

    inline bool empty() const
    { return _M_data.empty(); }

    inline void clear() 
    { 
        _M_data.clear(); 
        _M_order.clear();
    }

    // 获取成员列表
    inline std::list<std::string>& keys()
    { return _M_order; }

    inline const std::list<std::string>& keys() const
    { return _M_order; }

    /* 获取成员的值
     * @key  成员名称
     * @return  若不存在，返回空成员；否则返回对应成员值
     */
    JObj& value(const std::string &key);

};

class __JsonBool : __JObj
{
private:
    friend class JsonBool;
    friend class JObj;

private:
    bool _M_data;

    __JsonBool() : __JObj(JsonObjectType::BOOL), _M_data(false) { }
    __JsonBool(bool v) : __JObj(JsonObjectType::BOOL), _M_data(v) { }
    __JsonBool(const __JsonBool &__jb) : __JObj(JsonObjectType::BOOL), _M_data(__jb._M_data) { }
    __JsonBool(__JsonBool &&__jb) : __JObj(JsonObjectType::BOOL), _M_data(__jb._M_data) { }
    virtual ~__JsonBool();
public:

    inline bool value() const
    { return _M_data; }

    inline void set(bool v)
    { _M_data = v; }
};

#endif // JSON_IMPL_H