/*
	forsaken_gamerules.cpp
	Game rules class for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_gamerules_territorywar.h"
#include "ammodef.h"
#include "KeyValues.h"
#include "forsaken_weapon_base.h"

#ifndef CLIENT_DLL

#include "filesystem.h"
#include "gameinterface.h"
#include "forsaken/forsaken_player.h"
#include "forsaken/forsaken_fortified_entity.h"
#include "forsaken/forsaken_team.h"
#include "forsaken/forsaken_spawnpoint.h"
#include "voice_gamemgr.h"
#include "team.h"
#include "mapentities.h"
#include "UtlSortVector.h"
#include "forsaken/forsaken_objective_entity.h"
#include "forsaken/forsaken_capture_entity.h"
#include "forsaken/forsaken_destroy_entity.h"
#include "forsaken/forsaken_mapentity_filter.h"
#include "forsaken/forsaken_winlogic_entity.h"
#include "forsaken/forsaken_util.h"
#include "viewport_panel_names.h"
#include "hltvdirector.h"
#include "tier1/fmtstr.h"

#endif

#include "tier0/memdbgon.h"

// Function Prototypes
CAmmoDef *GetAmmoDef();
char *ParseVoiceSelection_Direcitonal(int nSelection);
char *ParseVoiceSelection_General(int nSelection);
char *ParseVoiceSelection_Team(int nSelection);
static void CC_Leader_ChooseSpawn();
static void CC_Leader_UpgradeFortification();
static void CC_RoundRestart(ConVar *pConsoleVar, const char *czOldValue);
static void CC_WarInfo();
static void CC_WarRestart(ConVar *pConsoleVar, const char *czOldValue);

// HL2 Class Macros
REGISTER_GAMERULES_CLASS(CForsakenGameRules);

BEGIN_NETWORK_TABLE_NOBASE(CForsakenGameRules, DT_ForsakenGameRules)
#ifdef CLIENT_DLL
	RecvPropInt(RECVINFO(m_nRoundTime)),
#else
	SendPropInt(SENDINFO(m_nRoundTime)),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(forsaken_gamerules, CForsakenGameRulesProxy);
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenGameRulesProxy, DT_ForsakenGameRulesProxy)

// Global Variable Decleration
#ifndef CLIENT_DLL
ConVar sv_fskn_roundduration("sv_fskn_roundduration", "300", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"Sets the round duration time.");
ConVar sv_fskn_roundlimit("sv_fskn_roundlimit", "10", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"The number of rounds a team needs to win a map.");
ConVar sv_fskn_roundspawntime("sv_fskn_roundspawntime", "10", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"Sets the amount of time a player joining or changing teams can spawn after a round start.");
ConVar sv_fskn_roundwinpause("sv_fskn_roundwinpause", "5", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"The amount of time between rounds.");
ConVar sv_fskn_enableauras("sv_fskn_enableauras", "0", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"Enables(1) or disables(0) the aura system.");
ConVar sv_fskn_endwarsummary("sv_fskn_endwarsummary", "15", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"The amount of time to wait after a war has ended before starting a new one.");
ConVar sv_fskn_starvinghealthdecrease("sv_fskn_starvinghealthdecrease", "20", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, 
	"The amount of health to decrease the player by when a faction is starving.");
static ConCommand fskn_warinfo("fskn_warinfo", CC_WarInfo, "Retrieves information on the war.");
static ConCommand leader_choosespawn("fskn_leader_choosespawn", CC_Leader_ChooseSpawn, 
	"If the executing player is the leader, sets the teams spawn.");
static ConCommand leader_upgrade("fskn_leader_upgradefortification", CC_Leader_UpgradeFortification, 
	"If the executing player is the leader, upgrades the fortification.");
static ConVar sv_fskn_restartround("sv_fskn_restartround", "0", FCVAR_REPLICATED | FCVAR_NOTIFY, 
	"Restart the current round (set the value to 1).", true, 0, true, 1, CC_RoundRestart);
static ConVar sv_fskn_restartwar("sv_fskn_restartwar", "0", FCVAR_REPLICATED | FCVAR_NOTIFY, 
	"Restart the war(set the value to 1).", true, 0, true, 1, CC_WarRestart);
extern char *g_czMapName;
extern ConVar mp_chattime;
extern ConVar tv_delaymapchange;
#endif

// Functions
// CForsakenGameRules
bool CForsakenGameRules::ShouldCollide(int nCollisionGroup0, int nCollisionGroup1)
/*
	
	Pre: 
	Post: 
*/
{
	// Swap so the lowest is always first.
	if (nCollisionGroup0 > nCollisionGroup1)
		swap(nCollisionGroup0, nCollisionGroup1);

	// Don't stand on weapons.
	if (nCollisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT &&
		nCollisionGroup1 == COLLISION_GROUP_WEAPON)
		return false;

	return BaseClass::ShouldCollide( nCollisionGroup0, nCollisionGroup1 ); 
}

int CForsakenGameRules::PlayerRelationship(CBaseEntity *pentPlayer, CBaseEntity *pentTarget)
/*
	
	Pre: 
	Post: 
*/
{
#ifndef CLIENT_DLL
	if (!pentPlayer || !pentTarget || !pentTarget->IsPlayer() || !IsTeamplay())
		return GR_NOTTEAMMATE;

	if (pentPlayer->GetTeamNumber() == pentTarget->GetTeamNumber())
		return GR_TEAMMATE;
#endif

	return GR_NOTTEAMMATE;
}

void CForsakenGameRules::FireGameEvent(IGameEvent *pEvent)
/*
	
	Pre: 
	Post: 
*/
{
#ifndef CLIENT_DLL
	if (Q_stricmp(pEvent->GetName(), "forsaken_objective_add") == 0)
		ObjectiveAdd(pEvent);
	else if (Q_stricmp(pEvent->GetName(), "forsaken_objective_completed") == 0)
		ObjectiveCompleted(pEvent);
	else if (Q_stricmp(pEvent->GetName(), "forsaken_objective_damaged") == 0)
		ObjectiveTakingDamage(pEvent);
	else if (Q_stricmp(pEvent->GetName(), "forsaken_objective_destroyed") == 0)
		ObjectiveDestroyed(pEvent);
#endif
}

// Global Functions
CAmmoDef *GetAmmoDef()
/*
	
	Pre: 
	Post: 
*/
{
	static bool bInitialized = false;
	static CAmmoDef ammoDef;

	if (!bInitialized)
	{
		bInitialized = true;

		// Add the forsaken-specific ammo types.
		ammoDef.AddAmmoType(AMMO_GRENADE, DMG_BLAST, TRACER_LINE, 0, 0,	2, 150, 0);
		ammoDef.AddAmmoType(AMMO_MOLOTOV, DMG_BURN, TRACER_LINE, 0, 0,	2, 150, 0);
		ammoDef.AddAmmoType(AMMO_12GAUGE, DMG_BULLET, TRACER_LINE, 0, 0, 90, 150, 0);
		ammoDef.AddAmmoType(AMMO_223, DMG_BULLET, TRACER_LINE, 0, 0, 90, 150, 0);
		ammoDef.AddAmmoType(AMMO_300, DMG_BULLET, TRACER_LINE, 0, 0, 12, 150, 0);
		ammoDef.AddAmmoType(AMMO_45CAL_ACP, DMG_BULLET, TRACER_LINE, 0, 0, 90, 150, 0);
		ammoDef.AddAmmoType(AMMO_45CAL_COLT, DMG_BULLET, TRACER_LINE, 0, 0, 90, 150, 0);
		ammoDef.AddAmmoType(AMMO_9MM, DMG_BULLET, TRACER_LINE, 0, 0, 90, 150, 0);
		ammoDef.AddAmmoType(AMMO_44CAL, DMG_BULLET, TRACER_LINE, 0, 0, 90, 150, 0);
		ammoDef.AddAmmoType(AMMO_RPGMISSILE, DMG_BULLET, TRACER_LINE, 0, 0, 190, 50, 0);
		ammoDef.AddAmmoType(AMMO_STUN, DMG_BLAST, TRACER_LINE, 0, 0, 2, 0, 0);
	}

	return &ammoDef;
}

#ifdef CLIENT_DLL

// Function Prototypes
void RecvProxy_ForsakenGameRules(const RecvProp *pProp, void **pOut, void *pData, int nObjectID);

// HL2 Class Macros
BEGIN_RECV_TABLE(CForsakenGameRulesProxy, DT_ForsakenGameRulesProxy)
	RecvPropDataTable("forsaken_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE(DT_ForsakenGameRules), 
		RecvProxy_ForsakenGameRules)
END_RECV_TABLE()

// Global Functions
void RecvProxy_ForsakenGameRules(const RecvProp *pProp, void **pOut, void *pData, int nObjectID)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenGameRules *pRules = ForsakenGameRules();

	Assert(pRules);

	*pOut = pRules;
}

#else

// Function Prototypes
const char *FindEngineArg(const char *czName);
int FindEngineArgInt(const char *czName, int nDefaultVal);
void InitBodyQue();
void *SendProxy_ForsakenGameRules(const SendProp *pProp, const void *pStructBase, const void *pData, 
								 CSendProxyRecipients *pRecipients, int nObjectID);

