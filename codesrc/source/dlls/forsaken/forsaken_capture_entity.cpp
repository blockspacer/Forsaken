/*
	forsaken_capture_entity.cpp
	The capture entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken_player.h"
#include "forsaken_objective_entity.h"
#include "forsaken_capture_entity.h"
#include "forsaken_fortified_entity.h"
#include "forsaken_team.h"

extern char *sTeamNames[];

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_captureobjective_entity, CForsakenCaptureObjectiveEntity);

BEGIN_DATADESC(CForsakenCaptureObjectiveEntity)
	// Fields
	DEFINE_FIELD(m_fCaptureTime, FIELD_FLOAT),
	DEFINE_FIELD(m_nResourceAmount, FIELD_INTEGER),
	DEFINE_FIELD(m_nResourceType, FIELD_INTEGER),
	DEFINE_FIELD(m_nTeamCanCapture, FIELD_INTEGER),
	DEFINE_KEYFIELD(m_fCaptureTime, FIELD_FLOAT, "objective_capturelength"),
	DEFINE_KEYFIELD(m_nResourceAmount, FIELD_INTEGER, "objective_resourceamount"),
	DEFINE_KEYFIELD(m_nResourceType, FIELD_INTEGER, "objective_resourcetype"),
	DEFINE_KEYFIELD(m_nTeamCanCapture, FIELD_INTEGER, "objective_teamcapture"),

	// Outputs

	// Think Functions
	DEFINE_THINKFUNC(CaptureThink),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CForsakenCaptureObjectiveEntity, DT_ForsakenCaptureObjectiveEntity)
	SendPropFloat(SENDINFO(m_fCaptureTime)),
	SendPropFloat(SENDINFO(m_fRemainingCaptureTimeCiv)),
	SendPropFloat(SENDINFO(m_fRemainingCaptureTimeGov)),
	SendPropFloat(SENDINFO(m_fRemainingCaptureTimeRel)),
	SendPropFloat(SENDINFO(m_fStepOutTimeCiv)),
	SendPropFloat(SENDINFO(m_fStepOutTimeGov)),
	SendPropFloat(SENDINFO(m_fStepOutTimeRel)),
	SendPropInt(SENDINFO(m_nPlayerWeighting)),
	SendPropInt(SENDINFO(m_nResourceAmount)),
	SendPropInt(SENDINFO(m_nResourceType)),
	SendPropInt(SENDINFO(m_nTeamCanCapture)),
	SendPropInt(SENDINFO(m_nTeamCapturing)),
END_SEND_TABLE()

// Global Variable Decleration
ConVar sv_fskn_capturesafetytime("sv_fskn_capturesafetytime", "5", FCVAR_REPLICATED, 
		"The time allowed to elapse before the capture progress is reset after your " 
		"faction leaves the area mid-capture.");

// Constructor & Deconstructor
CForsakenCaptureObjectiveEntity::CForsakenCaptureObjectiveEntity() : CForsakenObjectiveEntity()
{
	m_bIsCaptureable = true;
	m_fRemainingCaptureTimeCiv = 0.0f;
	m_fRemainingCaptureTimeGov = 0.0f;
	m_fRemainingCaptureTimeRel = 0.0f;
	m_fStepOutTimeCiv = 0.0f;
	m_fStepOutTimeGov = 0.0f;
	m_fStepOutTimeRel = 0.0f;
	m_nTeamCapturing = TEAM_UNASSIGNED;
}

CForsakenCaptureObjectiveEntity::~CForsakenCaptureObjectiveEntity()
{
}

// Functions
bool CForsakenCaptureObjectiveEntity::IsPlayerAffectingObjective(CForsakenPlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	// Loop through all players in the capture zone.
	for (int i = 0; i < m_vecPlayersInCapture.Count(); i++)
	{
		CForsakenPlayer *pCapturePlayer = m_vecPlayersInCapture[i];

		// Are they the same player?
		if (pCapturePlayer == pPlayer)
			return true;
	}

	return false;
}

int CForsakenCaptureObjectiveEntity::FindPlayersInCapture(CUtlVector<CForsakenPlayer*> &rvecPlayerList, 
	int &nPlayerWeighting)
/*
	
	Pre: 
	Post: 
*/
{
	bool bCivInZone = false, bGovInZone = false, bRelInZone = false, bNonHomogenous = false;
	int nCapturingTeam = TEAM_UNASSIGNED, nTeamCount = 0;

	// Loop through all the players.
	for (int i = 1; i <= gpGlobals->maxClients || bNonHomogenous; i++)
	{
		// Get the player.
		CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_PlayerByIndex(i));

		// Is the player alive and connected?
		if (pPlayer && pPlayer->IsAlive() && pPlayer->IsConnected())
		{
			Hull_t hullType;
			trace_t modelTrace;

			// Set the hull type.
			hullType = HULL_HUMAN;
			if (pPlayer->GetFlags() & FL_DUCKING)
				hullType = HULL_SMALL_CENTERED;

			// Is the player in our bounds?
			UTIL_TraceModel(pPlayer->GetAbsOrigin(), pPlayer->GetAbsOrigin(), 
				NAI_Hull::Mins(hullType), NAI_Hull::Maxs(hullType), this, COLLISION_GROUP_NONE, 
				&modelTrace);

			// It is, so store the player index.
			if (modelTrace.startsolid)
			{
				// Find out what team the player is.
				switch (pPlayer->GetTeamNumber())
				{
				case TEAM_CIVILIANS:
					bCivInZone = true;
					nCapturingTeam = TEAM_CIVILIANS;

					break;

				case TEAM_GOVERNORS:
					bGovInZone = true;
					nCapturingTeam = TEAM_GOVERNORS;

					break;

				case TEAM_RELIGIOUS:
					bRelInZone = true;
					nCapturingTeam = TEAM_RELIGIOUS;

					break;
				}

				// Tell the player what objective they are capturing.
				pPlayer->SetObjectiveEntity(this);

				// One player counts as one weighting.
				nPlayerWeighting++;

				// A leader counts as two.
				if (pPlayer->IsLeader())
					nPlayerWeighting++;

				// Add the player to the player list.
				rvecPlayerList.AddToTail(pPlayer);
			}
		}
	}

	// Reset the step-out time for civilians.
	if (bCivInZone)
	{
		m_fStepOutTimeCiv = gpGlobals->curtime + sv_fskn_capturesafetytime.GetFloat();
		nTeamCount++;
	}

	// Reset the step-out time for governors.
	if (bGovInZone)
	{
		m_fStepOutTimeGov = gpGlobals->curtime + sv_fskn_capturesafetytime.GetFloat();
		nTeamCount++;
	}

	// Reset the step-out time for religious.
	if (bRelInZone)
	{
		m_fStepOutTimeRel = gpGlobals->curtime + sv_fskn_capturesafetytime.GetFloat();
		nTeamCount++;
	}

	// If there are no teams in the capture zone or too many, nobody can capture us.
	if (nTeamCount == 0)
		nCapturingTeam = TEAM_UNASSIGNED;
	else if (nTeamCount > 1)
		nCapturingTeam = TEAM_SPECTATOR;

	// Return the capturing team.
	return nCapturingTeam;
}

