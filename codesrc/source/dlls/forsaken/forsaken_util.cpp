/*
	forsaken_fx_shared.cpp
	Forsaken effects.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_util.h"
#include "gameinterface.h"
#include "player.h"
#include "forsaken/forsaken_player.h"
#include "forsaken_fortified_entity.h"
#include "forsaken/forsaken_team.h"

// Functions
int ForsakenUtil_TeamToTeamBitmask(int nTeam)
/*
	Converts the team number to a team bitmask.
	Pre: 
	Post: The equivalent bitmask is returned.
*/
{
	switch (nTeam)
	{
	case TEAM_CIVILIANS:
		return TEAM_CIVILIANS_BITMASK;

	case TEAM_GOVERNORS:
		return TEAM_GOVERNORS_BITMASK;

	case TEAM_RELIGIOUS:
		return TEAM_RELIGIOUS_BITMASK;

	default:
		return TEAM_UNASSIGNED;
	}
}

int ForsakenUtil_TeamBitmaskToTeam(int nBitmask)
/*
	Converts the team bitmask to a team number.
	Pre: 
	Post: The equivalent team is returned.
*/
{
	switch (nBitmask)
	{
	case TEAM_CIVILIANS_BITMASK:
		return TEAM_CIVILIANS;

	case TEAM_GOVERNORS_BITMASK:
		return TEAM_GOVERNORS;

	case TEAM_RELIGIOUS_BITMASK:
		return TEAM_RELIGIOUS;

	default:
		return TEAM_UNASSIGNED;
	}
}

void ForsakenUtil_PlayGlobalSound(char *czSoundName)
/*
	
	Pre: 
	Post: 
*/
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_PlayerByIndex(i));

		if (!pPlayer || !pPlayer->IsClient() || !pPlayer->IsConnected())
			continue;

		CSingleUserRecipientFilter filter(pPlayer);

		// Emit the sound to that player.
		pPlayer->EmitSound(filter, pPlayer->entindex(), czSoundName, &pPlayer->GetAbsOrigin());
	}
}