#include "fwjsonhelper.h"

FwJSON::Helper::Helper(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar) :
    m_beginChar(beginChar),
    m_endChar(endChar)
{
}
