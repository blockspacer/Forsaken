/*
	forsaken_player.cpp
	Server-side forsaken player.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "engine/IEngineSound.h"
#include "forsaken_player.h"
#include "forsaken_fortified_entity.h"
#include "forsaken_team.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken/forsaken_weapon_base.h"
#include "forsaken/forsaken_objective_entity.h"
#include "predicted_viewmodel.h"
#include "iservervehicle.h"
#include "viewport_panel_names.h"
#include "util_shared.h"

// Player Models
// Civilians
#define CIV_LEADER		"models/player/civLeader/civLeader.mdl"
#define CIV_REGULAR		"models/player/civSoldier/civSoldier.mdl"

// Governors
#define GOV_LEADER		"models/player/govLeader/govLeader.mdl"
#define GOV_REGULAR		"models/player/govSoldier/govSoldier.mdl"

// Religious
#define REL_LEADER		"models/player/govLeader/govLeader.mdl"
#define REL_REGULAR		"models/player/govSoldier/govSoldier.mdl"

// Function Prototypes
bool operator == (const tagPointsSummary &left, const tagPointsSummary &right);
void CC_Forsaken_DropWeapon();
void CC_Forsaken_LeaderChoice();
void TE_PlayerAnimEvent(CBasePlayer *pPlayer, ForsakenPlayerAnimEvent_t animEvent);

// Internal Classes
class CForsakenPlayerRagdoll : public CBaseAnimatingOverlay
{
public:
	DECLARE_CLASS(CForsakenPlayerRagdoll, CBaseAnimatingOverlay);
	DECLARE_SERVERCLASS();

	// Constructor & Deconstructor

	// Public Accessor Functions

	// Public Functions
	virtual int UpdateTransmitState(void);

	// Public Variables
	CNetworkHandle(CBaseEntity, m_hPlayer);
	CNetworkVector(m_vecRagdollOrigin);
	CNetworkVector(m_vecRagdollVelocity);
};

class CTEPlayerAnimEvent : public CBaseTempEntity
{
public:
	DECLARE_CLASS(CTEPlayerAnimEvent, CBaseTempEntity);
	DECLARE_SERVERCLASS();

	// Constructor & Deconstructor
	CTEPlayerAnimEvent(const char *czName);

	// Public Accessor Functions

	// Public Functions

	// Public Variables
	CNetworkHandle(CBasePlayer, m_hPlayer);
	CNetworkVar(int, m_nEvent);
};

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_ragdoll, CForsakenPlayerRagdoll);
LINK_ENTITY_TO_CLASS(player, CForsakenPlayer);
PRECACHE_REGISTER(player);

BEGIN_SEND_TABLE_NOBASE(CForsakenPlayer, DT_ForsakenLocalPlayer)
	SendPropEHandle(SENDINFO(m_hObjectiveEntity)),
	SendPropInt(SENDINFO(m_nShotsFired), 8, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_nSharedResources), 8, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_nThrowCounter), 3, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_nLeaderChoice), 4),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST(CForsakenPlayer, DT_ForsakenPlayer)
	SendPropExclude("DT_BaseAnimating", "m_flPoseParameter"),
	SendPropExclude("DT_BaseAnimating", "m_flPlaybackRate"),
	SendPropExclude("DT_BaseAnimating", "m_nSequence"),
	//SendPropExclude("DT_BaseEntity", "m_angRotation"),
	SendPropExclude("DT_BaseAnimatingOverlay", "overlay_vars"),
	SendPropExclude("DT_ServerAnimationData", "m_flCycle"),
	SendPropExclude("DT_AnimTimeMustBeFirst", "m_flAnimTime"),
	SendPropDataTable("forsakenlocaldata", 0, &REFERENCE_SEND_TABLE(DT_ForsakenLocalPlayer), SendProxy_SendLocalDataTable),
	SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11),
	SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11),
	SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 2), 11),
	SendPropEHandle(SENDINFO(m_hRagdoll)),
	SendPropBool(SENDINFO(m_bIsKicking)),
	SendPropBool(SENDINFO(m_bIsLeader)),
	SendPropBool(SENDINFO(m_bIsLeaning)),
	SendPropBool(SENDINFO(m_bIsSprinting)),
	SendPropBool(SENDINFO(m_bIsLowSprint)),
	SendPropBool(SENDINFO(m_bIsStunned)),
	SendPropFloat(SENDINFO(m_fForwardSpeed)),
	SendPropFloat(SENDINFO(m_fKickEndTime)),
	SendPropFloat(SENDINFO(m_fMaxSprint)),
	SendPropFloat(SENDINFO(m_fSprint)),
	SendPropFloat(SENDINFO(m_fStunEndTime)),
	SendPropInt(SENDINFO(m_nLean), 2, SPROP_UNSIGNED),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST_NOBASE(CForsakenPlayerRagdoll, DT_ForsakenPlayerRagdoll)
	SendPropVector(SENDINFO(m_vecRagdollOrigin), -1, SPROP_COORD),
	SendPropEHandle(SENDINFO(m_hPlayer)),
	SendPropModelIndex(SENDINFO(m_nModelIndex)),
	SendPropInt(SENDINFO(m_nForceBone), 8, 0),
	SendPropVector(SENDINFO(m_vecForce), -1, SPROP_NOSCALE),
	SendPropVector(SENDINFO(m_vecRagdollVelocity)),
END_SEND_TABLE()

IMPLEMENT_SERVERCLASS_ST_NOBASE(CTEPlayerAnimEvent, DT_TEPlayerAnimEvent)
	SendPropEHandle(SENDINFO(m_hPlayer)),
	SendPropInt(SENDINFO(m_nEvent), Q_log2(FORSAKEN_PLAYERANIMEVENT_COUNT) + 1, SPROP_UNSIGNED),
END_SEND_TABLE()

// Global Variable Declerations
extern char *g_czMapName;
static ConCommand cc_dropweapon("dropweapon", CC_Forsaken_DropWeapon, "Drops the current weapon.");
static ConCommand cc_fskn_leaderchoice("fskn_leaderchoice", CC_Forsaken_LeaderChoice, 
	"Sets your preference on being leader. 0 = Yes, 1 = No, 2 = Don't Care");
ConVar sv_fskn_idplayertime("sv_fskn_idplayertime", "0.2", FCVAR_REPLICATED | FCVAR_ARCHIVE, 
	"The amount of time needed to be looking at a friendly before it identifies them.");
ConVar sv_fskn_loadouttime("sv_fskn_loadouttime", "30", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"The amount of time after spawning a player has to perform a loadout.");
static CTEPlayerAnimEvent g_TEPlayerAnimEvent("PlayerAnimEvent");

// Constructor & Deconstructor
CForsakenPlayer::CForsakenPlayer()
{
	m_bHasAuth = false;
	m_bHasSpawned = false;
	m_bInCredits = false;
	m_bIsLeaning = false;
	m_bIsKicking = false;
	m_bIsSprinting = false;
	m_fEndAuthTime = 0.0f;
	m_fLoadoutTime = 0.0f;
	m_fStunEndTime = 0.0f;
	m_fKickEndTime = 0.0f;
	m_nLeaderChoice = LEADEROPTION_MAYBE;

	m_pPlayerAnimState = CreatePlayerAnimState(this, this, LEGANIM_9WAY, true);

	UseClientSideAnimation();

	m_angEyeAngles.Init();
}

CForsakenPlayer::~CForsakenPlayer()
{
	m_pPlayerAnimState->Release();
}

CTEPlayerAnimEvent::CTEPlayerAnimEvent(const char *czName) : CBaseTempEntity(czName)
{
}

// Functions
// CForsakenPlayer
bool CForsakenPlayer::ClientCommand(const char *czCommand)
{
	// Join team command.
	if (Q_stricmp(czCommand, "jointeam") == 0)
	{
		if (engine->Cmd_Argc() < 2)
			return true;

		// Obtain the team number and team pointer.
		int nTeam = atoi(engine->Cmd_Argv(1));
		CForsakenTeam *pPlayerTeam = static_cast<CForsakenTeam*>(GetTeam());
		CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(g_Teams[nTeam]);

		// Are we auto-joining?
		if (nTeam == TEAM_UNASSIGNED)
		{
			// Re-fetch the team.
			nTeam = HandleAutoJoin();
			pTeam = static_cast<CForsakenTeam*>(g_Teams[nTeam]);
		}

		// The team isn't available for joining... so don't let them join.
		if (!pTeam->IsTeamAvailable())
		{
			CSingleUserRecipientFilter filter(this);

			UTIL_ClientPrintFilter(filter, HUD_PRINTCENTER, "That team is currently unavailable.");

			return true;
		}

		// If we aren't dead and we are switching teams, kill ourself.
		if (!IsDead() && GetTeamNumber() != nTeam && GetTeamNumber() != TEAM_UNASSIGNED)
			CommitSuicide();
		// We are in spectator mode and we are switching out of it, so end observer mode.
		else if (GetTeamNumber() == TEAM_SPECTATOR && nTeam != TEAM_SPECTATOR)
			StopObserverMode();

		// Change our team to the new team, if we haven't somehow gotten an unassigned team.
		if (nTeam != TEAM_UNASSIGNED)
			ChangeTeam(nTeam);

		return true;
	}

	return BaseClass::ClientCommand(czCommand);
}

bool CForsakenPlayer::SendPointSummary(CBaseEntity *pEntity, tagPointsSummary &rPointSum)
/*

	Pre: 
	Post: 
*/
{
	const char *czNewName = NULL;
	int nLen = 0, nOffset = 0;

	// What kind of score is it?
	switch (rPointSum.nPointID)
	{
	case POINTID_PLAYER_ASSIST:
	case POINTID_PLAYER_DAMAGE:
	case POINTID_PLAYER_KILL:
	case POINTID_PLAYER_LEADERDFND:
	case POINTID_PLAYER_SUICIDE:
		czNewName = ToForsakenPlayer(pEntity)->GetPlayerName();
		break;

	case POINTID_OBJECTIVE_ASSIST:
	case POINTID_OBJECTIVE_COMPLETE:
		czNewName = dynamic_cast<CForsakenObjectiveEntity*>(pEntity)->GetObjectiveName();
		break;

	case POINTID_NONE:
	default:
		return false;
	}

	// Determine the length and allocate memory.
	nLen = Q_strlen(czNewName);
	rPointSum.czName = new char[nLen];

	// Copy the string.
	Q_strncpy(rPointSum.czName, czNewName, nLen);

	// Look for an existing point for the object.
	nOffset = m_pointsummary.Find(rPointSum);

	// Add to the players score.
	IncrementScore(rPointSum.nPoints);

	// If the point exists, replace it with the new system.
	if (m_pointsummary.IsValidIndex(nOffset))
	{
		m_pointsummary[nOffset].nPointID = rPointSum.nPointID;
		m_pointsummary[nOffset].nPoints = rPointSum.nPoints;
	}
	else
		m_pointsummary.AddToTail(rPointSum);

	return true;
}

