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

#ifndef FIREWORKS_JSON_CHARMAP_H
#define FIREWORKS_JSON_CHARMAP_H

#include <QtGlobal>

namespace FwJSON
{
    namespace Charmap
    {
        enum Type
        {
            C_AZ,      //Alpha (A..Z, a..z, _)
            C_Ee,      //Char 'E' and 'e'

            C_Uni,
            C_Un0,
            C_Un1,
            C_Un2,
            C_Un3,
            C_Un4,
            C_Un5,

            C_Num,     //Numbers (0..9)
            C_Fra,     //Decimal point (.)
            C_Sig,     //Char '+' and '-'

            C_Sp,      //Space (' ')

            C_Str,     //Quotation mark (")

            //These are the six structural characters
            C_Col,     //Name separator, colon (:)
            C_LCu,     //Begin-object, left curly bracket ({)
            C_RCu,     //End-object, right curly bracket (})
            C_LSq,     //Left square bracket ([)
            C_RSq,     //Right square bracket (])
            C_Sep,     //Items separator: Comma (,) or (;)

            C_RSo,     //Reverse solidus (\)
            C_SCh,     //Special chars (/, b, f, n, r, t)
            C_SCu,     //Special char 'u' (u)

            //Error
            C_Err,     //Unknow

            C_MAX = C_Err + 1
         };

        //Types of ASCII chars
        const Type chars_table[256] =
        {
                  /*0 */ /*1 */ /*2 */ /*3 */ /*4 */ /*5 */ /*6 */ /*7 */
        /*  0 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
        /*  8 */  C_Err, C_Sp,  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
        /* 16 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,
        /* 24 */  C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err, C_Err,

        /* 32 */  C_Sp,  C_Uni, C_Str, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 40 */  C_Uni, C_Uni, C_Uni, C_Sig, C_Sep, C_Sig, C_Fra, C_SCh,
        /* 48 */  C_Num, C_Num, C_Num, C_Num, C_Num, C_Num, C_Num, C_Num,
        /* 56 */  C_Num, C_Num, C_Col, C_Sep, C_Uni, C_Uni, C_Uni, C_Uni,

        /* 64 */  C_Uni, C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_Ee,  C_AZ,  C_AZ,
        /* 72 */  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
        /* 80 */  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
        /* 88 */  C_AZ,  C_AZ,  C_AZ,  C_LSq, C_RSo, C_RSq, C_Uni, C_AZ,

        /* 96 */  C_Uni, C_AZ,  C_SCh, C_AZ,  C_AZ,  C_Ee,  C_SCh, C_AZ,
        /* 104*/  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_SCh, C_AZ,
        /* 112*/  C_AZ,  C_AZ,  C_SCh, C_AZ,  C_SCh, C_SCu,  C_AZ, C_AZ,
        /* 120*/  C_AZ,  C_AZ,  C_AZ,  C_LCu, C_Uni, C_RCu, C_Uni, C_Err,

                  /*0 */ /*1 */ /*2 */ /*3 */ /*4 */ /*5 */ /*6 */ /*7 */
        /* 128*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,
        /* 136*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,
        /* 144*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,
        /* 152*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,

        /* 160*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,
        /* 168*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,
        /* 176*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,
        /* 184*/  C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0, C_Un0,

        /* 192*/  C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1,
        /* 200*/  C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1,
        /* 208*/  C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1,
        /* 216*/  C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1, C_Un1,

        /* 224*/  C_Un2, C_Un2, C_Un2, C_Un2, C_Un2, C_Un2, C_Un2, C_Un2,
        /* 232*/  C_Un2, C_Un2, C_Un2, C_Un2, C_Un2, C_Un2, C_Un2, C_Un2,
        /* 240*/  C_Un3, C_Un3, C_Un3, C_Un3, C_Un3, C_Un3, C_Un3, C_Un3,
        /* 248*/  C_Un4, C_Un4, C_Un4, C_Un4, C_Un5, C_Un5, C_Un5, C_Un5,
        };

        const quint8 utf8_magickNumber[7] =
        {
            0x3F, 0x1F, 0xF, 0x7, 0x3, 0x1
        };

    }
}

#endif //FIREWORKS_JSON_CHARMAP_H
    