void CForsakenCaptureObjectiveEntity::CheckCaptureTimes()
/*
	
	Pre: 
	Post: 
*/
{
	// If the civilians have been capturing and their step out time was exceeded, reset their time.
	if (m_fCaptureTime != m_fRemainingCaptureTimeCiv && m_fStepOutTimeCiv <= gpGlobals->curtime && 
		m_nTeamCapturing != TEAM_CIVILIANS)
	{
		m_fRemainingCaptureTimeCiv = m_fCaptureTime;
		m_fStepOutTimeCiv = 0.0f;
	}

	// If the governors have been capturing and their step out time was exceeded, reset their time.
	if (m_fCaptureTime != m_fRemainingCaptureTimeGov && m_fStepOutTimeGov <= gpGlobals->curtime && 
		m_nTeamCapturing != TEAM_GOVERNORS)
	{
		m_fRemainingCaptureTimeGov = m_fCaptureTime;
		m_fStepOutTimeGov = 0.0f;
	}

	// If the religious have been capturing and their step out time was exceeded, reset their time.
	if (m_fCaptureTime != m_fRemainingCaptureTimeRel && m_fStepOutTimeRel <= gpGlobals->curtime && 
		m_nTeamCapturing != TEAM_RELIGIOUS)
	{
		m_fRemainingCaptureTimeRel = m_fCaptureTime;
		m_fStepOutTimeRel = 0.0f;
	}
}

