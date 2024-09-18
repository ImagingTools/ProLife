#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgql/TObjectCollectionControllerSearchCompWrap.h>

// ProLife includes
#include <prolifegql/COrderCollectionControllerComp.h>
#include <prolifegql/CDeviceCollectionControllerComp.h>
#include <prolifegql/CSoftwareProductCollectionControllerComp.h>
#include <prolifegql/CHardwareProductCollectionControllerComp.h>
#include <prolifegql/CHardwareProductBindingControllerComp.h>
#include <prolifegql/CDeviceChangeGeneratorComp.h>
#include <prolifegql/CLicenseChangeGeneratorComp.h>
#include <prolifegql/COrderChangeGeneratorComp.h>
#include <prolifegql/CHardwareBindingChangeGeneratorComp.h>
#include <prolifegql/CCustomerCollectionControllerComp.h>
#include <prolifedata/CCustomerInfo.h>


/**
	ProLifeGql package
*/
namespace ProLifeGqlPck
{


typedef imtgql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::COrderCollectionControllerComp> OrderCollectionController;
typedef imtgql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::CDeviceCollectionControllerComp> DeviceCollectionController;
typedef imtgql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::CSoftwareProductCollectionControllerComp> SoftwareProductCollectionController;
typedef prolifegql::CHardwareProductCollectionControllerComp HardwareProductCollectionController;
typedef prolifegql::CHardwareProductBindingControllerComp HardwareProductBindingController;
typedef prolifegql::CDeviceChangeGeneratorComp DeviceChangeGenerator;
typedef prolifegql::CLicenseChangeGeneratorComp LicenseChangeGenerator;
typedef prolifegql::COrderChangeGeneratorComp OrderChangeGenerator;
typedef prolifegql::CHardwareBindingChangeGeneratorComp HardwareBindingChangeGenerator;
typedef imtgql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::CCustomerCollectionControllerComp> CustomerCollectionController;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						prolifedata::CCustomerInfo,
						prolifedata::ICustomerInfo,
						imtauth::ICompanyInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> CustomerInfo;


} // namespace ImtLicenseGqlPck