CForsakenPlayer *CForsakenPlayer::CreatePlayer(const char *czName, edict_t *pEdict)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer::s_PlayerEdict = pEdict;

	return (CForsakenPlayer*)CreateEntityByName(czName);
}

CForsakenWeaponBase *CForsakenPlayer::GetActiveForsakenWeapon() const
/*
	
	Pre: 
	Post: 
*/
{
	return dynamic_cast<CForsakenWeaponBase*>(GetActiveWeapon());
}

int CForsakenPlayer::AutoJoin(int nTeamA, int nTeamB, CForsakenTeam *pTeamA, 
	CForsakenTeam *pTeamB)
/*
	
	Pre: 
	Post: 
*/
{
	// To start off with, the team that isn't available is the one we go to.
	if (!pTeamA->IsTeamAvailable())
		return nTeamB;
	else if (!pTeamB->IsTeamAvailable())
		return nTeamA;
	// Now we go to the team with the lower score.
	else if (pTeamA->GetNumPlayers() > pTeamB->GetNumPlayers())
		return nTeamB;
	else if (pTeamB->GetNumPlayers() > pTeamA->GetNumPlayers())
		return nTeamA;
	// Well, obviously they have the same number of players... so go by score.
	else if (pTeamA->GetScore() > pTeamB->GetScore())
		return nTeamB;
	else if (pTeamB->GetScore() > pTeamB->GetScore())
		return nTeamA;
	// Hmmm, okay, now we need to go with the people that don't have enough food.
	else if (pTeamA->GetNutritionResources() > pTeamB->GetNutritionResources())
		return nTeamB;
	else if (pTeamB->GetNutritionResources() > pTeamA->GetNutritionResources())
		return nTeamA;
	// Wow, so things are pretty even so far. How about weapons?
	else if (pTeamA->GetOrdinanceResources() > pTeamB->GetOrdinanceResources())
		return nTeamB;
	else if (pTeamB->GetOrdinanceResources() > pTeamA->GetOrdinanceResources())
		return nTeamA;

	// At this point, it doesn't even matter.
	return TEAM_UNASSIGNED;
}

