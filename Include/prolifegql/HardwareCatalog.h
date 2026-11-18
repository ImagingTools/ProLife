#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>

// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>
#include <prolifedata/IHardwareConfigurationInfo.h>


namespace prolifegql
{


/**
	Checks that the product exists and belongs to the Hardware category.
	A product that cannot be read is refused.
*/
bool CheckHardwareModel(const imtbase::IObjectCollection& productCollection, const QByteArray& productId, QString& errorMessage);

/**
	Checks that every component exists in the collection. A component that cannot be read is refused.
*/
bool CheckHardwareComponents(const imtbase::IObjectCollection& componentCollection, const QByteArrayList& componentIds, QString& errorMessage);

/**
	Checks the fields a hardware component cannot do without: part number and name.
*/
bool CheckHardwareComponentFields(const prolifedata::IHardwareComponentInfo& componentInfo, QString& errorMessage);

/**
	Checks that the manufacturer of the component (if given) is a company with the manufacturer role,
	each of its suppliers a company with the supplier role, and that no supplier is listed twice.
*/
bool CheckHardwareComponentVendors(
			const prolifedata::IHardwareComponentInfo& componentInfo,
			const imtbase::IObjectCollection& companyCollection,
			QString& errorMessage);

/**
	Checks the fields a hardware configuration cannot do without: article, name and hardware model.
*/
bool CheckHardwareConfigurationFields(const prolifedata::IHardwareConfigurationInfo& configurationInfo, QString& errorMessage);


} // namespace prolifegql


