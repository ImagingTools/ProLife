#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>

// ProLife includes
#include <prolifedata/IProcurementOrderInfo.h>


namespace prolifedata
{


class CProcurementOrderInfo: virtual public IProcurementOrderInfo
{
public:
	static QByteArray GetTypeId();

	CProcurementOrderInfo();

	// reimplemented (IProcurementOrderInfo)
	virtual QString GetProcurementOrderNumber() const override;
	virtual void SetProcurementOrderNumber(const QString& orderNumber) override;
	virtual QByteArray GetSupplierId() const override;
	virtual void SetSupplierId(const QByteArray& supplierId) override;
	virtual QString GetSupplierName() const override;
	virtual void SetSupplierName(const QString& supplierName) override;
	virtual ProcurementStatus GetProcurementStatus() const override;
	virtual void SetProcurementStatus(ProcurementStatus status) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual QByteArray GetLinkedOrderUuid() const override;
	virtual void SetLinkedOrderUuid(const QByteArray& linkedOrderUuid) override;
	virtual QString GetExpectedDeliveryDate() const override;
	virtual void SetExpectedDeliveryDate(const QString& expectedDeliveryDate) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_procurementOrderNumber;
	QByteArray m_supplierId;
	QString m_supplierName;
	ProcurementStatus m_status;
	QString m_description;
	QByteArray m_linkedOrderUuid;
	QString m_expectedDeliveryDate;
};


typedef imtbase::TIdentifiableWrap<CProcurementOrderInfo> CIdentifiableProcurementOrderInfo;


} // namespace prolifedata
