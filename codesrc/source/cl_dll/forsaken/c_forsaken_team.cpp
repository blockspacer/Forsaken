/*
	c_forsaken_team.cpp
	Client-side forsaken team.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "engine/IEngineSound.h"
#include "hud.h"
#include "recvproxy.h"
#include "c_forsaken_team.h"

// HL2 Class Macros
IMPLEMENT_CLIENTCLASS_DT(C_ForsakenTeam, DT_ForsakenTeam, CForsakenTeam)
	RecvPropBool(RECVINFO(m_bStarving)),
	RecvPropBool(RECVINFO(m_bTeamAvailable)),
	RecvPropEHandle(RECVINFO(m_hFortifiedSpawnEntity)),
	RecvPropInt(RECVINFO(m_nNutritionResources)),
	RecvPropInt(RECVINFO(m_nOrdinanceResources))
END_RECV_TABLE()

// Constructor & Deconstructor
C_ForsakenTeam::C_ForsakenTeam()
{
}

C_ForsakenTeam::~C_ForsakenTeam()
{
}

// Functions
// C_Forsaken_Team