// Internal Classes
class CVoiceGameManagerHelper : public IVoiceGameMgrHelper
{
public:
	// Public Functions
	virtual bool CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pTalker)
	{
		if (!pTalker->IsAlive())
		{
			if (!pListener->IsAlive())
				return pListener->InSameTeam(pTalker);

			return false;
		}

		return pListener->InSameTeam(pTalker);
	}
};

// Global Variable Declarations
CVoiceGameManagerHelper g_VoiceGameMgrHelper;
IVoiceGameMgrHelper *g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;
char *sTeamNames[] =
{
		"Unassigned",
		"Governors",
		"Civilians",
		"Religious",
		"Spectator"
};

// HL2 Class Macros
BEGIN_SEND_TABLE(CForsakenGameRulesProxy, DT_ForsakenGameRulesProxy)
	SendPropDataTable("forsaken_gamerules_data", 0, &REFERENCE_SEND_TABLE(DT_ForsakenGameRules), SendProxy_ForsakenGameRules)
END_SEND_TABLE()

// Constructor & Deconstructor
CForsakenGameRules::CForsakenGameRules()
{
	for (int i = 0; i < ARRAYSIZE(sTeamNames); i++)
	{
		CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(CreateEntityByName("forsaken_team_manager"));

		// TODO: Remove religious not being available.
		if (i == TEAM_RELIGIOUS)
			pTeam->SetTeamAvailability(false);
		else
			pTeam->SetTeamAvailability(true);

		// Set the default spawn health for each team.
		switch (i)
		{
		case TEAM_CIVILIANS:
			pTeam->SetSpawnHealth(80);
			break;

		case TEAM_GOVERNORS:
			pTeam->SetSpawnHealth(100);
			break;

		case TEAM_RELIGIOUS:
			pTeam->SetSpawnHealth(120);
			break;

		default:
			pTeam->SetSpawnHealth(5);
			break;
		}

		pTeam->Init(sTeamNames[i], i);

		g_Teams.AddToTail(pTeam);
	}

	// Initialize variables.
	m_bInRound = false;
	m_bWarOver = false;
	m_fNextAuraThinkTime = 0.0f;
	m_fRoundFinalizeTime = 0.0f;
	m_fRoundStartTime = 0.0f;
	m_nRoundCount = 0;
	m_nRoundDuration = 0;

	// We want to listen to these events.
	gameeventmanager->AddListener(this, "forsaken_objective_add", true);
	gameeventmanager->AddListener(this, "forsaken_objective_completed", true);
	gameeventmanager->AddListener(this, "forsaken_objective_damaged", true);
	gameeventmanager->AddListener(this, "forsaken_objective_destroyed", true);

	CreateEntityByName("forsaken_gamerules");
}

CForsakenGameRules::~CForsakenGameRules()
{
	gameeventmanager->RemoveListener(this);

	g_Teams.Purge();
}

// Functions
// CForsakenGameRules
bool CForsakenGameRules::ClientCommand(const char *czCommand, CBaseEntity *pentEdict)
/*
	
	Pre: 
	Post: 
*/
{	
	CForsakenPlayer *pPlayer = ToForsakenPlayer(ToBasePlayer(pentEdict));

	// Player issuing a voice command?
	if (pPlayer && FStrEq(czCommand, "menuselect") && pPlayer->GetVoiceCommandState() != VOICECMD_NONE)
	{
		CForsakenTeam *pTeam = (CForsakenTeam*)g_Teams[pPlayer->GetTeamNumber()];
		char czSound[256] = "";
		char czTalkMsg[1024] = "";

		if (engine->Cmd_Argc() < 2 || pPlayer->GetVoiceCommandState() == VOICECMD_NONE)
			return true;

		const char *czGenericSound = NULL;
		int nSlot = atoi(engine->Cmd_Argv(1));

		// It's an exit.
		if (nSlot == 0 || nSlot == 10)
			return true;

		// What sound should we play?
		switch (pPlayer->GetVoiceCommandState())
		{
		case VOICECMD_DIRECTIONAL:
			czGenericSound = ParseVoiceSelection_Direcitonal(nSlot);
			break;

		case VOICECMD_GENERAL:
			czGenericSound = ParseVoiceSelection_General(nSlot);
			break;

		case VOICECMD_TEAM:
			czGenericSound = ParseVoiceSelection_Team(nSlot);
			break;
		}

		// What team-specific sound should we be making?
		switch (pPlayer->GetTeamNumber())
		{
		case TEAM_CIVILIANS:
			Q_snprintf(czSound, sizeof(czSound), "Civ.%s", czGenericSound);
			break;

		case TEAM_GOVERNORS:
			Q_snprintf(czSound, sizeof(czSound), "Gov.%s", czGenericSound);
			break;

		case TEAM_RELIGIOUS:
			Q_snprintf(czSound, sizeof(czSound), "Rel.%s", czGenericSound);
			break;

		default:
			Q_snprintf(czSound, sizeof(czSound), "%s", czGenericSound);
			break;
		}

		// A message to teammates for radio commands.
		Q_snprintf(czTalkMsg, sizeof(czTalkMsg), "(%s) - %s", pPlayer->GetPlayerName(), 
			czSound);

		// Loop through all players on our team.
		for (int i = 0; i < pTeam->GetNumPlayers(); i++)
		{
			CBasePlayer *pBroadcastPlayer = pTeam->GetPlayer(i);
			CSingleUserRecipientFilter filter(pBroadcastPlayer);

			// Emit the sound to that player.
			CBaseEntity::EmitSound(filter, pBroadcastPlayer->entindex(), czSound, 
				&pBroadcastPlayer->GetAbsOrigin());
			UTIL_ClientPrintFilter(filter, HUD_PRINTTALK, czTalkMsg);
		}

		// Play a sound local to the player that emitted the sound.
		CPASAttenuationFilter filter(pPlayer, SNDLVL_40dB);
		pPlayer->EmitSound(filter, pPlayer->entindex(), czSound);

		// No more voice!
		pPlayer->SetVoiceCommandState(VOICECMD_NONE);

		return true;
	}
	// Player selecting spawn?
	else if (pPlayer && FStrEq(czCommand, "menuselect") && pPlayer->IsSelectingSpawn())
	{
		// They chose a spawn, can't select anymore.
		pPlayer->SetSpawnSelection(false);

		// Are they still a leader?
		if (pPlayer->IsLeader())
		{
			CBaseEntity *pFoundEntity = gEntList.FindEntityByClassname(NULL, 
				"forsaken_fortified_entity");
			CForsakenTeam *pPlayerTeam = static_cast<CForsakenTeam*>(pPlayer->GetTeam());
			int nSlot = atoi(engine->Cmd_Argv(1));
			int nPlayerTeamBitMask = TEAM_UNASSIGNED;

			switch (pPlayer->GetTeamNumber())
			{
			case TEAM_CIVILIANS:
				nPlayerTeamBitMask = TEAM_CIVILIANS_BITMASK;
				break;
			case TEAM_GOVERNORS:
				nPlayerTeamBitMask = TEAM_GOVERNORS_BITMASK;
				break;
			case TEAM_RELIGIOUS:
				nPlayerTeamBitMask = TEAM_RELIGIOUS_BITMASK;
				break;
			}

			// Default spawn selection.
			if (nSlot == 9)
			{
				pPlayerTeam->SetFortificationSpawn(NULL);

				return true;
			}
			// Cancel spawn selection.
			else if (nSlot == 0)
				return true;

			// Loop through all fortified objective entities.
			for (int i = 1; pFoundEntity != NULL; i++)
			{
				CForsakenFortifiedObjectiveEntity *pFortifiedEntity = 
					static_cast<CForsakenFortifiedObjectiveEntity*>(pFoundEntity);

				if (i == nSlot && pFortifiedEntity->GetTeamOwner() == nPlayerTeamBitMask)
				{
					pPlayerTeam->SetFortificationSpawn(pFortifiedEntity);

					return true;
				}

				// Fetch the next fortified objective entity.
				pFoundEntity = gEntList.FindEntityByClassname(pFoundEntity, "forsaken_fortified_entity");
			}
		}

		return true;
	}

	return BaseClass::ClientCommand(czCommand, pentEdict);
}

bool CForsakenGameRules::ClientConnected(edict_t *pEntity, const char *czName, 
	const char *czAddress, char *czReject, int nMaxRejectLen)
/*
	
	Pre: 
	Post: 
*/
{
	return true;
}

bool CForsakenGameRules::FPlayerCanRespawn(CBasePlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	return true;
}

