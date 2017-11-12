#ifndef __FORSAKEN_FORTIFIEDSUPPORT_ENTITY_H_
#define __FORSAKEN_FORTIFIEDSUPPORT_ENTITY_H_

#include "forsaken_objective_entity.h"
#include "forsaken_spawnpoint.h"
#include "utlvector.h"

#define FORTIFICATION_UNKNOWN	0
#define FORTIFICATION_DESTROYED 1
#define FORTIFICATION_BUILDING	2
#define FORTIFICATION_COMPLETE	3

class CForsakenFortifiedSupportEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS(CForsakenFortifiedSupportEntity, CBaseAnimating);
	DECLARE_DATADESC();
	//DECLARE_SERVERCLASS();

	// Constructor & Deconstructor
	CForsakenFortifiedSupportEntity();
	virtual ~CForsakenFortifiedSupportEntity();

	// Public Accessor Functions
	const char *GetObjectiveName() { return m_czObjectiveName.Get(); }
	int GetTeamOwner() { return m_nTeamOwner; }
	void SetTeamMapOwner(int nTeamOwner);

	// Public Functions
	bool KeyValue(const char *czKey, const char *czValue);
	virtual int OnTakeDamage(const CTakeDamageInfo &rDmgInfo);
	virtual int UpdateTransmitState() { return FL_EDICT_ALWAYS; }
	void DamageThink();
	void Event_Killed(const CTakeDamageInfo &rDmgInfo);
	void FortificationUpgraded(inputdata_t &rInput);
	void Precache();
	void SetTeamOwner(inputdata_t &rInput);
	void Spawn();

	// Public Variables

protected:
	// Protected Functions
	bool FindFortificationEntity();

	// Protected Variables
	CNetworkHandle(CForsakenFortifiedObjectiveEntity, m_hFortifiedEntity);
	CNetworkString(m_czObjectiveName, OBJECTIVE_NAME_LENGTH);
	CNetworkString(m_czCivModel, MAX_PATH);
	CNetworkString(m_czGovModel, MAX_PATH);
	CNetworkString(m_czRelModel, MAX_PATH);
	CNetworkVar(bool, m_bTakingDamage);
	CNetworkVar(int, m_nHealth);
	CNetworkVar(int, m_nLevel);
	CNetworkVar(int, m_nTeamOwner);
	CNetworkVar(int, m_nCivModel);
	CNetworkVar(int, m_nGovModel);
	CNetworkVar(int, m_nRelModel);
};

#endif