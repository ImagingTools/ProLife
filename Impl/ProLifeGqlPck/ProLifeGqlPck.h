#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgql/TObjectCollectionControllerSearchCompWrap.h>

// ProLife includes
#include <prolifegql/COrderControllerComp.h>
#include <prolifegql/CDeviceControllerComp.h>
#include <prolifegql/COrderCollectionControllerComp.h>
#include <prolifegql/CDeviceCollectionControllerComp.h>
#include <prolifegql/CSoftwareProductCollectionControllerComp.h>
#include <prolifegql/CHardwareProductCollectionControllerComp.h>
#include <prolifegql/CSoftwareProductControllerComp.h>
#include <prolifegql/CHardwareProductBindingControllerComp.h>
#include <prolifegql/CDeviceChangeGeneratorComp.h>
#include <prolifegql/CLicenseChangeGeneratorComp.h>
#include <prolifegql/COrderChangeGeneratorComp.h>
#include <prolifegql/CHardwareBindingChangeGeneratorComp.h>
#include <prolifegql/CCustomerControllerComp.h>
#include <prolifegql/CCustomerCollectionControllerComp.h>
#include <prolifedata/CCustomerInfo.h>


/**
	ProLifeGql package
*/
namespace ProLifeGqlPck
{


typedef prolifegql::COrderControllerComp OrderController;
typedef imtgql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::COrderCollectionControllerComp> OrderCollectionController;
typedef prolifegql::CDeviceControllerComp DeviceController;
typedef imtgql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::CDeviceCollectionControllerComp> DeviceCollectionController;
typedef imtgql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::CSoftwareProductCollectionControllerComp> SoftwareProductCollectionController;
typedef prolifegql::CHardwareProductCollectionControllerComp HardwareProductCollectionController;
typedef prolifegql::CSoftwareProductControllerComp SoftwareProductController;
typedef prolifegql::CHardwareProductBindingControllerComp HardwareProductBindingController;
typedef prolifegql::CDeviceChangeGeneratorComp DeviceChangeGenerator;
typedef prolifegql::CLicenseChangeGeneratorComp LicenseChangeGenerator;
typedef prolifegql::COrderChangeGeneratorComp OrderChangeGenerator;
typedef prolifegql::CHardwareBindingChangeGeneratorComp HardwareBindingChangeGenerator;
typedef prolifegql::CCustomerControllerComp CustomerController;
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


