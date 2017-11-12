#ifndef __FORSAKEN_FORTIFIED_ENTITY_H_
#define __FORSAKEN_FORTIFIED_ENTITY_H_

#include "forsaken_objective_entity.h"
#include "forsaken_spawnpoint.h"
#include "utlvector.h"

#define FORTIFICATION_UNKNOWN	0
#define FORTIFICATION_DESTROYED 1
#define FORTIFICATION_BUILDING	2
#define FORTIFICATION_COMPLETE	3

class CForsakenFortifiedObjectiveEntity : public CBaseAnimating, public IGameEventListener2
{
public:
	DECLARE_CLASS(CForsakenFortifiedObjectiveEntity, CBaseAnimating);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	// Constructor & Deconstructor
	CForsakenFortifiedObjectiveEntity();
	virtual ~CForsakenFortifiedObjectiveEntity();

	// Public Accessor Functions
	const char *GetObjectiveName() { return m_czObjectiveName.Get(); }
	int GetFortificationLevel() { return m_nLevel; }
	int GetFortificationStatus() { return m_nFortificationStatus; }
	int GetTeamOwner() { return m_nTeamOwner; }
	void AddObjectiveSpawn(CForsakenSpawnPoint_Objective *pSpawn) { m_vecObjectiveSpawns.AddToTail(pSpawn); }
	void SetTeamMapOwner(int nTeamOwner);

	// Public Functions
	bool KeyValue(const char *czKey, const char *czValue);
	bool Upgrade(int nDesiredLevel);
	virtual int OnTakeDamage(const CTakeDamageInfo &rDmgInfo);
	virtual int UpdateTransmitState() { return FL_EDICT_ALWAYS; }
	virtual void FireGameEvent(IGameEvent *pEvent);
	void DamageThink();
	void Event_Killed(const CTakeDamageInfo &rDmgInfo);
	void Precache();
	void SetTeamOwner(inputdata_t &rInput);
	void Spawn();

	// Public Variables
	CUtlVector<CForsakenSpawnPoint_Objective*> m_vecObjectiveSpawns;

protected:
	// Protected Functions
	bool CanAffordUpgrade(int nLevel);
	bool FindObjectiveEntity();
	int GetUpgradeCost(int nLevel);
	void FortificationUpgraded();
	void SetObjectiveCaptureable(bool bState);

	// Protected Variables
	CNetworkHandle(CForsakenObjectiveEntity, m_hObjectiveEntity);
	CNetworkString(m_czObjectiveName, OBJECTIVE_NAME_LENGTH);
	CNetworkString(m_czCivModel, MAX_PATH);
	CNetworkString(m_czGovModel, MAX_PATH);
	CNetworkString(m_czRelModel, MAX_PATH);
	CNetworkVar(bool, m_bTakingDamage);
	CNetworkVar(int, m_nFortificationStatus);
	CNetworkVar(int, m_nHealth);
	CNetworkVar(int, m_nLevel);
	CNetworkVar(int, m_nTeamOwner);
	CNetworkVar(int, m_nCivModel);
	CNetworkVar(int, m_nGovModel);
	CNetworkVar(int, m_nRelModel);
	COutputInt m_FortificationComplete;
	COutputInt m_FortificationDestroyed;
	COutputInt m_FortificationUpgraded;
	int m_nDesiredLevel;
	int m_nRoundBuildNumber;
};

#endif