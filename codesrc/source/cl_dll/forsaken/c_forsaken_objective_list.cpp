/*
	c_forsaken_objective_list.cpp
	Forsaken objective list code.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "utlvector.h"
#include "c_forsaken_objective_entity.h"
#include "c_forsaken_objective_list.h"

// Global Variable Declerations
C_ForsakenObjectiveList g_objectiveList;
C_ForsakenObjectiveList *g_pObjectiveList = &g_objectiveList;

// Constructor & Deconstructor
C_ForsakenObjectiveList::C_ForsakenObjectiveList()
{
}

C_ForsakenObjectiveList::~C_ForsakenObjectiveList()
{
}

// Functions
// C_ForsakenObjectiveList
void C_ForsakenObjectiveList::RegisterObjective(C_ForsakenObjectiveEntity *pObjective)
/*
	
	Pre: 
	Post: 
*/
{
	m_vecObjectives.AddToTail(pObjective);
}

void C_ForsakenObjectiveList::UnregisterObjective(C_ForsakenObjectiveEntity *pObjective)
/*
	
	Pre: 
	Post: 
*/
{
	m_vecObjectives.FindAndRemove(pObjective);
}