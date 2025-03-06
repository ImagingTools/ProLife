#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IPermissionChecker.h>

// ProLife includes
#include <prolifedata/IGroupFilterParamJoiner.h>


namespace prolifegql
{


class CGroupFilterParamJoinerComp: public ilog::CLoggerComponentBase, public virtual prolifedata::IGroupFilterParamJoiner
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupFilterParamJoinerComp);
		I_REGISTER_INTERFACE(prolifedata::IGroupFilterParamJoiner);
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission ID for show all devices", true, "");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
	I_END_COMPONENT;
	
	// reimplemented (prolifedata::IGroupFilterParamJoiner)
	virtual bool JoinGroupFilterParam(const imtgql::IGqlRequest& gqlRequest, iprm::IParamsSet& filterParam) const override;
	
private:
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
};


} // namespace prolifegql