int CForsakenPlayer::HandleAutoJoin()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);

	// Determine the player counts in each team.
	int nCivs = pCivTeam->GetNumPlayers();
	int nGovs = pGovTeam->GetNumPlayers();
	int nRels = pRelTeam->GetNumPlayers();
	int nCivsFood = pCivTeam->GetNutritionResources();
	int nGovsFood = pCivTeam->GetNutritionResources();
	int nRelsFood = pCivTeam->GetNutritionResources();
	int nCivsScore = pCivTeam->GetScore();
	int nGovsScore = pGovTeam->GetScore();
	int nRelsScore = pRelTeam->GetScore();
	int nCivsWeap = pCivTeam->GetOrdinanceResources();
	int nGovsWeap = pCivTeam->GetOrdinanceResources();
	int nRelsWeap = pCivTeam->GetOrdinanceResources();

	// Let's start by comparing the number of people playing.
	if (pCivTeam->IsTeamAvailable() && nCivs > nGovs && nCivs > nRels)
	{
		// If both teams are disabled, skip 'em.
		if (!pGovTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_GOVERNORS, TEAM_RELIGIOUS, pGovTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_GOVERNORS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pGovTeam->IsTeamAvailable() && nGovs > nCivs && nGovs > nRels)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_RELIGIOUS, pCivTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pRelTeam->IsTeamAvailable() && nRels > nCivs && nRels > nGovs)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pGovTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_GOVERNORS, pCivTeam, pGovTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_GOVERNORS;
		}

		return nTeam;
	}
	// So nobody has more players than anyone else... so let's start comparing scores.
	else if (pCivTeam->IsTeamAvailable() && nCivsScore > nGovsScore && nCivsScore > nRelsScore)
	{
		// If both teams are disabled, skip 'em.
		if (!pGovTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_GOVERNORS, TEAM_RELIGIOUS, pGovTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_GOVERNORS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pGovTeam->IsTeamAvailable() && nGovsScore > nCivsScore && nGovsScore > nRelsScore)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_RELIGIOUS, pCivTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pRelTeam->IsTeamAvailable() && nRelsScore > nCivsScore && nRelsScore > nGovsScore)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pGovTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_GOVERNORS, pCivTeam, pGovTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_GOVERNORS;
		}

		return nTeam;
	}
	// So nobody has a higher score than anyone else... so let's start comparing food.
	else if (pCivTeam->IsTeamAvailable() && nCivsFood > nGovsFood && nCivsFood > nRelsFood)
	{
		// If both teams are disabled, skip 'em.
		if (!pGovTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_GOVERNORS, TEAM_RELIGIOUS, pGovTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_GOVERNORS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pGovTeam->IsTeamAvailable() && nGovsFood > nCivsFood && nGovsFood > nRelsFood)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_RELIGIOUS, pCivTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pRelTeam->IsTeamAvailable() && nRelsFood > nCivsFood && nRelsFood > nGovsFood)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pGovTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_GOVERNORS, pCivTeam, pGovTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_GOVERNORS;
		}

		return nTeam;
	}
	// So nobody has more food than anyone else... so let's start comparing weapons.
	else if (pCivTeam->IsTeamAvailable() && nCivsWeap > nGovsWeap && nCivsWeap > nRelsWeap)
	{
		// If both teams are disabled, skip 'em.
		if (!pGovTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_GOVERNORS, TEAM_RELIGIOUS, pGovTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_GOVERNORS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pGovTeam->IsTeamAvailable() && nGovsWeap > nCivsWeap && nGovsWeap > nRelsWeap)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pRelTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_RELIGIOUS, pCivTeam, pRelTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_RELIGIOUS;
		}

		return nTeam;
	}
	else if (pRelTeam->IsTeamAvailable() && nRelsWeap > nCivsWeap && nRelsWeap > nGovsWeap)
	{
		// If both teams are disabled, skip 'em.
		if (!pCivTeam->IsTeamAvailable() && !pGovTeam->IsTeamAvailable())
			return TEAM_UNASSIGNED;

		// Fetch the team they should join.
		int nTeam = AutoJoin(TEAM_CIVILIANS, TEAM_GOVERNORS, pCivTeam, pGovTeam);

		// We can't decide on a team, so choose between the two randomly.
		if (nTeam == TEAM_UNASSIGNED)
		{
			if ((int)gpGlobals->curtime % 2)
				return TEAM_CIVILIANS;
			else
				return TEAM_GOVERNORS;
		}

		return nTeam;
	}
	// Well... fuck it. At this point we randomize.
	else
	{
		int nTeam = RandomInt(TEAM_GOVERNORS, TEAM_RELIGIOUS);
		int nOrigTeam = nTeam;

		// Loop until we find a valid team.
		while (!static_cast<CForsakenTeam*>(g_Teams[nTeam])->IsTeamAvailable())
		{
			nTeam++;

			// We gotta re-cycle it.
			if (nTeam > TEAM_RELIGIOUS)
				nTeam = TEAM_GOVERNORS;

			// Well, we already hit all the possible teams... so have them go spectator.
			if (nTeam == nOrigTeam)
				return TEAM_SPECTATOR;
		}

		return nTeam;
	}
}

