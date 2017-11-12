#ifndef __FORSAKEN_DESTROY_ENTITY_H_
#define __FORSAKEN_DESTROY_ENTITY_H_

class C_ForsakenDestroyObjectiveEntity : public C_ForsakenObjectiveEntity
{
public:
	DECLARE_CLASS(C_ForsakenDestroyObjectiveEntity, C_ForsakenObjectiveEntity);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor
	C_ForsakenDestroyObjectiveEntity();
	virtual ~C_ForsakenDestroyObjectiveEntity();

	// Public Accessor Functions
	bool IsTakingDamage() { return m_bTakingDamage; }
	int GetHealth() { return m_nHealth; }
	virtual int GetObjectiveType() { return OBJECTIVE_TYPE_DESTROY; }

	// Public Functions

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	bool m_bTakingDamage;
	int m_nHealth;
};

#endif