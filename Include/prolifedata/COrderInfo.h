#pragma once

// ImtCore includes
#include <imtlic/CProductInstanceCollection.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifedata
{


class COrderInfo: virtual public IOrderInfo
{
public:
	static QByteArray GetTypeId();

	COrderInfo();
	~COrderInfo();

	// reimplemented (IOrderInfo)
	virtual QByteArray GetOrderId() const override;
	virtual void SetOrderId(const QByteArray& orderId) override;
	virtual QByteArray GetCustomerId() const override;
	virtual void SetCustomerId(const QByteArray& customerId) override;
	virtual QByteArray GetDescription() const override;
	virtual void SetDescription(const QByteArray& description) override;
	virtual QByteArray GetStatus() const override;
	virtual void SetStatus(const QByteArray& status) override;
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
	QByteArray m_customerId;
	QByteArray m_description;
	QByteArray m_status;
    IOrderInfo::ProductIds m_orderedProducts;

	imtbase::CObjectCollection m_productInstanceCollection;

//	imtlic::CProductInstanceCollection m_productInstanceCollection;
};


} // namespace prolifedata


