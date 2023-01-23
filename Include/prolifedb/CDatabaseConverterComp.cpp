#include <prolifedb/CDatabaseConverterComp.h>
#include "QtCore/qdebug.h"


namespace prolifedb
{

// protected methods

// reimplemented (icomp::CComponentBase)

void CDatabaseConverterComp::OnComponentCreated()
{
    BaseClass::OnComponentCreated();

    qDebug() << "Component created";
}


void CDatabaseConverterComp::OnComponentDestroyed()
{
    BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


