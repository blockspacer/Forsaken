#ifndef __FORSAKEN_OBJECTIVE_ENTITY_H_
#define __FORSAKEN_OBJECTIVE_ENTITY_H_

class C_ForsakenObjectiveEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS(C_ForsakenObjectiveEntity, CBaseAnimating);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor
	C_ForsakenObjectiveEntity();
	virtual ~C_ForsakenObjectiveEntity();

	// Public Accessor Functions
	const char *GetObjectiveName() { return m_czObjectiveName; }
	int GetTeamOwner() { return m_nTeamOwner; }
	virtual int GetObjectiveType() { return OBJECTIVE_TYPE_BASE; }

	// Public Functions

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	char m_czObjectiveName[OBJECTIVE_NAME_LENGTH];
	int m_nTeamOwner;
};

#endif