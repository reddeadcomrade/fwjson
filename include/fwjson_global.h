#pragma once

#include <QtCore/qglobal.h>

#if defined(FWJSON_LIBRARY)
#  define FWJSON_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define FWJSON_SHARED_EXPORT Q_DECL_IMPORT
#endif