int CForsakenPlayer::OnTakeDamage(const CTakeDamageInfo &rDmgInfo)
/*
	
	Pre: 
	Post: 
*/
{
	bool bFound = false;
	CTakeDamageInfo dmgInfo = rDmgInfo;
	CForsakenPlayer *pAttacker = ToForsakenPlayer(dmgInfo.GetAttacker());
	tagPlayerAttacked newAttacked;
	tagPlayerAttackers newAttacker;

	// We are attacking ourselves if there is no attacker.
	if (!pAttacker)
		pAttacker = this;

	const char *czAttackerSteamID = engine->GetPlayerNetworkIDString(pAttacker->edict());
	const char *czMySteamID = engine->GetPlayerNetworkIDString(edict());

	// Make sure the attacker isn't already in our table.
	for (int i = 0; i < m_attackers.Count(); i++)
	{
		// Attacker already exists, so just update the last attack time.
		if (Q_stricmp(m_attackers[i].czSteamID, czAttackerSteamID) == 0)
		{
			bFound = true;
			m_attackers[i].fAttackTime = gpGlobals->curtime;

			break;
		}
	}

	// Not found, so add em to the list.
	if (!bFound)
	{
		Q_strcpy(newAttacker.czSteamID, czAttackerSteamID);
		newAttacker.fAttackTime = gpGlobals->curtime;
		newAttacker.nEntIndex = pAttacker->entindex();

		m_attackers.AddToTail(newAttacker);
	}

	bFound = false;

	// Now let's add this player to our attackers attacked table.
	if (pAttacker)
	{
		for (int i = 0; i < pAttacker->m_attacked.Count(); i++)
		{
			// Attacked already exists, so just update the damage amount.
			if (Q_stricmp(pAttacker->m_attacked[i].czSteamID, czMySteamID) == 0)
			{
				bFound = true;
				pAttacker->m_attacked[i].fDamage += dmgInfo.GetDamage();

				break;
			}
		}

		// Not found, so add em to the list.
		if (!bFound)
		{
			Q_strcpy(newAttacked.czSteamID, czMySteamID);
			newAttacked.fDamage = dmgInfo.GetDamage();
			newAttacked.nEntIndex = entindex();

			pAttacker->m_attacked.AddToTail(newAttacked);
		}
	}

	// Forsaken AURA: Religious pain threshold.
	if (GetAura() == RelAura_PainThreshold)
		dmgInfo.SetDamage(dmgInfo.GetDamage() * 0.75f);

	return BaseClass::OnTakeDamage(dmgInfo);
}

void CForsakenPlayer::AuraThink()
/*
	
	Pre: 
	Post: 
*/
{
	switch (m_laCurrentAura)
	{
	case Aura_None:
	default:
		break;
	}
}

