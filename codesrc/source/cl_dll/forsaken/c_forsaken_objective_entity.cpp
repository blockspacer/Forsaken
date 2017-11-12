/*
	forsaken_objective_entity.cpp
	The objective entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "utlvector.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "c_forsaken_objective_entity.h"
#include "c_forsaken_objective_list.h"

// HL2 Class Macros
IMPLEMENT_CLIENTCLASS_DT(C_ForsakenObjectiveEntity, DT_ForsakenObjectiveEntity, 
	CForsakenObjectiveEntity)
	RecvPropInt(RECVINFO(m_nTeamOwner)),
	RecvPropString(RECVINFO(m_czObjectiveName)),
END_RECV_TABLE()

// Global Variable Decleration

// Constructor & Deconstructor
C_ForsakenObjectiveEntity::C_ForsakenObjectiveEntity() : CBaseAnimating()
{
	SetPredictionEligible(true);

	g_pObjectiveList->RegisterObjective(this);

	m_nTeamOwner = TEAM_UNASSIGNED;
}

C_ForsakenObjectiveEntity::~C_ForsakenObjectiveEntity()
{
	g_pObjectiveList->UnregisterObjective(this);
}

// Functions