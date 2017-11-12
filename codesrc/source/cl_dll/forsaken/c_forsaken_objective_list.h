#ifndef __FORSAKEN_OBJECTIVE_LIST_H_
#define __FORSAKEN_OBJECTIVE_LIST_H_

class C_ForsakenObjectiveList
{
public:
	// Constructor & Deconstructor
	C_ForsakenObjectiveList();
	virtual ~C_ForsakenObjectiveList();

	// Public Accessor Functions
	C_ForsakenObjectiveEntity *GetObjective(int nObjective) { return m_vecObjectives[nObjective]; }
	int GetObjectiveCount() { return m_vecObjectives.Count(); }

	// Public Functions
	void RegisterObjective(C_ForsakenObjectiveEntity *pObjective);
	void UnregisterObjective(C_ForsakenObjectiveEntity *pObjective);

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	CUtlVector<C_ForsakenObjectiveEntity*> m_vecObjectives;
};

extern C_ForsakenObjectiveList *g_pObjectiveList;

#endif