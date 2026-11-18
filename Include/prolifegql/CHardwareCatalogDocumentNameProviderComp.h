#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdoc/IDocumentNameProvider.h>


namespace prolifegql
{


/**
	Names a hardware component or a hardware configuration document by its name.
*/
class CHardwareCatalogDocumentNameProviderComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentNameProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareCatalogDocumentNameProviderComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentNameProvider);
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentNameProvider)
	virtual QString GetDefaultDocumentName(
				const istd::IChangeable& document,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
};


} // namespace prolifegql