CBaseEntity *CForsakenGameRules::GetPlayerSpawnSpot(CBasePlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity *pSpawnSpot = NULL;
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);

	// Team-dependent spawning.
	switch (pPlayer->GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		pSpawnSpot = pCivTeam->SpawnPlayer(pPlayer);
		break;

	case TEAM_GOVERNORS:
		pSpawnSpot = pGovTeam->SpawnPlayer(pPlayer);
		break;

	case TEAM_RELIGIOUS:
		pSpawnSpot = pRelTeam->SpawnPlayer(pPlayer);
		break;

	default:
		return NULL;
	}

	// No spawn points available, so put them in chase observer mode and let them know.
	if (!pSpawnSpot)
	{
		CSingleUserRecipientFilter filter(pPlayer);

		UTIL_ClientPrintFilter(filter, HUD_PRINTCENTER, 
			"You were put in observer mode due to spawn problems... let the devs know.");

		pPlayer->StartObserverMode(OBS_MODE_CHASE);

		return NULL;
	}

	// Set the players initial values.
	pPlayer->SetLocalOrigin(pSpawnSpot->GetAbsOrigin() + Vector(0,0,1));
	pPlayer->SetAbsVelocity(vec3_origin);
	pPlayer->SetLocalAngles(pSpawnSpot->GetLocalAngles());
	pPlayer->m_Local.m_vecPunchAngle = vec3_angle;
	pPlayer->m_Local.m_vecPunchAngleVel = vec3_angle;
	pPlayer->SnapEyeAngles(pSpawnSpot->GetLocalAngles());

	return pSpawnSpot;
}

const char *CForsakenGameRules::GetGameDescription()
/*
	
	Pre: 
	Post: 
*/
{
	return "Territory War";
}

const char *CForsakenGameRules::GetChatPrefix(bool bTeamOnly, CBasePlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	if (!pPlayer)
		return NULL;

	if (bTeamOnly)
		return "(Team)";

	switch (pPlayer->GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		return "(Civilians)";

	case TEAM_GOVERNORS:
		return "(Governors)";

	case TEAM_RELIGIOUS:
		return "(Religious)";

	default:
		return "(Spectator)";
	}
}

int CForsakenGameRules::GetRoundRemainTime()
/*
	
	Pre: 
	Post: 
*/
{
	if (m_bInRound)
		return m_nRoundDuration - (int)(gpGlobals->curtime - m_fRoundStartTime);
	else
		return 0;
}

int CForsakenGameRules::IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pFsknAttacker = ToForsakenPlayer(pAttacker);
	CForsakenPlayer *pFsknAttackersLeader = NULL;
	CForsakenPlayer *pFsknKilled = ToForsakenPlayer(pKilled);
	CSingleUserRecipientFilter attackerFilter(pFsknAttacker);
	int nScoreForKill = 2; // Default value for just killing someone.
	int nAttackersTeam = TEAM_UNASSIGNED;
	int nPointType = UMSG_POINTS_KILL;
	int nPointsAmt = 2;
	tagPointsSummary newPoint;

	const char *czKilledSteamID = engine->GetPlayerNetworkIDString(pFsknKilled->edict());
	const char *czAttackerSteamID = engine->GetPlayerNetworkIDString(pFsknAttacker->edict());

	// Are they teammates?
	if (g_pGameRules->PlayerRelationship(pFsknAttacker, pFsknKilled) == GR_TEAMMATE)
	{
		// Add a point summary item.
		newPoint.nPointID = POINTID_PLAYER_SUICIDE;
		newPoint.nPoints = -2;

		pFsknAttacker->SendPointSummary(pFsknKilled, newPoint);

		// The message for points notice.
		UserMessageBegin(attackerFilter, "PointsNotice");
			WRITE_SHORT(-2);
			WRITE_SHORT(UMSG_POINTS_TEAMKILL);
			WRITE_SHORT(pFsknAttacker->GetTeamNumber());
		MessageEnd();

		// The attackers score goes down two.
		pFsknAttacker->IncrementScore(-2);

		return 0;
	}

	// They get four score points if it is a leader.
	if (pFsknKilled->IsLeader())
	{
		nPointsAmt = 4;
		nScoreForKill = 4;
	}
	else
	{
		// Was the attacker protecting the leader?
		nAttackersTeam = pFsknAttacker->GetTeamNumber();
		pFsknAttackersLeader = static_cast<CForsakenTeam*>(g_Teams[nAttackersTeam])->GetLeader();

		if (pFsknAttackersLeader && pFsknAttacker->entindex() != pFsknAttackersLeader->entindex())
		{
			// Loop through all of the players that attacked our leader.
			for (int i = 0; i < pFsknAttackersLeader->m_attackers.Count(); i++)
			{
				// Did the person we kill attack our leader within the past 3s?
				if (Q_stricmp(pFsknAttackersLeader->m_attackers[i].czSteamID, 
					czKilledSteamID) == 0 && 
					pFsknAttackersLeader->m_attackers[i].fAttackTime >= (gpGlobals->curtime - 3.0f))
				{
					nPointsAmt = 3;
					nPointType = UMSG_POINTS_PROTECTLDR;
					nScoreForKill = 3;
				}
			}
		}
	}

	// Find out who has attacked this player and give them points for the assist.
	for (int i = 0; i < pFsknKilled->m_attackers.Count(); i++)
	{
		// The attacker has already done damage, so no need for them to get additional points.
		// If it's been too long just ignore the points they should get.
		if (Q_stricmp(pFsknKilled->m_attackers[i].czSteamID, czAttackerSteamID) == 0 
			|| pFsknKilled->m_attackers[i].fAttackTime < (gpGlobals->curtime - 3.0f))
			continue;

		// If the person killed is also in the attackers list, don't give them points.
		if (Q_stricmp(pFsknKilled->m_attackers[i].czSteamID, czKilledSteamID) == 0)
			continue;

		// Lookup the player name.
		CForsakenPlayer *pFsknAttackerLookup = 
			ToForsakenPlayer(UTIL_PlayerByIndex(pFsknKilled->m_attackers[i].nEntIndex));

		// Increase their score by one.
		if (pFsknAttackerLookup)
		{
			pFsknAttackerLookup->IncrementScore(1);

			// Add a point summary item.
			newPoint.nPointID = POINTID_PLAYER_ASSIST;
			newPoint.nPoints = 1;

			if (pFsknAttacker != pFsknAttackerLookup)
			{
				CSingleUserRecipientFilter assistFilter(pFsknAttackerLookup);

				// The message for points notice.
				UserMessageBegin(assistFilter, "PointsNotice");
					WRITE_SHORT(1);
					WRITE_SHORT(UMSG_POINTS_KILLASSIST);
					WRITE_SHORT(pFsknAttackerLookup->GetTeamNumber());
					WRITE_STRING(pFsknKilled->GetPlayerName());
				MessageEnd();
			}

			pFsknAttackerLookup->SendPointSummary(pFsknKilled, newPoint);
		}
	}

	pFsknAttacker->IncrementScore(nScoreForKill);

	// Add a point summary item.
	newPoint.nPointID = POINTID_PLAYER_KILL;
	newPoint.nPoints = nScoreForKill;

	pFsknAttacker->SendPointSummary(pFsknKilled, newPoint);

	// The message for points notice.
	UserMessageBegin(attackerFilter, "PointsNotice");
		WRITE_SHORT(nPointsAmt);
		WRITE_SHORT(nPointType);
		WRITE_SHORT(pFsknAttacker->GetTeamNumber());
		WRITE_STRING(pFsknKilled->GetPlayerName());
	MessageEnd();

	return 1;
}

void CForsakenGameRules::AurasThink()
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable aura's for release.
#ifdef BETA_FORSAKEN
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	CForsakenPlayer *pCivLeader = static_cast<CForsakenPlayer*>(pCivTeam->GetLeader());
	CForsakenPlayer *pGovLeader = static_cast<CForsakenPlayer*>(pGovTeam->GetLeader());
	CForsakenPlayer *pRelLeader = static_cast<CForsakenPlayer*>(pRelTeam->GetLeader());
	float fDefaultRadius = 300.0f;

	if (m_fNextAuraThinkTime > gpGlobals->curtime)
		return;

	// Aura think time.
	m_fNextAuraThinkTime = gpGlobals->curtime + 2.5f;

	// Reset the auras on all players.
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_PlayerByIndex(i));

		if (!pPlayer || !pPlayer->IsConnected())
			continue;

		// If we are on a valid team reset the aura.
		if (pPlayer->GetTeamNumber() != TEAM_UNASSIGNED && 
			pPlayer->GetTeamNumber() != TEAM_SPECTATOR)
			pPlayer->SetAura(Aura_None);
	}

	if (pCivLeader)
	{
		CBaseEntity *pEntity = NULL;
		CEntitySphereQuery radiusCheck(pCivLeader->GetAbsOrigin(), fDefaultRadius);

		while ((pEntity = radiusCheck.GetCurrentEntity()) != NULL)
		{
			CForsakenPlayer *pPlayer = ToForsakenPlayer(pEntity);

			// If it's a player update their active aura to the leaders.
			if (pEntity && pPlayer && pEntity->IsPlayer() && 
				pPlayer->GetTeamNumber() == pCivLeader->GetTeamNumber() &&
				pPlayer->IsAlive())
				pPlayer->SetAura(pCivLeader->GetAura());

			// Go to the next entity.
			radiusCheck.NextEntity();
		}
	}

	if (pGovLeader)
	{
		CBaseEntity *pEntity = NULL;
		CEntitySphereQuery radiusCheck(pGovLeader->GetAbsOrigin(), fDefaultRadius);

		while ((pEntity = radiusCheck.GetCurrentEntity()) != NULL)
		{
			CForsakenPlayer *pPlayer = ToForsakenPlayer(pEntity);

			// If it's a player update their active aura to the leaders.
			if (pEntity && pPlayer && pEntity->IsPlayer() && 
				pPlayer->GetTeamNumber() == pGovLeader->GetTeamNumber() &&
				pPlayer->IsAlive())
				pPlayer->SetAura(pGovLeader->GetAura());

			// Go to the next entity.
			radiusCheck.NextEntity();
		}
	}

	if (pRelLeader)
	{
		CBaseEntity *pEntity = NULL;
		CEntitySphereQuery radiusCheck(pRelLeader->GetAbsOrigin(), fDefaultRadius);

		while ((pEntity = radiusCheck.GetCurrentEntity()) != NULL)
		{
			CForsakenPlayer *pPlayer = ToForsakenPlayer(pEntity);

			// If it's a player update their active aura to the leaders.
			if (pEntity && pPlayer && pEntity->IsPlayer() && 
				pPlayer->GetTeamNumber() == pRelLeader->GetTeamNumber() &&
				pPlayer->IsAlive())
				pPlayer->SetAura(pRelLeader->GetAura());

			// Go to the next entity.
			radiusCheck.NextEntity();
		}
	}
