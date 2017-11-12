/*
	forsaken_objective_entity.cpp
	The objective entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "c_forsaken_objective_entity.h"
#include "c_forsaken_destroy_entity.h"

// HL2 Class Macros
IMPLEMENT_CLIENTCLASS_DT(C_ForsakenDestroyObjectiveEntity, DT_ForsakenDestroyObjectiveEntity, 
	CForsakenDestroyObjectiveEntity)
	RecvPropBool(RECVINFO(m_bTakingDamage)),
	RecvPropInt(RECVINFO(m_nHealth)),
END_RECV_TABLE()

// Global Variable Decleration

// Constructor & Deconstructor
C_ForsakenDestroyObjectiveEntity::C_ForsakenDestroyObjectiveEntity() : C_ForsakenObjectiveEntity()
{
	m_bTakingDamage = false;
	m_nHealth = 0;
}

C_ForsakenDestroyObjectiveEntity::~C_ForsakenDestroyObjectiveEntity()
{
}

// Functions