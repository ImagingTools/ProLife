#pragma once


// ProLife includes
#include <prolifegql/CHardwareReferenceCleanerCompBase.h>


namespace prolifegql
{


/**
	A removed hardware component is removed from the configurations that take it.
*/
class CHardwareComponentReferenceCleanerComp: public CHardwareReferenceCleanerCompBase
{
public:
	typedef CHardwareReferenceCleanerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareComponentReferenceCleanerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CHardwareReferenceCleanerCompBase)
	virtual bool RemoveReferences(istd::IChangeable& object, const QByteArrayList& removedIds) const override;
};


} // namespace prolifegql


