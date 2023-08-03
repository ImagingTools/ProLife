#pragma once


// ProLife includes
#include <prolifegql/COrderControllerComp.h>
#include <prolifegql/CDeviceControllerComp.h>
#include <prolifegql/COrderCollectionControllerComp.h>
#include <prolifegql/CDeviceCollectionControllerComp.h>
#include <prolifegql/CSoftwareProductCollectionControllerComp.h>
#include <prolifegql/CHardwareProductCollectionControllerComp.h>
#include <prolifegql/CSoftwareProductControllerComp.h>
#include <prolifegql/COrderHistoryControllerComp.h>


/**
	ProLifeLicenseGqlPck package
*/
namespace ProLifeGqlPck
{


typedef prolifegql::COrderControllerComp OrderController;
typedef prolifegql::COrderCollectionControllerComp OrderCollectionController;
typedef prolifegql::CDeviceControllerComp DeviceController;
typedef prolifegql::CDeviceCollectionControllerComp DeviceCollectionController;
typedef prolifegql::CSoftwareProductCollectionControllerComp SoftwareProductCollectionController;
typedef prolifegql::CHardwareProductCollectionControllerComp HardwareProductCollectionController;
typedef prolifegql::CSoftwareProductControllerComp SoftwareProductController;
typedef prolifegql::COrderHistoryControllerComp OrderHistoryController;


} // namespace ImtLicenseGqlPck


