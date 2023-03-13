#include <array>
#include <cassert>
#include <exception>
#include <sstream>
#include <string>
#include <climits>

#include <fwjson/fwjson.h>
#include <fwjson/strings.h>

namespace fwjson {

namespace {

struct ParseData;

enum CharType
{
	C_AZ,     //Alpha (A..Z, a..z, _)
	C_Ee,     //Char 'E' and 'e'
	C_Uni,    //Unicode symbol

	C_Num,    //Numbers (0..9)
	C_Fra,    //Decimal point (.)
	C_Sig,    //Char '+' and '-'
	C_Ast,    //Asterisk(*)

	C_Sp,     //Space (' ')

	C_Str,    //Quotation mark (")
	C_Bsl,    //Backslash (\)
	C_Slh,    //Slash (/)

	//These are the six structural characters
	C_Col,    //Name separator, colon (:)
	C_LCu,    //Begin-object, left curly bracket ({)
	C_RCu,    //End-object, right curly bracket (})
	C_LSq,    //Left square bracket ([)
	C_RSq,    //Right square bracket (])
	C_Sep,    //Items separator: Comma (,) or (;)

	//Error
	C_Err,    //Unknow

	C_MAX = C_Err + 1
};

//Types of ASCII chars
const CharType chars_type[128] = {

          /*0 */ /*1 */ /*2 */ /*3 */ /*4 */ /*5 */ /*6 */ /*7 */
/*  0 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
/*  8 */  C_Err, C_Sp,  C_Sp,  C_Err, C_Err, C_Err, C_Err, C_Err,
/* 16 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
/* 24 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,

/* 32 */  C_Sp,  C_Uni, C_Str, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
/* 40 */  C_Uni, C_Uni, C_Ast, C_Sig, C_Sep, C_Sig, C_Fra, C_Slh,
/* 48 */  C_Num, C_Num, C_Num, C_Num, C_Num, C_Num, C_Num, C_Num,
/* 56 */  C_Num, C_Num, C_Col, C_Sep, C_Uni, C_Uni, C_Uni, C_Uni,

/* 64 */  C_Uni, C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_Ee,  C_AZ,  C_AZ,
/* 72 */  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
/* 80 */  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
/* 88 */  C_AZ,  C_AZ,  C_AZ,  C_LSq, C_Bsl, C_RSq, C_Uni, C_AZ,

/* 96 */  C_Uni, C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_Ee,  C_AZ,  C_AZ,
/* 104*/  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
/* 112*/  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
/* 120*/  C_AZ,  C_AZ,  C_AZ,  C_LCu, C_Uni, C_RCu, C_Uni, C_Err,

};

typedef void(*CommandFunc)(const CharIterator& c, ParseData* data);

void x_doc(const CharIterator& c, ParseData* data);
void x_var(const CharIterator& c, ParseData* data);
void x_bst(const CharIterator& c, ParseData* data);
void x_est(const CharIterator& c, ParseData* data);
void x_bsc(const CharIterator& c, ParseData* data);
void x_esc(const CharIterator& c, ParseData* data);
void x_atr(const CharIterator& c, ParseData* data);
void x_int(const CharIterator& c, ParseData* data);
void x_re1(const CharIterator& c, ParseData* data);
void x_re2(const CharIterator& c, ParseData* data);
void x_enu(const CharIterator& c, ParseData* data);
void x_sg1(const CharIterator& c, ParseData* data);
void x_rn3(const CharIterator& c, ParseData* data);
void x_err(const CharIterator& c, ParseData* data);
void x_ob1(const CharIterator& c, ParseData* data);
void x_ob2(const CharIterator& c, ParseData* data);
void x_eob(const CharIterator& c, ParseData* data);
void x_val(const CharIterator& c, ParseData* data);
void x_ign(const CharIterator& c, ParseData* data);
void x_ar1(const CharIterator& c, ParseData* data);
void x_ar2(const CharIterator& c, ParseData* data);
void x_ear(const CharIterator& c, ParseData* data);
void x_com(const CharIterator& c, ParseData* data);

/*void x_val(ParseData* data);

void x_est(ParseData* data);
void x_rnu(ParseData* data);
void x_eob(ParseData* data);*/

enum
{
	X_DOC,
	X_VAR,
	X_STR,
	X_SCH,
	X_VAL,
	X_INT,
	X_RE1,
	X_RE2,
	X_RE3,
	X_ATR,
	X_SEO,
	X_SEA,
	X_EAT,
	X_SLC,
	X_MLC,

