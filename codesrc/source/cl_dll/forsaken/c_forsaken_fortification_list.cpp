/*
	c_forsaken_fortification_list.cpp
	Forsaken fortification list code.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "utlvector.h"
#include "c_forsaken_fortified_entity.h"
#include "c_forsaken_fortification_list.h"

// Global Variable Declerations
C_ForsakenFortificationList g_fortificationList;
C_ForsakenFortificationList *g_pFortificationList = &g_fortificationList;

// Constructor & Deconstructor
C_ForsakenFortificationList::C_ForsakenFortificationList()
{
}

C_ForsakenFortificationList::~C_ForsakenFortificationList()
{
}

// Functions
// C_ForsakenObjectiveList
void C_ForsakenFortificationList::RegisterFortification(C_ForsakenFortifiedObjectiveEntity *pFortification)
/*
	
	Pre: 
	Post: 
*/
{
	m_vecFortifications.AddToTail(pFortification);
}

void C_ForsakenFortificationList::UnregisterFortification(C_ForsakenFortifiedObjectiveEntity *pFortification)
/*
	
	Pre: 
	Post: 
*/
{
	m_vecFortifications.FindAndRemove(pFortification);
}