#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace prolifedata
{


class CSplitInAction: virtual public iser::ISerializable
{
public:
	CSplitInAction();
	CSplitInAction(const QByteArray& sourceLicenseId, int receivedCount);

	QByteArray GetSourceLicenseId() const;
	void SetSourceLicenseId(const QByteArray& sourceLicenseId);
	int GetReceivedCount() const;
	void SetReceivedCount(int receivedCount);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_sourceLicenseId;
	int m_receivedCount;
};


} // namespace prolifedata


