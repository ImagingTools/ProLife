#include <prolifegql/HardwareRepresentation.h>


// ImtCore includes
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/ICustomerInfo.h>


namespace prolifegql
{


void FillComponentFromRepresentation(
			const sdl::V1_0::prolife::CHardwareComponentData& representation,
			prolifedata::IHardwareComponentInfo& componentInfo)
{
	if (representation.partNumber){
		componentInfo.SetPartNumber(*representation.partNumber);
	}
	if (representation.name){
		componentInfo.SetName(*representation.name);
	}
	if (representation.description){
		componentInfo.SetDescription(*representation.description);
	}
	if (representation.componentType){
		componentInfo.SetComponentType(*representation.componentType);
	}
	if (representation.revision){
		componentInfo.SetRevision(*representation.revision);
	}
	if (representation.manufacturerId){
		componentInfo.SetManufacturerId(*representation.manufacturerId);
	}
	if (representation.mpn){
		componentInfo.SetMpn(*representation.mpn);
	}
	if (representation.supplierOffers){
		prolifedata::IHardwareComponentInfo::SupplierOffers offers;
		for (const sdl::V1_0::prolife::CHardwareSupplierOffer& offerRepresentation : representation.supplierOffers->ToList()){
			prolifedata::IHardwareComponentInfo::SupplierOffer offer;
			if (offerRepresentation.supplierId){
				offer.supplierId = *offerRepresentation.supplierId;
			}
			if (offerRepresentation.sku){
				offer.sku = *offerRepresentation.sku;
			}

			offers << offer;
		}

		componentInfo.SetSupplierOffers(offers);
	}
	if (representation.lifecycleStatus){
		componentInfo.SetLifecycleStatus(*representation.lifecycleStatus);
	}
	if (representation.serialized){
		componentInfo.SetSerialized(*representation.serialized);
	}
}


void FillRepresentationFromComponent(
			const prolifedata::IHardwareComponentInfo& componentInfo,
			const imtbase::IObjectCollection* companyCollectionPtr,
			sdl::V1_0::prolife::CHardwareComponentData& representation)
{
	representation.partNumber = componentInfo.GetPartNumber();
	representation.name = componentInfo.GetName();
	representation.description = componentInfo.GetDescription();
	representation.componentType = componentInfo.GetComponentType();
	representation.revision = componentInfo.GetRevision();
	representation.manufacturerId = componentInfo.GetManufacturerId();
	representation.manufacturerName = GetCompanyName(companyCollectionPtr, componentInfo.GetManufacturerId());
	representation.mpn = componentInfo.GetMpn();

	imtsdl::TElementList<sdl::V1_0::prolife::CHardwareSupplierOffer> offerRepresentations;
	const prolifedata::IHardwareComponentInfo::SupplierOffers offers = componentInfo.GetSupplierOffers();
	for (const prolifedata::IHardwareComponentInfo::SupplierOffer& offer : offers){
		sdl::V1_0::prolife::CHardwareSupplierOffer offerRepresentation;
		offerRepresentation.supplierId = offer.supplierId;
		offerRepresentation.supplierName = GetCompanyName(companyCollectionPtr, offer.supplierId);
		offerRepresentation.sku = offer.sku;

		offerRepresentations << offerRepresentation;
	}
	representation.supplierOffers = offerRepresentations;
	representation.lifecycleStatus = componentInfo.GetLifecycleStatus();
	representation.serialized = componentInfo.IsSerialized();
}


QString GetCompanyName(const imtbase::IObjectCollection* companyCollectionPtr, const QByteArray& companyId)
{
	if (companyId.isEmpty()){
		return QString();
	}

	imtbase::IObjectCollection::DataPtr companyDataPtr;
	if (companyCollectionPtr != nullptr && companyCollectionPtr->GetObjectData(companyId, companyDataPtr)){
		const prolifedata::ICustomerInfo* companyInfoPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(companyDataPtr.GetPtr());
		if (companyInfoPtr != nullptr){
			return companyInfoPtr->GetName();
		}
	}

	return QString::fromUtf8(companyId);
}


QString GetProductName(const imtbase::IObjectCollection* productCollectionPtr, const QByteArray& productId)
{
	imtbase::IObjectCollection::DataPtr productDataPtr;
	if (!productId.isEmpty() && productCollectionPtr != nullptr && productCollectionPtr->GetObjectData(productId, productDataPtr)){
		const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			return productInfoPtr->GetName();
		}
	}

	return QString();
}


void FillConfigurationFromRepresentation(
			const sdl::V1_0::prolife::CHardwareConfigurationData& representation,
			prolifedata::IHardwareConfigurationInfo& configurationInfo)
{
	if (representation.configurationId){
		configurationInfo.SetConfigurationId(*representation.configurationId);
	}
	if (representation.name){
		configurationInfo.SetName(*representation.name);
	}
	if (representation.description){
		configurationInfo.SetDescription(*representation.description);
	}
	if (representation.productId){
		configurationInfo.SetProductId(*representation.productId);
	}
	if (representation.componentIds){
		configurationInfo.SetComponentIds(representation.componentIds->ToList());
	}
}


void FillRepresentationFromConfiguration(
			const prolifedata::IHardwareConfigurationInfo& configurationInfo,
			sdl::V1_0::prolife::CHardwareConfigurationData& representation)
{
	representation.configurationId = configurationInfo.GetConfigurationId();
	representation.name = configurationInfo.GetName();
	representation.description = configurationInfo.GetDescription();
	representation.productId = configurationInfo.GetProductId();
	representation.componentIds.Emplace().FromList(configurationInfo.GetComponentIds());
}


imtsdl::TElementList<sdl::V1_0::prolife::CHardwareConfigurationComponent> CreateComponentsRepresentation(
			const imtbase::IObjectCollection* componentCollectionPtr,
			const QByteArrayList& componentIds)
{
	imtsdl::TElementList<sdl::V1_0::prolife::CHardwareConfigurationComponent> components;
	for (const QByteArray& componentId : componentIds){
		sdl::V1_0::prolife::CHardwareConfigurationComponent componentRepresentation;
		componentRepresentation.id = componentId;
		componentRepresentation.name = QString::fromUtf8(componentId);
		componentRepresentation.partNumber = QString();

		imtbase::IObjectCollection::DataPtr componentDataPtr;
		if (componentCollectionPtr != nullptr && componentCollectionPtr->GetObjectData(componentId, componentDataPtr)){
			const prolifedata::IHardwareComponentInfo* componentInfoPtr = dynamic_cast<const prolifedata::IHardwareComponentInfo*>(componentDataPtr.GetPtr());
			if (componentInfoPtr != nullptr){
				componentRepresentation.name = componentInfoPtr->GetName();
				componentRepresentation.partNumber = QString::fromUtf8(componentInfoPtr->GetPartNumber());
			}
		}

		components << componentRepresentation;
	}

	return components;
}


} // namespace prolifegql


