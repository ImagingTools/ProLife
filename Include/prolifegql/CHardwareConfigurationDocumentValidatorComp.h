#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentValidator.h>


namespace prolifegql
{


/**
	Refuses a hardware configuration without an article, a name or a hardware model,
	and one referring to a missing model or to missing components.
*/
class CHardwareConfigurationDocumentValidatorComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentValidator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareConfigurationDocumentValidatorComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Products: the hardware model of a configuration has to exist there", true, "ProductCollection");
		I_ASSIGN(m_componentCollectionCompPtr, "HardwareComponentCollection", "Components: the components of a configuration have to exist there", true, "HardwareComponentCollection");
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentValidator)
	virtual bool ValidateDocumentData(
				const istd::IChangeable& document,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_componentCollectionCompPtr);
};


} // namespace prolifegql


