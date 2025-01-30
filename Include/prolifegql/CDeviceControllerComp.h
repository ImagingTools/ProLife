#pragma once


// ACF includes
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <imtcrypt/IEncryption.h>

// ProLife includes
#include <prolifedata/IHardwareProductBinding.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Sensors.h>


#undef GetObject


namespace prolifegql
{


class CDeviceControllerComp:
			public sdl::prolife::Sensors::V1_0::CGraphQlHandlerCompBase,
			virtual public imtcrypt::IEncryptionKeysProvider
{
public:
	typedef sdl::prolife::Sensors::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceControllerComp)
		I_REGISTER_INTERFACE(imtcrypt::IEncryptionKeysProvider);
		I_ASSIGN(m_vectorKeyCompPtr, "VectorKey", "Additional key for AES encryption", false, "VectorKey");
		I_ASSIGN(m_basedPersistenceCompPtr, "LicensePersistence", "Persistence used for license export", false, "LicensePersistence");
		I_ASSIGN(m_encryptionBasedPersistenceCompPtr, "EncryptionBasedPersistence", "Encryption persistence used for license export", false, "EncryptionBasedPersistence");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_deviceBindingCollectionCompPtr, "DeviceBindingCollection", "Device binding collection", true, "DeviceBindingCollection");
		I_ASSIGN(m_deviceOperationContextControllerCompPtr, "DeviceOperationContextController", "Device operation context controller", true, "DeviceOperationContextController");
		I_ASSIGN(m_softwareOperationContextControllerCompPtr, "SoftwareOperationContextController", "Software operation context controller", true, "SoftwareOperationContextController");
		I_ASSIGN(m_encryptionCompPtr, "Encryption", "Encrypt/Decrypt instances", true, "Encryption");
	I_END_COMPONENT

protected:
	virtual sdl::imtbase::ImtCollection::CVisualStatus::V1_0 OnGetObjectVisualStatus(const sdl::prolife::Sensors::V1_0::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CDeviceBindingData::V1_0 OnGetDeviceBinding(const sdl::prolife::Sensors::V1_0::CGetDeviceBindingGqlRequest& getDeviceBindingRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 OnUpdateDeviceBinding(const sdl::prolife::Sensors::V1_0::CUpdateDeviceBindingGqlRequest& updateDeviceBindingRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CTransferLicensesPayload::V1_0 OnTransferLicenses(const sdl::prolife::Sensors::V1_0::CTransferLicensesGqlRequest& transferLicensesRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CCreateLicenseFilePayload::V1_0 OnCreateLicenseFile(const sdl::prolife::Sensors::V1_0::CCreateLicenseFileGqlRequest& createLicenseFileRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CDecryptLicenseFilePayload::V1_0 OnDecryptLicenseFile(
		const sdl::prolife::Sensors::V1_0::CDecryptLicenseFileGqlRequest& decryptLicenseFileRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const override;

	// reimplemented (imtcrypt::IEncryptionKeysProvider)
	virtual QByteArray GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const override;

private:
	prolifedata::IHardwareProductBinding* GetOrCreateDeviceBinding(const QByteArray& deviceId) const;
	void CreateDeviceOperationContext(const QByteArray& deviceId, const QByteArray& project, QByteArrayList addedLicenses, QByteArrayList removedLicenses) const;
	void CreateSoftwareOperationContext(const QByteArray& deviceId, const QByteArray& project, QByteArrayList addedLicenses, QByteArrayList removedLicenses) const;
	QByteArrayList GetAllLicenseDependencies(const QByteArray& licenseId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceBindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_REF(ifile::IFilePersistence, m_basedPersistenceCompPtr);
	I_REF(ifile::IFilePersistence, m_encryptionBasedPersistenceCompPtr);
	I_REF(iprm::IIdParam, m_vectorKeyCompPtr);

	I_REF(imtbase::IOperationContextController, m_deviceOperationContextControllerCompPtr);
	I_REF(imtbase::IOperationContextController, m_softwareOperationContextControllerCompPtr);
	I_REF(imtcrypt::IEncryption, m_encryptionCompPtr);

private:
	mutable QByteArray m_productInstanceId;
};


} // namespace prolifegql


