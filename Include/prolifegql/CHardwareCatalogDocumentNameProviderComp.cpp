#include <prolifegql/CHardwareCatalogDocumentNameProviderComp.h>


// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>
#include <prolifedata/IHardwareConfigurationInfo.h>


namespace prolifegql
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CHardwareCatalogDocumentNameProviderComp::GetDefaultDocumentName(
			const istd::IChangeable& document,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const prolifedata::IHardwareComponentInfo* componentInfoPtr = dynamic_cast<const prolifedata::IHardwareComponentInfo*>(&document);
	if (componentInfoPtr != nullptr){
		return componentInfoPtr->GetName();
	}

	const prolifedata::IHardwareConfigurationInfo* configurationInfoPtr = dynamic_cast<const prolifedata::IHardwareConfigurationInfo*>(&document);
	if (configurationInfoPtr != nullptr){
		return configurationInfoPtr->GetName();
	}

	return QString();
}


} // namespace prolifegql


