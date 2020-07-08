#ifndef JSON_OBJ_H
#define JSON_OBJ_H

#include "json_impl.h"

class JsonString;
class JsonNumber;
class JsonList;
class JsonObject;
class JsonBool;
class JObj;

class JObj
{
protected:
    __JObj *_M_pointer;
    friend class __JObj;

public:
    JObj();
    JObj(const JObj &jo);
    JObj(JObj &&jo);
    virtual ~JObj();

    virtual inline JsonString& string_cast()
    { return *reinterpret_cast<JsonString *>(this); }

    virtual inline JsonNumber& number_cast()
    { return *reinterpret_cast<JsonNumber *>(this); }

    virtual inline JsonList& list_cast()
    { return *reinterpret_cast<JsonList *>(this); }

    virtual inline JsonObject& object_cast()
    { return *reinterpret_cast<JsonObject *>(this); }

    virtual inline JsonBool& bool_cast()
    { return *reinterpret_cast<JsonBool *>(this); }

    virtual inline const JsonString& string_cast() const
    { return *reinterpret_cast<const JsonString *>(this); }

    virtual inline const JsonNumber& number_cast() const
    { return *reinterpret_cast<const JsonNumber *>(this); }

    virtual inline const JsonList& list_cast() const
    { return *reinterpret_cast<const JsonList *>(this); }

    virtual inline const JsonObject& object_cast() const
    { return *reinterpret_cast<const JsonObject *>(this); }

    virtual inline const JsonBool& bool_cast() const
    { return *reinterpret_cast<const JsonBool *>(this); }

    __JObj* operator->()
    { return _M_pointer; }

    const __JObj* operator->() const
    { return _M_pointer; }

    __JObj& operator*()
    { return *_M_pointer; }

    const __JObj& operator*() const
    { return *_M_pointer; }

    virtual JObj& operator=(const JObj &jo);

    virtual JObj& operator=(JObj &&jo);

    virtual void swap(JObj &jo);
};

class JsonString : public JObj
{ 
public:
    JsonString();
    JsonString(const JsonString &j) : JObj(j) { }
    JsonString(JsonString &&j) : JObj(std::move(j)) { }
    
    __JsonString* operator->()
    { return reinterpret_cast<__JsonString*>(_M_pointer); }

    const __JsonString* operator->() const
    { return reinterpret_cast<__JsonString*>(_M_pointer); }

    __JsonString& operator*()
    { return *reinterpret_cast<__JsonString*>(_M_pointer); }

    const __JsonString& operator*() const
    { return *reinterpret_cast<__JsonString*>(_M_pointer); }
};

class JsonNumber : public JObj
{ 
public:
    JsonNumber();
    JsonNumber(const JsonNumber &j) : JObj(j) { }
    JsonNumber(JsonNumber &&j) : JObj(std::move(j)) { }

    __JsonNumber* operator->()
    { return reinterpret_cast<__JsonNumber*>(_M_pointer); }

    const __JsonNumber* operator->() const
    { return reinterpret_cast<__JsonNumber*>(_M_pointer); }

    __JsonNumber& operator*()
    { return *reinterpret_cast<__JsonNumber*>(_M_pointer); }

    const __JsonNumber& operator*() const
    { return *reinterpret_cast<__JsonNumber*>(_M_pointer); }
};

class JsonList : public JObj
{ 
public:
    typedef typename __JsonList::iterator iterator;
    typedef typename __JsonList::const_iterator const_iterator;

public:
    JsonList();
    JsonList(const JsonList &j) : JObj(j) { }
    JsonList(JsonList &&j) : JObj(std::move(j)) { }

    __JsonList* operator->()
    { return reinterpret_cast<__JsonList*>(_M_pointer); }

    const __JsonList* operator->() const
    { return reinterpret_cast<__JsonList*>(_M_pointer); }

    __JsonList& operator*()
    { return *reinterpret_cast<__JsonList*>(_M_pointer); }

    const __JsonList& operator*() const
    { return *reinterpret_cast<__JsonList*>(_M_pointer); }
};

class JsonObject : public JObj
{ 
public:
    JsonObject();
    JsonObject(const JsonObject &j) : JObj(j) { }
    JsonObject(JsonObject &&j) : JObj(std::move(j)) { }

    __JsonObject* operator->()
    { return reinterpret_cast<__JsonObject*>(_M_pointer); }

    const __JsonObject* operator->() const
    { return reinterpret_cast<__JsonObject*>(_M_pointer); }

    __JsonObject& operator*()
    { return *reinterpret_cast<__JsonObject*>(_M_pointer); }

    const __JsonObject& operator*() const
    { return *reinterpret_cast<__JsonObject*>(_M_pointer); }
};

class JsonBool : public JObj
{ 
public:
    JsonBool();
    JsonBool(const JsonBool &j) : JObj(j) { }
    JsonBool(JsonBool &&j) : JObj(std::move(j)) { }

    __JsonBool* operator->()
    { return reinterpret_cast<__JsonBool*>(_M_pointer); }

    const __JsonBool* operator->() const
    { return reinterpret_cast<__JsonBool*>(_M_pointer); }

    __JsonBool& operator*()
    { return *reinterpret_cast<__JsonBool*>(_M_pointer); }

    const __JsonBool& operator*() const
    { return *reinterpret_cast<__JsonBool*>(_M_pointer); }
};
#endif // JSON_OBJ_H