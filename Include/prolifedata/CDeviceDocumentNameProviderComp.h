#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/IDocumentNameProvider.h>


namespace prolifedata
{


class CDeviceDocumentNameProviderComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentNameProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceDocumentNameProviderComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentNameProvider);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Device object collection for name resolution", true, "ObjectCollection");
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentNameProvider)
	virtual QString GetDefaultDocumentName(
				const QByteArray& objectId,
				const istd::IChangeable& document) const override;

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace prolifedata


