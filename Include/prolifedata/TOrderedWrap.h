#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


template<class Base>
class TOrderedWrap : public Base
{
public:
	virtual const QByteArray GetOrderId() const;
	virtual void SetOrderId(const QByteArray& orderId);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

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


// reimplemented (iser::ISerializable)

template <class Base>
bool TOrderedWrap<Base>::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag orderIdTag("OrderId", "Order id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderIdTag);
	retVal = retVal && archive.Process(m_orderId);
	retVal = retVal && archive.EndTag(orderIdTag);

	retVal = retVal && Base::Serialize(archive);

	return retVal;
}


} // namespace prolifedata


