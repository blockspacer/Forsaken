#ifndef __FORSAKEN_FORTIFICATION_LIST_H_
#define __FORSAKEN_FORTIFICATION_LIST_H_

#include "c_forsaken_fortified_entity.h"

class C_ForsakenFortificationList
{
public:
	// Constructor & Deconstructor
	C_ForsakenFortificationList();
	virtual ~C_ForsakenFortificationList();

	// Public Accessor Functions
	C_ForsakenFortifiedObjectiveEntity *GetFortification(int nFortification) { return m_vecFortifications[nFortification]; }
	int GetFortificationCount() { return m_vecFortifications.Count(); }

	// Public Functions
	void RegisterFortification(C_ForsakenFortifiedObjectiveEntity *pFortification);
	void UnregisterFortification(C_ForsakenFortifiedObjectiveEntity *pFortification);

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	CUtlVector<C_ForsakenFortifiedObjectiveEntity*> m_vecFortifications;
};

extern C_ForsakenFortificationList *g_pFortificationList;

#endif