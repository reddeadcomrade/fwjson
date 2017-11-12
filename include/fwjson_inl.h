#ifndef FIREWORKS_JSON_INL_H
#define FIREWORKS_JSON_INL_H

#include "fwjson.h"

template <class T>
T* FwJSON::cast(Node* node)
{
    return node && node->type() == T::typeID ? static_cast<T*>(node) : nullptr;
}

///////////////////////////////////////////////////////////////////////////////

FwJSON::Node* FwJSON::Node::parent() const
{
    return m_parent;
}

///////////////////////////////////////////////////////////////////////////////

bool FwJSON::String::isEmpty() const
{
    return value().isEmpty();
}

///////////////////////////////////////////////////////////////////////////////

FwJSON::Node* FwJSON::Object::attribute(const QByteArray& name) const
{
    return m_attributes.value(name, 0);
}

QByteArray FwJSON::Object::attributeName(FwJSON::Node* child) const
{
    return m_attributes.key(child, "");
}

template<class T>
bool FwJSON::Object::hasValue(const QByteArray& name, typename T::BaseType* value)
{
    if (T* node = cast<T>(attribute(name)))
    {
        if (value)
        {
            (*value) = node->value();
        }
        return true;
    }
    return false;
}

template<class T>
typename T::BaseType FwJSON::Object::value(const QByteArray& name, const typename T::BaseType& defaultValue)
{
    if (T* node = cast<T>(attribute(name)))
    {
        return node->value();
    }
    return defaultValue;
}

template<class T> T* FwJSON::Object::setValue(const QByteArray& name, const typename T::BaseType& value)
{
    if(T* node = cast<T>(attribute(name)))
    {
        node->setValue(value);
        return node;
    }
    return static_cast<T*>(addAttribute(name, new T(value), true));
}

QHash<QByteArray, FwJSON::Node*> FwJSON::Object::attributes() const
{
    return m_attributes;
}

QList<FwJSON::Node*> FwJSON::Object::toList() const
{
    return m_attributes.values();
}

void FwJSON::Object::removeAttribute(const QByteArray& name)
{
    if(m_attributes.contains(name))
    {
        delete m_attributes.take(name);
    }
}

int FwJSON::Object::attributesCount() const
{
    return m_attributes.size();
}

FwJSON::String* FwJSON::Object::addString(const QByteArray& name, const QString& value)
{
    return static_cast<String*>(addAttribute(name, new String(value)));
}

FwJSON::Number* FwJSON::Object::addNumber(const QByteArray& name, double value)
{
    return static_cast<Number*>(addAttribute(name, new Number(value)));
}

FwJSON::Boolean* FwJSON::Object::addBoolean(const QByteArray& name, bool value)
{
    return static_cast<Boolean*>(addAttribute(name, new Boolean(value)));
}

FwJSON::Object* FwJSON::Object::addObject(const QByteArray& name)
{
    return static_cast<Object*>(addAttribute(name, new Object()));
}

FwJSON::Array* FwJSON::Object::addArray(const QByteArray& name)
{
    return static_cast<Array*>(addAttribute(name, new Array()));
}

///////////////////////////////////////////////////////////////////////////////

int FwJSON::Array::size() const
{
    return m_data.size();
}

int FwJSON::Array::indexOf(FwJSON::Node* item) const
{
    return m_data.indexOf(item);
}

FwJSON::Node* FwJSON::Array::item(int index) const
{
    if(index < m_data.size() && index >= 0)
    {
        return m_data.at(index);
    }
    return 0;
}

QVector<FwJSON::Node*> FwJSON::Array::toQVector() const
{
    return m_data;
}

FwJSON::String* FwJSON::Array::addString(const QString& value)
{
    return static_cast<String*>(addValue(new String(value)));
}

FwJSON::Number* FwJSON::Array::addNumber(double value)
{
    return static_cast<Number*>(addValue(new Number(value)));
}

FwJSON::Boolean* FwJSON::Array::addBoolean(bool value)
{
    return static_cast<Boolean*>(addValue(new Boolean(value)));
}

FwJSON::Object* FwJSON::Array::addObject()
{
    return static_cast<Object*>(addValue(new Object()));
}

FwJSON::Array* FwJSON::Array::addArray()
{
    return static_cast<Array*>(addValue(new Array()));
}

#endif //FIREWORKS_JSON_INL_H
