#pragma once


// ACF includes
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtcrypt/IEncryption.h>

// ProLife includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/OnlineLicenses.h>


namespace prolifegql
{


/**
	Server-side controller for online license activation.
	Handles activation key management, license activation/deactivation,
	and heartbeat validation for online licensing.
*/
class COnlineLicenseControllerComp:
			public sdl::prolife::OnlineLicenses::CGraphQlHandlerCompBase
{
public:
	typedef sdl::prolife::OnlineLicenses::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(COnlineLicenseControllerComp)
		I_ASSIGN(m_activationKeyCollectionCompPtr, "ActivationKeyCollection", "Activation key collection", true, "ActivationKeyCollection");
		I_ASSIGN(m_onlineActivationCollectionCompPtr, "OnlineActivationCollection", "Online activation collection", true, "OnlineActivationCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
		I_ASSIGN(m_encryptionCompPtr, "Encryption", "Encrypt/Decrypt instances", true, "Encryption");
		I_ASSIGN(m_heartbeatTimeoutDaysAttrPtr, "HeartbeatTimeoutDays", "Days after which an activation expires without heartbeat", true, 7);
		I_ASSIGN(m_tokenValidityHoursAttrPtr, "TokenValidityHours", "Hours a token is valid before requiring heartbeat refresh", true, 24);
	I_END_COMPONENT

protected:
	// Client-facing mutations: Activation flow
	virtual sdl::prolife::OnlineLicenses::CActivateLicensePayload OnActivateLicense(
				const sdl::prolife::OnlineLicenses::CActivateLicenseGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::prolife::OnlineLicenses::CValidateLicensePayload OnValidateLicense(
				const sdl::prolife::OnlineLicenses::CValidateLicenseGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::prolife::OnlineLicenses::CDeactivateLicensePayload OnDeactivateLicense(
				const sdl::prolife::OnlineLicenses::CDeactivateLicenseGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::prolife::OnlineLicenses::CHeartbeatPayload OnHeartbeat(
				const sdl::prolife::OnlineLicenses::CHeartbeatGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// Admin mutations: Key management
	virtual sdl::prolife::OnlineLicenses::CCreateActivationKeyPayload OnCreateActivationKey(
				const sdl::prolife::OnlineLicenses::CCreateActivationKeyGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::prolife::OnlineLicenses::CRevokeActivationKeyPayload OnRevokeActivationKey(
				const sdl::prolife::OnlineLicenses::CRevokeActivationKeyGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual sdl::prolife::OnlineLicenses::CDeactivateOnlineActivationPayload OnDeactivateOnlineActivation(
				const sdl::prolife::OnlineLicenses::CDeactivateOnlineActivationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	/**
		Generate a unique activation key string (e.g., "XXXX-XXXX-XXXX-XXXX").
	*/
	static QString GenerateActivationKey();

	/**
		Generate a secure token for a given activation.
	*/
	QString GenerateToken(const QByteArray& activationKeyId, const QByteArray& machineFingerprint) const;

	/**
		Check if an activation key has available activation slots.
	*/
	bool HasAvailableSlots(const QByteArray& activationKeyId) const;

	/**
		Retrieve features associated with a license.
	*/
	QList<sdl::prolife::OnlineLicenses::CActivationFeatureItem> GetLicenseFeatures(const QByteArray& licenseId) const;

	/**
		Calculate token expiration time based on configuration.
	*/
	QDateTime CalculateTokenExpiry() const;

	/**
		Check if activation has expired due to heartbeat timeout.
	*/
	bool IsHeartbeatExpired(const QDateTime& lastHeartbeat) const;

private:
	I_REF(imtbase::IObjectCollection, m_activationKeyCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_onlineActivationCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_REF(imtcrypt::IEncryption, m_encryptionCompPtr);
	I_ATTR(int, m_heartbeatTimeoutDaysAttrPtr);
	I_ATTR(int, m_tokenValidityHoursAttrPtr);
};


} // namespace prolifegql