void CForsakenPlayer::CheatImpulseCommands(int nImpulse)
/*
	
	Pre: 
	Post: 
*/
{
	if (nImpulse != 101)
	{
		BaseClass::CheatImpulseCommands(nImpulse);

		return ;
	}

	if (IsAlive() && sv_cheats->GetBool() && GetTeamNumber() != TEAM_SPECTATOR && 
		GetTeamNumber() != TEAM_UNASSIGNED)
	{
		// Give the player ammo.
		GiveAmmo(90, AMMO_12GAUGE);
		GiveAmmo(90, AMMO_223);
		GiveAmmo(12, AMMO_300);
		GiveAmmo(90, AMMO_45CAL_ACP);
		GiveAmmo(90, AMMO_45CAL_COLT);
		GiveAmmo(90, AMMO_9MM);
		GiveAmmo(90, AMMO_44CAL);
		GiveAmmo(2, AMMO_RPGMISSILE);

		if (GetHealth() < 100)
			TakeHealth(25, DMG_GENERIC);
	}
}

void CForsakenPlayer::CreateRagdollEntity()
/*
	
	Pre: 
	Post: 
*/
{
	if (m_hRagdoll)
	{
		UTIL_RemoveImmediate(m_hRagdoll);

		m_hRagdoll = NULL;
	}

	CForsakenPlayerRagdoll *pRagdoll = dynamic_cast<CForsakenPlayerRagdoll*>(m_hRagdoll.Get());

	if (!pRagdoll)
		pRagdoll =  dynamic_cast<CForsakenPlayerRagdoll*>(CreateEntityByName("forsaken_ragdoll"));

	if (pRagdoll)
	{
		pRagdoll->m_hPlayer = this;
		pRagdoll->m_vecRagdollOrigin = GetAbsOrigin();
		pRagdoll->m_vecRagdollVelocity = GetAbsVelocity();
		pRagdoll->m_nModelIndex = m_nModelIndex;
		pRagdoll->m_nForceBone = m_nForceBone;
		pRagdoll->m_vecForce = Vector(0, 0, 0);

		if(this->IsOnFire())
		{
			//pRagdoll->Ignite( 7.0f, false, 7.0f, false );
		}
	}

	m_hRagdoll = pRagdoll;
}

void CForsakenPlayer::CreateViewModel(int nViewModelIndex)
/*
	
	Pre: 
	Post: 
*/
{
	Assert(nViewModelIndex >= 0 && nViewModelIndex < MAX_VIEWMODELS);

	if (GetViewModel(nViewModelIndex))
		return;

	CPredictedViewModel *pPredViewModel = (CPredictedViewModel*)CreateEntityByName("predicted_viewmodel");

	if (pPredViewModel)
	{
		pPredViewModel->SetAbsOrigin(GetAbsOrigin());
		pPredViewModel->SetOwner(this);
		pPredViewModel->SetIndex(nViewModelIndex);

		DispatchSpawn(pPredViewModel);

		pPredViewModel->FollowEntity(this, false);

		m_hViewModel.Set(nViewModelIndex, pPredViewModel);
	}
}

void CForsakenPlayer::DisplayPointSummary()
/*
	
	Pre: 
	Post: 
*/
{
	CSingleUserRecipientFilter filter(this);

	UTIL_ClientPrintFilter(filter, HUD_PRINTCONSOLE, "Damage Summary\n");
	UTIL_ClientPrintFilter(filter, HUD_PRINTCONSOLE, "==============\n");

	// Just for shits and giggles, when we re-spawn em we'll print out the stuff.
	for (int i = 0; i < m_attacked.Count(); i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(m_attacked[i].nEntIndex);
		char czMessage[512] = "";

		// Make sure that it is still the same player.
		if (Q_stricmp(m_attacked[i].czSteamID, engine->GetPlayerNetworkIDString(pPlayer->edict())) == 0)
		{
			Q_snprintf(czMessage, sizeof(czMessage), "You did %f damage to %s.\n", 
				m_attacked[i].fDamage, pPlayer->GetPlayerName());

			UTIL_ClientPrintFilter(filter, HUD_PRINTCONSOLE, czMessage);
		}
	}

	UTIL_ClientPrintFilter(filter, HUD_PRINTCONSOLE, "Point Summary\n");
	UTIL_ClientPrintFilter(filter, HUD_PRINTCONSOLE, "==============\n");

	// Loop through all points.
	for (int i = 0; i < m_pointsummary.Count(); i++)
	{
		char czMessage[2048] = "";
		char *czPointMessage = NULL;
		tagPointsSummary pointSumm = m_pointsummary[i];

		switch (pointSumm.nPointID)
		{
		case POINTID_PLAYER_ASSIST:
			czPointMessage = "assisting in the kill of";
			break;

		case POINTID_PLAYER_DAMAGE:
			czPointMessage = "doing damage to";
			break;

		case POINTID_PLAYER_KILL:
		case POINTID_PLAYER_SUICIDE:
			czPointMessage = "killing";
			break;

		case POINTID_PLAYER_LEADERDFND:
			czPointMessage = "defending your leader";
			break;

		case POINTID_OBJECTIVE_ASSIST:
			czPointMessage = "assisting in the completion of";
			break;

		case POINTID_OBJECTIVE_COMPLETE:
			czPointMessage = "completing";
			break;

		default:
			czPointMessage = "";
			break;
		}

		Q_snprintf(czMessage, sizeof(czMessage), "%d points for %s %s", pointSumm.nPoints, 
			czPointMessage, pointSumm.czName);

		UTIL_ClientPrintFilter(filter, HUD_PRINTCONSOLE, czMessage);

		delete [] pointSumm.czName;
	}

	// Reset who we attacked, who attacked us, and the point summary.
	m_attacked.RemoveAll();
	m_attackers.RemoveAll();
	m_pointsummary.RemoveAll();
}

