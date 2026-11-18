#pragma once


// ProLife includes
#include <prolifegql/CHardwareReferenceCleanerCompBase.h>


namespace prolifegql
{


/**
	A removed company stops being the manufacturer or a supplier of the components.
*/
class CCompanyReferenceCleanerComp: public CHardwareReferenceCleanerCompBase
{
public:
	typedef CHardwareReferenceCleanerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCompanyReferenceCleanerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (CHardwareReferenceCleanerCompBase)
	virtual bool RemoveReferences(istd::IChangeable& object, const QByteArrayList& removedIds) const override;
};


} // namespace prolifegql


