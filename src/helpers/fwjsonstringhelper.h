#ifndef FWJSONSTRINGHELPER_H
#define FWJSONSTRINGHELPER_H

#include <QtCore/QString>

#include "fwjsonexception.h"

#include "helpers/fwjsonhelper.h"

namespace FwJSON
{
    class StringHelper;
}

class FwJSON::StringHelper : public FwJSON::Helper
{
    typedef FwJSON::Helper BaseClass;

public:

    typedef unsigned int utf32;

    /*
       Magic values subtracted from a buffer value during UTF8 conversion.
       This table contains as many values as there might be trailing bytes
       in a UTF-8 sequence.
    */
    static const utf32 offsetsFromUTF8[6];

    StringHelper(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar);

    QString parse() throw (FwJSON::Exception);

protected:
    utf32 parseUtf8Chars(int extraBytes) throw (FwJSON::Exception);
};

#endif // FWJSONSTRINGHELPER_H
