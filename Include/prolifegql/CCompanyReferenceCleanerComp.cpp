#include <prolifegql/CCompanyReferenceCleanerComp.h>


// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (CHardwareReferenceCleanerCompBase)

bool CCompanyReferenceCleanerComp::RemoveReferences(istd::IChangeable& object, const QByteArrayList& removedIds) const
{
	prolifedata::IHardwareComponentInfo* componentInfoPtr = dynamic_cast<prolifedata::IHardwareComponentInfo*>(&object);
	if (componentInfoPtr == nullptr){
		return false;
	}

	bool changed = false;

	if (removedIds.contains(componentInfoPtr->GetManufacturerId())){
		componentInfoPtr->SetManufacturerId(QByteArray());
		changed = true;
	}

	const prolifedata::IHardwareComponentInfo::SupplierOffers offers = componentInfoPtr->GetSupplierOffers();

	prolifedata::IHardwareComponentInfo::SupplierOffers keptOffers;
	for (const prolifedata::IHardwareComponentInfo::SupplierOffer& offer : offers){
		if (!removedIds.contains(offer.supplierId)){
			keptOffers << offer;
		}
	}

	if (keptOffers.count() != offers.count()){
		componentInfoPtr->SetSupplierOffers(keptOffers);
		changed = true;
	}

	return changed;
}


} // namespace prolifegql


