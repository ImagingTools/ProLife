#include <prolifegql/CHardwareComponentDocumentValidatorComp.h>


// ProLife includes
#include <prolifegql/HardwareCatalog.h>


namespace prolifegql
{


// reimplemented (imtdoc::IDocumentValidator)

bool CHardwareComponentDocumentValidatorComp::ValidateDocumentData(
			const istd::IChangeable& document,
			QString& errorMessage,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const prolifedata::IHardwareComponentInfo* infoPtr = dynamic_cast<const prolifedata::IHardwareComponentInfo*>(&document);
	if (infoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to validate the document. Error: It is not a hardware component");

		return false;
	}

	if (!CheckHardwareComponentFields(*infoPtr, errorMessage)){
		return false;
	}

	return CheckHardwareComponentVendors(*infoPtr, *m_companyCollectionCompPtr, errorMessage);
}


} // namespace prolifegql