	X_MAX
};

//Parse command or parse state
const CommandFunc parse_commands[X_MAX][C_MAX] = {
/*          C_AZ,   C_Ee,   C_Uni,  C_Num,  C_Fra,  C_Sig,  C_Ast,  C_Sp,   C_Str,  C_Bsl,  C_Slh,  C_Col,            C_LCu,  C_RCu,  C_LSq,  C_RSq,  C_Sep,  C_Err */
/*X_DOC*/{  &x_var, &x_var, &x_err, &x_err, &x_err, &x_err, &x_com, &x_ign, &x_bst, &x_err, &x_com, &x_err, /*X_DOC*/ &x_doc, &x_err, &x_err, &x_err, &x_err, &x_err  },
/*X_VAR*/{  0,      0,      &x_err, 0,      &x_err, &x_err, 0,      &x_est, &x_err, &x_err, 0,      &x_atr, /*X_VAR*/ &x_ob2, &x_eob, &x_ar2, &x_ear, &x_val, &x_err  },
/*X_STR*/{  0,      0,      0,      0,      0,      0,      0,      0,      &x_est, &x_bsc, 0,      0,      /*X_STR*/ 0,      0,      0,      0,      0,      &x_err  },
/*X_SCH*/{  &x_esc, &x_err, &x_err, &x_err, &x_err, &x_err, 0,      &x_err, &x_esc, &x_esc, 0,      &x_err, /*X_STR*/ &x_err, &x_err, &x_err, &x_err, &x_err, &x_err  },
/*X_VAL*/{  &x_var, &x_var, &x_err, &x_int, &x_err, &x_sg1, &x_com, &x_ign, &x_bst, &x_err, &x_com, &x_err, /*X_VAL*/ &x_ob1, &x_err, &x_ar1, &x_ear, &x_val, &x_err  },
/*X_INT*/{  &x_err, &x_re2, &x_err, 0,      &x_re1, &x_err, 0,      &x_enu, &x_err, &x_err, 0,      &x_err, /*X_INT*/ &x_err, &x_eob, &x_err, &x_ear, &x_val, &x_err  },
/*X_RE1*/{  &x_err, &x_re2, &x_err, 0,      &x_err, &x_err, 0,      &x_enu, &x_err, &x_err, 0,      &x_err, /*X_RE1*/ &x_err, &x_eob, &x_err, &x_ear, &x_val, &x_err  },
/*X_RE2*/{  &x_err, &x_err, &x_err, &x_rn3, &x_err, &x_rn3, 0,      &x_err, &x_err, &x_err, 0,      &x_err, /*X_RE2*/ &x_err, &x_err, &x_err, &x_err, &x_err, &x_err  },
/*X_RE3*/{  &x_err, &x_err, &x_err, 0,      &x_err, &x_err, 0,      &x_enu, &x_err, &x_err, 0,      &x_err, /*X_RE3*/ &x_err, &x_eob, &x_err, &x_ear, &x_val, &x_err  },
/*X_ATR*/{  &x_var, &x_var, &x_err, &x_err, &x_err, &x_err, &x_com, &x_ign, &x_bst, &x_err, &x_com, &x_err, /*X_ATR*/ &x_err, &x_eob, &x_err, &x_err, &x_err, &x_err  },
/*X_SEO*/{  &x_err, &x_err, &x_err, &x_err, &x_err, &x_err, &x_com, &x_ign, &x_err, &x_err, &x_com, &x_err, /*X_SEO*/ &x_err, &x_eob, &x_err, &x_err, &x_val, &x_err  },
/*X_SEA*/{  &x_err, &x_err, &x_err, &x_err, &x_err, &x_err, 0,      &x_ign, &x_err, &x_err, 0,      &x_err, /*X_SEA*/ &x_err, &x_err, &x_err, &x_ear, &x_val, &x_err  },
/*X_EAT*/{  &x_err, &x_err, &x_err, &x_err, &x_err, &x_err, 0,      &x_ign, &x_err, &x_err, 0,      &x_atr, /*X_EAT*/ &x_ob2, &x_err, &x_ar2, &x_err, &x_err, &x_err  },
/*X_SLC*/{  &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, /*X_EAT*/ &x_com, &x_com, &x_com, &x_com, &x_com, &x_err  },
/*X_SLC*/{  &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, &x_com, /*X_EAT*/ &x_com, &x_com, &x_com, &x_com, &x_com, &x_err  },
};

struct ParseData
{
	void setupAttributeName();
	void structureUp(const CharIterator& c);
	void setupValue(const CharIterator& c);
	void setupAttributeValue(const CharIterator& c);
	void setupArrayValue(const CharIterator& c);