void CForsakenPlayer::DoAnimationEvent(ForsakenPlayerAnimEvent_t animEvent)
/*
	
	Pre: 
	Post: 
*/
{
	if (animEvent == FORSAKEN_PLAYERANIMEVENT_THROW)
		m_nThrowCounter = (m_nThrowCounter + 1) % (1 << 3);
	else
	{
		m_pPlayerAnimState->DoAnimationEvent(animEvent);
		TE_PlayerAnimEvent(this, animEvent);
	}
}

void CForsakenPlayer::Event_Killed(const CTakeDamageInfo &rInfo)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Event_Killed(rInfo);

	if (GetTeamNumber() != TEAM_SPECTATOR && GetTeamNumber() != TEAM_UNASSIGNED)
		CreateRagdollEntity();

	CBaseEntity *pEffect = GetEffectEntity();

	if (pEffect != NULL)
	{
		// Remove effects, like being on fire.
		UTIL_Remove(pEffect);
	}

	// Display the point summary when they die.
	DisplayPointSummary();
}

void CForsakenPlayer::InitialSpawn()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::InitialSpawn();

	if (Q_stricmp(g_czMapName, "credits"))
	{
		const ConVar *pHostname = cvar->FindVar("hostname");
		const char *czTitle = pHostname ? pHostname->GetString() : "Message of the Day";
		KeyValues *pData = new KeyValues("data");

		pData->SetString("title", czTitle);
		pData->SetString("type", "1");
		pData->SetString("msg", "motd");

		ShowViewPortPanel(PANEL_INFO, true, pData);

		pData->deleteThis();
	}
	else
		m_bInCredits = true;

	m_bHasSpawned = false;
}

void CForsakenPlayer::PostThink()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::PostThink();

	// Do that special aura stuff.
	AuraThink();

	int nSprintMultiplier = 1;
	QAngle newAngle = GetLocalAngles();

	m_angEyeAngles = EyeAngles();
	newAngle[PITCH] = 0;

	m_pPlayerAnimState->Update(m_angEyeAngles[YAW], m_angEyeAngles[PITCH]);
	SetLocalAngles(newAngle);

	// Team-specific handling of data.
	switch (GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		// Increase the sprint.
		if (m_fSprint > GetMaxSprint())
			nSprintMultiplier = -1;

		IncrementSprint((GetMaxSprint() / 36.0) * gpGlobals->frametime * nSprintMultiplier);

		break;

	case TEAM_RELIGIOUS:
		// Increase the sprint.
		if (m_fSprint > GetMaxSprint())
			nSprintMultiplier = -1;

		IncrementSprint((GetMaxSprint() / 60.0) * gpGlobals->frametime * nSprintMultiplier);

		break;

	default:
		break;
	}

	// Clamp the sprint value.
	if (m_fSprint > GetMaxSprint() && nSprintMultiplier == 1)
		m_fSprint = m_fMaxSprint;

	// For catching other players on fire
	TouchThink();
}

void CForsakenPlayer::Precache()
/*
	
	Pre: 
	Post: 
*/
{
	// Player model precache.
	// Civilians.
	PrecacheModel(CIV_LEADER);
	PrecacheModel(CIV_REGULAR);

	// Governors.
	PrecacheModel(GOV_LEADER);
	PrecacheModel(GOV_REGULAR);

	// Religious.
	PrecacheModel(REL_LEADER);
	PrecacheModel(REL_REGULAR);

	// Player sounds.
	PrecacheScriptSound("Player.LowHealth");
	PrecacheScriptSound("Player.SprintNoPower");
	PrecacheScriptSound("Player.SprintStart");

	BaseClass::Precache();
}

void CForsakenPlayer::PreThink()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenObjectiveEntity *pObjectiveEntity = m_hObjectiveEntity.Get();

	// If we are dead, clear our animation state and other flags.
	if (m_lifeState == LIFE_DEAD)
	{
		m_bIsStunned = false;
		m_bIsSprinting = false;
		m_fStunEndTime = 0.0f;
		m_pPlayerAnimState->ClearAnimationState();
	}

	// If we ran out of stun time, clear the flags.
	if (gpGlobals->curtime >= m_fStunEndTime)
	{
		m_bIsStunned = false;
		m_fStunEndTime = 0.0f;
	}

	// Our custom movement code :)
	HandleCustomMovement();

	// Are we still affecting the objective entity? If not, hide that shit.
	if (pObjectiveEntity && !pObjectiveEntity->IsPlayerAffectingObjective(this))
		m_hObjectiveEntity = NULL;

	BaseClass::PreThink();
}

void CForsakenPlayer::SetStunned(bool bIsStunned, float fPercentage)
/*
	
	Pre: 
	Post: 
*/
{
	CSingleUserRecipientFilter user(this);

	// We need to give the user a ringing-in-the-ears.
	enginesound->SetPlayerDSP(user, random->RandomInt(35, 37), false);

	// Set the player to stunned.
	m_bIsStunned = bIsStunned;
	m_fStunEndTime = gpGlobals->curtime + (FORSAKEN_M84STUN_TIME * fPercentage);
}

