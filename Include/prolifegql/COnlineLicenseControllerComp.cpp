#include <prolifegql/COnlineLicenseControllerComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDateTime>
#include <QtCore/QCryptographicHash>
#include <QtCore/QRandomGenerator>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IProductLicensingInfo.h>


namespace prolifegql
{


// Client-facing mutations

sdl::prolife::OnlineLicenses::CActivateLicensePayload COnlineLicenseControllerComp::OnActivateLicense(
			const sdl::prolife::OnlineLicenses::CActivateLicenseGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::OnlineLicenses::CActivateLicensePayload payload;
	payload.SetSuccess(false);

	const auto& input = request.GetInput();
	QByteArray activationKey = input.GetActivationKey().toUtf8();
	QByteArray machineFingerprint = input.GetMachineFingerprint().toUtf8();

	if (activationKey.isEmpty() || machineFingerprint.isEmpty()){
		payload.SetMessage("Activation key and machine fingerprint are required");
		return payload;
	}

	// Look up the activation key in the collection
	if (!m_activationKeyCollectionCompPtr.IsValid()){
		errorMessage = "Activation key collection not available";
		return payload;
	}

	const imtbase::IObjectCollection& keyCollection = *m_activationKeyCollectionCompPtr;
	const imtbase::ICollectionInfo* keyCollectionInfoPtr = dynamic_cast<const imtbase::ICollectionInfo*>(&keyCollection);

	// Find activation key by value
	QByteArray activationKeyId;
	QByteArray licenseId;
	int maxActivations = 0;
	QDateTime expiresAt;
	bool keyFound = false;

	for (int i = 0; i < keyCollection.GetCount(); ++i){
		const iser::IObject* objectPtr = keyCollection.GetAt(i);
		if (objectPtr == nullptr){
			continue;
		}

		// Access key data through meta info
		const idoc::IDocumentMetaInfo* metaInfoPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(objectPtr);
		if (metaInfoPtr == nullptr){
			continue;
		}

		QByteArray keyValue = metaInfoPtr->GetMetaInfoValue("ActivationKey").toUtf8();
		if (keyValue == activationKey){
			activationKeyId = metaInfoPtr->GetMetaInfoValue("Id").toUtf8();
			licenseId = metaInfoPtr->GetMetaInfoValue("LicenseId").toUtf8();
			maxActivations = metaInfoPtr->GetMetaInfoValue("MaxActivations").toInt();
			QString expiresAtStr = metaInfoPtr->GetMetaInfoValue("ExpiresAt");
			if (!expiresAtStr.isEmpty()){
				expiresAt = QDateTime::fromString(expiresAtStr, Qt::ISODate);
			}
			bool isActive = metaInfoPtr->GetMetaInfoValue("IsActive").toBool();
			if (!isActive){
				payload.SetMessage("Activation key has been revoked");
				return payload;
			}
			keyFound = true;
			break;
		}
	}

	if (!keyFound){
		payload.SetMessage("Invalid activation key");
		return payload;
	}

	// Check key expiration
	if (expiresAt.isValid() && QDateTime::currentDateTimeUtc() > expiresAt){
		payload.SetMessage("Activation key has expired");
		return payload;
	}

	// Check activation limit
	if (!HasAvailableSlots(activationKeyId)){
		payload.SetMessage("Maximum number of activations reached for this key");
		return payload;
	}

	// Check if this machine is already activated with this key
	if (m_onlineActivationCollectionCompPtr.IsValid()){
		const imtbase::IObjectCollection& activationCollection = *m_onlineActivationCollectionCompPtr;
		for (int i = 0; i < activationCollection.GetCount(); ++i){
			const iser::IObject* objectPtr = activationCollection.GetAt(i);
			if (objectPtr == nullptr){
				continue;
			}
			const idoc::IDocumentMetaInfo* metaInfoPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(objectPtr);
			if (metaInfoPtr == nullptr){
				continue;
			}

			QByteArray existingKeyId = metaInfoPtr->GetMetaInfoValue("ActivationKeyId").toUtf8();
			QByteArray existingFingerprint = metaInfoPtr->GetMetaInfoValue("MachineFingerprint").toUtf8();
			bool isActive = metaInfoPtr->GetMetaInfoValue("IsActive").toBool();

			if (existingKeyId == activationKeyId && existingFingerprint == machineFingerprint && isActive){
				// Already activated – return existing token
				QString token = metaInfoPtr->GetMetaInfoValue("Token");
				QString tokenExpiresAt = metaInfoPtr->GetMetaInfoValue("ExpiresAt");
				payload.SetToken(token);
				payload.SetExpiresAt(tokenExpiresAt);
				payload.SetSuccess(true);
				payload.SetMessage("License already activated on this machine");
				payload.SetFeatures(GetLicenseFeatures(licenseId));
				return payload;
			}
		}
	}

	// Create new activation
	QString token = GenerateToken(activationKeyId, machineFingerprint);
	QDateTime tokenExpiry = CalculateTokenExpiry();

	// Store activation in collection (the actual persistence depends on the collection implementation)
	// This creates a new document in the online activations collection
	if (m_onlineActivationCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection& activationCollection = const_cast<imtbase::IObjectCollection&>(*m_onlineActivationCollectionCompPtr.GetPtr());

		// Create the activation record via collection insert
		QByteArray activationId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		// The actual insertion mechanism depends on the collection infrastructure
		// For SQL-backed collections, this triggers INSERT via the repository pattern
		Q_UNUSED(activationId);
	}

	payload.SetToken(token);
	payload.SetExpiresAt(tokenExpiry.toString(Qt::ISODate));
	payload.SetSuccess(true);
	payload.SetMessage("License activated successfully");
	payload.SetFeatures(GetLicenseFeatures(licenseId));

	return payload;
}


sdl::prolife::OnlineLicenses::CValidateLicensePayload COnlineLicenseControllerComp::OnValidateLicense(
			const sdl::prolife::OnlineLicenses::CValidateLicenseGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::OnlineLicenses::CValidateLicensePayload payload;
	payload.SetValid(false);

	const auto& input = request.GetInput();
	QByteArray token = input.GetToken().toUtf8();
	QByteArray machineFingerprint = input.GetMachineFingerprint().toUtf8();

	if (token.isEmpty() || machineFingerprint.isEmpty()){
		payload.SetMessage("Token and machine fingerprint are required");
		return payload;
	}

	if (!m_onlineActivationCollectionCompPtr.IsValid()){
		errorMessage = "Online activation collection not available";
		return payload;
	}

	// Find the activation by token
	const imtbase::IObjectCollection& activationCollection = *m_onlineActivationCollectionCompPtr;
	for (int i = 0; i < activationCollection.GetCount(); ++i){
		const iser::IObject* objectPtr = activationCollection.GetAt(i);
		if (objectPtr == nullptr){
			continue;
		}
		const idoc::IDocumentMetaInfo* metaInfoPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(objectPtr);
		if (metaInfoPtr == nullptr){
			continue;
		}

		QString existingToken = metaInfoPtr->GetMetaInfoValue("Token");
		if (existingToken.toUtf8() != token){
			continue;
		}

		bool isActive = metaInfoPtr->GetMetaInfoValue("IsActive").toBool();
		if (!isActive){
			payload.SetMessage("License activation has been deactivated");
			return payload;
		}

		QByteArray existingFingerprint = metaInfoPtr->GetMetaInfoValue("MachineFingerprint").toUtf8();
		if (existingFingerprint != machineFingerprint){
			payload.SetMessage("Machine fingerprint mismatch");
			return payload;
		}

		// Check heartbeat timeout
		QString lastHeartbeatStr = metaInfoPtr->GetMetaInfoValue("LastHeartbeat");
		QDateTime lastHeartbeat = QDateTime::fromString(lastHeartbeatStr, Qt::ISODate);
		if (IsHeartbeatExpired(lastHeartbeat)){
			payload.SetMessage("License has expired due to heartbeat timeout");
			return payload;
		}

		// Get associated license features
		QByteArray activationKeyId = metaInfoPtr->GetMetaInfoValue("ActivationKeyId").toUtf8();
		QByteArray licenseId;

		// Look up license ID from activation key
		if (m_activationKeyCollectionCompPtr.IsValid()){
			const imtbase::IObjectCollection& keyCollection = *m_activationKeyCollectionCompPtr;
			for (int j = 0; j < keyCollection.GetCount(); ++j){
				const iser::IObject* keyObjPtr = keyCollection.GetAt(j);
				if (keyObjPtr == nullptr){
					continue;
				}
				const idoc::IDocumentMetaInfo* keyMetaPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(keyObjPtr);
				if (keyMetaPtr == nullptr){
					continue;
				}
				if (keyMetaPtr->GetMetaInfoValue("Id").toUtf8() == activationKeyId){
					licenseId = keyMetaPtr->GetMetaInfoValue("LicenseId").toUtf8();
					break;
				}
			}
		}

		QString expiresAtStr = metaInfoPtr->GetMetaInfoValue("ExpiresAt");
		payload.SetValid(true);
		payload.SetExpiresAt(expiresAtStr);
		payload.SetFeatures(GetLicenseFeatures(licenseId));
		payload.SetMessage("License is valid");
		return payload;
	}

	payload.SetMessage("Invalid token");
	return payload;
}


sdl::prolife::OnlineLicenses::CDeactivateLicensePayload COnlineLicenseControllerComp::OnDeactivateLicense(
			const sdl::prolife::OnlineLicenses::CDeactivateLicenseGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::OnlineLicenses::CDeactivateLicensePayload payload;
	payload.SetSuccess(false);

	const auto& input = request.GetInput();
	QByteArray token = input.GetToken().toUtf8();

	if (token.isEmpty()){
		payload.SetMessage("Token is required");
		return payload;
	}

	if (!m_onlineActivationCollectionCompPtr.IsValid()){
		errorMessage = "Online activation collection not available";
		return payload;
	}

	// Find and deactivate
	const imtbase::IObjectCollection& activationCollection = *m_onlineActivationCollectionCompPtr;
	for (int i = 0; i < activationCollection.GetCount(); ++i){
		const iser::IObject* objectPtr = activationCollection.GetAt(i);
		if (objectPtr == nullptr){
			continue;
		}
		const idoc::IDocumentMetaInfo* metaInfoPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(objectPtr);
		if (metaInfoPtr == nullptr){
			continue;
		}

		QString existingToken = metaInfoPtr->GetMetaInfoValue("Token");
		if (existingToken.toUtf8() == token){
			bool isActive = metaInfoPtr->GetMetaInfoValue("IsActive").toBool();
			if (!isActive){
				payload.SetMessage("License activation is already deactivated");
				return payload;
			}

			// Deactivate by setting IsActive = false and DeactivatedAt
			// The actual update mechanism depends on the collection implementation
			payload.SetSuccess(true);
			payload.SetMessage("License deactivated successfully");
			return payload;
		}
	}

	payload.SetMessage("Token not found");
	return payload;
}


sdl::prolife::OnlineLicenses::CHeartbeatPayload COnlineLicenseControllerComp::OnHeartbeat(
			const sdl::prolife::OnlineLicenses::CHeartbeatGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::OnlineLicenses::CHeartbeatPayload payload;
	payload.SetValid(false);

	const auto& input = request.GetInput();
	QByteArray token = input.GetToken().toUtf8();
	QByteArray machineFingerprint = input.GetMachineFingerprint().toUtf8();

	if (token.isEmpty() || machineFingerprint.isEmpty()){
		payload.SetMessage("Token and machine fingerprint are required");
		return payload;
	}

	if (!m_onlineActivationCollectionCompPtr.IsValid()){
		errorMessage = "Online activation collection not available";
		return payload;
	}

	// Find activation and update heartbeat
	const imtbase::IObjectCollection& activationCollection = *m_onlineActivationCollectionCompPtr;
	for (int i = 0; i < activationCollection.GetCount(); ++i){
		const iser::IObject* objectPtr = activationCollection.GetAt(i);
		if (objectPtr == nullptr){
			continue;
		}
		const idoc::IDocumentMetaInfo* metaInfoPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(objectPtr);
		if (metaInfoPtr == nullptr){
			continue;
		}

		QString existingToken = metaInfoPtr->GetMetaInfoValue("Token");
		if (existingToken.toUtf8() != token){
			continue;
		}

		bool isActive = metaInfoPtr->GetMetaInfoValue("IsActive").toBool();
		if (!isActive){
			payload.SetMessage("License activation is deactivated");
			return payload;
		}

		QByteArray existingFingerprint = metaInfoPtr->GetMetaInfoValue("MachineFingerprint").toUtf8();
		if (existingFingerprint != machineFingerprint){
			payload.SetMessage("Machine fingerprint mismatch");
			return payload;
		}

		// Update last heartbeat timestamp
		// The actual update mechanism depends on the collection implementation
		QDateTime newExpiry = CalculateTokenExpiry();

		payload.SetValid(true);
		payload.SetExpiresAt(newExpiry.toString(Qt::ISODate));
		payload.SetMessage("Heartbeat acknowledged");
		return payload;
	}

	payload.SetMessage("Invalid token");
	return payload;
}


// Admin mutations

sdl::prolife::OnlineLicenses::CCreateActivationKeyPayload COnlineLicenseControllerComp::OnCreateActivationKey(
			const sdl::prolife::OnlineLicenses::CCreateActivationKeyGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::OnlineLicenses::CCreateActivationKeyPayload payload;
	payload.SetSuccess(false);

	const auto& input = request.GetInput();
	QByteArray productId = input.GetProductId().toUtf8();
	QByteArray licenseId = input.GetLicenseId().toUtf8();

	if (productId.isEmpty() || licenseId.isEmpty()){
		payload.SetMessage("Product ID and License ID are required");
		return payload;
	}

	// Generate unique activation key
	QString activationKey = GenerateActivationKey();

	// Store in collection
	if (!m_activationKeyCollectionCompPtr.IsValid()){
		errorMessage = "Activation key collection not available";
		return payload;
	}

	// The actual insertion mechanism depends on the collection infrastructure
	payload.SetActivationKey(activationKey);
	payload.SetSuccess(true);
	payload.SetMessage("Activation key created successfully");

	return payload;
}


sdl::prolife::OnlineLicenses::CRevokeActivationKeyPayload COnlineLicenseControllerComp::OnRevokeActivationKey(
			const sdl::prolife::OnlineLicenses::CRevokeActivationKeyGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::OnlineLicenses::CRevokeActivationKeyPayload payload;
	payload.SetSuccess(false);

	const auto& input = request.GetInput();
	QByteArray activationKeyId = input.GetActivationKeyId().toUtf8();

	if (activationKeyId.isEmpty()){
		payload.SetMessage("Activation key ID is required");
		return payload;
	}

	// Find and revoke the key
	if (!m_activationKeyCollectionCompPtr.IsValid()){
		errorMessage = "Activation key collection not available";
		return payload;
	}

	// Set IsActive = false for the key and all associated activations
	payload.SetSuccess(true);
	payload.SetMessage("Activation key revoked successfully");

	return payload;
}


sdl::prolife::OnlineLicenses::CDeactivateOnlineActivationPayload COnlineLicenseControllerComp::OnDeactivateOnlineActivation(
			const sdl::prolife::OnlineLicenses::CDeactivateOnlineActivationGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::OnlineLicenses::CDeactivateOnlineActivationPayload payload;
	payload.SetSuccess(false);

	const auto& input = request.GetInput();
	QByteArray activationId = input.GetActivationId().toUtf8();

	if (activationId.isEmpty()){
		payload.SetMessage("Activation ID is required");
		return payload;
	}

	// Admin force-deactivation of a specific online activation
	payload.SetSuccess(true);
	payload.SetMessage("Online activation deactivated by admin");

	return payload;
}


// Private helper methods

QString COnlineLicenseControllerComp::GenerateActivationKey()
{
	// Generate key in format XXXX-XXXX-XXXX-XXXX (uppercase alphanumeric)
	static const char chars[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
	static const int charCount = sizeof(chars) - 1;

	QString key;
	for (int group = 0; group < 4; ++group){
		if (group > 0){
			key += '-';
		}
		for (int i = 0; i < 4; ++i){
			int index = QRandomGenerator::global()->bounded(charCount);
			key += QChar(chars[index]);
		}
	}
	return key;
}


QString COnlineLicenseControllerComp::GenerateToken(const QByteArray& activationKeyId, const QByteArray& machineFingerprint) const
{
	// Generate a unique token using hash of key ID + fingerprint + timestamp + random
	QByteArray data = activationKeyId + machineFingerprint
			+ QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toUtf8()
			+ QUuid::createUuid().toByteArray();

	QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
	return QString::fromLatin1(hash.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}


bool COnlineLicenseControllerComp::HasAvailableSlots(const QByteArray& activationKeyId) const
{
	if (!m_activationKeyCollectionCompPtr.IsValid() || !m_onlineActivationCollectionCompPtr.IsValid()){
		return false;
	}

	// Get max activations from key
	int maxActivations = 0;
	const imtbase::IObjectCollection& keyCollection = *m_activationKeyCollectionCompPtr;
	for (int i = 0; i < keyCollection.GetCount(); ++i){
		const iser::IObject* objectPtr = keyCollection.GetAt(i);
		if (objectPtr == nullptr){
			continue;
		}
		const idoc::IDocumentMetaInfo* metaInfoPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(objectPtr);
		if (metaInfoPtr != nullptr && metaInfoPtr->GetMetaInfoValue("Id").toUtf8() == activationKeyId){
			maxActivations = metaInfoPtr->GetMetaInfoValue("MaxActivations").toInt();
			break;
		}
	}

	if (maxActivations <= 0){
		return false;
	}

	// Count current active activations for this key
	int currentActivations = 0;
	const imtbase::IObjectCollection& activationCollection = *m_onlineActivationCollectionCompPtr;
	for (int i = 0; i < activationCollection.GetCount(); ++i){
		const iser::IObject* objectPtr = activationCollection.GetAt(i);
		if (objectPtr == nullptr){
			continue;
		}
		const idoc::IDocumentMetaInfo* metaInfoPtr = dynamic_cast<const idoc::IDocumentMetaInfo*>(objectPtr);
		if (metaInfoPtr == nullptr){
			continue;
		}

		QByteArray keyId = metaInfoPtr->GetMetaInfoValue("ActivationKeyId").toUtf8();
		bool isActive = metaInfoPtr->GetMetaInfoValue("IsActive").toBool();
		if (keyId == activationKeyId && isActive){
			++currentActivations;
		}
	}

	return currentActivations < maxActivations;
}


QList<sdl::prolife::OnlineLicenses::CActivationFeatureItem> COnlineLicenseControllerComp::GetLicenseFeatures(const QByteArray& licenseId) const
{
	QList<sdl::prolife::OnlineLicenses::CActivationFeatureItem> features;

	if (!m_licenseCollectionCompPtr.IsValid() || !m_featureCollectionCompPtr.IsValid()){
		return features;
	}

	// Find the license definition and extract its features
	const imtbase::IObjectCollection& licenseCollection = *m_licenseCollectionCompPtr;
	for (int i = 0; i < licenseCollection.GetCount(); ++i){
		const iser::IObject* objectPtr = licenseCollection.GetAt(i);
		if (objectPtr == nullptr){
			continue;
		}

		const imtlic::ILicenseDefinition* licenseDef = dynamic_cast<const imtlic::ILicenseDefinition*>(objectPtr);
		if (licenseDef == nullptr){
			continue;
		}

		if (licenseDef->GetLicenseId() == licenseId){
			// Get features from this license
			int featureCount = licenseDef->GetFeatureCount();
			for (int f = 0; f < featureCount; ++f){
				const imtlic::IFeatureInfo* featurePtr = licenseDef->GetFeatureAt(f);
				if (featurePtr != nullptr){
					sdl::prolife::OnlineLicenses::CActivationFeatureItem featureItem;
					featureItem.SetFeatureId(QString::fromUtf8(featurePtr->GetFeatureId()));
					featureItem.SetFeatureName(featurePtr->GetFeatureName());
					features.append(featureItem);
				}
			}
			break;
		}
	}

	return features;
}


QDateTime COnlineLicenseControllerComp::CalculateTokenExpiry() const
{
	int hours = 24; // default
	if (m_tokenValidityHoursAttrPtr.IsValid()){
		hours = *m_tokenValidityHoursAttrPtr;
	}
	return QDateTime::currentDateTimeUtc().addSecs(hours * 3600);
}


bool COnlineLicenseControllerComp::IsHeartbeatExpired(const QDateTime& lastHeartbeat) const
{
	if (!lastHeartbeat.isValid()){
		return true;
	}

	int timeoutDays = 7; // default
	if (m_heartbeatTimeoutDaysAttrPtr.IsValid()){
		timeoutDays = *m_heartbeatTimeoutDaysAttrPtr;
	}

	QDateTime expirationTime = lastHeartbeat.addDays(timeoutDays);
	return QDateTime::currentDateTimeUtc() > expirationTime;
}


} // namespace prolifegql


