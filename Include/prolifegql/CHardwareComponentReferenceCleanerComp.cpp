#include <prolifegql/CHardwareComponentReferenceCleanerComp.h>


// ProLife includes
#include <prolifedata/IHardwareConfigurationInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (CHardwareReferenceCleanerCompBase)

bool CHardwareComponentReferenceCleanerComp::RemoveReferences(istd::IChangeable& object, const QByteArrayList& removedIds) const
{
	prolifedata::IHardwareConfigurationInfo* configurationInfoPtr = dynamic_cast<prolifedata::IHardwareConfigurationInfo*>(&object);
	if (configurationInfoPtr == nullptr){
		return false;
	}

	const QByteArrayList componentIds = configurationInfoPtr->GetComponentIds();

	QByteArrayList keptIds;
	for (const QByteArray& componentId : componentIds){
		if (!removedIds.contains(componentId)){
			keptIds << componentId;
		}
	}

	if (keptIds.count() == componentIds.count()){
		return false;
	}

	configurationInfoPtr->SetComponentIds(keptIds);

	return true;
}


} // namespace prolifegql


