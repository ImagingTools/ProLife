#pragma once


// ACF includes
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace prolifegql
{


/**
	Removes the references to the objects removed from the source collection out of the objects of the dependent collection.
	A derived class knows the reference of its object type.
*/
class CHardwareReferenceCleanerCompBase: public ilog::CLoggerComponentBase, protected imod::CSingleModelObserverBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CHardwareReferenceCleanerCompBase);
		I_ASSIGN(m_sourceCollectionCompPtr, "SourceCollection", "Collection whose removed objects are referred to", true, "SourceCollection");
		I_ASSIGN_TO(m_sourceCollectionModelCompPtr, m_sourceCollectionCompPtr, true);
		I_ASSIGN(m_dependentCollectionCompPtr, "DependentCollection", "Collection whose objects refer to the source collection", true, "DependentCollection");
	I_END_COMPONENT;

protected:
	/**
		Removes the references to \a removedIds from \a object. Returns true if the object was changed.
	*/
	virtual bool RemoveReferences(istd::IChangeable& object, const QByteArrayList& removedIds) const = 0;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(imtbase::IObjectCollection, m_sourceCollectionCompPtr);
	I_REF(imod::IModel, m_sourceCollectionModelCompPtr);
	I_REF(imtbase::IObjectCollection, m_dependentCollectionCompPtr);
};


} // namespace prolifegql