	Node* parent = nullptr;
	std::string attribute;
	std::string buffer;
	bool isVariable = false;
	int xcmd = X_DOC;
	int xcmdRestore = 0;
	Type type = Type::Null;
};

void ParseData::setupAttributeName()
{
	attribute = buffer;
	buffer.clear();
}

void ParseData::structureUp(const CharIterator& c)
{
	setupValue(c);
	parent = parent->parent();
	if(parent)
	{
		switch(parent->type())
		{
		case Type::Array:
			xcmd = X_SEA;
			return;

		case Type::Object:
			xcmd = X_SEO;
			return;

		default:
			assert(false);
			break;
		}
	}
	else
	{
		xcmd = X_SEO;
	}
}

void ParseData::setupValue(const CharIterator& c)
{
	switch(parent->type())
	{
	case Type::Array:
		setupArrayValue(c);
		return;

	case Type::Object:
		setupAttributeValue(c);
		return;

	default:
		assert(false);
		break;
	}
}

void ParseData::setupAttributeValue(const CharIterator& c)
{
	switch(type)
	{
	case Type::String:
		{
			if(isVariable)
			{
				bool bOk = false;
				bool value = strings::toBool(buffer, &bOk);
				if(bOk)
				{
					static_cast<Object*>(parent)->addBoolean(attribute, value);
				}
				else
				{
					static_cast<Object*>(parent)->addString(attribute, buffer);
				}
			}
			else
			{
				static_cast<Object*>(parent)->addString(attribute, buffer);
			}
			buffer = std::string();
		}
		break;

	case Type::Number:
		{
			size_t index = 0;
			double value = std::stod(buffer, &index);
			if (buffer.size() != index) {
				throw Exception("Invalid number value", c.currentPosition().line, c.currentPosition().column);
			}
			static_cast<Object*>(parent)->addNumber(attribute, value);
			buffer = std::string();
		}
		break;

	case Type::Array:
		parent = static_cast<Object*>(parent)->addArray(attribute);
		break;

	case Type::Object:
		parent = static_cast<Object*>(parent)->addObject(attribute);
		break;

	case Type::Null:
		break;

	default:
		assert(false);
		return;
	}
	attribute = "";
	type = Type::Null;
}

void ParseData::setupArrayValue(const CharIterator& c)
{
	switch(type)
	{
	case Type::String:
		{
			if(isVariable)
			{
				bool bOk = false;
				bool value = strings::toBool(buffer, &bOk);
				if(bOk) {
					static_cast<Array*>(parent)->addBoolean(value);
				} else {
					static_cast<Array*>(parent)->addString(buffer);
				}
			}
			else
			{
				static_cast<Array*>(parent)->addString(buffer);
			}
			buffer = std::string();
		}
		break;

	case Type::Number:
		{
			size_t index = 0;
			double value = std::stod(buffer, &index);
			if(buffer.size() != index) {
				throw Exception("Invalid number value", c.currentPosition().line, c.currentPosition().column);
			}
			static_cast<Array*>(parent)->addNumber(value);
			buffer = std::string();
		}
		break;

	case Type::Array:
		parent = static_cast<Array*>(parent)->addArray();
		break;

	case Type::Object:
		parent = static_cast<Array*>(parent)->addObject();
		break;

	case Type::Null:
		break;

	default:
		assert(false);
		return;
	}
	type = Type::Null;
}

void x_doc(const CharIterator& c, ParseData* data)
{
	data->xcmd = X_ATR;
}

void x_var(const CharIterator& c, ParseData* data)
{
	if (data->parent) {
		data->type = Type::String;
		data->xcmd = X_VAR;
		data->isVariable = true;
		data->buffer += c->value;
		return;
	}
	throw Exception(c);
}

void x_bst(const CharIterator& c, ParseData* data)
{
	if (data->parent) {
		data->type = Type::String;
		data->xcmd = X_STR;
		data->isVariable = false;
		return;
	}
	throw Exception(c);
}

void x_est(const CharIterator&, ParseData* data)
{
	switch(data->parent->type())
	{
	case Type::Array:
		data->xcmd = X_SEA;
		return;

	case Type::Object:
		data->xcmd = data->attribute.empty() ? X_EAT : X_SEO;
		return;

	default:
		assert(false);
		return;
	}
}

void x_bsc(const CharIterator&, ParseData* data)
{
	data->buffer += '\\';
	data->xcmd = X_SCH;
}

void x_esc(const CharIterator& c, ParseData* data)
{
	switch(c->value)
	{
	case '"':
	case '\\':
	case '/':
	case 'b':
	case 'f':
	case 'n':
	case 'r':
	case 't':
	case 'u':
		data->buffer += c->value;
		data->xcmd = X_STR;
		break;

	default:
		throw Exception(c);
	}
}

void x_atr(const CharIterator& c, ParseData* data)
{
	if (data->parent->type() == Type::Object) {
		data->setupAttributeName();
		data->xcmd = X_VAL;
		return;
	}
	throw Exception(c);
}

void x_int(const CharIterator& c, ParseData* data)
{
	data->xcmd = X_INT;
	data->type = Type::Number;
	data->buffer += c->value;
}

void x_re1(const CharIterator& c, ParseData* data)
{
	data->xcmd = X_RE1;
	data->type = Type::Number;
	data->buffer += c->value;
}

void x_re2(const CharIterator& c, ParseData* data)
{
	data->xcmd = X_RE2;
	data->type = Type::Number;
	data->buffer += c->value;
}

void x_enu(const CharIterator&, ParseData* data)
{
	switch(data->parent->type())
	{
	case Type::Array:
		data->xcmd = X_SEA;
		return;

	case Type::Object:
		data->xcmd = X_SEO;
		return;

	default:
		assert(false);
		return;
	}
}

void x_sg1(const CharIterator& c, ParseData* data)
{
	if(c == '+')
	{
		data->xcmd = X_INT;
		data->type = Type::Number;
	}
	else if(c == '-')
	{
		data->xcmd = X_INT;
		data->type = Type::Number;
		data->buffer += c->value;
	}
}

void x_rn3(const CharIterator& c, ParseData* data)
{
	data->xcmd = X_RE3;
	data->buffer += c->value;
}

void x_ob1(const CharIterator& c, ParseData* data)
{
	data->type = Type::Object;
	data->setupValue(c);
	data->xcmd = X_ATR;
}

void x_ob2(const CharIterator& c, ParseData* data)
{
	data->setupAttributeName();
	x_ob1(c,data);
}

void x_eob(const CharIterator& c, ParseData* data)
{
	if (data->parent->type() == Type::Object) {
		data->structureUp(c);
		return;
	}
	throw Exception(c);
}

void x_ar1(const CharIterator& c, ParseData* data)
{
	data->type = Type::Array;
	data->setupValue(c);
	data->xcmd = X_VAL;
}

void x_ar2(const CharIterator& c, ParseData* data)
{
	data->setupAttributeName();
	x_ar1(c, data);
}

void x_ear(const CharIterator& c, ParseData* data)
{
	if (data->parent->type() == Type::Array) {
		data->structureUp(c);
		return;
	}
	throw Exception(c);
}

void x_err(const CharIterator& c, ParseData*)
{
	throw Exception(c);
}

void x_val(const CharIterator& c, ParseData* data)
{
	if (data->parent) {
		data->setupValue(c);
		switch(data->parent->type())
		{
		case Type::Array:
			data->xcmd = X_VAL;
			break;

		case Type::Object:
			data->xcmd = X_ATR;
			break;

		default:
			assert(false);
			break;
		}
		return;
	}
	throw Exception(c);
}

void x_com(const CharIterator& c, ParseData* data)
{
	if (data->buffer.ends_with("/")) {
		if (c == '/' && data->xcmd != X_SLC) {
			data->buffer += c->value;
			data->xcmdRestore = data->xcmd;
			data->xcmd = X_SLC;
			return;
		}
		if (c == '*' && data->xcmd != X_MLC) {
			data->buffer += c->value;
			data->xcmdRestore = data->xcmd;
			data->xcmd = X_MLC;
			return;
		}
	}

	if (data->xcmd == X_SLC) {
		if (c == '\n') {
			data->xcmd = data->xcmdRestore;
			data->xcmdRestore = 0;
			data->buffer.clear();
			return;
		}
	} else if (data->xcmd == X_MLC) {
		if (c == '/' && data->buffer.ends_with('*')) {
			data->xcmd = data->xcmdRestore;
			data->xcmdRestore = 0;
			data->buffer.clear();
			return;
		}
	}

	data->buffer += c->value;
}

void x_ign(const CharIterator&, ParseData*)
{}

} // namespace

Node::~Node()
{
	takeFromParent();
}

std::string Node::name() const
{
	if (m_parent) {
		switch(m_parent->type())
		{
		case Type::Object:
			return cast<Object>(m_parent)->attributeName(const_cast<Node*>(this));

		case Type::Array:
			return "[" + std::to_string(cast<Array>(m_parent)->indexOf(const_cast<Node*>(this))) + "]";

		default:
			assert(false);
			break;
		}
	} else if(type() == Type::Object || type() == Type::Array) {
		return "root";
	}
	return "";
}

void Node::takeFromParent()
{
	Node* parent = nullptr;
	std::swap(parent, m_parent);
	if (parent) {
		switch (parent->type()) {
		case Type::Object:
			{
				auto* object = static_cast<Object*>(parent);
				auto it = object->m_attributes.begin();
				while (it != object->m_attributes.end()) {
					if (it->second == this) {
						object->m_attributes.erase(it);
						return;
					}
					++it;
				}
			}
			break;

		case Type::Array:
			{
				auto* array = static_cast<Array*>(parent);
				auto it = array->m_data.begin();
				while (it != array->m_data.end()) {
					if (*it == this) {
						array->m_data.erase(it);
						return;
					}
					++it;
				}
			}
			break;

		default:
			assert(false);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

String::String(const std::string& value)
	: BaseValue<std::string, Type::String>(value)
{}

std::string String::toJson() const
{
	return "\"" + value() + "\"";
}

int String::toInt(bool* bOk) const
{
    return strings::toInt32(value(), bOk);
}

std::uint32_t String::toUint(bool* bOk) const
{
    return strings::toUint32(value(), bOk);
}

bool String::toBool(bool* bOk) const
{
	return strings::toBool(value(), bOk);
}

double String::toNumber(bool* bOk) const
{
    return strings::toDouble(value(), bOk);
}

std::string String::toString(bool* bOk) const
{
	// std::string value = this->value();

	// std::string out;
	// out.reserve(value.size());

	// std::string::ConstIterator iter = value.begin();
	// std::string::ConstIterator end = value.end();
	// try {
	//	 while(iter != end) {
	//		 if(*iter != QChar('\\')) {
	//			 out.append(*iter);
	//			 ++iter;
	//			 continue;
	//		 }

	//		 if((++iter) == end) {
	//			 throw Exception();
	//		 }

	//		 switch(iter->unicode())
	//		 {
	//		 case '"':
	//		 case '\\':
	//		 case '/':
	//		 case 'b':
	//		 case 'f':
	//		 case 'n':
	//		 case 'r':
	//		 case 't':
	//			 out.append(QChar(iter->unicode()));
	//			 break;

	//		 case 'u':
	//			 if((end - iter) >= 5)
	//			 {
	//				 bool ok = false;
	//				 short uch = std::string(iter+1, 4).toShort(&ok, 16);
	//				 if(!ok)
	//				 {
	//					 throw Exception();
	//				 }

	//				 out.append(QChar(uch));

	//				 iter += 5;
	//				 continue;
	//			 }
	//			 break;

	//		 default:
	//			 throw Exception();
	//		 }

	//		 ++iter;
	//	 }
	// }
	// catch(const Exception& e)
	// {
	//	 (*bOk) = false;
	//	 return std::string();
	// }

	// (*bOk) = true;
	// return out;
	return value();
}

Node* String::clone() const
{
	return new String(value());
}

////////////////////////////////////////////////////////////////////////////////

Number::Number(double value)
	: BaseValue<double, Type::Number>(value)
{}

std::string Number::toJson() const
{
	return std::to_string(value());
}

int Number::toInt(bool* bOk) const
{
	if(std::abs(value()) > 0. && (std::abs(value()) - INT_MAX) < 0.)
	{
		if(bOk) { (*bOk) = true; }
		return static_cast<int>(value());
	}
	if(bOk) { (*bOk) = true; }
	return 0;
}

std::uint32_t Number::toUint(bool* bOk) const
{
	if(value() > 0. && ((value() - UINT_MAX) < 0.))
	{
		if(bOk) { (*bOk) = true; }
		return static_cast<std::uint32_t>(value());
	}

	if(bOk) { (*bOk) = false; }
	return 0;
}

bool Number::toBool(bool* bOk) const
{
	if(bOk) { (*bOk) = true; }
	return value() != 0;
}

double Number::toNumber(bool* bOk) const
{
	if(bOk) { (*bOk) = true; }
	return value();
}

std::string Number::toString(bool* bOk) const
{
	if(bOk) { (*bOk) = true; }
	return std::to_string(value());
}

Node* Number::clone() const
{
	return new Number(value());
}

///////////////////////////////////////////////////////////////////////////////

Boolean::Boolean(bool value)
	: BaseValue<bool, Type::Bool>(value)
{}

std::string Boolean::toJson() const
{
	return strings::fromBool(value());
}

int Boolean::toInt(bool* bOk) const
{
	(*bOk) = true;
	return value();
}

std::uint32_t Boolean::toUint(bool* bOk) const
{
	(*bOk) = true;
	return value();
}

bool Boolean::toBool(bool* bOk) const
{
	(*bOk) = true;
	return value();
}

double Boolean::toNumber(bool* bOk) const
{
	(*bOk) = true;
	return value();
}

std::string Boolean::toString(bool* bOk) const
{
	(*bOk) = true;
	return strings::fromBool(value());
}

Node* Boolean::clone() const
{
	return new Boolean(value());
}

////////////////////////////////////////////////////////////////////////////////

Object::Object()
	: Base<Type::Object>()
{}

Object::~Object()
{
	clear();
}

String* Object::addString(const std::string& name, const std::string& value)
{
	return static_cast<String*>(addAttribute(name, new String(value)));
}

Number* Object::addNumber(const std::string& name, double value)
{
	return static_cast<Number*>(addAttribute(name, new Number(value)));
}

Boolean* Object::addBoolean(const std::string& name, bool value)
{
	return static_cast<Boolean*>(addAttribute(name, new Boolean(value)));
}

Object* Object::addObject(const std::string& name)
{
	return static_cast<Object*>(addAttribute(name, new Object()));
}

Array* Object::addArray(const std::string& name)
{
	return static_cast<Array*>(addAttribute(name, new Array()));
}

void Object::clear()
{
	for (const auto& [_, node] : m_attributes) {
		assert(node->m_parent == this);
		node->m_parent = nullptr;
		delete node;
	}
	m_attributes.clear();
}

Node* Object::addAttribute(const std::string& name, Node* value, bool replace)
{
	if (!value) {
		return nullptr;
	}

	if (value->m_parent)
	{
		if  (value->m_parent == this) {
			return value;
		}
		value->takeFromParent();
	}

	auto it = m_attributes.find(name);
	if (it != m_attributes.end()) {
		if (replace) {
			delete it->second;
			value->m_parent = this;
			it->second = value;
			return value;
		}
		if (auto* addArray = cast<Array>(it->second)) {
		   return addArray->addValue(value);
		}

		auto* current = it->second;
		current->m_parent = nullptr;

		auto* newArray = new Array();
		it->second = newArray;

		newArray->addValue(current);
		return newArray->addValue(value);
	}

	value->m_parent = this;
	m_attributes.emplace(name, value);
	return value;
}

std::string Object::toJson() const
{
	std::string attributes;
	for (const auto& it : m_attributes) {
		std::string value = it.second->toJson();
		if(!value.empty()) {
			if(!attributes.empty()) {
			   attributes += ",";
			}
			attributes += ("\"" + it.first + "\"");
			attributes += ":";
			attributes += value;
		}
	}
	return "{" + attributes + "}";
}

void Object::parse(const std::string& utf8String)
{
	if (utf8String.empty()) {
		throw Exception("Input string is empty");
	}
	std::istringstream stream(utf8String);
	parseStream(stream);
}

void Object::parseStream(std::istream& stream)
{
	try
	{
		ParseData data;
		data.parent = this;

		CharIterator it(stream);
		while (it) {
			CharType charType = it->value > 0 ? chars_type[it->value] : C_Uni;
			if (CommandFunc cmd = parse_commands[data.xcmd][charType]) {
				cmd(it, &data);
			} else {
				data.buffer += it->value;
			}
			++it;
		}
		if(!data.buffer.empty()) {
			data.setupValue(it);
		}
	}
	catch(const Exception& e)
	{
		clear();
		throw e;
	}
}

void Object::parseFile(const std::string& fileName)
{
	// QFile file(QDir::toNativeSeparators(fileName));
	// if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	// {
	//	 throw Exception(file);
	// }
	// parse(&file);
}

int Object::toInt(bool* bOk) const
{
	(*bOk) = false;
	return 0;
}

std::uint32_t Object::toUint(bool* bOk) const
{
	(*bOk) = false;
	return 0;
}

bool Object::toBool(bool* bOk) const
{
	(*bOk) = false;
	return false;
}

double Object::toNumber(bool* bOk) const
{
	(*bOk) = false;
	return 0.;
}

std::string Object::toString(bool* bOk) const
{
	(*bOk) = false;
	return std::string();
}

Node* Object::clone() const
{
	Object* newObject = new Object();
	for (const auto& [key, value] : m_attributes) {
		auto* child = value->clone();
		child->m_parent = newObject;
		newObject->m_attributes.emplace(key, child);
	}
	return newObject;
}

////////////////////////////////////////////////////////////////////////////////

Array::Array()
	: Base<Type::Array>()
{}

Array::~Array()
{
	clear();
}

void Array::clear()
{
	for(auto& node : m_data) {
		assert(node->m_parent == this);
		node->m_parent = nullptr;
		delete node;
	}
	m_data.clear();
}

std::string Array::toJson() const
{
	std::string items;
	for (auto& node : m_data) {
		if(!items.empty()) {
			items += ",";
		}
		items += node->toJson();
	}
	if(items.empty()) {
		return std::string();
	}
	return "[" + items + "]";
}

int Array::toInt(bool* bOk) const
{
	if(size() == 1)
	{
		return m_data.at(0)->toInt(bOk);
	}
	(*bOk) = false;
	return 0;
}

std::uint32_t Array::toUint(bool* bOk) const
{
	if(size() == 1)
	{
		return m_data.at(0)->toUint(bOk);
	}
	(*bOk) = false;
	return 0;
}

bool Array::toBool(bool* bOk) const
{
	if(size() == 1)
	{
		return m_data.at(0)->toBool(bOk);
	}
	(*bOk) = false;
	return false;
}

double Array::toNumber(bool* bOk) const
{
	if(size() == 1)
	{
		return m_data.at(0)->toNumber(bOk);
	}
	(*bOk) = false;
	return 0.;
}

std::string Array::toString(bool* bOk) const
{
	if(size() == 1)
	{
		return m_data.at(0)->toString(bOk);
	}
	(*bOk) = false;
	return std::string();
}

Node* Array::clone() const
{
	Array* newArray = new Array();
	newArray->m_data.reserve(m_data.size());
	for(auto& node : m_data) {
		newArray->addValue(node->clone());
	}
	return newArray;
}

Node* Array::addValue(Node* node)
{
	if(node->m_parent) {
		if(node->m_parent == this) {
			return node;
		}
		node->takeFromParent();
	}
	node->m_parent = this;
	m_data.emplace_back(node);
	return node;
}

String* Array::addString(const std::string& value)
{
	return static_cast<String*>(addValue(new String(value)));
}

Number* Array::addNumber(double value)
{
	return static_cast<Number*>(addValue(new Number(value)));
}

Boolean* Array::addBoolean(bool value)
{
	return static_cast<Boolean*>(addValue(new Boolean(value)));
}

Object* Array::addObject()
{
	return static_cast<Object*>(addValue(new Object()));
}

Array* Array::addArray()
{
	return static_cast<Array*>(addValue(new Array()));
}

////////////////////////////////////////////////////////////////////////////////

Exception::Exception(
	const std::string& error,
	const std::string& fileName,
	int line,
	int  column) throw()
	: std::exception()
{
	if(fileName.empty())
	{
		m_error = error;
	} else {
		std::string tmp = fileName;
		if(line != -1)
		{
			tmp += " (" + std::to_string(line);
			if(column != -1)
			{
				tmp += ", " + std::to_string(column);
			}
			tmp += ")";
		}
		m_error = tmp + ": " + error;
	}

	if(m_error.empty() && error.empty())
	{
		m_error = "Unknow error";
	}
}

Exception::Exception(
	const std::string& error,
	int line,
	int  column) throw()
	: std::exception()
{
	if(line != -1)
	{
		std::string tmp = " (" + std::to_string(line);
		if(column != -1)
		{
			tmp += ", " + std::to_string(column);
		}
		m_error = tmp + "): " + error;
	}
	else
	{
		m_error = error;
	}
}

Exception::Exception(const CharIterator& c) throw()
	: std::exception()
{
	m_error = " (" +
			  std::to_string(c.currentPosition().line) +
			  ", " +
			  std::to_string(c.currentPosition().column) +
			  "): Unexcepted char '" +
			  c->value +
			  "'";
}

Exception::~Exception() throw()
{}

const char* Exception::what() const throw()
{
	return m_error.c_str();
}

} // namespace fwjson
