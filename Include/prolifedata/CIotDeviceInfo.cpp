// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include <prolifedata/CIotDeviceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>


namespace prolifedata
{


// public methods

CIotDeviceInfo::CIotDeviceInfo()
{
}


// reimplemented (IIotDeviceInfo)

QByteArray CIotDeviceInfo::GetSerialNumber() const
{
	return m_serialNumber;
}


void CIotDeviceInfo::SetSerialNumber(const QByteArray& serialNumber)
{
	if (m_serialNumber != serialNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_serialNumber = serialNumber;
	}
}


QByteArray CIotDeviceInfo::GetManufacturer() const
{
	return m_manufacturer;
}


void CIotDeviceInfo::SetManufacturer(const QByteArray& manufacturer)
{
	if (m_manufacturer != manufacturer){
		istd::CChangeNotifier changeNotifier(this);

		m_manufacturer = manufacturer;
	}
}


QByteArray CIotDeviceInfo::GetModelId() const
{
	return m_modelId;
}


void CIotDeviceInfo::SetModelId(const QByteArray& modelId)
{
	if (m_modelId != modelId){
		istd::CChangeNotifier changeNotifier(this);

		m_modelId = modelId;
	}
}


// reimplemented (iser::IObject)

QByteArray CIotDeviceInfo::GetFactoryId() const
{
	return QByteArray("IotDeviceInfo");
}


// reimplemented (iser::ISerializable)

bool CIotDeviceInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag serialNumberTag("SerialNumber", "Serial Number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serialNumberTag);
	retVal = retVal && archive.Process(m_serialNumber);
	retVal = retVal && archive.EndTag(serialNumberTag);

	iser::CArchiveTag manufacturerTag("Manufacturer", "Manufacturer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(manufacturerTag);
	retVal = retVal && archive.Process(m_manufacturer);
	retVal = retVal && archive.EndTag(manufacturerTag);

	iser::CArchiveTag modelIdTag("ModelId", "Model-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(modelIdTag);
	retVal = retVal && archive.Process(m_modelId);
	retVal = retVal && archive.EndTag(modelIdTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CIotDeviceInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CIotDeviceInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CIotDeviceInfo* sourcePtr = dynamic_cast<const CIotDeviceInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_serialNumber = sourcePtr->m_serialNumber;
		m_manufacturer = sourcePtr->m_manufacturer;
		m_modelId = sourcePtr->m_modelId;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CIotDeviceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CIotDeviceInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CIotDeviceInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_manufacturer.clear();
	m_serialNumber.clear();
	m_modelId.clear();

	return true;
}


} // namespace prolifedata
