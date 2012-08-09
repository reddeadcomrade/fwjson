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

        QString parseString(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar) throw (FwJSON::Exception);

        utf32 parseUtf8Chars(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar, int extraBytes) throw (FwJSON::Exception);

    }
}


#endif // FIREWORKS_JSON_PARSERHELPER_H
