/******************************************************************************

Copyright (c) 2012 Egor Popov <garlero@yandex.ru>

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom 
the Software is furnished to do so, subject to the following conditions:
    
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
    
#include <QtCore/qcoreapplication.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qdebug.h>

#include "fwjson.h"
#include "fwjsoncharmap.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    QByteArray json("123456789");

    quint8* begin = reinterpret_cast<quint8*>(json.begin());
    quint8* end = reinterpret_cast<quint8*>(json.end());
    for(quint8* c = begin; c != end; c++)
    {
        QByteArray char_type_name = "Unknow";

        FwJSON::Charmap::Type type = FwJSON::Charmap::chars_table[*c];
        switch(type)
        {
        case FwJSON::Charmap::C_AZ:
            char_type_name = "C_AZ";
            break;

        case FwJSON::Charmap::C_Ee:
            char_type_name = "C_Ee";
            break;

        case FwJSON::Charmap::C_Uni:
            char_type_name = "C_Uni";
            break;

        case FwJSON::Charmap::C_Num:
            char_type_name = "C_Num";
            break;

        case FwJSON::Charmap::C_Fra:
            char_type_name = "C_Fra";
            break;

        case FwJSON::Charmap::C_Sig:
            char_type_name = "C_Sig";
            break;

        case FwJSON::Charmap::C_Sp:
            char_type_name = "C_Sp";
            break;

        case FwJSON::Charmap::C_Str:
            char_type_name = "C_Str";
            break;

        case FwJSON::Charmap::C_Esc:
            char_type_name = "C_Esc";
            break;

        case FwJSON::Charmap::C_Col:
            char_type_name = "C_Col";
            break;

        case FwJSON::Charmap::C_LCu:
            char_type_name = "C_LCu";
            break;

        case FwJSON::Charmap::C_RCu:
            char_type_name = "C_RCu";
            break;

        case FwJSON::Charmap::C_LSq:
            char_type_name = "C_LSq";
            break;

        case FwJSON::Charmap::C_RSq:
            char_type_name = "C_RSq";
            break;

        case FwJSON::Charmap::C_Sep:
            char_type_name = "Sep (,)";
            break;

        case FwJSON::Charmap::C_Err:
            char_type_name = "Error";
            break;

        default:
            Q_ASSERT(false);
        }

        qDebug() << *(reinterpret_cast<char*>(c)) << char_type_name;
    }

    return a.exec();
}
