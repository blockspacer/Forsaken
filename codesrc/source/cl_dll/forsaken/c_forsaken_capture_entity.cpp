/*
	forsaken_capture_entity.cpp
	The capture entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "c_forsaken_objective_entity.h"
#include "c_forsaken_capture_entity.h"
#include "c_forsaken_player.h"

// HL2 Class Macros
IMPLEMENT_CLIENTCLASS_DT(C_ForsakenCaptureObjectiveEntity, DT_ForsakenCaptureObjectiveEntity, 
	CForsakenCaptureObjectiveEntity)
	RecvPropFloat(RECVINFO(m_fCaptureTime)),
	RecvPropFloat(RECVINFO(m_fRemainingCaptureTimeCiv)),
	RecvPropFloat(RECVINFO(m_fRemainingCaptureTimeGov)),
	RecvPropFloat(RECVINFO(m_fRemainingCaptureTimeRel)),
	RecvPropFloat(RECVINFO(m_fStepOutTimeCiv)),
	RecvPropFloat(RECVINFO(m_fStepOutTimeGov)),
	RecvPropFloat(RECVINFO(m_fStepOutTimeRel)),
	RecvPropInt(RECVINFO(m_nPlayerWeighting)),
	RecvPropInt(RECVINFO(m_nResourceAmount)),
	RecvPropInt(RECVINFO(m_nResourceType)),
	RecvPropInt(RECVINFO(m_nTeamCanCapture)),
	RecvPropInt(RECVINFO(m_nTeamCapturing)),
END_RECV_TABLE()

// Global Variable Decleration

// Constructor & Deconstructor
C_ForsakenCaptureObjectiveEntity::C_ForsakenCaptureObjectiveEntity() : C_ForsakenObjectiveEntity()
{
}

C_ForsakenCaptureObjectiveEntity::~C_ForsakenCaptureObjectiveEntity()
{
}

// Functions
void C_ForsakenCaptureObjectiveEntity::Simulate()
{
	// Simulate the capture time remaining.
	switch (m_nTeamCapturing)
	{
	case TEAM_CIVILIANS:
		m_fRemainingCaptureTimeCiv -= m_nPlayerWeighting * gpGlobals->frametime;
		break;

	case TEAM_GOVERNORS:
		m_fRemainingCaptureTimeGov -= m_nPlayerWeighting * gpGlobals->frametime;
		break;

	case TEAM_RELIGIOUS:
		m_fRemainingCaptureTimeRel -= m_nPlayerWeighting * gpGlobals->frametime;
		break;
	}

	BaseClass::Simulate();
}