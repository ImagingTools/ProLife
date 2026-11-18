#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QStringList>

// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


/**
	Variant of a hardware model that can be ordered and built: the components it takes and the
	software licenses that ship with it. A device refers to its configuration, not to a license.
*/
class IHardwareConfigurationInfo: virtual public iser::IObject
{
public:
	/**
		Article (order code) of the configuration
	*/
	virtual QByteArray GetConfigurationId() const = 0;
	virtual void SetConfigurationId(const QByteArray& configurationId) = 0;
	/**
		Configuration name
	*/
	virtual QString GetName() const = 0;
	virtual void SetName(const QString& name) = 0;
	/**
		Configuration description
	*/
	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& description) = 0;
	/**
		ID of the hardware model (product) the configuration belongs to
	*/
	virtual QByteArray GetProductId() const = 0;
	virtual void SetProductId(const QByteArray& productId) = 0;
	/**
		IDs of the components taken by the configuration
	*/
	virtual QByteArrayList GetComponentIds() const = 0;
	virtual void SetComponentIds(const QByteArrayList& componentIds) = 0;
};


} // namespace prolifedata


