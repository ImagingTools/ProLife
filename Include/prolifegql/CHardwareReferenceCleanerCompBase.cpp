#include <prolifegql/CHardwareReferenceCleanerCompBase.h>


namespace prolifegql
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CHardwareReferenceCleanerCompBase::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!changeSet.Contains(imtbase::ICollectionInfo::CF_REMOVED)){
		return;
	}

	const QVariant changeInfo = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED);
	if (!changeInfo.isValid()){
		return;
	}

	const QByteArrayList removedIds = changeInfo.value<imtbase::ICollectionInfo::MultiElementNotifierInfo>().elementIds;
	if (removedIds.isEmpty()){
		return;
	}

	const imtbase::ICollectionInfo::Ids objectIds = m_dependentCollectionCompPtr->GetElementIds();
	for (const QByteArray& objectId : objectIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_dependentCollectionCompPtr->GetObjectData(objectId, dataPtr) || !dataPtr.IsValid()){
			continue;
		}

		if (!RemoveReferences(*dataPtr, removedIds)){
			continue;
		}

		if (!m_dependentCollectionCompPtr->SetObjectData(objectId, *dataPtr)){
			SendErrorMessage(0, QStringLiteral("Unable to remove the references to the removed objects from '%1'").arg(QString::fromUtf8(objectId)));
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CHardwareReferenceCleanerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_sourceCollectionModelCompPtr.IsValid()){
		m_sourceCollectionModelCompPtr->AttachObserver(this);
	}
}


void CHardwareReferenceCleanerCompBase::OnComponentDestroyed()
{
	EnsureModelDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace prolifegql


