#pragma once

#include <exception>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <istream>

namespace fwjson
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
class Exception;

enum class Type
{
	Null,
	String,
	Number,
	Bool,
	Object,
	Array
};

template <Type type_id>
class Base;

template <typename T, Type type_id>
class BaseValue;

class Node
{
public:
	friend class Object;
	friend class Array;

	Node() = default;
	virtual ~Node();

	std::string name() const;

	Node* parent() const {
		return m_parent;
	}

	void takeFromParent();

	virtual Type type() const = 0;
	virtual std::int32_t toInt(bool* bOk) const = 0;
	virtual std::uint32_t toUint(bool* bOk) const = 0;
	virtual bool toBool(bool* bOk) const = 0;
	virtual double toNumber(bool* bOk) const = 0;
	virtual std::string toString(bool* bOk) const = 0;
	virtual std::string toJson() const = 0;

	virtual Node* clone() const = 0;

	template <class T>
	T* cast() {
		return type() == T::typeID ? static_cast<T*>(this) : nullptr;
	}

	template <class T>
	static T* cast(Node* node) {
		return node && node->type() == T::typeID ? static_cast<T*>(node) : nullptr;
	}

private:
	Node* m_parent = nullptr;
};

////////////////////////////////////////////////////////////////////////////////

template <Type type_id>
class Base
	: public Node
{
public:
	static constexpr Type typeID = type_id;

	Type type() const final { return typeID; }
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, Type type_id>
class BaseValue
	: public Base<type_id>
{
public:
	using BaseType = T;

	static BaseType defaultValue() {
		return BaseType();
	}

	BaseValue(const BaseType& value)
		: m_value(value)
	{}

	const BaseType& value() const {
		return m_value;
	}

	void setValue(const BaseType& value) {
		m_value = value;
	}

private:
	BaseType m_value;
};

////////////////////////////////////////////////////////////////////////////////

class String final
	: public BaseValue<std::string, Type::String>
{
public:
	explicit String(const std::string& value = defaultValue());

	int toInt(bool* bOk) const final;
	uint toUint(bool* bOk) const final;
	bool toBool(bool* bOk) const final;
	double toNumber(bool* bOk) const final;
	std::string toString(bool* bOk) const final;
	std::string toJson() const;

	Node* clone() const;
};


////////////////////////////////////////////////////////////////////////////////

class Number final
	: public BaseValue<double, Type::Number>
{
public:
	Number(double value = defaultValue());

	int toInt(bool* bOk) const final;
	uint toUint(bool* bOk) const final;
	bool toBool(bool* bOk) const final;
	double toNumber(bool* bOk) const final;
	std::string toString(bool* bOk) const final;
	std::string toJson() const;

	Node* clone() const;
};

/////////////////////////////////////////////////////////////////////////////////

class Boolean final
	: public BaseValue<bool, Type::Bool>
{
public:
	Boolean(bool value = defaultValue());

	std::int32_t toInt(bool* bOk) const final;
	std::uint32_t toUint(bool* bOk) const final;
	bool toBool(bool* bOk) const final;
	double toNumber(bool* bOk) const final;
	std::string toString(bool* bOk) const final;
	std::string toJson() const;

	Node* clone() const;
};

////////////////////////////////////////////////////////////////////////////////

class Object final
	: public Base<Type::Object>
{
public:
	friend class Node;

	Object();
	~Object();

	Node* addAttribute(const std::string& name, Node* value, bool replace = true);

	void removeAttribute(const std::string& name) {
		auto it = m_attributes.find(name);
		if (it != m_attributes.end()) {
			delete it->second;
			m_attributes.erase(it);
		}
	}

	std::size_t attributesCount() const {
		return m_attributes.size();
	}

	String* addString(const std::string& name, const std::string& value);
	Number* addNumber(const std::string& name, double value);
	Boolean* addBoolean(const std::string& name, bool value);
	Object* addObject(const std::string& name);
	Array* addArray(const std::string& name);

	void clear();

	template<class T>
	T* child(const std::string& name) const {
		const auto it = m_attributes.find(name);
		return it != m_attributes.end() ? it->second->cast<T>() : nullptr;
	}

	Node* at(const std::string& name) const {
		const auto it = m_attributes.find(name);
		return it != m_attributes.end() ? it->second : nullptr;
	}

	std::string attributeName(Node* child) const {
		for (const auto&[key, value] : m_attributes) {
			if (value == child) {
				return key;
			}
		}
		return "";
	}

	template<class T>
	bool hasValue(const std::string& name, typename T::BaseType* value = nullptr)
	{
		if (auto* node = child<T>(name)) {
			if (value) {
				(*value) = node->value();
			}
			return true;
		}
		return false;
	}

	template<class T>
	typename T::BaseType value(const std::string& name, const typename T::BaseType& defaultValue) {
		auto* node = child<T>(name);
		return node ? node->value() : defaultValue;
	}

	template<class T>
	T* setValue(const std::string& name, const typename T::BaseType& value) {
		if(auto* node = child<T>(name)) {
			node->setValue(value);
			return node;
		}
		return static_cast<T*>(addAttribute(name, new T(value), true));
	}

	const std::unordered_map<std::string, Node*> attributes() const {
		return m_attributes;
	}

	std::vector<Node*> toVector() const {
		std::vector<Node*> results;
		results.reserve(m_attributes.size());
		for (const auto&[_, value] : m_attributes) {
			results.emplace_back(value);
		}
		return results;
	}

	std::string toJson() const;

	void parse(const std::string& utf8String);
	void parseStream(std::istream& stream);
	void parseFile(const std::string& filename);

	int toInt(bool* bOk) const final;
	uint toUint(bool* bOk) const final;
	bool toBool(bool* bOk) const final;
	double toNumber(bool* bOk) const final;
	std::string toString(bool* bOk) const final;

	Node* clone() const;

private:
	std::unordered_map<std::string, Node*> m_attributes;
};

////////////////////////////////////////////////////////////////////////////////

class Array final : public Base<Type::Array>
{
public:

	friend class Node;

	Array();
	~Array();

	void clear();

	Node* addValue(Node* node);

	inline String* addString(const std::string& value);
	inline Number* addNumber(double value);
	inline Boolean* addBoolean(bool value);
	inline Object* addObject();
	inline Array* addArray();

	size_t size() const {
		return m_data.size();
	}

	int32_t indexOf(Node* item) const {
		for (int32_t index = 0; const auto& it : m_data) {
			if(it == item) {
				return index;
			}
			++index;
		}
		return -1;
	}

	template<class T>
	T* child(size_t index) const {
		return index < m_data.size() ? m_data.at(index)->cast<T>() : nullptr;
	}

	Node* at(size_t index) const {
		return index < m_data.size() ? m_data.at(index) : nullptr;
	}

	std::string toJson() const;

	int toInt(bool* bOk) const final;
	uint toUint(bool* bOk) const final;
	bool toBool(bool* bOk) const final;
	double toNumber(bool* bOk) const final;
	std::string toString(bool* bOk) const final;

	Node* clone() const;

	const std::vector<Node*>& toVector() const {
		return m_data;
	}

private:
	std::vector<Node*> m_data;
};

////////////////////////////////////////////////////////////////////////////////

class Exception final : public std::exception
{
public:
	Exception(
		const std::string& error = std::string(),
		const std::string& fileName = std::string(),
		int line = -1,
		int  column = -1) throw();

	Exception(
		const std::string& error,
		int line,
		int  column) throw();

	Exception(
		char c,
		int line,
		int  column) throw();

	virtual ~Exception() throw();

	virtual const char* what() const throw();

	inline std::string error() const;

protected:
	std::string m_error;
};

std::string Exception::error() const
{
	return m_error;
}

} // namespace fwjson