void CForsakenPlayer::TouchThink()
/*
	
	Pre: 
	Post: 
*/
{
	if (!IsOnFire() || IsDead())
	{
		return;
	}

	Vector nearbyMins;
	Vector nearbyMaxs;

	float boundingRadius = BoundingRadius();
	float scale = 1.2f;	// Set people on fire even if not quite touching

	boundingRadius *= scale;

	nearbyMins.Init(-boundingRadius, -boundingRadius, 0);
	nearbyMaxs.Init(boundingRadius, boundingRadius, 2*boundingRadius);

	//NDebugOverlay::Box( GetAbsOrigin(), nearbyMins, nearbyMaxs, 255, 255, 255, 0, 0.1f );
	nearbyMins += GetAbsOrigin();
	nearbyMaxs += GetAbsOrigin();

	CBaseEntity *pNearby[256];
	int nearbyCount = UTIL_EntitiesInBox( pNearby, ARRAYSIZE(pNearby), nearbyMins, nearbyMaxs, 0 );

	for ( int nNearEnt = 0; nNearEnt < nearbyCount; nNearEnt++ )
	{
		CForsakenPlayer *pOther = ToForsakenPlayer(pNearby[nNearEnt]);

		if (pOther == NULL)
		{
			continue;
		}
		else if ( pOther == this )
		{
			continue;
		}
		else if ( FClassnameIs( pOther, "env_fire" ) )
		{
			continue;
		}
		else
		{
			// Make sure can actually see entity (don't damage through walls)
			trace_t tr;
			// NPCs and grates do not prevent fire from travelling
			int	mask = MASK_SOLID & (~(CONTENTS_MONSTER|CONTENTS_GRATE));
			UTIL_TraceLine( WorldSpaceCenter(), pOther->WorldSpaceCenter(), mask, pOther, COLLISION_GROUP_NONE, &tr );
			
			//debugoverlay->AddLineOverlay(WorldSpaceCenter(), pOther->WorldSpaceCenter(), 255, 255, 255, false, 0.1f);

			if (tr.fraction == 1.0 && !tr.startsolid)
			{
				if (!pOther->IsAlive())
				{
					continue;
				}

				CBaseEntity *pEffect = GetEffectEntity();

				if (pEffect != NULL)
				{
					//pOther->Ignite( 7.0f, false, 7.0f, false, pEffect->GetOwnerEntity() );	
				}
			}
		}
	}
}

void CForsakenPlayer::TraceAttack(const CTakeDamageInfo &rInfo, const Vector &vecDirection, 
	trace_t *pTrace)
/*
	
	Pre: 
	Post: 
*/
{
	CSingleUserRecipientFilter filter(this);
	BaseClass::TraceAttack(rInfo, vecDirection, pTrace);

	// Send the damage user message.
	UserMessageBegin(filter, "Damage");
		WRITE_BYTE(0);
		WRITE_BYTE((byte)rInfo.GetDamage());
		WRITE_LONG(rInfo.GetDamageType());
		WRITE_FLOAT(vecDirection.x);
		WRITE_FLOAT(vecDirection.y);
		WRITE_FLOAT(vecDirection.z);
	MessageEnd();
}

void CForsakenPlayer::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	char *czLoadoutPanel = NULL;

	// Remove the ragdoll.
	if (m_hRagdoll)
	{
		UTIL_RemoveImmediate(m_hRagdoll);

		m_hRagdoll = NULL;
	}

	// We're spawning, so we can't be sprinting and set the has spawned flag to true.
	m_bHasSpawned = true;
	m_bIsLeaning = false;
	m_bIsKicking = false;
	m_bIsSprinting = false;
	m_fKickEndTime = 0.0f;
	m_nLean = LEAN_DIR_NONE;

	// Display point summary on spawn if they lived.
	if (IsAlive())
		DisplayPointSummary();

	// If they are dead they should open the weapon loadout screen.
	if (IsDead())
	{
		switch (GetTeamNumber())
		{
		case TEAM_CIVILIANS:
			czLoadoutPanel = PANEL_WEAPLOADOUT_CIV;

			break;

		case TEAM_GOVERNORS:
			czLoadoutPanel = PANEL_WEAPLOADOUT_GOV;

			break;

		case TEAM_RELIGIOUS:
			czLoadoutPanel = PANEL_WEAPLOADOUT_REL;

			break;

		default:
			break;
		}
	}

	// We are now solid, and we walk.
	SetMoveType(MOVETYPE_WALK);
	RemoveSolidFlags(FSOLID_NOT_SOLID);

	// Set the amount of time they have to loadout.
	m_fLoadoutTime = gpGlobals->curtime + sv_fskn_loadouttime.GetFloat();

	// Reset our animation state.
	//SetAnimation(PLAYER_WALK);

	// No ragdoll, and don't hide the HUD.
	m_hRagdoll = NULL;
	m_Local.m_iHideHUD = 0;

	// Bug Fix ID 0000032: Hide the round win panel when we spawn.
	ShowViewPortPanel(PANEL_ROUNDWIN, false);

	// Bug Fix ID 0000071: Show the weapon loadout panel if they died.
	if (czLoadoutPanel)
		ShowViewPortPanel(czLoadoutPanel, true);

	// Bug Fix ID 0000057: We want more accurate hit detection.
	CollisionProp()->SetSurroundingBoundsType(USE_HITBOXES);

	CBaseEntity *pEffect = GetEffectEntity();

	if (pEffect != NULL)
	{
		// Remove effects, like being on fire.
		UTIL_Remove(pEffect);
	}

	BaseClass::Spawn();

	// Team-specific handling of data.
	switch (GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		// Set the model to regular by default, and change it later if you become leader.
		SetModel(CIV_REGULAR);
		m_fSprint = m_fMaxSprint = 100.0f;

		// They at least get a melee weapon.
		GiveNamedItem("forsaken_melee_chisel");

		break;

	case TEAM_GOVERNORS:
		// Set the model to regular by default, and change it later if you become leader.
		SetModel(GOV_REGULAR);
		m_fSprint = m_fMaxSprint = 0.0f;

		// They at least get a melee weapon.
		GiveNamedItem("forsaken_melee_govknife");

		break;

	case TEAM_RELIGIOUS:
		// Set the model to regular by default, and change it later if you become leader.
		SetModel(REL_REGULAR);
		m_fSprint = m_fMaxSprint = 100.0f;

		// They at least get a melee weapon.
		GiveNamedItem("forsaken_melee_relknife");

		break;

	default:
		SetModel(GOV_REGULAR);

		break;
	}
}

