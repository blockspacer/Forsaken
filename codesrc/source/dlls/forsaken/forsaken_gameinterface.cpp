/*
	forsaken_gameinterface.cpp
	Base grenade class for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "gameinterface.h"
#include "mapentities.h"
#include "forsaken/forsaken_gamerules_territorywar.h"

// Global Variable Decleration
char *g_czMapName = NULL;

// Functions
void CServerGameClients::GetPlayerLimits(int &nMinPlayers, int &nMaxPlayers, 
		int &nDefaultMaxPlayers) const
/*
	
	Pre: 
	Post: 
*/
{
	nMinPlayers = 2;
	nMaxPlayers = MAX_PLAYERS;
	nDefaultMaxPlayers = 32;
}

void CServerGameDLL::LevelInit_ParseAllEntities(const char *czMapEntities, const char *czMapName)
/*
	
	Pre: 
	Post: 
*/
{
	int nLen = Q_strlen(czMapName) + 1;

	// If we are allocated, free.
	if (g_czMapName)
		delete [] g_czMapName;

	// Allocate memory for the map name.
	g_czMapName = new char[nLen+1];

	// Print in the map name.
	Q_snprintf(g_czMapName, nLen + 1, "%s", czMapName);

	// Parse all entities.
	MapEntity_ParseAllEntities(czMapEntities);

	// We want the game rules to reload it's state after we have parsed all entities.
	ForsakenGameRules()->LoadState();
}