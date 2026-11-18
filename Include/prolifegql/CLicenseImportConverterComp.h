#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IObjectImportConverter.h>


namespace prolifegql
{


/**
	Sorts the imported license definitions. A license of a hardware model described a hardware configuration: with
	ImportHardwareConfigurations it is turned into one, keeping its ID, and the other licenses are left out. Without it only the
	licenses of the other products are imported, unchanged.
*/
class CLicenseImportConverterComp: public ilog::CLoggerComponentBase, virtual public imtdb::IObjectImportConverter
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseImportConverterComp);
		I_REGISTER_INTERFACE(imtdb::IObjectImportConverter);
		I_ASSIGN(m_sourceProductCollectionCompPtr, "SourceProductCollection", "Products of the imported licenses, they tell which licenses belong to hardware models", true, "SourceProductCollection");
		I_ASSIGN(m_importHardwareConfigurationsAttrPtr, "ImportHardwareConfigurations", "Import the licenses of hardware models as hardware configurations instead of the other licenses", true, false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtdb::IObjectImportConverter)
	virtual ConversionResult ConvertObject(
				const QByteArray& objectId,
				const istd::IChangeable& sourceObject,
				TargetObject& targetObject,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_sourceProductCollectionCompPtr);
	I_ATTR(bool, m_importHardwareConfigurationsAttrPtr);
};


} // namespace prolifegql