void CForsakenCaptureObjectiveEntity::CaptureThink()
/*
	
	Pre: 
	Post: 
*/
{
	float fRemainCaptureTime = 0.0f;
	int nCapturingTeam = TEAM_UNASSIGNED, nCapturingTeamMask = 0, nPlayerWeighting = 0;

	// Don't allow capturing if we aren't in a round.
	if (ForsakenGameRules()->GetRemainingTimeInRound() == 0)
	{
		// Bug ID #0000073: We don't want to keep capturing due to client simulation.
		m_nPlayerWeighting = 0;
		m_nTeamCapturing = TEAM_UNASSIGNED;

		goto end_capture_think;
	}

	// Clear out the list of players in the objective.
	m_vecPlayersInCapture.RemoveAll();

	// Obtain a list of players in the capture zone.
	nCapturingTeam = FindPlayersInCapture(m_vecPlayersInCapture, nPlayerWeighting);

	// If nobody is capturing or if there are multiple teams, abort early.
	if (!m_bIsCaptureable || nCapturingTeam == TEAM_UNASSIGNED || 
		nCapturingTeam == TEAM_SPECTATOR)
	{
		if (!m_bIsCaptureable)
			m_nTeamCapturing = TEAM_UNASSIGNED;

		// We need to set the capturing team to who is capturing.
		// We do this here because either nobody is capturing or we have a challenger.
		m_nTeamCapturing = nCapturingTeam;

		// Reset any capture times necessary.
		CheckCaptureTimes();

		// There is no player weighting since nobody can capture.
		m_nPlayerWeighting = 0;

		goto end_capture_think;
	}

	// Reset any capture times necessary.
	CheckCaptureTimes();

	// This team already owns the objective.
	if (nCapturingTeam == m_nTeamOwner)
	{
		m_nTeamCapturing = TEAM_UNASSIGNED;

		// Bug Fix ID 0000040
		// We don't want them to have that annoying capture bar if they already own it :)
		m_vecPlayersInCapture.RemoveAll();

		goto end_capture_think;
	}

	// Who is capturing?
	switch (nCapturingTeam)
	{
	case TEAM_CIVILIANS:
		nCapturingTeamMask = TEAM_CIVILIANS_BITMASK;
		fRemainCaptureTime = m_fRemainingCaptureTimeCiv;
		break;

	case TEAM_GOVERNORS:
		nCapturingTeamMask = TEAM_GOVERNORS_BITMASK;
		fRemainCaptureTime = m_fRemainingCaptureTimeGov;
		break;

	case TEAM_RELIGIOUS:
		nCapturingTeamMask = TEAM_RELIGIOUS_BITMASK;
		fRemainCaptureTime = m_fRemainingCaptureTimeRel;
		break;
	}

	// Can they capture this objective?
	if (!(nCapturingTeamMask & m_nTeamCanCapture))
		goto end_capture_think;

	// Save the capturing team and player weighting.
	m_nTeamCapturing = nCapturingTeam;
	m_nPlayerWeighting = nPlayerWeighting;

	// Decrement the capture time.
	fRemainCaptureTime -= nPlayerWeighting * gpGlobals->frametime;

	// Somebody captured the objective.
	if (fRemainCaptureTime <= 0.0f)
	{
		// Loop through all of the players in the capture zone.
		for (int i = 0; i < m_vecPlayersInCapture.Count(); i++)
		{
			CForsakenPlayer *pPlayer = m_vecPlayersInCapture[i];
			CSingleUserRecipientFilter filter(pPlayer);
			tagPointsSummary newPoint;

			// Add a point summary item.
			newPoint.nPointID = POINTID_OBJECTIVE_COMPLETE;
			newPoint.nPoints = 5;

			UserMessageBegin(filter, "PointsNotice");
				WRITE_SHORT(5);
				WRITE_SHORT(UMSG_POINTS_OBJCAPTURE);
				WRITE_SHORT(m_nTeamCapturing);
			MessageEnd();

			// Notify the player of their successful capture.
			pPlayer->SendPointSummary(this, newPoint);
		}

		// Obtain a pointer to the new team owner and play a sound for the capture team.
		switch (m_nTeamCapturing)
		{
		case TEAM_CIVILIANS:
			EmitSound("Objective.CivCapture");
			break;

		case TEAM_GOVERNORS:
			EmitSound("Objective.GovCapture");
			break;

		case TEAM_RELIGIOUS:
			EmitSound("Objective.RelCapture");
			break;
		}

		// Reset capture, step-out safety times, and percentages.
		m_fRemainingCaptureTimeCiv = m_fRemainingCaptureTimeGov = m_fRemainingCaptureTimeRel = 
			m_fCaptureTime;
		m_fStepOutTimeCiv = m_fStepOutTimeGov = m_fStepOutTimeRel = 0.0f;

		// Send the objective completed event.
		m_ObjectiveCompleted.Set(nCapturingTeamMask, this, this);

		// Set the new team owner.
		m_nTeamOwner = m_nTeamCapturing;

		{
			IGameEvent *event = gameeventmanager->CreateEvent("forsaken_objective_completed");

			if (event)
			{
				event->SetInt("entindex", entindex());
				event->SetInt("team", m_nTeamOwner);

				gameeventmanager->FireEvent(event);
			}
		}

		goto end_capture_think;
	}
	else
	{
		// Set team capture times.
		switch (nCapturingTeam)
		{
		case TEAM_CIVILIANS:
			m_fRemainingCaptureTimeCiv = fRemainCaptureTime;
			break;

		case TEAM_GOVERNORS:
			m_fRemainingCaptureTimeGov = fRemainCaptureTime;
			break;

		case TEAM_RELIGIOUS:
			m_fRemainingCaptureTimeRel = fRemainCaptureTime;
			break;
		}

		goto end_capture_think;
	}

end_capture_think:
	// Define our think function.
	SetNextThink(gpGlobals->curtime);
	SetThink(&CForsakenCaptureObjectiveEntity::CaptureThink);

	return;
}

