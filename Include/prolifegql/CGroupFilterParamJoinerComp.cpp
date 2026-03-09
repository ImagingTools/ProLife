// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include <prolifegql/CGroupFilterParamJoinerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserGroupFilter.h>


namespace prolifegql
{


// public methods

// reimplemented (prolifedata::IGroupFilterParamJoiner)

bool CGroupFilterParamJoinerComp::JoinGroupFilterParam(const imtgql::IGqlRequest& gqlRequest, iprm::IParamsSet& filterParam) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create an object filter. GraphQL context is nullptr"), "CGroupFilterParamJoinerComp");
		return false;
	}
	
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to join group filter. Error: User info from GraphQL context invalid"), "CGroupFilterParamJoinerComp");
		return false;
	}
	
	bool filterByGroup = true;
	
	QByteArray userId = userInfoPtr->GetId();
	QByteArrayList userGroupIds = userInfoPtr->GetGroups();
	
	if (userInfoPtr->IsAdmin()){
		filterByGroup = false;
	}
	else{
		if (m_checkPermissionCompPtr.IsValid()){
			QByteArrayList userPermissions = userInfoPtr->GetPermissions();
			
			QByteArrayList permissions;
			permissions << *m_permissionIdAttrPtr;
			filterByGroup = !m_checkPermissionCompPtr->CheckPermission(userPermissions, permissions);
		}
	}
	
	if (filterByGroup){
		iprm::CParamsSet* paramSetPtr = dynamic_cast<iprm::CParamsSet*>(&filterParam);
		
		imtauth::CUserGroupFilter* groupFilterPtr = new imtauth::CUserGroupFilter();
		groupFilterPtr->SetUserId(userId);
		groupFilterPtr->SetGroupIds(userGroupIds);
		paramSetPtr->SetEditableParameter("GroupFilter", groupFilterPtr, true);
	}
	
	return true;
}


} // namespace prolifegql


