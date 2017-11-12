#ifndef __FORSAKEN_TEAM_H_
#define __FORSAKEN_TEAM_H_

#include "utlvector.h"
#include "team.h"

class IForsakenSpawnPoint;

class CForsakenTeam : public CTeam
{
	DECLARE_CLASS(CForsakenTeam, CTeam);
	DECLARE_SERVERCLASS();

public:
	// Constructor & Deconstructor
	CForsakenTeam();
	virtual ~CForsakenTeam();

	// Public Accessor Functions
	bool IsTeamAvailable() { return m_bTeamAvailable; }
	bool IsTeamStarving() { return m_bStarving; }
	CForsakenPlayer *GetLeader() { return m_pLeader; }
	int GetNumberAlivePlayers();
	int GetNumberAlivePlayers(CBasePlayer *pExclude);
	int GetNutritionResources() { return m_nNutritionResources; }
	int GetOrdinanceResources() { return m_nOrdinanceResources; }
	int GetSpawnHealth() { return m_nSpawnHealth; }
	void AddForsakenSpawn(IForsakenSpawnPoint *pSpawn);
	void IncreaseNutritionResources(int nAmount) { m_nNutritionResources += nAmount; }
	void IncreaseOrdinanceResources(int nAmount) { m_nOrdinanceResources += nAmount; }
	void TakeNutrition(int nAmount) { m_nNutritionResources -= nAmount; }
	void TakeOrdinance(int nAmount) { m_nOrdinanceResources -= nAmount; }
	void NullLeader() { m_pLeader = NULL; }
	void RemoveForsakenSpawn(IForsakenSpawnPoint *pSpawn);
	void SetFortificationSpawn(CForsakenFortifiedObjectiveEntity *pFortifiedSpawnEntity)
	{
		m_hFortifiedSpawnEntity.GetForModify() = pFortifiedSpawnEntity;
	}
	void SetSpawnHealth(int nSpawnHealth) { m_nSpawnHealth = nSpawnHealth; }
	void SetTeamAvailability(bool bAvailable) { m_bTeamAvailable = bAvailable; }
	void SetTeamStarving(bool bStarving) { m_bStarving = bStarving; }

	// Public Functions
	virtual CBaseEntity *SpawnPlayer(CBasePlayer *pPlayer);
	virtual void Init(const char *czName, int nNumber);
	virtual void LoadState(KeyValues *pKeyValues);
	virtual void SaveState(KeyValues *pKeyValues);
	void AssignLeader();
	void DoResourceCalculations();
	void ForcePlayersToSpectator();
	void SpawnTeam();

	// Public Variables
	CForsakenPlayer *m_pLeader;
	CNetworkHandle(CForsakenFortifiedObjectiveEntity, m_hFortifiedSpawnEntity);
	CNetworkVar(bool, m_bStarving);
	CNetworkVar(bool, m_bTeamAvailable);
	CNetworkVar(int, m_nNutritionResources);
	CNetworkVar(int, m_nOrdinanceResources);
	CNetworkVar(int, m_nSpawnHealth);
	CUtlVector<IForsakenSpawnPoint *> m_vecSpawns;
	int m_nFortifiedSpawnAmount;

protected:
	// Protected Functions

	// Protected Variables
};

extern CForsakenTeam *GetGlobalForsakenTeam(int nIndex);

#endif