#pragma once

#include <QtCore/qhash.h>
#include <QtCore/qvector.h>
#include <QtCore/qmetatype.h>

#include "fwjson_global.h"
#include "fwjsonexception.h"

namespace FwJSON
{
    class Parser;
    class Node;
    class Null;
    class String;
    class Number;
    class Boolean;
    class Array;
    class Object;
    class Array;

    enum class Type
    {
        Null,
        String,
        Number,
        Bool,
        Object,
        Array
    };

    template <Type type_id> class Base;
    template <typename T, Type type_id> class BaseValue;

    const char constantTrue[] = "true";
    const char constantFalse[] = "false";

    FWJSON_SHARED_EXPORT bool nameToBool(const QByteArray&, bool* bOk);
    FWJSON_SHARED_EXPORT QByteArray boolToName(bool value);

    template <class T>
    T* cast(Node* node);
}

////////////////////////////////////////////////////////////////////////////////

class FWJSON_SHARED_EXPORT FwJSON::Node
{
public:

    friend class FwJSON::Object;
    friend class FwJSON::Array;

    Node();
    virtual ~Node();

    virtual Type type() const = 0;
    inline bool isNull() const;

    QByteArray name() const;

    virtual QByteArray toUtf8() const = 0;

    inline FwJSON::Node* parent() const;

    void takeFromParent();

    virtual int toInt(bool* bOk) const = 0;
    virtual uint toUint(bool* bOk) const = 0;
    virtual bool toBool(bool* bOk) const = 0;
    virtual double toNumber(bool* bOk) const = 0;
    virtual QString toString(bool* bOk) const = 0;

    virtual FwJSON::Node* clone() const = 0;

private:
    FwJSON::Node* parent_ = nullptr;
};

////////////////////////////////////////////////////////////////////////////////

