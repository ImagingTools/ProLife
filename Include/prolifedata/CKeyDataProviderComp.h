#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtlic/CProductInstanceInfo.h>


namespace prolifedata
{


/**
	This component accepts data and returns it in encrypted form
*/
class CKeyDataProviderComp:
			public imtbase::IBinaryDataProvider,
			public ilog::CLoggerComponentBase,
			virtual public imtcrypt::IEncryptionKeysProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CKeyDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IBinaryDataProvider);
		I_REGISTER_INTERFACE(imtcrypt::IEncryptionKeysProvider);
		I_ASSIGN(m_licensePersistenceCompPtr, "LicensePersistence", "Persistence used for license export", false, "LicensePersistence");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Binding collection", true, "BindingCollection");
		I_ASSIGN(m_vectorKeyCompPtr, "VectorKey", "Additional key for AES encryption", false, "VectorKey");
		I_ASSIGN(m_gqlLicenseRequestCompPtr, "GqlLicenseRequest", "License GraphQL request", true, "GqlLicenseRequest");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
	I_END_COMPONENT;

	enum CommandGroup
	{
		CG_LICENSE = 2150
	};

	// reimplemented (imtbase::IBinaryDataProvider)
	virtual bool GetData(QByteArray& data, const QByteArray& dataId) const override;

	// reimplemented (imtcrypt::IEncryptionKeysProvider)
	virtual QByteArray GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const override;

protected:
	virtual QByteArrayList GetAllLicenseDependencies(const QByteArray& licenseId, const imtbase::CTreeItemModel& dependenciesModel) const;
	virtual QString GetLicenseName(const QByteArray& licenseId, const imtbase::CTreeItemModel& licensesModel) const;

private:
	mutable QByteArray m_productInstanceId;

	I_REF(ifile::IFilePersistence, m_licensePersistenceCompPtr);
	I_REF(iprm::IIdParam, m_vectorKeyCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_gqlLicenseRequestCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
};


} // namespace prolifedata


