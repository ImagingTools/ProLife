#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ProLife includes
#include <prolifedata/CCustomerInfo.h>
#include <prolifegql/COrderCollectionControllerComp.h>
#include <prolifegql/CDeviceCollectionControllerComp.h>
#include <prolifegql/CSoftwareProductCollectionControllerComp.h>
#include <prolifegql/CDeviceControllerComp.h>
#include <prolifegql/CDeviceChangeGeneratorComp.h>
#include <prolifegql/CLicenseChangeGeneratorComp.h>
#include <prolifegql/COrderChangeGeneratorComp.h>
#include <prolifegql/CHardwareBindingChangeGeneratorComp.h>
#include <prolifegql/CCustomerCollectionControllerComp.h>
#include <prolifegql/CDeviceControllerComp.h>
#include <prolifegql/CCustomerChangeGeneratorComp.h>
#include <prolifegql/CDeviceMetaInfoDelegateComp.h>
#include <prolifegql/CSoftwareMetaInfoDelegateComp.h>
#include <prolifegql/COrderMetaInfoDelegateComp.h>
#include <prolifegql/CCustomerMetaInfoDelegateComp.h>
#include <prolifegql/CHardwareBindingMetaInfoDelegateComp.h>
#include <prolifegql/CGroupFilterParamJoinerComp.h>
#include <prolifegql/CWorkspaceControllerComp.h>
#include <prolifegql/CSoftwareControllerComp.h>
#include <prolifegql/CDeviceCollectionDocumentManagerComp.h>


/**
	ProLifeGql package
*/
namespace ProLifeGqlPck
{


typedef prolifegql::COrderCollectionControllerComp OrderCollectionController;
typedef prolifegql::CDeviceCollectionControllerComp DeviceCollectionController;
typedef prolifegql::CSoftwareProductCollectionControllerComp SoftwareProductCollectionController;
typedef prolifegql::CDeviceChangeGeneratorComp DeviceChangeGenerator;
typedef prolifegql::CLicenseChangeGeneratorComp LicenseChangeGenerator;
typedef prolifegql::COrderChangeGeneratorComp OrderChangeGenerator;
typedef prolifegql::CHardwareBindingChangeGeneratorComp HardwareBindingChangeGenerator;
typedef prolifegql::CCustomerCollectionControllerComp CustomerCollectionController;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<
						prolifedata::CCustomerInfo,
						prolifedata::ICustomerInfo,
						imtauth::ICompanyInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> CustomerInfo;
typedef prolifegql::CDeviceControllerComp DeviceController;
typedef prolifegql::CCustomerChangeGeneratorComp CustomerChangeGenerator;
typedef prolifegql::CDeviceMetaInfoDelegateComp DeviceMetaInfoDelegate;
typedef prolifegql::CSoftwareMetaInfoDelegateComp SoftwareMetaInfoDelegate;
typedef prolifegql::COrderMetaInfoDelegateComp OrderMetaInfoDelegate;
typedef prolifegql::CCustomerMetaInfoDelegateComp CustomerMetaInfoDelegate;
typedef prolifegql::CHardwareBindingMetaInfoDelegateComp HardwareBindingMetaInfoDelegate;
typedef prolifegql::CGroupFilterParamJoinerComp GroupFilterParamJoiner;
typedef prolifegql::CWorkspaceControllerComp WorkspaceController;
typedef prolifegql::CSoftwareControllerComp SoftwareController;
typedef prolifegql::CDeviceCollectionDocumentManagerComp DeviceCollectionDocumentManager;


} // namespace ImtLicenseGqlPck


