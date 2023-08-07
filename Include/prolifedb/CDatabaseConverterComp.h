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
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Binding collection", true, "BindingCollection");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
};


} // namespace prolifedb


