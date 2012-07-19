#ifndef FIREWORKS_JSON_H
#define FIREWORKS_JSON_H

#include <QtCore/qglobal.h>

#if defined(FWJSON_LIBRARY)
#  define FWJSON_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define FWJSON_SHARED_EXPORT Q_DECL_IMPORT
#endif

namespace FwJSON
{
    class Parser;
    class Node;
    class Null;
    class String;
    class Number;
    class Boolean;
    class Array;
    class Object;
    class Array;
    class Exception;
}

#endif //FIREWORKS_JSON_H