#endif
}

void CForsakenGameRules::ChangeLevel()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ChangeLevel();
}

void CForsakenGameRules::ClientDisconnected(edict_t *pClient)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	int nCivilians = pCivTeam->GetNumPlayers();
	int nGovernors = pGovTeam->GetNumPlayers();
	int nReligious = pRelTeam->GetNumPlayers();
	int nCivsAlive = pCivTeam->GetNumberAlivePlayers();
	int nGovsAlive = pGovTeam->GetNumberAlivePlayers();
	int nRelAlive = pRelTeam->GetNumberAlivePlayers();

	// Do we have enough people to play?
	if ((nCivilians > 0 && nGovernors == 0 && nReligious == 0) || 
		(nCivilians == 0 && nGovernors > 0 && nReligious == 0) || 
		(nCivilians == 0 && nGovernors == 0 && nReligious > 0))
	{
		// Reset the round.
		m_bInRound = false;
		m_fRoundFinalizeTime = 0.0f;
		m_fRoundStartTime = 0.0f;
	}
	// Bug Fix ID 0000042
	// Did the person changing team just force a end-of-round?
	else if (nCivsAlive > 0 && nGovsAlive == 0 && nRelAlive == 0)
		EndCurrentRound(TEAM_CIVILIANS_BITMASK);
	else if (nCivsAlive == 0 && nGovsAlive > 0 && nRelAlive == 0)
		EndCurrentRound(TEAM_GOVERNORS_BITMASK);
	else if	(nCivsAlive == 0 && nGovsAlive == 0 && nRelAlive > 0)
		EndCurrentRound(TEAM_RELIGIOUS_BITMASK);

	BaseClass::ClientDisconnected(pClient);
}

void CForsakenGameRules::EnableTeam(int nTeam, bool bEnable)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(g_Teams[nTeam]);

	if (pTeam)
		pTeam->SetTeamAvailability(bEnable);
}

void CForsakenGameRules::EndCurrentRound(int nWinner)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	int nTeamAvailabilityCount = 3;
	int nWarWinner = TEAM_CIVILIANS_BITMASK | TEAM_GOVERNORS_BITMASK | TEAM_RELIGIOUS_BITMASK;
	KeyValues *pData = new KeyValues("data");

	pData->SetInt("team", nWinner);

	// Check to see if a disabled team has more nutrition resources.
	if (!pCivTeam->IsTeamAvailable() && pCivTeam->GetNutritionResources() > 0)
		EnableTeam(TEAM_CIVILIANS, true);
	// If not, remove all players on the team.
	else if (!pCivTeam->IsTeamAvailable())
		pCivTeam->ForcePlayersToSpectator();

	// Check to see if a disabled team has more nutrition resources.
	if (!pGovTeam->IsTeamAvailable() && pGovTeam->GetNutritionResources() > 0)
		EnableTeam(TEAM_GOVERNORS, true);
	// If not, remove all players on the team.
	else if (!pGovTeam->IsTeamAvailable())
		pGovTeam->ForcePlayersToSpectator();

	// Check to see if a disabled team has more nutrition resources.
	if (!pRelTeam->IsTeamAvailable() && pRelTeam->GetNutritionResources() > 0)
		EnableTeam(TEAM_RELIGIOUS, true);
	// If not, remove all players on the team.
	else if (!pRelTeam->IsTeamAvailable())
		pRelTeam->ForcePlayersToSpectator();

	// Determine how many teams are disabled.
	if (!pCivTeam->IsTeamAvailable())
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_CIVILIANS_BITMASK);
	}
	if (!pGovTeam->IsTeamAvailable())
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_GOVERNORS_BITMASK);
	}
	if (!pRelTeam->IsTeamAvailable())
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_RELIGIOUS_BITMASK);
	}

	// Somebody has won the war, so end it.
	if (nTeamAvailabilityCount <= 1)
	{
		EndWar(nWarWinner);

		goto reset_round;
	}

	// Loop through all players and show the round win panel.
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || !pPlayer->IsConnected())
			continue;

		// Show the round win panel.
		pPlayer->ShowViewPortPanel(PANEL_ROUNDWIN, true, pData);
	}

	pData->deleteThis();

	// Play sounds and increase the score for the winning team.
	switch (nWinner)
	{
	case TEAM_CIVILIANS_BITMASK:
		pCivTeam->AddScore(1);
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_ROUND_CIV);
		break;

	case TEAM_GOVERNORS_BITMASK:
		pGovTeam->AddScore(1);
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_ROUND_GOV);
		break;

	case TEAM_RELIGIOUS_BITMASK:
		pRelTeam->AddScore(1);
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_ROUND_REL);
		break;

	case TEAM_UNASSIGNED:
		break;
	}

	goto reset_round;

	// Reset the round.
reset_round:
	m_bInRound = false;
	m_nRoundDuration = 0;
	m_fRoundFinalizeTime = gpGlobals->curtime + sv_fskn_roundwinpause.GetFloat();
}

void CForsakenGameRules::EndMap(int nWinner)
/*
	
	Pre: 
	Post: 
*/
{
	float flWaitTime = mp_chattime.GetInt();
	KeyValues *pData = new KeyValues("data");

	pData->SetInt("team", nWinner);

	// Is the game over?
	if (g_fGameOver)
		return;

	// Set the game to over.
	g_fGameOver = true;

	// Which is longer, map summary or HLTV time?
	if (tv_delaymapchange.GetBool() && HLTVDirector()->IsActive())	
		flWaitTime = max(flWaitTime, HLTVDirector()->GetDelay());

	// Set the intermission end time.
	SetIntermissionTime(gpGlobals->curtime + flWaitTime);

	// Loop through all players.
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		// Is the player valid?
		if (!pPlayer || !pPlayer->IsConnected())
			continue;

		// Show the player the war summary panel.
		pPlayer->ShowViewPortPanel(PANEL_MAPWIN, true, pData);
	}

	pData->deleteThis();

	// Play winning sounds depending on who won the map.
	switch (nWinner)
	{
	case TEAM_CIVILIANS_BITMASK:
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_MAP_CIV);
		break;

	case TEAM_GOVERNORS_BITMASK:
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_MAP_GOV);
		break;

	case TEAM_RELIGIOUS_BITMASK:
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_MAP_REL);
		break;

	case TEAM_UNASSIGNED:
		break;
	}
}

void CForsakenGameRules::EndWar(int nWinner)
/*
	
	Pre: 
	Post: 
*/
{
	float flWaitTime = sv_fskn_endwarsummary.GetInt();

	// Is the game over?
	if (g_fGameOver)
		return;

	// Set the game to over.
	g_fGameOver = true;
	m_bWarOver = true;

	// Which is longer, war summary or HLTV time?
	if (tv_delaymapchange.GetBool() && HLTVDirector()->IsActive())	
		flWaitTime = max (flWaitTime, HLTVDirector()->GetDelay());

	// Set the intermission end time.
	SetIntermissionTime(gpGlobals->curtime + flWaitTime);

	// Loop through all players.
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);
		KeyValues *pData = new KeyValues("data");

		pData->SetInt("team", nWinner);

		// Is the player valid?
		if (!pPlayer || !pPlayer->IsConnected())
			continue;

		// Show the player the war summary panel and hide the round win panel.
		pPlayer->ShowViewPortPanel(PANEL_ROUNDWIN, false);
		pPlayer->ShowViewPortPanel(PANEL_WARSUMMARY, true, pData);

		// Delete the data storage.
		pData->deleteThis();
	}

	// Depending on who won the war, play that teams winning sound.
	switch (nWinner)
	{
	case TEAM_CIVILIANS_BITMASK:
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_WAR_CIV);
		break;

	case TEAM_GOVERNORS_BITMASK:
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_WAR_GOV);
		break;

	case TEAM_RELIGIOUS_BITMASK:
		ForsakenUtil_PlayGlobalSound(FORSAKEN_WINSOUND_WAR_REL);
		break;

	case TEAM_UNASSIGNED:
		break;
	}
}

