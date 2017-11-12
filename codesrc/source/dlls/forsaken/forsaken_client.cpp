/*
	forsaken_client.cpp
	The server implementation of all base client handling in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "entitylist.h"
#include "physics.h"
#include "game.h"
#include "ai_network.h"
#include "ai_node.h"
#include "ai_hull.h"
#include "shake.h"
#include "player_resource.h"
#include "engine/IEngineSound.h"
#include "forsaken/forsaken_player.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "tier0/vprof.h"
#include "tier0/memdbgon.h"
#include "tier1/fmtstr.h"
#include "../version.h"

// Function Prototypes
const char *GetGameDescription();
extern CBaseEntity *FindPickerEntity(CBasePlayer *pPlayer);
void ClientActive(edict_t *pEdict, bool bLoadGame);
void ClientGamePrecache(void);
void ClientPutInServer(edict_t *pEdict, const char *czPlayerName);
void FinishClientPutInServer(CForsakenPlayer *pPlayer);
void GameStartFrame(void);
void InstallGameRules();
void respawn(CBaseEntity *pEdict, bool bCopyCorpse);

// Global Variable Decleration
extern bool g_fGameOver;

// Functions
// Global
const char *GetGameDescription()
/*
	
	Pre: 
	Post: 
*/
{
	if (g_pGameRules)
		return g_pGameRules->GetGameDescription();
	else
		return "Forsaken";
}

void ClientActive(edict_t *pEdict, bool bLoadGame)
/*
	
	Pre: 
	Post: 
*/
{
	Assert(!bLoadGame);

	CForsakenPlayer *pPlayer = static_cast<CForsakenPlayer*>(CBaseEntity::Instance(pEdict));

	FinishClientPutInServer(pPlayer);
}

void ClientGamePrecache(void)
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity::PrecacheModel("sprites/white.vmt");
	CBaseEntity::PrecacheModel("sprites/physbeam.vmt");
}

void ClientPutInServer(edict_t *pEdict, const char *czPlayerName)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = CForsakenPlayer::CreatePlayer("player", pEdict);

	pPlayer->SetPlayerEndAuthTime(gpGlobals->curtime + 120);
	pPlayer->SetPlayerAuthorized(false);
	pPlayer->SetPlayerName(czPlayerName);
	pPlayer->PlayerData()->netname = AllocPooledString(czPlayerName);
}

void FinishClientPutInServer(CForsakenPlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	// Look for the join camera.
	char czName[128] = "";
	CBaseEntity *pEntity = gEntList.FindEntityByClassname(NULL, "forsaken_joinspawn");
	int nChoice = 0;

	pPlayer->SetPlayerEndAuthTime(gpGlobals->curtime + 60.0f);

	// Initial client-join code.
	pPlayer->InitialSpawn();
	pPlayer->StartObserverMode(OBS_MODE_FIXED);
	pPlayer->ChangeTeam(TEAM_SPECTATOR);

	// Set the player's view and position if we have a join camera.
	if (pEntity)
	{
		pPlayer->SetAbsOrigin(pEntity->GetAbsOrigin());
		pPlayer->SetAbsAngles(pEntity->GetAbsAngles());
	}
	else
		pPlayer->SetAbsOrigin(vec3_origin);

	// Fetch the player name.
	Q_strncpy(czName, pPlayer->GetPlayerName(), sizeof(czName));

	// We need to make the client name safe.
	for (char *c = czName; c != NULL && *c != 0; c++)
	{
		if (*c == '%')
			*c = ' ';
	}

	// The client joined.
	UTIL_ClientPrintAll(HUD_PRINTNOTIFY, "#Game_connected", 
		czName[0] != 0 ? czName : "<unconnected>");

	// Set the players leader preferences.
	nChoice = atoi(engine->GetClientConVarValue(pPlayer->entindex(), "cl_fskn_leaderpreference"));

	// Is it a valid player and do they have a valid choice?
	if (nChoice >= LEADEROPTION_YES && nChoice <= LEADEROPTION_MAYBE)
		pPlayer->SetLeaderChoice(nChoice);
}

void GameStartFrame(void)
/*
	
	Pre: 
	Post: 
*/
{
	VPROF("GameStartFrame");

	// Forsaken Addition: Do a game over check before anything else.
	if (g_fGameOver)
		return;

	if (g_pGameRules)
		g_pGameRules->Think();

	gpGlobals->teamplay = teamplay.GetInt() ? true : false;
}

void InstallGameRules()
/*
	
	Pre: 
	Post: 
*/
{
	CreateGameRulesObject("CForsakenGameRules");
}

void respawn(CBaseEntity *pEdict, bool bCopyCorpse)
/*
	
	Pre: 
	Post: 
*/
{
	CBasePlayer *pPlayer = static_cast<CBasePlayer*>(pEdict);

	if (bCopyCorpse)
		pPlayer->CreateCorpse();

	static_cast<CForsakenGameRules*>(g_pGameRules)->HandleRespawn(pPlayer);
}