#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ProLife includes
#include <prolifedata/CIotDeviceInfo.h>


namespace prolifedata
{


class CIotDeviceInfoComp:
		public icomp::CComponentBase,
		virtual public COrderedIdentifiableIotDeviceInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CIotDeviceInfoComp);
		I_REGISTER_INTERFACE(IIotDeviceInfo);
	I_END_COMPONENT;
};


} // namespace prolifedata