template <FwJSON::Type type_id>
class FwJSON::Base
    : public FwJSON::Node
{
    using BaseClass = FwJSON::Node;

public:
    static const Type typeID = type_id;

    Type type() const override { return typeID; }
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, FwJSON::Type type_id>
class FwJSON::BaseValue
    : public FwJSON::Base<type_id>
{
    using BaseClass = FwJSON::Base<type_id>;

public:

    typedef T BaseType;

    BaseValue(const BaseType& value)
        : m_value(value)
    {}

    inline const BaseType& value() const
    {
        return m_value;
    }

    inline void setValue(const BaseType& value)
    {
        m_value = value;
    }

private:
    BaseType m_value;
};

////////////////////////////////////////////////////////////////////////////////

class FWJSON_SHARED_EXPORT FwJSON::String
    : public FwJSON::BaseValue<QString, FwJSON::Type::String>
{
    using BaseClass = FwJSON::BaseValue<QString, FwJSON::Type::String>;

public:

    inline static BaseType defaultValue()
    {
        return QString();
    }

    explicit String(const QString& value = defaultValue());

    inline bool isEmpty() const;

    QByteArray toUtf8() const;

    virtual int toInt(bool* bOk) const;
    virtual uint toUint(bool* bOk) const;
    virtual bool toBool(bool* bOk) const;
    virtual double toNumber(bool* bOk) const;
    virtual QString toString(bool* bOk) const;

    FwJSON::Node* clone() const;
};

////////////////////////////////////////////////////////////////////////////////

class FWJSON_SHARED_EXPORT FwJSON::Number
   : public FwJSON::BaseValue<double, FwJSON::Type::Number>
{
    using BaseClass = FwJSON::BaseValue<double, FwJSON::Type::Number>;
public:

    inline static BaseType defaultValue()
    {
        return 0.;
    }

    Number(double value = defaultValue());

    QByteArray toUtf8() const;

    virtual int toInt(bool* bOk) const;
    virtual uint toUint(bool* bOk) const;
    virtual bool toBool(bool* bOk) const;
    virtual double toNumber(bool* bOk) const;
    virtual QString toString(bool* bOk) const;

    FwJSON::Node* clone() const;
};

/////////////////////////////////////////////////////////////////////////////////

class FWJSON_SHARED_EXPORT FwJSON::Boolean
    : public FwJSON::BaseValue<bool, FwJSON::Type::Bool>
{
  using BaseClass = FwJSON::BaseValue<bool, FwJSON::Type::Bool>;

public:

    inline static BaseType defaultValue()
    {
        return false;
    }

    Boolean(bool value = defaultValue());

    QByteArray toUtf8() const;

    virtual int toInt(bool* bOk) const;
    virtual uint toUint(bool* bOk) const;
    virtual bool toBool(bool* bOk) const;
    virtual double toNumber(bool* bOk) const;
    virtual QString toString(bool* bOk) const;

    FwJSON::Node* clone() const;
};

////////////////////////////////////////////////////////////////////////////////

class FWJSON_SHARED_EXPORT FwJSON::Object
    : public FwJSON::Base<FwJSON::Type::Object>
{
    using BaseClass = FwJSON::Base<FwJSON::Type::Object>;
public:

    friend class FwJSON::Node;

    Object();
    ~Object();

    Node* addAttribute(const QByteArray& name, FwJSON::Node* value, bool replace = true);
    inline void removeAttribute(const QByteArray& name);

    inline String* addString(const QByteArray& name, const QString& value);
    inline Number* addNumber(const QByteArray& name, double value);
    inline Boolean* addBoolean(const QByteArray& name, bool value);
    inline Object* addObject(const QByteArray& name);
    inline Array* addArray(const QByteArray& name);

    void clear();

    inline FwJSON::Node* attribute(const QByteArray& name) const;
    inline QByteArray attributeName(FwJSON::Node* child) const;

    template<class T> bool hasValue(const QByteArray& name, typename T::BaseType* value = 0);

    template<class T> typename T::BaseType value(const QByteArray& name, const typename T::BaseType& defaultValue = T::defaultValue());
    template<class T> T* setValue(const QByteArray& name, const typename T::BaseType& value);

    inline QHash<QByteArray, FwJSON::Node*> attributes() const;
    inline QList<FwJSON::Node*> toList() const;
    inline int attributesCount() const;

    QByteArray toUtf8() const;

    void parse(const QByteArray& utf8String);
    void parse(QIODevice* ioDevice);
    void parseFile(const QString& fileName);

    virtual int toInt(bool* bOk) const;
    virtual uint toUint(bool* bOk) const;
    virtual bool toBool(bool* bOk) const;
    virtual double toNumber(bool* bOk) const;
    virtual QString toString(bool* bOk) const;

    FwJSON::Node* clone() const;

private:
    QHash<QByteArray, FwJSON::Node*> m_attributes;
};

////////////////////////////////////////////////////////////////////////////////

class FWJSON_SHARED_EXPORT FwJSON::Array : public FwJSON::Base<FwJSON::Type::Array>
{
    using BaseClass = FwJSON::Base<FwJSON::Type::Array>;

public:

    friend class FwJSON::Node;

    Array();
    ~Array();

    void clear();

    Node* addValue(FwJSON::Node* node);

    inline String* addString(const QString& value);
    inline Number* addNumber(double value);
    inline Boolean* addBoolean(bool value);
    inline Object* addObject();
    inline Array* addArray();

    inline int size() const;

    inline int indexOf(FwJSON::Node* item) const;
    inline FwJSON::Node* item(int index) const;

    QByteArray toUtf8() const;

    virtual int toInt(bool* bOk) const;
    virtual uint toUint(bool* bOk) const;
    virtual bool toBool(bool* bOk) const;
    virtual double toNumber(bool* bOk) const;
    virtual QString toString(bool* bOk) const;

    FwJSON::Node* clone() const;

    inline QVector<FwJSON::Node*> toQVector() const;

private:
    QVector<FwJSON::Node*> m_data;
};

#include "fwjson_inl.h"
