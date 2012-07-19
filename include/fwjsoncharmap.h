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

namespace FwJSON
{
    namespace Charmap
    {
        enum Type
        {
            C_AZ,      //Alpha (A..Z, a..z, _)
            C_Ee,      //Char 'E' and 'e'
            C_Uni,     //Unicode symbol

            C_Num,     //Numbers (0..9)
            C_Fra,     //Decimal point (.)
            C_Sig,     //Char '+' and '-'

            C_Sp,      //Space (' ')

            C_Str,     //Quotation mark (")
            C_Esc,     //Escape

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
        const Type chars_table[256] =
        {
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
        /* 88 */  C_AZ,  C_AZ,  C_AZ,  C_LSq, C_Esc, C_RSq, C_Uni, C_AZ,

        /* 96 */  C_Uni, C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_Ee,  C_AZ,  C_AZ,
        /* 104*/  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
        /* 112*/  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,  C_AZ,
        /* 120*/  C_AZ,  C_AZ,  C_AZ,  C_LCu, C_Uni, C_RCu, C_Uni, C_Err,

                  /*0 */ /*1 */ /*2 */ /*3 */ /*4 */ /*5 */ /*6 */ /*7 */
        /* 128*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 136*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 144*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 152*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,

        /* 160*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 168*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 176*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 184*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,

        /* 192*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 200*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 208*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 216*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,

        /* 224*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 232*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 240*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        /* 248*/  C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni, C_Uni,
        };
    }
}

#endif //FIREWORKS_JSON_CHARMAP_H
    
