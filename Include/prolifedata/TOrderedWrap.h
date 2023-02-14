#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/CChangeNotifier.h>


namespace prolifedata
{


template<class Base>
class TOrderedWrap : public Base
{
public:
	virtual const QByteArray GetOrderId() const;
	virtual void SetOrderId(const QByteArray& orderId);

private:
	QByteArray m_orderId;
};


// public methods

template <class Base>
const QByteArray TOrderedWrap<Base>::GetOrderId() const
{
	return m_orderId;
}


template <class Base>
void TOrderedWrap<Base>::SetOrderId(const QByteArray& orderId)
{
	if (m_orderId != orderId){
		istd::CChangeNotifier changeNotifier(this);

		m_orderId = orderId;
	}
}


} // namespace prolifedata


