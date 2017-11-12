#ifndef __FORSAKEN_DESTROY_ENTITY_H_
#define __FORSAKEN_DESTROY_ENTITY_H_

class CForsakenDestroyObjectiveEntity : public CForsakenObjectiveEntity
{
public:
	DECLARE_CLASS(CForsakenDestroyObjectiveEntity, CForsakenObjectiveEntity);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	// Constructor & Deconstructor
	CForsakenDestroyObjectiveEntity();
	virtual ~CForsakenDestroyObjectiveEntity();

	// Public Accessor Functions
	virtual int GetObjectiveType() { return OBJECTIVE_TYPE_DESTROY; }
	virtual void SetTeamMapOwner(int nTeamOwner);

	// Public Functions
	virtual int OnTakeDamage(const CTakeDamageInfo &rDmgInfo);
	void DamageThink();
	void Event_Killed(const CTakeDamageInfo &rDmgInfo);
	void Precache();
	void Spawn();

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	CNetworkVar(bool, m_bTakingDamage);
	CNetworkVar(int, m_nHealth);
};

#endif