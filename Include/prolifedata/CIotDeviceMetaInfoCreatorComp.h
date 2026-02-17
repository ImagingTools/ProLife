#pragma once


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/CObjectMetaInfoCreatorCompBase.h>
#include <imtbase/IObjectCollection.h>


namespace prolifedata
{


class CIotDeviceMetaInfoCreatorComp: public imtbase::CObjectMetaInfoCreatorCompBase
{
public:
	typedef imtbase::CObjectMetaInfoCreatorCompBase BaseClass;

	I_BEGIN_COMPONENT(CIotDeviceMetaInfoCreatorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, idoc::MetaInfoPtr& metaInfoPtr) const override;

private:
	class MetaInfo: public idoc::CStandardDocumentMetaInfo
	{
	public:
		typedef idoc::CStandardDocumentMetaInfo BaseClass;

		// reimplemented (idoc::IDocumentMetaInfo)
		virtual QString GetMetaInfoName(int metaInfoType) const override;
	};
};


} // namespace prolifedata


