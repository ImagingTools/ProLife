#pragma once


// ProLife includes
#include <prolifedata/IIqcRunInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/IQC.h>


namespace prolifegql
{


/**
	Controller providing an idempotent push-based ingestion endpoint for automated
	inspection systems. External systems push their results using (systemId, externalRunId)
	as a composite key to upsert a run and append measurements.
*/
class CIqcIngestControllerComp: public sdl::prolife::IQC::CIqcIngestControllerCompBase
{
public:
	typedef sdl::prolife::IQC::CIqcIngestControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CIqcIngestControllerComp);
		I_ASSIGN(m_iqcRunCollectionCompPtr, "IqcRunCollection", "IQC run collection", true, "IqcRunCollection");
		I_ASSIGN(m_iqcRunInfoFactCompPtr, "IqcRunFactory", "Factory for IQC run instance", true, "IqcRunFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::prolife::IQC::CIqcIngestControllerCompBase)
	virtual sdl::imtbase::ImtCollection::CUpdatedNotificationPayload OnIqcRunIngest(
				const sdl::prolife::IQC::CIqcRunIngestGqlRequest& ingestRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	/**
		Find an existing IQC run by systemId + externalRunId.
		Returns nullptr if not found.
	*/
	prolifedata::IIqcRunInfo* FindExistingRun(
				const QString& systemId,
				const QString& externalRunId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_iqcRunCollectionCompPtr);
	I_FACT(prolifedata::IIqcRunInfo, m_iqcRunInfoFactCompPtr);
};


} // namespace prolifegql
