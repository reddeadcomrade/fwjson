#include <QtCore/qbuffer.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>

#include "fwjson.h"

//Parse utils
namespace
{
    struct ParseData;

    enum CharType
    {
        C_AZ,      //Alpha (A..Z, a..z, _)
        C_Ee,      //Char 'E' and 'e'
        C_Uni,     //Unicode symbol

        C_Num,     //Numbers (0..9)
        C_Fra,     //Decimal point (.)
        C_Sig,     //Char '+' and '-'

        C_Sp,      //Space (' ')

        C_Str,     //Quotation mark (")
        C_Bsl,     //Backslash (\)

        //These are the six structural characters
        C_Col,     //Name separator, colon (:)
        C_LCu,     //Begin-object, left curly bracket ({)
        C_RCu,     //End-object, right curly bracket (})
        C_LSq,     //Left square bracket ([)
        C_RSq,     //Right square bracket (])
        C_Sep,     //Items separator: Comma (,) or (;)

        //Error
        C_Err,     //Unknow

        C_MAX = C_Err + 1
    };

    //Types of ASCII chars
    const CharType chars_type[128] = {

          /*0 */ /*1 */ /*2 */ /*3 */ /*4 */ /*5 */ /*6 */ /*7 */
/*  0 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
/*  8 */  C_Err, C_Sp,  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
/* 16 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
/* 24 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,

/* 32 */  C_Sp,  C_Uni, C_Str, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
/* 40 */  C_Uni, C_Uni, C_Uni, C_Sig, C_Sep, C_Sig, C_Fra, C_Uni,
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

    typedef void(*CommandFunc)(char c, ParseData* data);

    void x_doc(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_var(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_bst(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_est(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_bsc(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_esc(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_atr(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_int(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_re1(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_re2(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_enu(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_sg1(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_rn3(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_rs3(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_err(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_ob1(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_ob2(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_eob(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_val(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_ign(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_ar1(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_ar2(char c, ParseData* data) throw(const FwJSON::Exception&);
    void x_ear(char c, ParseData* data) throw(const FwJSON::Exception&);
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

        X_MAX
    };

    //Parse command or parse state
    const CommandFunc parse_commands[X_MAX][C_MAX] = {
/*            C_AZ,   C_Ee,  C_Uni,  C_Num,  C_Fra, C_Sig,    C_Sp,  C_Str,  C_Bsl,  C_Col,            C_LCu,  C_RCu,  C_LSq,  C_RSq,  C_Sep,  C_Err */
/*X_DOC*/{  &x_var, &x_var, &x_err, &x_err, &x_err, &x_err, &x_ign, &x_bst, &x_err, &x_err, /*X_DOC*/ &x_doc, &x_err, &x_err, &x_err, &x_err, &x_err  },
/*X_VAR*/{       0,      0, &x_err,      0, &x_err, &x_err, &x_est, &x_err, &x_err, &x_atr, /*X_VAR*/ &x_ob2, &x_eob, &x_ar2, &x_ear, &x_val, &x_err  },
/*X_STR*/{       0,      0,      0,      0,      0,      0,      0, &x_est, &x_bsc,      0, /*X_STR*/      0,      0,      0,      0,      0, &x_err  },
/*X_SCH*/{  &x_esc, &x_err, &x_err, &x_err, &x_err, &x_err, &x_err, &x_esc, &x_esc, &x_err, /*X_STR*/ &x_err, &x_err, &x_err, &x_err, &x_err, &x_err  },
/*X_VAL*/{  &x_var, &x_var, &x_err, &x_int, &x_err, &x_sg1, &x_ign, &x_bst, &x_err, &x_err, /*X_VAL*/ &x_ob1, &x_err, &x_ar1, &x_ear, &x_val, &x_err  },
/*X_INT*/{  &x_err, &x_re2, &x_err,      0, &x_re1, &x_err, &x_enu, &x_err, &x_err, &x_err, /*X_INT*/ &x_err, &x_eob, &x_err, &x_ear, &x_val, &x_err  },
/*X_RE1*/{  &x_err, &x_re2, &x_err,      0, &x_err, &x_err, &x_enu, &x_err, &x_err, &x_err, /*X_RE1*/ &x_err, &x_eob, &x_err, &x_ear, &x_val, &x_err  },
/*X_RE2*/{  &x_err, &x_err, &x_err, &x_rn3, &x_err, &x_rn3, &x_err, &x_err, &x_err, &x_err, /*X_RE2*/ &x_err, &x_err, &x_err, &x_err, &x_err, &x_err  },
/*X_RE3*/{  &x_err, &x_err, &x_err,      0, &x_err, &x_err, &x_enu, &x_err, &x_err, &x_err, /*X_RE3*/ &x_err, &x_eob, &x_err, &x_ear, &x_val, &x_err  },
/*X_ATR*/{  &x_var, &x_var, &x_err, &x_err, &x_err, &x_err, &x_ign, &x_bst, &x_err, &x_err, /*X_ATR*/ &x_err, &x_eob, &x_err, &x_err, &x_err, &x_err  },
/*X_SEO*/{  &x_err, &x_err, &x_err, &x_err, &x_err, &x_err, &x_ign, &x_err, &x_err, &x_err, /*X_SEO*/ &x_err, &x_eob, &x_err, &x_err, &x_val, &x_err  },
/*X_SEA*/{  &x_err, &x_err, &x_err, &x_err, &x_err, &x_err, &x_ign, &x_err, &x_err, &x_err, /*X_SEA*/ &x_err, &x_err, &x_err, &x_ear, &x_val, &x_err  },
/*X_EAT*/{  &x_err, &x_err, &x_err, &x_err, &x_err, &x_err, &x_ign, &x_err, &x_err, &x_atr, /*X_EAT*/ &x_ob2, &x_err, &x_ar2, &x_err, &x_err, &x_err  },
    };

    struct ParseData
    {
        ParseData();

        inline void setupAttributeName();
        inline void structureUp();
        void setupValue();
        inline void setupAttributeValue();
        inline void setupArrayValue();

        FwJSON::Node* parent;
        QByteArray attribute;
        bool specialChar;
        QByteArray buffer;
        bool isVariable;
        int xcmd;
        int line;
        int column;
        quint32 uintNumber;
        bool declareRoot;
        FwJSON::NodeType type;
    };

    ParseData::ParseData() :
        parent(0),
        specialChar(false),
        isVariable(false),
        xcmd(X_DOC),
        line(0),
        column(0),
        uintNumber(0),
        declareRoot(false),
        type(FwJSON::T_Null)
    {
    }

    void ParseData::setupAttributeName()
    {
        attribute = buffer;
        buffer = QByteArray();
    }

    void ParseData::structureUp()
    {
        setupValue();
        parent = parent->parent();
        if(parent)
        {
            switch(parent->type())
            {
            case FwJSON::T_Array:
                xcmd = X_SEA;
                return;

            case FwJSON::T_Object:
                xcmd = X_SEO;
                return;

            default:
                Q_ASSERT(false);
                break;
            }
        }
        else
        {
            xcmd = X_SEO;
        }
    }

    void ParseData::setupValue()
    {
        switch(parent->type())
        {
        case FwJSON::T_Array:
            setupArrayValue();
            return;

        case FwJSON::T_Object:
            setupAttributeValue();
            return;

        default:
            Q_ASSERT(false);
            break;
        }
    }

    void ParseData::setupAttributeValue()
    {
        switch(type)
        {
        case FwJSON::T_String:
            {
                if(isVariable)
                {
                    bool bOk = false;
                    bool value = FwJSON::nameToBool(buffer, &bOk);
                    if(bOk)
                    {
                        static_cast<FwJSON::Object*>(parent)->addBoolean(attribute, value);
                    }
                    else
                    {
                        static_cast<FwJSON::Object*>(parent)->addString(attribute, QString::fromUtf8(buffer));
                    }
                }
                else
                {
                    static_cast<FwJSON::Object*>(parent)->addString(attribute, QString::fromUtf8(buffer));
                }
                buffer = QByteArray();
            }
            break;

        case FwJSON::T_Number:
            {
                bool bOk = false;
                double value = buffer.toDouble(&bOk);
                if(!bOk)
                {
                    throw FwJSON::Exception("Invalid number value", line, column);
                }
                static_cast<FwJSON::Object*>(parent)->addNumber(attribute, value);
                buffer = QByteArray();
            }
            break;

        case FwJSON::T_Array:
            parent = static_cast<FwJSON::Object*>(parent)->addArray(attribute);
            break;

        case FwJSON::T_Object:
            parent = static_cast<FwJSON::Object*>(parent)->addObject(attribute);
            break;

        case FwJSON::T_Null:
            break;

        default:
            Q_ASSERT(false);
            return;
        }
        attribute = "";
        type = FwJSON::T_Null;
    }

    void ParseData::setupArrayValue()
    {
        switch(type)
        {
        case FwJSON::T_String:
            {
                if(isVariable)
                {
                    bool bOk = false;
                    bool value = FwJSON::nameToBool(buffer, &bOk);
                    if(bOk)
                    {
                        static_cast<FwJSON::Array*>(parent)->addBoolean(value);
                    }
                    else
                    {
                        static_cast<FwJSON::Array*>(parent)->addString(QString::fromUtf8(buffer));
                    }
                }
                else
                {
                    static_cast<FwJSON::Array*>(parent)->addString(QString::fromUtf8(buffer));
                }
                buffer = QByteArray();
            }
            break;

        case FwJSON::T_Number:
            {
                bool bOk = false;
                double value = buffer.toDouble(&bOk);
                if(!bOk)
                {
                    throw FwJSON::Exception("Invalid number value", line, column);
                }
                static_cast<FwJSON::Array*>(parent)->addNumber(value);
                buffer = QByteArray();
            }
            break;

        case FwJSON::T_Array:
            parent = static_cast<FwJSON::Array*>(parent)->addArray();
            break;

        case FwJSON::T_Object:
            parent = static_cast<FwJSON::Array*>(parent)->addObject();
            break;

        case FwJSON::T_Null:
            break;

        default:
            Q_ASSERT(false);
            return;
        }
        type = FwJSON::T_Null;
    }

    void x_doc(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        data->xcmd = X_ATR;
        data->declareRoot = true;
    }

    void x_var(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        if(data->parent)
        {
            data->type = FwJSON::T_String;
            data->xcmd = X_VAR;
            data->isVariable = true;
            data->buffer += c;
            return;
        }
        throw FwJSON::Exception(c, data->line, data->column);
    }

    void x_bst(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        if(data->parent)
        {
            data->type = FwJSON::T_String;
            data->xcmd = X_STR;
            data->isVariable = false;
            return;
        }
        throw FwJSON::Exception(c, data->line, data->column);
    }

    void x_est(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        switch(data->parent->type())
        {
        case FwJSON::T_Array:
            data->xcmd = X_SEA;
            return;

        case FwJSON::T_Object:
            data->xcmd = data->attribute.isEmpty() ? X_EAT : X_SEO;
            return;

        default:
            Q_ASSERT(false);
            return;
        }
    }

    void x_bsc(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        data->buffer += '\\';
        data->xcmd = X_SCH;
    }

    void x_esc(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        switch(c)
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
            data->buffer += c;
            data->xcmd = X_STR;
            break;

        default:
            throw FwJSON::Exception(c, data->line, data->column);
        }
    }

    void x_atr(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        if(data->parent->type() != FwJSON::T_Object)
        {
            throw FwJSON::Exception(c, data->line, data->column);
        }
        data->setupAttributeName();
        data->xcmd = X_VAL;
    }

    void x_int(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        data->xcmd = X_INT;
        data->type = FwJSON::T_Number;
        data->buffer += c;
    }

    void x_re1(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        data->xcmd = X_RE1;
        data->type = FwJSON::T_Number;
        data->buffer += c;
    }

    void x_re2(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        data->xcmd = X_RE2;
        data->type = FwJSON::T_Number;
        data->buffer += c;
    }

    void x_enu(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        switch(data->parent->type())
        {
        case FwJSON::T_Array:
            data->xcmd = X_SEA;
            return;

        case FwJSON::T_Object:
            data->xcmd = X_SEO;
            return;

        default:
            Q_ASSERT(false);
            return;
        }
    }

    void x_sg1(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        if(c == '+')
        {
            data->xcmd = X_INT;
            data->type = FwJSON::T_Number;
        }
        else if(c == '-')
        {
            data->xcmd = X_INT;
            data->type = FwJSON::T_Number;
            data->buffer += c;
        }
    }

    void x_rn3(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        data->xcmd = X_RE3;
        data->buffer += c;
    }

    void x_ob1(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        data->type = FwJSON::T_Object;
        data->setupValue();
        data->xcmd = X_ATR;
    }

    void x_ob2(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        data->setupAttributeName();
        x_ob1(c,data);
    }

    void x_eob(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        if(data->parent->type() == FwJSON::T_Object)
        {
            data->structureUp();
            return;
        }
        throw FwJSON::Exception(c, data->line, data->column);
    }

    void x_ar1(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        data->type = FwJSON::T_Array;
        data->setupValue();
        data->xcmd = X_VAL;
    }

    void x_ar2(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        data->setupAttributeName();
        x_ar1(c, data);
    }

    void x_ear(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        if(data->parent->type() == FwJSON::T_Array)
        {
            data->structureUp();
            return;
        }
        throw FwJSON::Exception(c, data->line, data->column);
    }

    void x_err(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        throw FwJSON::Exception(c, data->line, data->column);
    }

    void x_val(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        if(data->parent)
        {
            data->setupValue();
            switch(data->parent->type())
            {
            case FwJSON::T_Array:
                data->xcmd = X_VAL;
                break;

            case FwJSON::T_Object:
                data->xcmd = X_ATR;
                break;

            default:
                Q_ASSERT(false);
                break;
            }
            return;
        }
        throw FwJSON::Exception(c, data->line, data->column);
    }

    void x_ign(char c, ParseData* data) throw(const FwJSON::Exception&)
    {
        Q_UNUSED(c);
        Q_UNUSED(data);
    }
}

////////////////////////////////////////////////////////////////////////////////

bool FwJSON::nameToBool(const QByteArray& value, bool* bOk)
{
    if(bOk) (*bOk) = false;
    if(!value.isEmpty())
    {

        if(value == "true")
        {
            if(bOk) (*bOk) = true;
            return true;
        }
        else if(value == "false")
        {
            if(bOk) (*bOk) = true;
            return false;
        }
    }

    return false;
}

QByteArray FwJSON::boolToName(bool value)
{
    return value ? "true": "false";
}

////////////////////////////////////////////////////////////////////////////////

FwJSON::Node::Node() :
    m_parent(0)
{
}

FwJSON::Node::~Node()
{
    takeFromParent();
}

QByteArray FwJSON::Node::name() const
{
    if(m_parent)
    {
        switch(m_parent->type())
        {
        case FwJSON::T_Object:
            return m_parent->cast<FwJSON::Object>()->attributeName(const_cast<FwJSON::Node*>(this));

        case FwJSON::T_Array:
            return "[" + QByteArray::number(m_parent->cast<FwJSON::Array>()->indexOf(const_cast<FwJSON::Node*>(this))) + "]";

        default:
            break;
        }
    }
    else if(type() == FwJSON::T_Object || type() == FwJSON::T_Array)
    {
        return "root";
    }
    return "";
}

void FwJSON::Node::takeFromParent()
{
    if(m_parent)
    {
        switch(m_parent->type())
        {
        case FwJSON::T_Object:
            {
                FwJSON::Object* object = static_cast<FwJSON::Object*>(m_parent);
                object->m_attributes.remove(object->m_attributes.key(this));
            }
            break;

        case FwJSON::T_Array:
            {
                FwJSON::Array* array = static_cast<FwJSON::Array*>(m_parent);
                array->m_data.remove(array->m_data.indexOf(this));
            }
            break;

        default:
            Q_ASSERT(false);
            break;
        }
        m_parent = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////

FwJSON::String::String(const QString& value) :
   BaseClass(value)
{
}

QByteArray FwJSON::String::toUtf8() const
{
    return "\"" + value().toUtf8() + "\"";
}

int FwJSON::String::toInt(bool* bOk) const
{
    return value().toInt(bOk);
}

uint FwJSON::String::toUint(bool* bOk) const
{
    if(!value().isEmpty())
    {
        if(value().at(0) == '#')
        {
            return value().right(value().length() - 1).toUInt(bOk, 16);
        }
        return value().toUInt(bOk);
    }
    (*bOk) = false;
    return 0;
}

bool FwJSON::String::toBool(bool* bOk) const
{
    QString lowerValue = value().toLower();
    if(lowerValue == "true" || lowerValue == "yes")
    {
        (*bOk) = true;
        return true;
    }
    (*bOk) = (lowerValue == "false" || lowerValue == "no");
    return false;
}

double FwJSON::String::toNumber(bool* bOk) const
{
    return value().toDouble(bOk);
}

QString FwJSON::String::toString(bool* bOk) const
{
    QString value = this->value();

    QString out;
    out.reserve(value.size());

    QString::ConstIterator iter = value.begin();
    QString::ConstIterator end = value.end();
    try
    {
        while(iter != end)
        {

            if(*iter != QChar('\\'))
            {
                out.append(*iter);
                ++iter;
                continue;
            }

            if((++iter) == end)
            {
                throw FwJSON::Exception();
            }

            switch(iter->unicode())
            {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                out.append(QChar(iter->unicode()));
                break;

            case 'u':
                if((end - iter) >= 5)
                {
                    bool ok = false;
                    short uch = QString(iter+1, 4).toShort(&ok, 16);
                    if(!ok)
                    {
                        throw FwJSON::Exception();
                    }

                    out.append(QChar(uch));

                    iter += 5;
                    continue;
                }
                break;

            default:
                throw FwJSON::Exception();
            }

            ++iter;
        }
    }
    catch(const FwJSON::Exception& e)
    {
        (*bOk) = false;
        return QString();
    }

    (*bOk) = true;
    return out;
}

FwJSON::Node* FwJSON::String::clone() const
{
    return new FwJSON::String(value());
}

////////////////////////////////////////////////////////////////////////////////

FwJSON::Number::Number(double value) :
    BaseClass(value)
{
}

QByteArray FwJSON::Number::toUtf8() const
{
    return QByteArray::number(value());
}

int FwJSON::Number::toInt(bool* bOk) const
{
    if(qAbs(value()) > 0. && (qAbs(value()) - INT_MAX) < 0.)
    {
        if(bOk) { (*bOk) = true; }
        return static_cast<int>(value());
    }
    if(bOk) { (*bOk) = true; }
    return 0;
}

uint FwJSON::Number::toUint(bool* bOk) const
{
    if(value() > 0. && ((value() - UINT_MAX) < 0.))
    {
        if(bOk) { (*bOk) = true; }
        return static_cast<uint>(value());
    }

    if(bOk) { (*bOk) = false; }
    return 0;
}

bool FwJSON::Number::toBool(bool* bOk) const
{
    if(bOk) { (*bOk) = true; }
    return qFuzzyCompare(value(), 0.);
}

double FwJSON::Number::toNumber(bool* bOk) const
{
    if(bOk) { (*bOk) = true; }
    return value();
}

QString FwJSON::Number::toString(bool* bOk) const
{
    if(bOk) { (*bOk) = true; }
    return QString::number(value());
}

FwJSON::Node* FwJSON::Number::clone() const
{
    return new FwJSON::Number(value());
}

///////////////////////////////////////////////////////////////////////////////

FwJSON::Boolean::Boolean(bool value) :
    BaseClass(value)
{
}

QByteArray FwJSON::Boolean::toUtf8() const
{
    return value() ? FwJSON::constantTrue : FwJSON::constantFalse;
}

int FwJSON::Boolean::toInt(bool* bOk) const
{
    (*bOk) = true;
    return value();
}

uint FwJSON::Boolean::toUint(bool* bOk) const
{
    (*bOk) = true;
    return value();
}

bool FwJSON::Boolean::toBool(bool* bOk) const
{
    (*bOk) = true;
    return value();
}

double FwJSON::Boolean::toNumber(bool* bOk) const
{
    (*bOk) = true;
    return value();
}

QString FwJSON::Boolean::toString(bool* bOk) const
{
    (*bOk) = true;
    return boolToName(value());
}

FwJSON::Node* FwJSON::Boolean::clone() const
{
    return new FwJSON::Boolean(value());
}

////////////////////////////////////////////////////////////////////////////////

FwJSON::Object::Object() :
    BaseClass()
{
}

FwJSON::Object::~Object()
{
    clear();
}

void FwJSON::Object::clear()
{
    foreach(FwJSON::Node* node, m_attributes.values())
    {
        Q_ASSERT(node->m_parent == this);
        node->m_parent = 0;
        delete node;
    }
    m_attributes.clear();
}

FwJSON::Node* FwJSON::Object::addAttribute(const QByteArray& name, FwJSON::Node* value, bool replace)
{
    if(value->m_parent)
    {
        if(value->m_parent == this)
        {
            return value;
        }
        value->takeFromParent();
    }

    FwJSON::Node* currentAttr = m_attributes.value(name);
    if(currentAttr)
    {
        if(replace)
        {
            delete currentAttr;
        }
        else
        {
            FwJSON::Array* addArray = currentAttr->cast<FwJSON::Array>();
            if(!addArray)
            {
                addArray = this->addArray(name);
                addArray->addValue(currentAttr);
            }
            return addArray->addValue(value);
        }
    }

    value->m_parent = this;
    m_attributes.insert(name, value);
    return value;
}

QByteArray FwJSON::Object::toUtf8() const
{
    QByteArray attributes;
    for(QHash<QByteArray, FwJSON::Node*>::const_iterator iter = m_attributes.begin(); iter != m_attributes.end(); ++iter)
    {
        QByteArray value = iter.value()->toUtf8();
        if(!value.isEmpty())
        {
            if(!attributes.isEmpty())
            {
               attributes += ",";
            }
            attributes += ("\"" + iter.key() + "\"");
            attributes += ":";
            attributes += value;
        }

    }
    return "{" + attributes + "}";
}

void FwJSON::Object::parse(const QByteArray& utf8String) throw (FwJSON::Exception)
{
    if(utf8String.isEmpty())
    {
        throw FwJSON::Exception("Input string is empty");
    }
    QByteArray tmpStr = utf8String;
    QBuffer buffer(&tmpStr);
    parse(&buffer);
}

void FwJSON::Object::parse(QIODevice* ioDevice) throw (FwJSON::Exception)
{
    try
    {
        if(!ioDevice->isOpen() && !ioDevice->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw FwJSON::Exception(ioDevice->errorString());
        }

        ParseData data;
        data.parent = this;
        while(!ioDevice->atEnd())
        {
            data.line++;
            data.column = 0;
            QByteArray line = ioDevice->readLine().trimmed();
            if(!line.isEmpty())
            {
                int line_size = line.size();
                char* c_ptr = line.data();
                for(;data.column < line_size; data.column++, c_ptr++)
                {
                    quint8 nextChar = static_cast<quint8>(*c_ptr);

                    CharType charType = nextChar > 0 ? chars_type[nextChar] : C_Uni;
                    if(CommandFunc cmd = parse_commands[data.xcmd][charType])
                    {
                        cmd((*c_ptr), &data);
                    }
                    else
                    {
                        data.buffer += (*c_ptr);
                    }
                }
            }
        }

        if(!data.buffer.isEmpty())
        {
            data.setupValue();
        }

    }
    catch(const FwJSON::Exception& e)
    {
        clear();
        throw e;
    }
}

void FwJSON::Object::parseFile(const QString& fileName) throw(FwJSON::Exception)
{
    QFile file(QDir::toNativeSeparators(fileName));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw FwJSON::Exception(file);
    }
    parse(&file);
}

int FwJSON::Object::toInt(bool* bOk) const
{
    (*bOk) = false;
    return 0;
}

uint FwJSON::Object::toUint(bool* bOk) const
{
    (*bOk) = false;
    return 0;
}

bool FwJSON::Object::toBool(bool* bOk) const
{
    (*bOk) = false;
    return false;
}

double FwJSON::Object::toNumber(bool* bOk) const
{
    (*bOk) = false;
    return 0.;
}

QString FwJSON::Object::toString(bool* bOk) const
{
    (*bOk) = false;
    return QString();
}

FwJSON::Node* FwJSON::Object::clone() const
{
    FwJSON::Object* newObject = new FwJSON::Object();
    for(QHash<QByteArray, FwJSON::Node*>::const_iterator iter = m_attributes.begin(); iter != m_attributes.end(); ++iter)
    {
        FwJSON::Node* child = iter.value()->clone();
        child->m_parent = newObject;
        newObject->m_attributes.insert(iter.key(), child);
    }
    return newObject;
}

////////////////////////////////////////////////////////////////////////////////

FwJSON::Array::Array() :
    BaseClass()
{
}

FwJSON::Array::~Array()
{
    clear();
}

void FwJSON::Array::clear()
{
    foreach(FwJSON::Node* node, m_data)
    {
        Q_ASSERT(node->m_parent == this);
        node->m_parent = 0;
        delete node;
    }
    m_data.clear();
}

QByteArray FwJSON::Array::toUtf8() const
{
    QByteArray items;
    foreach(FwJSON::Node* node, m_data)
    {
        if(!items.isEmpty())
        {
            items += ",";
        }
        items += node->toUtf8();
    }
    if(items.isEmpty())
    {
        return QByteArray();
    }
    return "[" + items + "]";
}

int FwJSON::Array::toInt(bool* bOk) const
{
    if(size() == 1)
    {
        return m_data.at(0)->toInt(bOk);
    }
    (*bOk) = false;
    return 0;
}

uint FwJSON::Array::toUint(bool* bOk) const
{
    if(size() == 1)
    {
        return m_data.at(0)->toUint(bOk);
    }
    (*bOk) = false;
    return 0;
}

bool FwJSON::Array::toBool(bool* bOk) const
{
    if(size() == 1)
    {
        return m_data.at(0)->toBool(bOk);
    }
    (*bOk) = false;
    return false;
}

double FwJSON::Array::toNumber(bool* bOk) const
{
    if(size() == 1)
    {
        return m_data.at(0)->toNumber(bOk);
    }
    (*bOk) = false;
    return 0.;
}

QString FwJSON::Array::toString(bool* bOk) const
{
    if(size() == 1)
    {
        return m_data.at(0)->toString(bOk);
    }
    (*bOk) = false;
    return QString();
}

FwJSON::Node* FwJSON::Array::clone() const
{
    FwJSON::Array* newArray = new FwJSON::Array();
    newArray->m_data.reserve(m_data.size());
    foreach(FwJSON::Node* child, m_data)
    {
        newArray->addValue(child->clone());
    }
    return newArray;
}

FwJSON::Node* FwJSON::Array::addValue(FwJSON::Node* node)
{
    if(node->m_parent)
    {
        if(node->m_parent == this)
        {
            return node;
        }
        node->takeFromParent();
    }
    node->m_parent = this;
    m_data.append(node);
    return node;
}
