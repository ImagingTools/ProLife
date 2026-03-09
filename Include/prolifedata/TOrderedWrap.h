// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/TDelPtr.h>
#include <istd/IChangeable.h>


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

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode = istd::IChangeable::CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(istd::IChangeable::CompatibilityMode mode = istd::IChangeable::CM_WITHOUT_REFS) const override;
	virtual bool ResetData(istd::IChangeable::CompatibilityMode mode = istd::IChangeable::CM_WITHOUT_REFS) override;

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


// reimplemented (IChangeable)

template<class Base>
int TOrderedWrap<Base>::GetSupportedOperations() const
{
	return istd::IChangeable::SO_CLONE | istd::IChangeable::SO_COPY | istd::IChangeable::SO_RESET;
}


template<class Base>
bool TOrderedWrap<Base>::CopyFrom(const istd::IChangeable& object,  istd::IChangeable::CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const TOrderedWrap<Base>* sourcePtr = dynamic_cast<const TOrderedWrap<Base>*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_orderId = sourcePtr->m_orderId;

		bool retVal = Base::CopyFrom(object);

		return retVal;
	}

	return false;
}


template<class Base>
bool TOrderedWrap<Base>::IsEqual(const istd::IChangeable& object) const
{
	const TOrderedWrap<Base>* sourcePtr = dynamic_cast<const TOrderedWrap<Base>*>(&object);
	if (sourcePtr != nullptr){
		return (m_orderId == sourcePtr->m_orderId);
	}

	return false;
}


template<class Base>
istd::IChangeableUniquePtr TOrderedWrap<Base>::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new TOrderedWrap<Base>());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


template<class Base>
bool TOrderedWrap<Base>::ResetData(istd::IChangeable::CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_orderId.clear();
	Base::ResetData();

	return true;
}


} // namespace prolifedata


