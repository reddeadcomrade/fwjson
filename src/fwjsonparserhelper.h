#ifndef FIREWORKS_JSON_PARSERHELPER_H
#define FIREWORKS_JSON_PARSERHELPER_H

#include <QtCore/QString>
#include <QtCore/QByteArray>

#include "fwjsonexception.h"

namespace FwJSON
{
    namespace ParserHelper
    {
        typedef unsigned int utf32;

        static const utf32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                                  0x03C82080UL, 0xFA082080UL, 0x82082080UL };

        QString parseString(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar) throw (FwJSON::Exception);

        utf32 parseUtf8Chars(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar, int extraBytes) throw (FwJSON::Exception);

    }
}


#endif // FIREWORKS_JSON_PARSERHELPER_H
