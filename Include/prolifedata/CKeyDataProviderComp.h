// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

﻿#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtrest/IBinaryDataProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtbase/IOperationContextController.h>
#include <imtlic/CProductInstanceInfo.h>


namespace prolifedata
{


/**
	This component accepts data and returns it in encrypted form
*/
class CKeyDataProviderComp:
			public imtrest::IBinaryDataProvider,
			public ilog::CLoggerComponentBase,
			virtual public imtcrypt::IEncryptionKeysProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CKeyDataProviderComp);
		I_REGISTER_INTERFACE(imtrest::IBinaryDataProvider);
		I_REGISTER_INTERFACE(imtcrypt::IEncryptionKeysProvider);
		I_ASSIGN(m_licensePersistenceCompPtr, "LicensePersistence", "Persistence used for license export", false, "LicensePersistence");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Binding collection", true, "BindingCollection");
		I_ASSIGN(m_vectorKeyCompPtr, "VectorKey", "Additional key for AES encryption", false, "VectorKey");
		I_ASSIGN(m_gqlLicenseRequestCompPtr, "GqlLicenseRequest", "License GraphQL request", true, "GqlLicenseRequest");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_deviceOperationContextControllerCompPtr, "DeviceOperationContextController", "Device operation context controller", true, "DeviceOperationContextController");
		I_ASSIGN(m_softwareOperationContextControllerCompPtr, "SoftwareOperationContextController", "Software operation context controller", true, "SoftwareOperationContextController");
	I_END_COMPONENT;

	enum CommandGroup
	{
		CG_LICENSE = 2150
	};

	// reimplemented (imtrest::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

	// reimplemented (imtcrypt::IEncryptionKeysProvider)
	virtual QByteArray GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const override;

protected:
	virtual QByteArrayList GetAllLicenseDependencies(const QByteArray& licenseId, const imtbase::CTreeItemModel& licensesModel) const;
	virtual QByteArrayList GetAllLicenseDependencies(const QByteArray& licenseId) const;

	virtual QByteArray GetLicenseId(const QByteArray& licenseUuid, const imtbase::CTreeItemModel& licensesModel) const;
	virtual QString GetLicenseName(const QByteArray& licenseId, const imtbase::CTreeItemModel& licensesModel) const;

	virtual istd::TUniqueInterfacePtr<imtlic::CLicenseDefinition> GetLicenseInfo(const QByteArray& licenseId) const;

	virtual QByteArray GetFeatureId(const QByteArray& featureUuid, const imtbase::CTreeItemModel& featuresModel) const;
	virtual QString GetFeatureName(const QByteArray& featureUuid, const imtbase::CTreeItemModel& featuresModel) const;

	virtual imtbase::CTreeItemModel* GetRemoteCollectionData(const QByteArray& collectionCommandId, QByteArrayList fields) const;
	virtual imtbase::CTreeItemModel* ExtractItemsFromResponseModel(const imtbase::CTreeItemModel& responseModel) const;

private:
	mutable QByteArray m_productInstanceId;

	I_REF(ifile::IFilePersistence, m_licensePersistenceCompPtr);
	I_REF(iprm::IIdParam, m_vectorKeyCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtgql::IGqlRequestHandler, m_gqlLicenseRequestCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);

	I_REF(imtbase::IOperationContextController, m_deviceOperationContextControllerCompPtr);
	I_REF(imtbase::IOperationContextController, m_softwareOperationContextControllerCompPtr);
};


} // namespace prolifedata