void CForsakenCaptureObjectiveEntity::SetTeamMapOwner(int nTeamOwner)
/*
	
	Pre: 
	Post: 
*/
{
	m_nTeamOwner = nTeamOwner;
}

void CForsakenCaptureObjectiveEntity::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	IGameEvent *event = gameeventmanager->CreateEvent("forsaken_objective_add");

	// Precache sounds.
	PrecacheScriptSound("Objective.CivCapture");
	PrecacheScriptSound("Objective.GovCapture");
	PrecacheScriptSound("Objective.RelCapture");

	// Use the model simply for hit testing.
	SetModel(STRING(GetModelName()));

	// We aren't a solid and we don't move.
	SetSolid(SOLID_NONE);
	SetMoveType(MOVETYPE_NONE);

	// Players don't see us.
	AddEffects(EF_NODRAW);

	// Reset the capture time.
	m_bIsCaptureable = true;
	m_fRemainingCaptureTimeCiv = m_fRemainingCaptureTimeGov = m_fRemainingCaptureTimeRel = m_fCaptureTime;
	m_nTeamOwner = TEAM_UNASSIGNED;
	m_nTeamCapturing = 0;

	// Set the event data.
	if (event)
	{
		event->SetInt("entindex", entindex());

		gameeventmanager->FireEvent(event);
	}

	// Define our think function.
	SetNextThink(gpGlobals->curtime + 0.1f);
	SetThink(&CForsakenCaptureObjectiveEntity::CaptureThink);
}