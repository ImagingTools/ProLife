#pragma once


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <prolifedata/IOrderedProductInfo.h>
#include <imtlic/CLicenseInstance.h>


namespace prolifedata
{


/**
	Common implementation of IProductInstanceInfo interface.
	\sa IProductInstanceInfo
	\ingroup LicenseManagement
*/
class COrderedProductInfo: virtual public prolifedata::IOrderedProductInfo
{
public:
	COrderedProductInfo();

	// reimplemented (imtlic::IProductInstanceInfo)
	virtual const imtbase::IObjectCollection* GetProductDatabase() const override;
	virtual const imtbase::IObjectCollection* GetCustomerDatabase() const override;
	virtual void SetupProductInstance(
				const QByteArray& productId,
				const QByteArray& instanceId,
				const QByteArray& customerId,
				const QByteArray& orderId) override;
	virtual void AddLicense(const QByteArray& licenseId, const QDateTime& expirationDate = QDateTime()) override;
	virtual void RemoveLicense(const QByteArray& licenseId) override;
	virtual void ClearLicenses() override;
	virtual QByteArray GetProductId() const override;
	virtual QByteArray GetProductInstanceId() const override;
	virtual QByteArray GetCustomerId() const override;
	virtual QByteArray GetOrderId() const override;

	// reimplemented (imtlic::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseInstances() const override;
	virtual const imtlic::ILicenseInstance* GetLicenseInstance(const QByteArray& licenseId) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	const imtbase::IObjectCollection* m_customerCollectionPtr;
	const imtbase::IObjectCollection* m_productCollectionPtr;

private:
	QByteArray FindProductByName(const QString& productName) const;

private:
	typedef istd::TSmartPtr<imtlic::CLicenseInstance> LicenseInstancePtr;
	typedef QMap<QByteArray /*ID of the license definition in the product*/, LicenseInstancePtr> LicenseInstances;

	QByteArray m_productId;
	QByteArray m_customerId;
	QByteArray m_instanceId;
	QByteArray m_orderId;
	LicenseInstances m_licenses;
	imtbase::CCollectionInfo m_licenseContainerInfo;

};


} // namespace imtlic


