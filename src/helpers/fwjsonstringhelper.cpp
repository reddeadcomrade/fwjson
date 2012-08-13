#include "fwjsoncharmap.h"
#include "fwjsonstringhelper.h"

using namespace FwJSON;

const StringHelper::utf32 StringHelper::offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                                               0x03C82080UL, 0xFA082080UL, 0x82082080UL };

StringHelper::StringHelper(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar) :
    BaseClass(beginChar, endChar)
{
}

QString StringHelper::parse() throw (FwJSON::Exception)
{
    QString out;

    bool specialChars = false;
    while(++m_beginChar != m_endChar)
    {
        if(specialChars)
        {
            switch(*m_beginChar)
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

        switch(Charmap::chars_table[*(reinterpret_cast<const quint8*>(m_beginChar))])
        {
        case Charmap::C_Str:
            ++m_beginChar;
            return out;

        case Charmap::C_RSo:
            specialChars = true;
            break;

        case Charmap::C_Un0:
            throw FwJSON::Exception("Invalid UTF-8 string");

        case Charmap::C_Un1:
            out.append(QChar(parseUtf8Chars(1)));
            break;

        case Charmap::C_Un2:
            out.append(QChar(parseUtf8Chars(2)));
            break;

        case Charmap::C_Un3:
            out.append(QChar(parseUtf8Chars(3)));
            break;

        case Charmap::C_Un4:
            out.append(QChar(parseUtf8Chars(4)));
            break;

        case Charmap::C_Un5:
            out.append(QChar(parseUtf8Chars(5)));
            break;

        default:
            out.append(QChar(*m_beginChar));
        }
    }

    return out;
}

StringHelper::utf32 StringHelper::parseUtf8Chars(int extraBytes) throw (FwJSON::Exception)
{
    if(m_beginChar + extraBytes >= m_endChar)
    {
        throw FwJSON::Exception("Invalid UTF-8 string");
    }

    utf32 result = 0;
    switch (extraBytes)
    {
        case 5: result += *(reinterpret_cast<const quint8*>(m_beginChar++)); result <<= 6;
        case 4: result += *(reinterpret_cast<const quint8*>(m_beginChar++)); result <<= 6;
        case 3: result += *(reinterpret_cast<const quint8*>(m_beginChar++)); result <<= 6;
        case 2: result += *(reinterpret_cast<const quint8*>(m_beginChar++)); result <<= 6;
        case 1: result += *(reinterpret_cast<const quint8*>(m_beginChar++)); result <<= 6;
        case 0: result += *(reinterpret_cast<const quint8*>(m_beginChar));
    }

    return result - offsetsFromUTF8[extraBytes];
}
