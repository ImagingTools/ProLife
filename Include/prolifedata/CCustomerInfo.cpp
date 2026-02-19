// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include <prolifedata/CCustomerInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>


namespace prolifedata
{


// public methods

void CCustomerInfo::SetCustomerId(const QByteArray& customerId)
{
	if (m_customerId != customerId){
		istd::CChangeNotifier notifier(this);

		m_customerId = customerId;
	}
}


// reimplemented (ICustomerInfo)

QByteArray CCustomerInfo::GetCustomerId() const
{
	return m_customerId;
}


// reimplemented (iser::ISerializable)

bool CCustomerInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();

	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	bool retVal = true;

	if (imtCoreVersion > 9541){
		iser::CArchiveTag customerIdTag("CustomerId", "Customer-ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(customerIdTag);
		retVal = retVal && archive.Process(m_customerId);
		retVal = retVal && archive.EndTag(customerIdTag);
	}

	retVal = retVal && BaseClass::Serialize(archive);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CCustomerInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CCustomerInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CCustomerInfo* sourcePtr = dynamic_cast<const CCustomerInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_customerId = sourcePtr->m_customerId;

		bool retVal = BaseClass::CopyFrom(object);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CCustomerInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CCustomerInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CCustomerInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_customerId.clear();

	BaseClass::ResetData();

	return true;
}


} // namespace prolifedata


