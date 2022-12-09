#pragma once


// ACF includes
#include <iimg/CBitmap.h>
#include <imod/TModelWrap.h>
#include <istd/TSmartPtr.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <prolifeauth/IProductInfo.h>



namespace prolifeauth
{


class CProductInfo: virtual public IProductInfo
{
public:
	static QByteArray GetTypeId();

	CProductInfo();
	~CProductInfo();

	// reimplemented (IProductInfo)
	virtual QString GetProductName() const override;
	virtual void SetProductName(QString ProductName) override;
	virtual QString GetProductDescription() const override;
	virtual void SetProductDescription(QString ProductDescription ) override;
	virtual QString GetProductManufacturer() const override;
	virtual void SetProductManufacturer(QString ProductManufacturer) override;


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
	QString m_productName;
	QString m_productDescription;
	QString m_productManufacturer;
};


} // namespace imtauth


