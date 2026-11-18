#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentValidator.h>


namespace prolifedata
{


class CDeviceDocumentValidatorComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentValidator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceDocumentValidatorComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentValidator);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Device object collection for uniqueness validation", true, "ObjectCollection");
		I_ASSIGN(m_configurationCollectionCompPtr, "HardwareConfigurationCollection", "Hardware configurations: the configuration of a device has to belong to its model", true, "HardwareConfigurationCollection");
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentValidator)
	virtual bool ValidateDocumentData(
				const istd::IChangeable& document,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_configurationCollectionCompPtr);
};


} // namespace prolifedata


