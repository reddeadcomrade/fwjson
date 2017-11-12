#pragma once

#include <QtCore/QByteArray>

namespace FwJSON
{
   class Helper;
}

class FwJSON::Helper
{
public:
    Helper(QByteArray::const_iterator& beginChar, const QByteArray::const_iterator& endChar);

protected:
    QByteArray::const_iterator& m_beginChar;
    const QByteArray::const_iterator& m_endChar;
};
