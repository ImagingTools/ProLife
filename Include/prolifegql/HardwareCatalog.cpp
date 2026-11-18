#include <prolifegql/HardwareCatalog.h>

// ProLife includes
#include <prolifedata/ICustomerInfo.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IProductInfo.h>



namespace prolifegql
{


bool CheckHardwareModel(const imtbase::IObjectCollection& productCollection, const QByteArray& productId, QString& errorMessage)
{
	imtbase::IObjectCollection::DataPtr productDataPtr;
	const imtlic::IProductInfo* productInfoPtr = nullptr;
	if (productCollection.GetObjectData(productId, productDataPtr)){
		productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		errorMessage = QStringLiteral("The hardware model '%1' cannot be read").arg(QString::fromUtf8(productId));

		return false;
	}

	if (productInfoPtr->GetCategoryId() != QByteArrayLiteral("Hardware")){
		errorMessage = QStringLiteral("The product '%1' is not a hardware model").arg(productInfoPtr->GetName());

		return false;
	}

	return true;
}


bool CheckHardwareComponents(const imtbase::IObjectCollection& componentCollection, const QByteArrayList& componentIds, QString& errorMessage)
{
	for (const QByteArray& componentId : componentIds){
		imtbase::IObjectCollection::DataPtr componentDataPtr;
		const bool isComponent =
					componentCollection.GetObjectData(componentId, componentDataPtr) &&
					dynamic_cast<const prolifedata::IHardwareComponentInfo*>(componentDataPtr.GetPtr()) != nullptr;
		if (!isComponent){
			errorMessage = QStringLiteral("The hardware component '%1' cannot be read").arg(QString::fromUtf8(componentId));

			return false;
		}
	}

	return true;
}


bool CheckHardwareComponentFields(const prolifedata::IHardwareComponentInfo& componentInfo, QString& errorMessage)
{
	if (componentInfo.GetPartNumber().trimmed().isEmpty()){
		errorMessage = QStringLiteral("The part number of the hardware component is not given");

		return false;
	}

	if (componentInfo.GetName().trimmed().isEmpty()){
		errorMessage = QStringLiteral("The name of the hardware component is not given");

		return false;
	}

	return true;
}


namespace
{


// The company \a companyId held by \a dataPtr, null if it cannot be read.
const prolifedata::ICustomerInfo* ReadCompany(
			const imtbase::IObjectCollection& companyCollection,
			const QByteArray& companyId,
			imtbase::IObjectCollection::DataPtr& dataPtr)
{
	if (companyId.isEmpty() || !companyCollection.GetObjectData(companyId, dataPtr)){
		return nullptr;
	}

	return dynamic_cast<const prolifedata::ICustomerInfo*>(dataPtr.GetPtr());
}


} // namespace


bool CheckHardwareComponentVendors(
			const prolifedata::IHardwareComponentInfo& componentInfo,
			const imtbase::IObjectCollection& companyCollection,
			QString& errorMessage)
{
	const QByteArray manufacturerId = componentInfo.GetManufacturerId();
	if (!manufacturerId.isEmpty()){
		imtbase::IObjectCollection::DataPtr manufacturerDataPtr;
		const prolifedata::ICustomerInfo* manufacturerPtr = ReadCompany(companyCollection, manufacturerId, manufacturerDataPtr);
		if (manufacturerPtr == nullptr){
			errorMessage = QStringLiteral("The manufacturer '%1' does not exist").arg(QString::fromUtf8(manufacturerId));

			return false;
		}

		if (!manufacturerPtr->IsManufacturer()){
			errorMessage = QStringLiteral("'%1' is not a manufacturer").arg(manufacturerPtr->GetName());

			return false;
		}
	}

	QByteArrayList listedSupplierIds;
	const prolifedata::IHardwareComponentInfo::SupplierOffers offers = componentInfo.GetSupplierOffers();
	for (const prolifedata::IHardwareComponentInfo::SupplierOffer& offer : offers){
		imtbase::IObjectCollection::DataPtr supplierDataPtr;
		const prolifedata::ICustomerInfo* supplierPtr = ReadCompany(companyCollection, offer.supplierId, supplierDataPtr);
		if (supplierPtr == nullptr){
			errorMessage = QStringLiteral("The supplier '%1' does not exist").arg(QString::fromUtf8(offer.supplierId));

			return false;
		}

		if (!supplierPtr->IsSupplier()){
			errorMessage = QStringLiteral("'%1' is not a supplier").arg(supplierPtr->GetName());

			return false;
		}

		if (listedSupplierIds.contains(offer.supplierId)){
			errorMessage = QStringLiteral("The supplier '%1' is listed twice").arg(supplierPtr->GetName());

			return false;
		}

		listedSupplierIds << offer.supplierId;
	}

	return true;
}


bool CheckHardwareConfigurationFields(const prolifedata::IHardwareConfigurationInfo& configurationInfo, QString& errorMessage)
{
	if (configurationInfo.GetConfigurationId().trimmed().isEmpty()){
		errorMessage = QStringLiteral("The article of the hardware configuration is not given");

		return false;
	}

	if (configurationInfo.GetName().trimmed().isEmpty()){
		errorMessage = QStringLiteral("The name of the hardware configuration is not given");

		return false;
	}

	if (configurationInfo.GetProductId().isEmpty()){
		errorMessage = QStringLiteral("The hardware model of the configuration is not given");

		return false;
	}

	return true;
}


} // namespace prolifegql


