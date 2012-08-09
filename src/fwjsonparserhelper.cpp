#include <QtCore/QDebug>

#include "fwjsoncharmap.h"

#include "fwjsonparserhelper.h"

QString FwJSON::ParserHelper::parseString(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar) throw (FwJSON::Exception)
{
    QString out;

    bool specialChars = false;
    while(++beginChar != endChar)
    {
        if(specialChars)
        {
            switch(*beginChar)
            {
            case '"':
                out.append(QChar('"'));
                break;

            case '\\':
                out.append(QChar('\\'));
                break;

            case '/':
                out.append(QChar('/'));
                break;

            case 'b':
                out.append(QChar('\b'));
                break;

            case 'f':
                out.append(QChar('\f'));
                break;

            case 'n':
                out.append(QChar('\n'));
                break;

            case 'r':
                out.append(QChar('\r'));
                break;

            case 't':
                out.append(QChar('\t'));
                break;

            default:
                Q_ASSERT(false);
            }

            specialChars = false;
            continue;
        }

        switch(Charmap::chars_table[*(reinterpret_cast<const quint8*>(beginChar))])
        {
        case Charmap::C_Str:
            ++beginChar;
            return out;

        case Charmap::C_RSo:
            specialChars = true;
            break;

        case Charmap::C_Un0:
            throw FwJSON::Exception("Invalid UTF-8 string");
            break;

        case Charmap::C_Un1:
            out.append(QChar(parseUtf8Chars(beginChar, endChar, 1)));
            break;

        case Charmap::C_Un2:
            out.append(QChar(parseUtf8Chars(beginChar, endChar, 2)));
            break;

        case Charmap::C_Un3:
            out.append(QChar(parseUtf8Chars(beginChar, endChar, 3)));
            break;

        case Charmap::C_Un4:
            out.append(QChar(parseUtf8Chars(beginChar, endChar, 4)));
            break;

        case Charmap::C_Un5:
            out.append(QChar(parseUtf8Chars(beginChar, endChar, 5)));
            break;

        default:
            out.append(QChar(*beginChar));
        }

        qDebug() << *(reinterpret_cast<const quint8*>(beginChar)) << *beginChar;
    }

    return out;
}

FwJSON::ParserHelper::utf32 FwJSON::ParserHelper::parseUtf8Chars(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar, int extraBytes) throw (FwJSON::Exception)
{
    if(beginChar + extraBytes >= endChar)
    {
        throw FwJSON::Exception("Invalid UTF-8 string");
    }

    utf32 result = *(reinterpret_cast<const quint8*>(beginChar)) & Charmap::utf8_magickNumber[extraBytes];
    switch(extraBytes)
    {
    case 5:
        result = ((result << 6) | (*(reinterpret_cast<const quint8*>(++beginChar)) & 0x3F));
    case 4:
        result = ((result << 6) | (*(reinterpret_cast<const quint8*>(++beginChar)) & 0x3F));
    case 3:
        result = ((result << 6) | (*(reinterpret_cast<const quint8*>(++beginChar)) & 0x3F));
    case 2:
        result = ((result << 6) | (*(reinterpret_cast<const quint8*>(++beginChar)) & 0x3F));
    case 1:
        result = ((result << 6) | (*(reinterpret_cast<const quint8*>(++beginChar)) & 0x3F));
        break;

    default:
        throw FwJSON::Exception("Invalid UTF-8 string");
    }

    return result;
}
