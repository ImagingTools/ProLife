// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include <prolifedata/CCustomerMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ProLife includes
#include <prolifedata/ICustomerInfo.h>


namespace prolifedata
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CCustomerMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (m_objectTypeIdsAttrPtr.FindValue(typeId) == -1){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const ICustomerInfo* customerInfoPtr = dynamic_cast<const ICustomerInfo*>(dataPtr);
	if (customerInfoPtr == nullptr){
		return false;
	}
	
	QByteArrayList groupIds = customerInfoPtr->GetGroups();

	metaInfoPtr->SetMetaInfo(ICustomerInfo::MIT_CUSTOMER_ID, customerInfoPtr->GetCustomerId());
	metaInfoPtr->SetMetaInfo(ICustomerInfo::MIT_CONTACT_NAME, customerInfoPtr->GetName());
	metaInfoPtr->SetMetaInfo(ICustomerInfo::MIT_CONTACT_DESCRIPTION, customerInfoPtr->GetDescription());
	metaInfoPtr->SetMetaInfo(ICustomerInfo::MIT_EMAIL, customerInfoPtr->GetEmail());
	metaInfoPtr->SetMetaInfo(ICustomerInfo::MIT_GROUPS, groupIds.join(';'));

	return true;
}


// public methods of embedded class MetaInfo

QString CCustomerMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace prolifedata