void CForsakenGameRules::HandleRespawn(CBasePlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	int nCivilians = pCivTeam->GetNumPlayers();
	int nGovernors = pGovTeam->GetNumPlayers();
	int nReligious = pRelTeam->GetNumPlayers();

	// Don't have enough players to do round-based behaviour, so just re-spawn 'em.
	if ((nCivilians > 0 && nGovernors == 0 && nReligious == 0) || 
		(nCivilians == 0 && nGovernors > 0 && nReligious == 0) || (
		nCivilians == 0 && nGovernors == 0 && nReligious > 0))
	{
		pPlayer->Spawn();
	}
	else
		pPlayer->StartObserverMode(OBS_MODE_ROAMING);
}

void CForsakenGameRules::LevelShutdown()
/*
	
	Pre: 
	Post: 
*/
{
	// Save the current state.
	SaveState();

	BaseClass::LevelShutdown();
}

void CForsakenGameRules::LoadState()
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity *pCurrentEntity = NULL;
	int nMapOwner = TEAM_UNASSIGNED, nMapOwnerBitMask = TEAM_UNASSIGNED;
	KeyValues *pLoadData = new KeyValues("GameSaveState");
	KeyValues *pMapData = NULL;

	if (pLoadData)
	{
		// Load the save state from the file.
		if (pLoadData->LoadFromFile(filesystem, FORSKANE_SAVESTATE, NULL))
		{
			// Load game type data if we aren't doing a reset.
			if (pLoadData->GetInt("reset", 1) != 1)
			{
				CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
				CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
				CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);

				// Load team data.
				pCivTeam->LoadState(pLoadData);
				pGovTeam->LoadState(pLoadData);
				pRelTeam->LoadState(pLoadData);

				// Find the map data.
				pMapData = pLoadData->FindKey("MapSaveState");

				if (pMapData)
				{
					// Determine who owns this map.
					nMapOwner = pMapData->GetInt(g_czMapName, TEAM_UNASSIGNED);

					switch (nMapOwner)
					{
					case TEAM_CIVILIANS:
						nMapOwnerBitMask = TEAM_CIVILIANS_BITMASK;
						break;
					case TEAM_GOVERNORS:
						nMapOwnerBitMask = TEAM_GOVERNORS_BITMASK;
						break;
					case TEAM_RELIGIOUS:
						nMapOwnerBitMask = TEAM_RELIGIOUS_BITMASK;
						break;
					}
				}
			}
		}

		pLoadData->deleteThis();
	}

	if (nMapOwnerBitMask != TEAM_UNASSIGNED)
	{
		// BETA DISABLE: Disable fortifications for release.
#ifdef BETA_FORSAKEN
		// Loop through all fortification entities.
		while ((pCurrentEntity = gEntList.FindEntityByClassname(pCurrentEntity, "forsaken_fortified_entity")) != NULL)
		{
			CForsakenFortifiedObjectiveEntity *pCurrentFortification = 
				dynamic_cast<CForsakenFortifiedObjectiveEntity*>(pCurrentEntity);

			if (pCurrentFortification)
				pCurrentFortification->SetTeamMapOwner(nMapOwnerBitMask);
		}
#endif

		// Loop through all capture objective entities.
		while ((pCurrentEntity = gEntList.FindEntityByClassname(pCurrentEntity, "forsaken_captureobjective_entity")) != NULL)
		{
			CForsakenObjectiveEntity *pCurrentObjective = 
				dynamic_cast<CForsakenObjectiveEntity*>(pCurrentEntity);

			if (pCurrentObjective)
				pCurrentObjective->SetTeamMapOwner(nMapOwnerBitMask);
		}

		// Loop through all destroy objective entities.
		while ((pCurrentEntity = gEntList.FindEntityByClassname(pCurrentEntity, "forsaken_destroyobjective_entity")) != NULL)
		{
			CForsakenObjectiveEntity *pCurrentObjective = 
				dynamic_cast<CForsakenObjectiveEntity*>(pCurrentEntity);

			if (pCurrentObjective)
				pCurrentObjective->SetTeamMapOwner(nMapOwnerBitMask);
		}

		// Loop through all win logic entities.
		while ((pCurrentEntity = gEntList.FindEntityByClassname(pCurrentEntity, "forsaken_winlogic_entity")) != NULL)
		{
			CForsakenWinLogicEntity *pCurrentWinLogic = 
				dynamic_cast<CForsakenWinLogicEntity*>(pCurrentEntity);

			if (pCurrentWinLogic)
				pCurrentWinLogic->SetTeamMapOwner(nMapOwnerBitMask);
		}
	}
}

void CForsakenGameRules::NotifyEntitiesOfNewRound()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenGameRules::ObjectiveAdd(IGameEvent *pEvent)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenGameRules::ObjectiveCompleted(IGameEvent *pEvent)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenObjectiveEntity *pCompletedObjective = NULL;
	int nEntIndex = pEvent->GetInt("entindex");

	pCompletedObjective = static_cast<CForsakenObjectiveEntity*>(UTIL_EntityByIndex(nEntIndex));

	if (pCompletedObjective == NULL)
		return;

	// We don't want any duplicate completed objective entries.
	for (int i = 0; i < m_vecCompletedObjectives.Count(); i++)
	{
		if (m_vecCompletedObjectives[i] == pCompletedObjective)
			return;
	}

	// Add the objective to the completed objectives list.
	m_vecCompletedObjectives.AddToTail(pCompletedObjective);
}

void CForsakenGameRules::ObjectiveDestroyed(IGameEvent *pEvent)
/*
	
	Pre: 
	Post: 
*/
{
	if (m_nRoundTime <= 0)
		return;/*

	CForsakenDestroyObjectiveEntity *pObjective = static_cast<CForsakenDestroyObjectiveEntity*>(rEvent.pSource);
	const CTakeDamageInfo *pDmgInfo = static_cast<CTakeDamageInfo*>(rEvent.pData);
	CForsakenPlayer *pAttacker = ToForsakenPlayer(pDmgInfo->GetAttacker());
	CSingleUserRecipientFilter filter(pAttacker);
	int nTeamOwners = pObjective->GetTeamOwners();
	tagPointsSummary newPoint;

	// Increase the attackers score by five for destroying an objective.
	pAttacker->IncrementScore(5);

	UserMessageBegin(filter, "PointsNotice");
		WRITE_SHORT(5);
		WRITE_SHORT(UMSG_POINTS_OBJDESTROY);
		WRITE_SHORT(pAttacker->GetTeamNumber());
	MessageEnd();

	// Add a point summary item.
	newPoint.nPointID = POINTID_OBJECTIVE_COMPLETE;
	newPoint.nPoints = 5;
	pAttacker->SendPointSummary(pObjective, newPoint);*/
}

void CForsakenGameRules::ObjectiveTakingDamage(IGameEvent *pEvent)
/*
	
	Pre: 
	Post: 
*/
{
	if (m_nRoundTime <= 0)
		return;
/*
	CForsakenDestroyObjectiveEntity *pObjective = static_cast<CForsakenDestroyObjectiveEntity*>(rEvent.pSource);
	const CTakeDamageInfo *pDmgInfo = static_cast<CTakeDamageInfo*>(rEvent.pData);
	CForsakenPlayer *pAttacker = ToForsakenPlayer(pDmgInfo->GetAttacker());
	tagPointsSummary newPoint;

	// Add a point summary item.
	newPoint.nPointID = POINTID_OBJECTIVE_ASSIST;
	newPoint.nPoints = 3;
	pAttacker->SendPointSummary(pObjective, newPoint);*/
}

void CForsakenGameRules::OnTeamChange(CBasePlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	int nCivilians = pCivTeam->GetNumPlayers();
	int nGovernors = pGovTeam->GetNumPlayers();
	int nReligious = pRelTeam->GetNumPlayers();

	// Bug Fix ID 0000014: Remove all player ammo & weapons.
	pPlayer->RemoveAllAmmo();
	pPlayer->RemoveAllWeapons();

	// Bug Fix ID 0000013: Reset player zoom.
	pPlayer->SetFOV(pPlayer, 0);

	// If we were in a round and the teams aren't condusive to a round... get out of it.
	if (m_bInRound)
	{
		// Do we have enough people to play?
		if ((nCivilians > 0 && nGovernors == 0 && nReligious == 0) || 
			(nCivilians == 0 && nGovernors > 0 && nReligious == 0) || 
			(nCivilians == 0 && nGovernors == 0 && nReligious > 0))
		{
			// Reset the round.
			m_bInRound = false;
			m_fRoundFinalizeTime = 0.0f;
			m_fRoundStartTime = 0.0f;
		}
	}

	// They shouldn't be a leader.
	ToForsakenPlayer(pPlayer)->SetLeader(false);

	// If he was the leader of a team, set them to not have a leader.
	if (pCivTeam->GetLeader() == pPlayer)
		pCivTeam->NullLeader();
	else if (pGovTeam->GetLeader() == pPlayer)
		pGovTeam->NullLeader();
	else if (pRelTeam->GetLeader() == pPlayer)
		pRelTeam->NullLeader();

	// Go ahead and spawn them if we are within the spawn time.
	if (!m_bInRound || (m_bInRound && 
		(m_fRoundStartTime + sv_fskn_roundspawntime.GetFloat()) >= gpGlobals->curtime))
		pPlayer->Spawn();
}

