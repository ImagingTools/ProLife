#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace prolifedb
{


class CDatabaseConverterComp: public icomp::CComponentBase
{
public:
    typedef icomp::CComponentBase BaseClass;

    I_BEGIN_COMPONENT(CDatabaseConverterComp)
        I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
        I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
        I_ASSIGN(m_productInstanceCollectionCompPtr, "ProductInstanceCollection", "Product instance collection", true, "ProductInstanceCollection");
        I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", false, "OrderCollection");
	I_END_COMPONENT

protected:
    // reimplemented (icomp::CComponentBase)
    virtual void OnComponentCreated() override;
    virtual void OnComponentDestroyed() override;

private:
    I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
    I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
    I_REF(imtbase::IObjectCollection, m_productInstanceCollectionCompPtr);
    I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
};


} // namespace prolifedb


