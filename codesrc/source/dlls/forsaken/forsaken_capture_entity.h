#ifndef __FORSAKEN_CAPTURE_ENTITY_H_
#define __FORSAKEN_CAPTURE_ENTITY_H_

class CForsakenCaptureObjectiveEntity : public CForsakenObjectiveEntity
{
public:
	DECLARE_CLASS(CForsakenCaptureObjectiveEntity, CForsakenObjectiveEntity);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	// Constructor & Deconstructor
	CForsakenCaptureObjectiveEntity();
	virtual ~CForsakenCaptureObjectiveEntity();

	// Public Accessor Functions
	bool IsCaptureable() { return m_bIsCaptureable; }
	int GetResourceAmount() { return m_nResourceAmount; }
	int GetResourceType() { return m_nResourceType; }
	virtual bool IsPlayerAffectingObjective(CForsakenPlayer *pPlayer);
	virtual int GetObjectiveType() { return OBJECTIVE_TYPE_CAPTURE; }
	virtual void SetTeamMapOwner(int nTeamOwner);
	void SetCaptureable(bool bCanCapture) { m_bIsCaptureable = bCanCapture; }

	// Public Functions
	void CaptureThink();
	void Spawn();

	// Public Variables

protected:
	// Protected Functions
	int FindPlayersInCapture(CUtlVector<CForsakenPlayer*> &rvecPlayerList, int &nPlayerWeighting);
	void CheckCaptureTimes();

	// Protected Variables
	bool m_bIsCaptureable;
	CNetworkVar(float, m_fCaptureTime);
	CNetworkVar(float, m_fRemainingCaptureTimeCiv);
	CNetworkVar(float, m_fRemainingCaptureTimeGov);
	CNetworkVar(float, m_fRemainingCaptureTimeRel);
	CNetworkVar(float, m_fStepOutTimeCiv);
	CNetworkVar(float, m_fStepOutTimeGov);
	CNetworkVar(float, m_fStepOutTimeRel);
	CNetworkVar(int, m_nPlayerWeighting);
	CNetworkVar(int, m_nResourceAmount);
	CNetworkVar(int, m_nResourceType);
	CNetworkVar(int, m_nTeamCanCapture);
	CNetworkVar(int, m_nTeamCapturing);
	CUtlVector<CForsakenPlayer*> m_vecPlayersInCapture;
};

#endif