void CForsakenGameRules::PlayerKilled(CBasePlayer *pVictim, const CTakeDamageInfo &rInfo)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	int nCivAlive = pCivTeam->GetNumberAlivePlayers(pVictim);
	int nGovAlive = pGovTeam->GetNumberAlivePlayers(pVictim);
	int nRelAlive = pRelTeam->GetNumberAlivePlayers(pVictim);

	// We aren't in a round.
	if (!m_bInRound)
		return;

	// Determine if we have a round-win event.
	if (nCivAlive > 0 && nGovAlive == 0 && nRelAlive == 0)
	{
		// Civilians alive, everyone else dead.
		EndCurrentRound(TEAM_CIVILIANS_BITMASK);
	}
	else if (nCivAlive == 0 && nGovAlive > 0 && nRelAlive == 0)
	{
		// Governors alive, everyone else dead.
		EndCurrentRound(TEAM_GOVERNORS_BITMASK);
	}
	else if (nCivAlive == 0 && nGovAlive == 0 && nRelAlive > 0)
	{
		// Religious alive, everyone else dead.
		EndCurrentRound(TEAM_RELIGIOUS_BITMASK);
	}

	BaseClass::PlayerKilled(pVictim, rInfo);
}

void CForsakenGameRules::Precache()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Precache();
}

void CForsakenGameRules::RadiusDamage(const CTakeDamageInfo &rtdiDamageInf, 
									  const Vector &rvecSource, float fRadius, int nClassIgnore)
/*
	
	Pre: 
	Post: 
*/
{
	RadiusDamage(rtdiDamageInf, rvecSource, fRadius, nClassIgnore, false);
}

void CForsakenGameRules::RadiusDamage(const CTakeDamageInfo &rtdiDamageInf, 
									  const Vector &rvecSource, float fRadius, int nClassIgnore, 
									  bool bIgnoreWorld)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenGameRules::RemoveAllEntityDecals()
/*
	
	Pre: 
	Post: 
*/
{
	// Loop through all allocated entities.
	for (int i = 0; i < engine->GetEntityCount(); i++)
	{
		CBaseEntity *pEntity = UTIL_EntityByIndex(i);
		CForsakenPlayer *pPlayer = ToForsakenPlayer(pEntity);

		if (pPlayer && pPlayer->IsConnected())
		{
			// Remove all of this entities decals.
			pPlayer->RemoveAllDecals();
		}
	}
}

void CForsakenGameRules::ResetMap()
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity *pCurrentEntity;
	CForsakenMapEntityFilter mapFilter;
	CBroadcastRecipientFilter filter;

	UserMessageBegin(filter, "RestartRound");
	MessageEnd();

	// Get the first entity.
	pCurrentEntity = gEntList.FirstEnt();

	while (pCurrentEntity != NULL)
	{
		bool bMarkForDelete = false;
		CBaseEntity *pNextEntity = gEntList.NextEnt(pCurrentEntity);
		CBaseEntity *pParentEntity = pCurrentEntity->GetParent();

		// If we need to recreate the entity, mark it for deletion.
		if (mapFilter.ShouldCreateEntity(pCurrentEntity->GetClassname()))
			bMarkForDelete = true;

		// Do we have a parent entity and are they a player?
		if (pParentEntity)
		{
			if (pParentEntity->IsPlayer())
			{
				/*
				// If we are a molotov or frag grenade, mark us for deletion.
				if (Q_stricmp(pCurrentEntity->GetClassname(), "forsaken_grenade_molotov") == 0)
					bMarkForDelete = true;
				else if (Q_stricmp(pCurrentEntity->GetClassname(), "forsaken_grenade_frag") == 0)
					bMarkForDelete = true;
				else*/
					bMarkForDelete = false;
			}
			else
			{
				bMarkForDelete = true;

				// We need to climb the hierarchy tree to see if it belongs to something specific 
				// we want to keep each round.
				while (pParentEntity != NULL)
				{
					if (Q_stricmp(pParentEntity->GetClassname(), "forsaken_env_campfire") == 0)
					{
						bMarkForDelete = false;

						break;
					}

					// Fetch the next parent.
					pParentEntity = pParentEntity->GetParent();
				}
			}
		}

		// Are we supposed to be removing the entity? If so, delete it.
		if (bMarkForDelete)
			UTIL_Remove(pCurrentEntity);

		// Go to the next entity.
		pCurrentEntity = pNextEntity;
	}

	// Clean up the entity list.
	gEntList.CleanupDeleteList();

	// Re-parse all entities from the map.
	MapEntity_ParseAllEntities(engine->GetMapEntitiesString(), &mapFilter, true);
}

void CForsakenGameRules::SaveState()
/*
	
	Pre: 
	Post: 
*/
{
	bool bAllocated = false;
	KeyValues *pSaveData = new KeyValues("GameSaveState");
	KeyValues *pMapData = NULL;

	// Did we allocate?
	if (pSaveData)
	{
		KeyValues *pLoadedMapData = NULL;

		// Load the previous saved state.
		if (pSaveData->LoadFromFile(filesystem, FORSKANE_SAVESTATE, NULL))
		{
			// We don't want to load any data that is going to be reset.
			if (pSaveData->GetInt("reset", 1) != 1)
				pLoadedMapData = pSaveData->FindKey("MapSaveState");
		}

		// Copy the map save data.
		if (pLoadedMapData)
			pMapData = pLoadedMapData->MakeCopy();

		pSaveData->deleteThis();

		pSaveData = NULL;
	}

	// Guess we didn't find the previous saved state.
	if (!pMapData)
		pMapData = new KeyValues("MapSaveState");

	pSaveData = new KeyValues("GameSaveState");

	// Save gametype data.
	if (pSaveData)
	{
		// If the war is over, reset all data for next map cycle.
		if (m_bWarOver)
			pSaveData->SetInt("reset", 1);
		else
		{
			pSaveData->SetInt("reset", 0);

			CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
			CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
			CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
			int nTeamMostWins = TEAM_UNASSIGNED;

			// Save team data.
			if (pCivTeam)
				pCivTeam->SaveState(pSaveData);
			if (pGovTeam)
				pGovTeam->SaveState(pSaveData);
			if (pRelTeam)
				pRelTeam->SaveState(pSaveData);

			// Do civilians have the most wins?
			if (pCivTeam->GetScore() > pGovTeam->GetScore() && 
				pCivTeam->GetScore() > pRelTeam->GetScore())
				nTeamMostWins = TEAM_CIVILIANS;
			// Do governors have the most wins?
			else if (pGovTeam->GetScore() > pCivTeam->GetScore() && 
				pGovTeam->GetScore() > pRelTeam->GetScore())
				nTeamMostWins = TEAM_GOVERNORS;
			// Do religious have the most wins?
			else if (pRelTeam->GetScore() > pCivTeam->GetScore() && 
				pRelTeam->GetScore() > pGovTeam->GetScore())
				nTeamMostWins = TEAM_RELIGIOUS;

			// Save the current level ownership to the team with the most wins.
			pMapData->SetInt(g_czMapName, nTeamMostWins);

			// Add the map save data to the game save state.
			pSaveData->AddSubKey(pMapData);
		}

		// Save the data to a file.
		pSaveData->SaveToFile(filesystem, FORSKANE_SAVESTATE);

		pSaveData->deleteThis();
	}
}

