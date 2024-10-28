#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtservergql/TObjectCollectionControllerSearchCompWrap.h>

// ProLife includes
#include <prolifegql/COrderCollectionControllerComp.h>
#include <prolifegql/CDeviceCollectionControllerComp.h>
#include <prolifegql/CSoftwareProductCollectionControllerComp.h>
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


typedef imtservergql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::COrderCollectionControllerComp> OrderCollectionController;
typedef imtservergql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::CDeviceCollectionControllerComp> DeviceCollectionController;
typedef imtservergql::TObjectCollectionControllerSearchCompWrap<
			prolifegql::CSoftwareProductCollectionControllerComp> SoftwareProductCollectionController;
typedef prolifegql::CHardwareProductBindingControllerComp HardwareProductBindingController;
typedef prolifegql::CDeviceChangeGeneratorComp DeviceChangeGenerator;
typedef prolifegql::CLicenseChangeGeneratorComp LicenseChangeGenerator;
typedef prolifegql::COrderChangeGeneratorComp OrderChangeGenerator;
typedef prolifegql::CHardwareBindingChangeGeneratorComp HardwareBindingChangeGenerator;
typedef imtservergql::TObjectCollectionControllerSearchCompWrap<
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