void CForsakenPlayer::UpdateModel()
/*
	Updates the player model to the new state.
	Pre: None.
	Post: Player model updated properly.
*/
{
	// Team-specific player model.
	switch (GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		if (IsLeader())
			SetModel(CIV_LEADER);
		else
			SetModel(CIV_REGULAR);

		break;

	case TEAM_GOVERNORS:
		if (IsLeader())
			SetModel(GOV_LEADER);
		else
			SetModel(GOV_REGULAR);

		break;

	case TEAM_RELIGIOUS:
		if (IsLeader())
			SetModel(REL_LEADER);
		else
			SetModel(REL_REGULAR);

		break;

	default:
		SetModel(GOV_REGULAR);

		break;
	}
}

void CForsakenPlayer::Weapon_Drop(CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, 
	const Vector *pVelocity)
/*
	Description.
	Pre: 
	Post: 
*/
{
	// Are we dead or dying?
	if (m_lifeState == LIFE_DYING || m_lifeState == LIFE_DEAD)
	{
		CForsakenWeaponBase *pForsakenWeapon = dynamic_cast<CForsakenWeaponBase*>(pWeapon);

		// This is a kill-drop weapon.
		if (pForsakenWeapon)
			pForsakenWeapon->m_bKillDrop = true;
	}

	// Bug Fix ID 0000055: If we drop a weapon and it doesn't use clips, we voluntarily lose all of the ammo in that weapon.
	if (pWeapon)
	{
		if (pWeapon->GetMaxClip1() == -1)
			SetAmmo(0, pWeapon->GetPrimaryAmmoType());
		if (pWeapon->GetMaxClip2() == -1)
			SetAmmo(0, pWeapon->GetSecondaryAmmoType());
	}

	BaseClass::Weapon_Drop(pWeapon, pvecTarget, pVelocity);
}

// CForsakenRagdoll
int CForsakenPlayerRagdoll::UpdateTransmitState(void)
/*
	
	Pre: 
	Post: 
*/
{
	return SetTransmitState(FL_EDICT_ALWAYS);
}

// Global
bool operator == (const tagPointsSummary &left, const tagPointsSummary &right)
{
	if (Q_strcmp(left.czName, right.czName) == 0)
		return true;

	return false;
}

void CC_Forsaken_DropWeapon()
/*

Pre: 
Post: 
*/
{
	CBasePlayer *pPlayer = UTIL_GetCommandClient();

	if (pPlayer && pPlayer->GetActiveWeapon())
	{
		CForsakenWeaponBase *pWeapon = dynamic_cast<CForsakenWeaponBase*>(pPlayer->GetActiveWeapon());
		const char *czClassname = pPlayer->GetActiveWeapon()->GetClassname();

		// Don't let players drop their melee weapons.
		if (Q_strcmp(czClassname, "forsaken_melee_chisel") != 0 && 
			Q_strcmp(czClassname, "forsaken_melee_govknife") != 0 &&
			Q_strcmp(czClassname, "forsaken_melee_relknife") != 0)
		{
			// If the weapon is a leader weapon, don't allow drop.
			if (pWeapon && !pWeapon->IsLeaderWeapon())
				pPlayer->Weapon_Drop(pPlayer->GetActiveWeapon(), NULL, NULL);
		}
	}
}

void CC_Forsaken_LeaderChoice()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	int nChoice = atoi(engine->Cmd_Argv(1));

	// Is it a valid player and do they have a valid choice?
	if (pPlayer && nChoice >= LEADEROPTION_YES && nChoice <= LEADEROPTION_MAYBE)
		pPlayer->SetLeaderChoice(nChoice);
}

void TE_PlayerAnimEvent(CBasePlayer *pPlayer, ForsakenPlayerAnimEvent_t animEvent)
/*
	
	Pre: 
	Post: 
*/
{
	CPVSFilter filter((const Vector &)pPlayer->EyePosition());

	g_TEPlayerAnimEvent.m_hPlayer = pPlayer;
	g_TEPlayerAnimEvent.m_nEvent = animEvent;

	g_TEPlayerAnimEvent.Create(filter, 0.0f);
}
