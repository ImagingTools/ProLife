#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>

namespace prolifedata
{


class IHardwareProductBinding: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_HARDWARE_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_SOFTWARE_IDS
	};
	
	/**
		Get hardware-ID.
	*/
	virtual QByteArray GetHardwareId() const = 0;

	/**
		Set hardware-ID.
	*/
	virtual void SetHardwareId(const QByteArray& hardwareId) = 0;

	/**
		Get software-IDs binding with this hardware product.
	*/
	virtual QByteArrayList GetSoftwareIds() const = 0;

	/**
		Set software-IDs binding with this hardware product.
	*/
	virtual void SetSoftwareIds(QByteArrayList softwareIds) = 0;

	/**
		Bind software product with this hardware product.
	*/
	virtual bool Bind(const QByteArray& softwareId) = 0;

	/**
		Unbind software product
	*/
	virtual bool Unbind(const QByteArray& softwareId) = 0;
};


} // namespace prolifedata