void CForsakenGameRules::StartNewRound()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	int nCivilians = pCivTeam->GetNumPlayers();
	int nGovernors = pGovTeam->GetNumPlayers();
	int nReligious = pRelTeam->GetNumPlayers();
	int nTeamAvailabilityCount = 3;
	int nWarWinner = TEAM_CIVILIANS_BITMASK | TEAM_GOVERNORS_BITMASK | TEAM_RELIGIOUS_BITMASK;

	// No players, so don't start a round.
	if (!CanDoNewRound())
		return;

	// Disable any teams that have starved to death.
	if ((80 + pCivTeam->GetNutritionResources() * sv_fskn_starvinghealthdecrease.GetInt())
		<= 0)
		EnableTeam(TEAM_CIVILIANS, false);
	if ((100 + pGovTeam->GetNutritionResources() * sv_fskn_starvinghealthdecrease.GetInt())
		<= 0)
		EnableTeam(TEAM_GOVERNORS, false);
	if ((120 + pRelTeam->GetNutritionResources() * sv_fskn_starvinghealthdecrease.GetInt())
		<= 0)
		EnableTeam(TEAM_RELIGIOUS, false);

	// Determine how many teams are disabled.
	if (!pCivTeam->IsTeamAvailable())
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_CIVILIANS_BITMASK);
	}
	if (!pGovTeam->IsTeamAvailable())
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_GOVERNORS_BITMASK);
	}
	if (!pRelTeam->IsTeamAvailable())
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_RELIGIOUS_BITMASK);
	}

	// Somebody has won the war, so end it.
	if (nTeamAvailabilityCount <= 1)
	{
		EndWar(nWarWinner);

		return;
	}

	// We've reached the round limit, so go to the next map in the war.
	if (pCivTeam->GetScore() >= sv_fskn_roundlimit.GetInt())
	{
		EndMap(TEAM_CIVILIANS_BITMASK);

		return;
	}
	else if (pGovTeam->GetScore() >= sv_fskn_roundlimit.GetInt())
	{
		EndMap(TEAM_GOVERNORS_BITMASK);

		return;
	}
	else if (pRelTeam->GetScore() >= sv_fskn_roundlimit.GetInt())
	{
		EndMap(TEAM_RELIGIOUS_BITMASK);

		return;
	}

	// Clear all decals from the map.
	RemoveAllEntityDecals();

	// We shouldn't have a leader anymore.
	pCivTeam->NullLeader();
	pGovTeam->NullLeader();
	pRelTeam->NullLeader();

	// Clear the team-starving flag.
	pCivTeam->SetTeamStarving(false);
	pGovTeam->SetTeamStarving(false);
	pRelTeam->SetTeamStarving(false);

	// Reset the leader flag and spawned flag on all players.
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_PlayerByIndex(i));

		if (!pPlayer || !pPlayer->IsConnected())
			continue;

		// If we are on a valid team don't make us the leader and reset spawn flag.
		if (pPlayer->GetTeamNumber() != TEAM_UNASSIGNED && 
			pPlayer->GetTeamNumber() != TEAM_SPECTATOR)
		{
			if (pPlayer->IsLeader())
				pPlayer->SetLeader(false);

			pPlayer->m_bHasSpawned = false;

			// While we are at it, hide the round win message panel as well.
			pPlayer->ShowViewPortPanel(PANEL_ROUNDWIN, false);
		}
	}

	// Increase team resources according to what objectives they have captured.
	for (int i = 0; i < m_vecCompletedObjectives.Count(); i++)
	{
		CForsakenCaptureObjectiveEntity *pCaptureObjective = 
			dynamic_cast<CForsakenCaptureObjectiveEntity*>(m_vecCompletedObjectives[i]);

		// Is this a capture objective?
		if (pCaptureObjective)
		{
			CForsakenTeam *pCaptureObjectiveOwner = NULL;
			char czMessage[512] = "";
			int nResourceAmount = ((nCivilians + nGovernors + nReligious) / nTeamAvailabilityCount);

			// Obtain a pointer to the current team owner.
			if (pCaptureObjective->GetTeamOwners() == TEAM_CIVILIANS)
			{
				pCaptureObjectiveOwner = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);

				Q_snprintf(czMessage, 512, "Civilians gain ");

				nResourceAmount += (nCivilians / 2);
			}
			else if (pCaptureObjective->GetTeamOwners() == TEAM_GOVERNORS)
			{
				pCaptureObjectiveOwner = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);

				Q_snprintf(czMessage, 512, "Governors gain ");

				nResourceAmount += (nGovernors / 2);
			}
			else if (pCaptureObjective->GetTeamOwners() == TEAM_RELIGIOUS)
			{
				pCaptureObjectiveOwner = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);

				Q_snprintf(czMessage, 512, "Religious gain ");

				nResourceAmount += (nReligious / 2);
			}
			else
				continue;

			// Determine which team gets the resources.
			switch (pCaptureObjective->GetResourceType())
			{
			case FORSAKEN_RESOURCETYPE_NUTRITION:
				Q_snprintf(czMessage, 512, "%s%d nutrition resources\n", czMessage, 
					nResourceAmount);
				pCaptureObjectiveOwner->IncreaseNutritionResources(nResourceAmount);
				break;

			case FORSAKEN_RESOURCETYPE_ORDINANCE:
				Q_snprintf(czMessage, 512, "%s%d ordinance resources\n", czMessage, 
					nResourceAmount);
				pCaptureObjectiveOwner->IncreaseOrdinanceResources(nResourceAmount);
				break;

			default:
				break;
			}

			UTIL_ClientPrintAll(HUD_PRINTCONSOLE, czMessage);
		}
	}

	// Do resource calculations for the teams.
	pCivTeam->DoResourceCalculations();
	pGovTeam->DoResourceCalculations();
	pRelTeam->DoResourceCalculations();

	// Re-spawn all players.
	pCivTeam->SpawnTeam();
	pGovTeam->SpawnTeam();
	pRelTeam->SpawnTeam();

	nTeamAvailabilityCount = 3;
	nWarWinner = TEAM_CIVILIANS_BITMASK | TEAM_GOVERNORS_BITMASK | TEAM_RELIGIOUS_BITMASK;

	// Determine how many teams are disabled.
	if (!pCivTeam->IsTeamAvailable() && pCivTeam->GetNumberAlivePlayers() == 0)
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_CIVILIANS_BITMASK);
	}
	if (!pGovTeam->IsTeamAvailable() && pGovTeam->GetNumberAlivePlayers() == 0)
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_GOVERNORS_BITMASK);
	}
	if (!pRelTeam->IsTeamAvailable() && pRelTeam->GetNumberAlivePlayers() == 0)
	{
		nTeamAvailabilityCount--;
		nWarWinner &= (~TEAM_RELIGIOUS_BITMASK);
	}

	// Somebody has won the war, so end it.
	if (nTeamAvailabilityCount <= 1)
	{
		EndWar(nWarWinner);

		return;
	}

	// Assign each team their random leader.
	pCivTeam->AssignLeader();
	pGovTeam->AssignLeader();
	pRelTeam->AssignLeader();

	// Set the round information.
	m_bInRound = true;
	m_fRoundStartTime = gpGlobals->curtime;
	m_nRoundCount++;
	m_nRoundDuration = sv_fskn_roundduration.GetInt();

	// We are starting a new round.
	IGameEvent *event = gameeventmanager->CreateEvent("forsaken_newround");

	// Set event data.
	if (event)
	{
		event->SetInt("roundnumber", m_nRoundCount);

		gameeventmanager->FireEvent(event);
	}
}

void CForsakenGameRules::Think()
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable steam ID auth checks for release.
#ifdef BETA_FORSAKEN
	// Loop through all users and determine if they are allowed to play.
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_PlayerByIndex(i));

		// Is this a valid player, are they connected, and have they authorized?
		if (pPlayer && pPlayer->IsConnected() && !pPlayer->IsAuthorized())
		{
			// Fetch the players steam ID.
			const char *czSteamID = engine->GetPlayerNetworkIDString(pPlayer->edict());

			// They aren't fully in the game yet.
			if (pPlayer->GetAuthEndTime() == 0.0f)
				continue;

			// See if their auth time has expired.
			if (pPlayer->GetAuthEndTime() <= gpGlobals->curtime)
				engine->ClientCommand(pPlayer->edict(), "disconnect");
			// If their steam ID is still pending, keep going.
			else if (Q_stricmp(czSteamID, "STEAM_ID_PENDING") == 0)
				continue;

			// Are they running an invalid steam ID?
			if (Q_stricmp(czSteamID, "STEAM_ID_LAN") == 0 ||		// No LAN play.
				(Q_stricmp(czSteamID, "STEAM_0:1:2577647") != 0 &&	// Obike
				Q_stricmp(czSteamID, "STEAM_0:1:3749310") != 0 &&	// Obike - Laptop
				Q_stricmp(czSteamID, "STEAM_0:0:4895399") != 0 &&	// Shrill
				Q_stricmp(czSteamID, "STEAM_0:1:3221960") != 0 &&	// Pax
				Q_stricmp(czSteamID, "STEAM_0:1:2380995") != 0 &&	// Snipey-X
				Q_stricmp(czSteamID, "STEAM_0:0:5102060") != 0 &&	// X-Convict
				Q_stricmp(czSteamID, "STEAM_0:0:5266353") != 0 &&	// Chamby
				Q_stricmp(czSteamID, "STEAM_0:1:3580592") != 0 &&	// Arrakis
				Q_stricmp(czSteamID, "STEAM_0:0:3093245") != 0 &&	// n30 :D My <3
				Q_stricmp(czSteamID, "STEAM_0:0:2864641") != 0 &&	// Rovient
				Q_stricmp(czSteamID, "STEAM_0:0:11152733") != 0 &&	// snake85027
				Q_stricmp(czSteamID, "STEAM_0:0:3297314") != 0 &&	// PhilE
				// BETA TESTERS
				Q_stricmp(czSteamID, "STEAM_0:0:1569527") != 0 &&	// LlamaYama
				Q_stricmp(czSteamID, "STEAM_0:1:504525") != 0 &&	// ante
				Q_stricmp(czSteamID, "STEAM_0:1:392100") != 0 &&	// Ambassador
				Q_stricmp(czSteamID, "STEAM_0:0:5268916") != 0 &&	// Skittles the Pirate
				Q_stricmp(czSteamID, "STEAM_0:0:4921990") != 0 &&	// Imp
				Q_stricmp(czSteamID, "STEAM_0:0:1130125") != 0 &&	// modbrit
				Q_stricmp(czSteamID, "STEAM_0:0:30233") != 0 &&		// st0lve
				Q_stricmp(czSteamID, "STEAM_0:1:207042") != 0 &&	// Spectator6
				Q_stricmp(czSteamID, "STEAM_0:0:1124665") != 0 &&	// Ennui
				Q_stricmp(czSteamID, "STEAM_0:1:5488311") != 0 &&	// OrangutanShoes
				Q_stricmp(czSteamID, "STEAM_0:1:922341") != 0 &&	// Desdinova
				Q_stricmp(czSteamID, "STEAM_0:1:4065099") != 0 &&	// Puzzlemaker
				Q_stricmp(czSteamID, "STEAM_0:1:8768545") != 0 &&	// Treborresor
				Q_stricmp(czSteamID, "STEAM_0:1:772992") != 0 &&	// Grider
				Q_stricmp(czSteamID, "STEAM_0:1:3773685") != 0 &&	// Debil
				Q_stricmp(czSteamID, "STEAM_0:0:4816653") != 0))	// jbird
				engine->ClientCommand(pPlayer->edict(), "disconnect");
			else
				pPlayer->SetPlayerAuthorized(true);
		}
	}
