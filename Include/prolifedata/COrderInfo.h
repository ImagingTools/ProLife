#pragma once


// ImtCore includes
#include <imtlic/CProductInstanceCollection.h>
#include <imtbase/TIdentifiableWrap.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifedata
{


class COrderInfo: virtual public IOrderInfo
{
public:
	typedef QByteArrayList ProductIds;

	static QByteArray GetTypeId();

	COrderInfo();
	~COrderInfo();

	// reimplemented (prolifedata::IOrdered)
	virtual QByteArray GetOrderId() const override;
	virtual void SetOrderId(const QByteArray& orderId) override;
	virtual QByteArray GetPurchaseOrderId() const override;
	virtual void SetPurchaseOrderId(const QByteArray& purchaseOrderId) override;

	// reimplemented (IOrderInfo)
	virtual QByteArray GetCustomerId() const override;
	virtual void SetCustomerId(const QByteArray& customerId) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual OrderStatus GetOrderStatus() const override;
	virtual void SetOrderStatus(OrderStatus status) override;
	imtbase::CObjectCollection* GetProducts() override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_orderId;
	QByteArray m_purchaseId;
	QByteArray m_customerId;
	QString m_description;
	OrderStatus m_status;
	ProductIds m_orderedProducts;

	imtbase::CObjectCollection m_productInstanceCollection;
};


typedef imtbase::TIdentifiableWrap<COrderInfo> CIdentifiableOrderInfo;




} // namespace prolifedata


