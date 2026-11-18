#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentValidator.h>


namespace prolifegql
{


/**
	Refuses a hardware component without a part number or a name, and one referring to a missing company or to a company without the role.
*/
class CHardwareComponentDocumentValidatorComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentValidator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareComponentDocumentValidatorComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator);
		I_ASSIGN(m_companyCollectionCompPtr, "CompanyCollection", "Companies: the manufacturer and the suppliers of a component are checked against it and shown by name", true, "CompanyCollection");
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentValidator)
	virtual bool ValidateDocumentData(
				const istd::IChangeable& document,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_companyCollectionCompPtr);
};


} // namespace prolifegql


