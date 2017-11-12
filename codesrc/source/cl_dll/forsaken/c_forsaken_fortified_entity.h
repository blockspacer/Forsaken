#ifndef __FORSAKEN_FORTIFIED_ENTITY_H_
#define __FORSAKEN_FORTIFIED_ENTITY_H_

class C_ForsakenFortifiedObjectiveEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS(C_ForsakenFortifiedObjectiveEntity, CBaseAnimating);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor
	C_ForsakenFortifiedObjectiveEntity();
	virtual ~C_ForsakenFortifiedObjectiveEntity();

	// Public Accessor Functions
	bool IsTakingDamage() { return m_bTakingDamage; }
	CBaseEntity *GetObjectiveEntity() { return m_hObjectiveEntity.Get(); }
	int GetFortificationLevel() { return m_nLevel; }
	int GetFortificationStatus() { return m_nFortificationStatus; }
	int GetHealth() { return m_nHealth; }
	int GetTeamOwner() { return m_nTeamOwner; }

	// Public Functions

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	bool m_bTakingDamage;
	EHANDLE m_hObjectiveEntity;
	int m_nFortificationStatus;
	int m_nHealth;
	int m_nLevel;
	int m_nTeamOwner;
};

#endif