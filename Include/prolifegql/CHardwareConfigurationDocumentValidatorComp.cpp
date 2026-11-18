#include <prolifegql/CHardwareConfigurationDocumentValidatorComp.h>


// ProLife includes
#include <prolifegql/HardwareCatalog.h>


namespace prolifegql
{


// reimplemented (imtdoc::IDocumentValidator)

bool CHardwareConfigurationDocumentValidatorComp::ValidateDocumentData(
			const istd::IChangeable& document,
			QString& errorMessage,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const prolifedata::IHardwareConfigurationInfo* infoPtr = dynamic_cast<const prolifedata::IHardwareConfigurationInfo*>(&document);
	if (infoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to validate the document. Error: It is not a hardware configuration");

		return false;
	}

	if (!CheckHardwareConfigurationFields(*infoPtr, errorMessage)){
		return false;
	}

	if (!CheckHardwareModel(*m_productCollectionCompPtr, infoPtr->GetProductId(), errorMessage)){
		return false;
	}

	return CheckHardwareComponents(*m_componentCollectionCompPtr, infoPtr->GetComponentIds(), errorMessage);
}


} // namespace prolifegql