#endif

	// The game is over, so no more thinking for us.
	if (!g_fGameOver)
	{
		if (m_nRoundTime <= 0)
		{
			if (gpGlobals->curtime >= m_fRoundFinalizeTime && CanDoNewRound())
			{
				// Reset the map.
				ResetMap();

				StartNewRound();
			}
			else if (m_bInRound)
				EndCurrentRound(TEAM_UNASSIGNED);
		}
		else
		{
			// If auras are enabled do our actions.
			if (sv_fskn_enableauras.GetBool())
				AurasThink();
		}
	}

	// Fetch the remaining time in the round.
	m_nRoundTime = GetRoundRemainTime();

	BaseClass::Think();
}

#ifndef CLIENT_DLL
// CForsakenGameRules - Server Side
bool CForsakenGameRules::CanDoNewRound()
{
	CForsakenTeam *pCivTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	CForsakenTeam *pGovTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	CForsakenTeam *pRelTeam = static_cast<CForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	int nCivilians = pCivTeam->GetNumPlayers();
	int nGovernors = pGovTeam->GetNumPlayers();
	int nReligious = pRelTeam->GetNumPlayers();

	// No players, so don't start a round.
	if (nCivilians == 0 && nGovernors == 0 && nReligious == 0)
		return false;

	// Do we have enough people to start a round?
	if (!(nCivilians > 0 && nGovernors > 0) && !(nCivilians > 0 && nReligious > 0) &&
		!(nGovernors > 0 && nReligious > 0))
		return false;

	return true;
}
#endif

// Global Functions
const char *FindEngineArg(const char *czName)
/*
	
	Pre: 
	Post: 
*/
{
	int nArgs = engine->Cmd_Argc();

	for (int i = 1; i < nArgs; i++)
	{
		if (stricmp(engine->Cmd_Argv(i), czName) == 0)
			return (i + 1) < nArgs ? engine->Cmd_Argv(i + 1) : "";
	}

	return 0;
}

int FindEngineArgInt(const char *czName, int nDefaultVal)
/*
	
	Pre: 
	Post: 
*/
{
		const char *pVal = FindEngineArg(czName);

		if (pVal)
			return atoi(pVal);
		else
			return nDefaultVal;
}

static void CC_Leader_ChooseSpawn()
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable spawn selection for release.
#ifdef BETA_FORSAKEN
	CBaseEntity *pFoundEntity = gEntList.FindEntityByClassname(NULL, "forsaken_fortified_entity");
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// We take two parameters and we have to be the leader.
	if (!pPlayer || !pPlayer->IsConnected() || !pPlayer->IsLeader() || engine->Cmd_Argc() < 2)
		return;

	char *czObjective = engine->Cmd_Argv(1);

	// Get the team of the player.
	pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

	// They selected their default spawn.
	if (Q_stricmp(czObjective, "-1") == 0)
		pTeam->SetFortificationSpawn(NULL);

	// Loop through all fortified objective entities.
	for (; pFoundEntity != NULL; )
	{
		CForsakenFortifiedObjectiveEntity *pFortifiedEntity = 
			static_cast<CForsakenFortifiedObjectiveEntity*>(pFoundEntity);
		int nPlayerTeamBitmask = 0;

		// Convert player team to player team bitmask.
		switch (pPlayer->GetTeamNumber())
		{
		case TEAM_CIVILIANS:
			nPlayerTeamBitmask = TEAM_CIVILIANS_BITMASK;
			break;

		case TEAM_GOVERNORS:
			nPlayerTeamBitmask = TEAM_GOVERNORS_BITMASK;
			break;

		case TEAM_RELIGIOUS:
			nPlayerTeamBitmask = TEAM_RELIGIOUS_BITMASK;
			break;
		}

		// Is this the objective we are looking for, and do we own it?
		if (Q_stricmp(pFortifiedEntity->GetObjectiveName(), czObjective) == 0 && 
			pFortifiedEntity->GetTeamOwner() == nPlayerTeamBitmask)
		{
			// Set the spawn to the desired fortification.
			pTeam->SetFortificationSpawn(pFortifiedEntity);

			break;
		}

		// Fetch the next fortified objective entity.
		pFoundEntity = gEntList.FindEntityByClassname(pFoundEntity, "forsaken_fortified_entity");
	}
#endif
}

static void CC_Leader_UpgradeFortification()
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable upgrading fortifications for release.
#ifdef BETA_FORSAKEN
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CBaseEntity *pFoundEntity = gEntList.FindEntityByClassname(NULL, "forsaken_fortified_entity");

	// We take two parameters and we have to be the leader.
	if (!pPlayer->IsLeader() || engine->Cmd_Argc() < 3)
		return;

	char *czObjective = engine->Cmd_Argv(1);
	int nLevel = atoi(engine->Cmd_Argv(2));

	// Loop through all fortified objective entities.
	for (; pFoundEntity != NULL; )
	{
		CForsakenFortifiedObjectiveEntity *pFortifiedEntity = 
			static_cast<CForsakenFortifiedObjectiveEntity*>(pFoundEntity);
		int nPlayerTeamBitmask = 0;

		// Convert player team to player team bitmask.
		switch (pPlayer->GetTeamNumber())
		{
		case TEAM_CIVILIANS:
			nPlayerTeamBitmask = TEAM_CIVILIANS_BITMASK;
			break;

		case TEAM_GOVERNORS:
			nPlayerTeamBitmask = TEAM_GOVERNORS_BITMASK;
			break;

		case TEAM_RELIGIOUS:
			nPlayerTeamBitmask = TEAM_RELIGIOUS_BITMASK;
			break;
		}

		// Is this the objective we are looking for, and do we own it?
		if (Q_stricmp(pFortifiedEntity->GetObjectiveName(), czObjective) == 0 && 
			pFortifiedEntity->GetTeamOwner() == nPlayerTeamBitmask)
		{
			// Upgrade the fortification to the desired level.
			pFortifiedEntity->Upgrade(nLevel);
		}

		// Fetch the next fortified objective entity.
		pFoundEntity = gEntList.FindEntityByClassname(pFoundEntity, "forsaken_fortified_entity");
	}
#endif
}

static void CC_RoundRestart(ConVar *pConsoleVar, const char *czOldValue)
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable round restarting for release.
#ifdef BETA_FORSAKEN
	// We want to restart the round, so reset it and go back to the old value.
	if (pConsoleVar->GetInt() == 1)
	{
		ForsakenGameRules()->EndCurrentRound(TEAM_UNASSIGNED);

		pConsoleVar->SetValue(0);
	}
#endif
}

static void CC_WarInfo()
/*
	
	Pre: 
	Post: 
*/
{
	char czWarInfo[1024] = "";

	// Display information for each team.
	// Format = Team_Name:Wins:Nutrition:Ordinance;Team_Name...
	for (int i = TEAM_GOVERNORS; i < TEAM_SPECTATOR; i++)
	{
		CForsakenTeam *pTeam = (CForsakenTeam*)g_Teams[i];

		// Fetch the team information.
		Q_snprintf(czWarInfo, sizeof(czWarInfo), "%s%s:%d:%d:%d;", czWarInfo, pTeam->GetName(), 
			pTeam->GetScore(), pTeam->GetNutritionResources(), pTeam->GetOrdinanceResources());
	}

	// We want endlines.
	Q_snprintf(czWarInfo, sizeof(czWarInfo), "%s\r\n", czWarInfo);

	// Output this to the console.
	Msg(czWarInfo);
}

static void CC_WarRestart(ConVar *pConsoleVar, const char *czOldValue)
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable war restarting for release.
#ifdef BETA_FORSAKEN
	// We want to restart the round, so reset it and go back to the old value.
	if (pConsoleVar->GetInt() == 1)
	{
		ForsakenGameRules()->EndWar(TEAM_UNASSIGNED);

		pConsoleVar->SetValue(0);
	}
#endif
}

void InitBodyQue()
/*
	
	Pre: 
	Post: 
*/
{
}

void *SendProxy_ForsakenGameRules(const SendProp *pProp, const void *pStructBase, const void *pData, 
	CSendProxyRecipients *pRecipients, int nObjectID)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenGameRules *pRules = ForsakenGameRules();

	Assert(pRules);
	pRecipients->SetAllRecipients();

	return pRules;
}

#endif
