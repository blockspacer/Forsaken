/*
	c_forsaken_fortified_entity.cpp
	The fortified objective entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "utlvector.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "c_forsaken_fortified_entity.h"
#include "c_forsaken_fortification_list.h"

// HL2 Class Macros
IMPLEMENT_CLIENTCLASS_DT(C_ForsakenFortifiedObjectiveEntity, DT_ForsakenFortifiedObjectiveEntity, 
	CForsakenFortifiedObjectiveEntity)
	RecvPropBool(RECVINFO(m_bTakingDamage)),
	RecvPropEHandle(RECVINFO(m_hObjectiveEntity)),
	RecvPropInt(RECVINFO(m_nHealth)),
	RecvPropInt(RECVINFO(m_nTeamOwner)),
	RecvPropInt(RECVINFO(m_nLevel)),
	RecvPropInt(RECVINFO(m_nFortificationStatus))
END_RECV_TABLE()

// Global Variable Decleration

// Constructor & Deconstructor
C_ForsakenFortifiedObjectiveEntity::C_ForsakenFortifiedObjectiveEntity() : CBaseAnimating()
{
	SetPredictionEligible(true);

	g_pFortificationList->RegisterFortification(this);

	m_bTakingDamage = false;
	m_nHealth = 0;
	m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_NONE;
	m_nLevel = 0;
	m_nTeamOwner = 0;
}

C_ForsakenFortifiedObjectiveEntity::~C_ForsakenFortifiedObjectiveEntity()
{
	g_pFortificationList->UnregisterFortification(this);
}

// Functions