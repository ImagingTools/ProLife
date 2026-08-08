#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/IObjectParamsFiller.h>


namespace prolifegql
{


/**
	Publishes object meta info as parameters of a selectable item, so that
	FilterableSelect lists can show the identifiers of a license or device instance
	without the client having to load the collection itself.

	\c ParamIds and \c MetaInfoIds are read pairwise: the value of the meta info
	named by \c MetaInfoIds[i] is published under the parameter id \c ParamIds[i].
	Software instances and devices carry different meta info, hence the separate
	\c Device* names.

	\ingroup FilterableSelect
*/
class CSelectableItemInfoProviderComp:
			public ilog::CLoggerComponentBase,
			public imtservergql::IObjectParamsFiller
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSelectableItemInfoProviderComp);
		I_REGISTER_INTERFACE(imtservergql::IObjectParamsFiller);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Collection providing the objects", true, "ObjectCollection");
		I_ASSIGN_MULTI_0(m_paramIdsAttrPtr, "ParamIds", "Parameter IDs to publish, paired with MetaInfoIds", true);
		I_ASSIGN_MULTI_0(m_metaInfoIdsAttrPtr, "MetaInfoIds", "Meta info names to read, paired with ParamIds. Software instances: ProductName, ProductId, ProductUuid, SerialNumber, Project, CustomerName, LicenseId, LicenseName, OrderId, DeliveryId, PurchaseId. Devices: DeviceProductName, DeviceProductId, DeviceType, DeviceConfigurationType, DeviceSerialNumber, DeviceMacAddress, DeviceProject, DeviceCustomerName, DeviceLicenseId, DeviceLicenseName, DeviceOrderId", true);
	I_END_COMPONENT;

	// reimplemented (imtservergql::IObjectParamsFiller)
	virtual bool FillParams(
				const QByteArray& objectId,
				iprm::IParamsSet& paramsSet,
				const QByteArray& contextTenantId = QByteArray()) const override;

private:
	//! Returns the meta info key of the configured name, or -1 when the name is unknown.
	static int GetMetaInfoType(const QByteArray& metaInfoId);

private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_MULTIATTR(QByteArray, m_paramIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_metaInfoIdsAttrPtr);
};


} // namespace prolifegql
