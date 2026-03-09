// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include <prolifedata/CRevokeOutAction.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


// public methods

CRevokeOutAction::CRevokeOutAction()
	:m_initialCount(0),
	m_revokedCount(0)
{
}


CRevokeOutAction::CRevokeOutAction(const QByteArray& parentLicenseId, int initialCount, int revokedCount)
	:m_parentLicenseId(parentLicenseId),
	m_initialCount(initialCount),
	m_revokedCount(revokedCount)
{
}


QByteArray CRevokeOutAction::GetParentLicenseId() const
{
	return m_parentLicenseId;
}


void CRevokeOutAction::SetParentLicenseId(const QByteArray& parentLicenseId)
{
	if (m_parentLicenseId != parentLicenseId){
		istd::CChangeNotifier notifier(this);

		m_parentLicenseId = parentLicenseId;
	}
}


int CRevokeOutAction::GetInitialCount() const
{
	return m_initialCount;
}


void CRevokeOutAction::SetInitialCount(int initialCount)
{
	if (m_initialCount != initialCount){
		istd::CChangeNotifier notifier(this);

		m_initialCount = initialCount;
	}
}


int CRevokeOutAction::GetRevokedCount() const
{
	return m_revokedCount;
}


void CRevokeOutAction::SetRevokedCount(int revokedCount)
{
	if (m_revokedCount != revokedCount){
		istd::CChangeNotifier notifier(this);

		m_revokedCount = revokedCount;
	}
}


// reimplemented (iser::ISerializable)

bool CRevokeOutAction::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag parentLicenseIdTag("ParentLicenseId", "Parent License-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(parentLicenseIdTag);
	retVal = retVal && archive.Process(m_parentLicenseId);
	retVal = retVal && archive.EndTag(parentLicenseIdTag);

	iser::CArchiveTag initialCountTag("InitialCount", "Initial Count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(initialCountTag);
	retVal = retVal && archive.Process(m_initialCount);
	retVal = retVal && archive.EndTag(initialCountTag);

	iser::CArchiveTag revokedCountTag("RevokedCount", "Revoked Count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revokedCountTag);
	retVal = retVal && archive.Process(m_revokedCount);
	retVal = retVal && archive.EndTag(revokedCountTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CRevokeOutAction::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CRevokeOutAction* sourcePtr = dynamic_cast<const CRevokeOutAction*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_parentLicenseId = sourcePtr->m_parentLicenseId;
		m_initialCount = sourcePtr->m_initialCount;
		m_revokedCount = sourcePtr->m_revokedCount;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CRevokeOutAction::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CRevokeOutAction());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CRevokeOutAction::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_parentLicenseId.clear();
	m_initialCount = 0;
	m_revokedCount = 0;

	return true;
}


} // namespace prolifedata


