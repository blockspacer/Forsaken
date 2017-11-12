/*
	forsaken_weapon_base.cpp
	Base weapon class used for all Forsaken weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_player.h"
#include "forsaken_fortified_entity.h"
#include "forsaken_team.h"
#include "forsaken_spawnpoint.h"
#include "entitylist.h"
#include "tier0/memdbgon.h"

// HL2 Class Macros
IMPLEMENT_SERVERCLASS_ST(CForsakenTeam, DT_ForsakenTeam)
	SendPropBool(SENDINFO(m_bStarving)),
	SendPropBool(SENDINFO(m_bTeamAvailable)),
	SendPropEHandle(SENDINFO(m_hFortifiedSpawnEntity)),
	SendPropInt(SENDINFO(m_nNutritionResources)),
	SendPropInt(SENDINFO(m_nOrdinanceResources))
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(forsaken_team_manager, CForsakenTeam);

// Function Prototypes

// Global Variable Decleration
ConVar sv_fskn_startnutrition("sv_fskn_startnutrition", "15", FCVAR_REPLICATED, 
		"The amount of nutrition (food resources) each team starts out with.");
ConVar sv_fskn_startordinance("sv_fskn_startordinance", "30", FCVAR_REPLICATED, 
		"The amount of ordinance (weapon resources) each team starts out with.");
extern ConVar sv_fskn_starvinghealthdecrease;

// Constructor & Deconstructor
CForsakenTeam::CForsakenTeam()
{
	// Initialize variables.
	m_bTeamAvailable = true;
	m_nNutritionResources = 0;
	m_nOrdinanceResources = 0;
	m_pLeader = NULL;

	// Clear our lists.
	m_vecSpawns.RemoveAll();
}

CForsakenTeam::~CForsakenTeam()
{
	// Clear our lists.
	m_vecSpawns.RemoveAll();
}

// Functions
// CForsakenTeam
CBaseEntity *CForsakenTeam::SpawnPlayer(CBasePlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	bool bDefaultSpawn = false;
	CUtlVector<IForsakenSpawnPoint*> vecSpawnList;
	int nNumSpawns = 0, nSpawn = m_iLastSpawn + random->RandomInt(1, 3);
	int nStartingSpawn = 0;
	int nPlayerTeamBitMask = TEAM_UNASSIGNED;
	int nSpawnAmount = 0;

	// Determine the team bitmask.
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

	// If they have no fortified entity to spawn at, or the owner of that position is a new team.
	if (!m_hFortifiedSpawnEntity.Get() || (m_hFortifiedSpawnEntity.Get() && 
		m_hFortifiedSpawnEntity.Get()->GetTeamOwner() != nPlayerTeamBitMask))
	{
		//DevMsg("Spawning player on team #%d at home spawn.\n", pPlayer->GetTeamNumber());

		bDefaultSpawn = true;
		m_hFortifiedSpawnEntity.GetForModify() = NULL;

		// Add all of the default spawns to our spawn list.
		for (int i = 0; i < m_vecSpawns.Count(); i++)
			vecSpawnList.AddToTail(m_vecSpawns[i]);
	}
	else
	{
		// Determine the number of people that can spawn at a fortified position.
		switch (m_hFortifiedSpawnEntity.Get()->GetFortificationLevel())
		{
		case FORSAKEN_FORTIFIEDLEVEL_ZERO:
			nSpawnAmount = m_aPlayers.Count() * FORSAKEN_FORTIFIEDLEVELSPAWN_ZERO;
			break;

		case FORSAKEN_FORTIFIEDLEVEL_ONE:
			nSpawnAmount = m_aPlayers.Count() * FORSAKEN_FORTIFIEDLEVELSPAWN_ONE;
			break;

		case FORSAKEN_FORTIFIEDLEVEL_TWO:
			nSpawnAmount = m_aPlayers.Count() * FORSAKEN_FORTIFIEDLEVELSPAWN_TWO;
			break;

		case FORSAKEN_FORTIFIEDLEVEL_THREE:
			nSpawnAmount = m_aPlayers.Count() * FORSAKEN_FORTIFIEDLEVELSPAWN_THREE;
			break;

		default:
			break;
		}

		//DevMsg("Spawning player on team #%d at fortified spawn (%d allowed).\n", pPlayer->GetTeamNumber(), nSpawnAmount);

		// They can only spawn so many in the fortified position depending on level.
		if (m_nFortifiedSpawnAmount < nSpawnAmount)
		{
			// Add all of our fortified objectives to our spawn list.
			for (int i = 0; i < m_hFortifiedSpawnEntity.Get()->m_vecObjectiveSpawns.Count(); i++)
				vecSpawnList.AddToTail(m_hFortifiedSpawnEntity.Get()->m_vecObjectiveSpawns[i]);
		}
	}

	// Nowhere to spawn! RUH-ROH! So send them to their default spawn.
	if (vecSpawnList.Count() == 0 && !bDefaultSpawn)
	{
		m_hFortifiedSpawnEntity.GetForModify() = NULL;

		vecSpawnList.RemoveAll();

		// Add all of the default spawns to our spawn list.
		for (int i = 0; i < m_vecSpawns.Count(); i++)
			vecSpawnList.AddToTail(m_vecSpawns[i]);
	}

	// Find the number of spawns.
	nNumSpawns = vecSpawnList.Count();

	// Wrap the spawn number to within our acceptable range.
	if (nSpawn >= nNumSpawns)
		nSpawn %= nNumSpawns;

	// Set the spawn we started at.
	nStartingSpawn = nSpawn;

	// Now loop through the spawnpoints and pick one
	do
	{
		int nCurrSpawn = nSpawn % nNumSpawns;

		// Is the spawn point valid?
		if (vecSpawnList[nCurrSpawn]->IsValid( pPlayer ))
		{
			// Fire the output.
			vecSpawnList[nCurrSpawn]->m_OnPlayerSpawn.FireOutput(pPlayer, vecSpawnList[nCurrSpawn]);

			// We just spawned here.
			m_iLastSpawn = nCurrSpawn;

			// If we are spawning them in the fortified area, increase the fortified spawn amount.
			if (m_nFortifiedSpawnAmount < nSpawnAmount)
				m_nFortifiedSpawnAmount++;

			// Now we spawn.
			return vecSpawnList[nCurrSpawn];
		}

		nSpawn++;
	} while ((nSpawn % nNumSpawns) != nStartingSpawn);

	// If we got here, we went through all spawns and hit our starting spawn.
	// This means there weren't enough spawns.
	return NULL;
}

int CForsakenTeam::GetNumberAlivePlayers()
/*
	
	Pre: 
	Post: 
*/
{
	int nAliveCount = 0;

	// Loop through all of our players.
	for (int i = 0; i < m_aPlayers.Count(); i++)
	{
		// If the player is allive, connected, and valid, add 'em.
		if (!m_aPlayers[i] || !m_aPlayers[i]->IsConnected() || !m_aPlayers[i]->IsAlive())
			continue;

		nAliveCount++;
	}

	return nAliveCount;
}

int CForsakenTeam::GetNumberAlivePlayers(CBasePlayer *pExclude)
/*
	
	Pre: 
	Post: 
*/
{
	int nAliveCount = 0;

	// Loop through all of our players.
	for (int i = 0; i < m_aPlayers.Count(); i++)
	{
		// If the player is alive, connected, valid and isn't our exclude guy, add 'em.
		if (!m_aPlayers[i] || !m_aPlayers[i]->IsConnected() || !m_aPlayers[i]->IsAlive() || 
			m_aPlayers[i] == pExclude)
			continue;

		nAliveCount++;
	}

	return nAliveCount;
}

void CForsakenTeam::AddForsakenSpawn(IForsakenSpawnPoint *pSpawn)
/*
	
	Pre: 
	Post: 
*/
{
	// We don't want any duplicates.
	for (int i = 0; i < m_vecSpawns.Count(); i++)
	{
		if (m_vecSpawns[i] == pSpawn)
			return;
	}

	m_vecSpawns.AddToTail(pSpawn);
}

void CForsakenTeam::AssignLeader()
/*
	
	Pre: 
	Post: 
*/
{
	if (m_aPlayers.Count() <= 0)
		return;

	int nPlayer = -1, nWeighting = 0;

	// Determine the maximum weighting.
	for (int i = 0; i < m_aPlayers.Count(); i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(m_aPlayers[i]);

		if (pPlayer && pPlayer->IsConnected())
		{
			// If they want to be leader, they get twice the chance of someone that might.
			switch (pPlayer->GetLeaderChoice())
			{
			case LEADEROPTION_YES:
				nWeighting += 2 + (2 * pPlayer->GetScore());
				break;

			case LEADEROPTION_MAYBE:
				nWeighting += 1 + pPlayer->GetScore();
				break;

			default:
				break;
			}
		}
	}

	// Apparently, nobody wants to be leader... so have an all even random guess.
	if (nWeighting == 0)
		nPlayer = random->RandomInt(0, m_aPlayers.Count() - 1);
	else
	{
		// Randomize who the leader is.
		int nLeaderWeighting = random->RandomInt(0, nWeighting);

		// Reset the weighting.
		nWeighting = 0;

		// Determine which player was chosen as leader.
		for (int i = 0; i < m_aPlayers.Count() && nPlayer == -1; i++)
		{
			CForsakenPlayer *pPlayer = ToForsakenPlayer(m_aPlayers[i]);

			if (pPlayer && pPlayer->IsConnected())
			{
				// Determine who was chosen based off of their weighting.
				switch (pPlayer->GetLeaderChoice())
				{
				case LEADEROPTION_YES:
					// Are we within our weighting range?
					if (nLeaderWeighting >= nWeighting && nLeaderWeighting < nWeighting + (2 + 2 * pPlayer->GetScore()))
						nPlayer = i;

					nWeighting += 2 + (2 * pPlayer->GetScore());

					break;

				case LEADEROPTION_MAYBE:
					// Are we within our weighting range?
					if (nLeaderWeighting >= nWeighting && nLeaderWeighting < nWeighting + (1 + pPlayer->GetScore()))
						nPlayer = i;

					nWeighting += 1 + pPlayer->GetScore();

					break;

				default:
					break;
				}
			}
		}

		// Did we find it based off of weighting? If not, go to the first person on the team.
		if (nPlayer == -1)
			nPlayer = 0;
	}

	CForsakenPlayer *pPlayer = ToForsakenPlayer(m_aPlayers[nPlayer]);

	// Set the random player to be the leader.
	if (pPlayer)
	{
		m_pLeader = pPlayer;
		pPlayer->SetLeader(true);
		pPlayer->UpdateModel();
	}
	else
		m_pLeader = NULL;
}

void CForsakenTeam::DoResourceCalculations()
/*
	
	Pre: 
	Post: 
*/
{
	if (m_aPlayers.Count() == 0)
		return;

	// Calculate those shared weapon resources.
	int nBaseResources = m_nOrdinanceResources / m_aPlayers.Count() / 2;
	int nTotalPointsTeam = 0;

	// Do an initial loop through the players and get their score.
	for (int i = 0; i < m_aPlayers.Count(); i++)
		nTotalPointsTeam += ToForsakenPlayer(m_aPlayers[i])->GetScore();

	// Do per-player weapon resource calculation.
	for (int i = 0; i < m_aPlayers.Count(); i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(m_aPlayers[i]);

		// Do we have a player? If so, give 'em their resources.
		if (pPlayer && nTotalPointsTeam != 0)
			pPlayer->SetSharedResources(nBaseResources + 
				((pPlayer->GetScore() / nTotalPointsTeam) / 2));
		else if (pPlayer)
			pPlayer->SetSharedResources(nBaseResources);
	}
}

void CForsakenTeam::ForcePlayersToSpectator()
/*
	
	Pre: 
	Post: 
*/
{
	CUtlVector<CBasePlayer *> vecPlayers;

	// Copy the player list.
	for (int i = 0; i < m_aPlayers.Count(); i++)
		vecPlayers.AddToTail(m_aPlayers[i]);

	// Loop through all players.
	for (int i = 0; i < vecPlayers.Count(); i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(vecPlayers[i]);

		// Force the player to spectator.
		pPlayer->ChangeTeam(TEAM_SPECTATOR);
		pPlayer->StartObserverMode(OBS_MODE_ROAMING);
	}
}

void CForsakenTeam::Init(const char *czName, int nNumber)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Init(czName, nNumber);

	// Set initial values.
	m_pLeader = NULL;

	// Set how many resources we should start out with.
	if (m_bTeamAvailable)
	{
		m_nNutritionResources = sv_fskn_startnutrition.GetInt();
		m_nOrdinanceResources = sv_fskn_startordinance.GetInt();
	}

	NetworkProp()->SetUpdateInterval(0.75f);
}

void CForsakenTeam::LoadState(KeyValues *pKeyValues)
/*
	
	Pre: 
	Post: 
*/
{
	switch (GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		m_bTeamAvailable = (pKeyValues->GetInt("civavailable", 1) > 0);
		m_nNutritionResources = pKeyValues->GetInt("civnutrition", 
			sv_fskn_startnutrition.GetInt());
		m_nOrdinanceResources = pKeyValues->GetInt("civordinance", 
			sv_fskn_startordinance.GetInt());

		break;

	case TEAM_GOVERNORS:
		m_bTeamAvailable = (pKeyValues->GetInt("govavailable", 1) > 0);
		m_nNutritionResources = pKeyValues->GetInt("govnutrition", 
			sv_fskn_startnutrition.GetInt());
		m_nOrdinanceResources = pKeyValues->GetInt("govordinance", 
			sv_fskn_startordinance.GetInt());

		break;

	case TEAM_RELIGIOUS:
		m_bTeamAvailable = (pKeyValues->GetInt("relavailable", 1) > 0);
		m_nNutritionResources = pKeyValues->GetInt("relnutrition", 
			sv_fskn_startnutrition.GetInt());
		m_nOrdinanceResources = pKeyValues->GetInt("relordinance", 
			sv_fskn_startordinance.GetInt());

		break;
	}
}

void CForsakenTeam::RemoveForsakenSpawn(IForsakenSpawnPoint *pSpawn)
/*
	
	Pre: 
	Post: 
*/
{
	for (int i = 0; i < m_vecSpawns.Count(); i++)
	{
		if (m_vecSpawns[i] == pSpawn)
		{
			m_vecSpawns.Remove(i);

			return;
		}
	}
}

void CForsakenTeam::SaveState(KeyValues *pKeyValues)
/*
	
	Pre: 
	Post: 
*/
{
	switch (GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		pKeyValues->SetInt("civavailable", IsTeamAvailable());
		pKeyValues->SetInt("civnutrition", GetNutritionResources());
		pKeyValues->SetInt("civordinance", GetOrdinanceResources());

		break;

	case TEAM_GOVERNORS:
		pKeyValues->SetInt("govavailable", IsTeamAvailable());
		pKeyValues->SetInt("govnutrition", GetNutritionResources());
		pKeyValues->SetInt("govordinance", GetOrdinanceResources());

		break;

	case TEAM_RELIGIOUS:
		pKeyValues->SetInt("relavailable", IsTeamAvailable());
		pKeyValues->SetInt("relnutrition", GetNutritionResources());
		pKeyValues->SetInt("relordinance", GetOrdinanceResources());

		break;
	}
}

void CForsakenTeam::SpawnTeam()
/*
	
	Pre: 
	Post: 
*/
{
	// We are re-spawning the team, so nobody has spawned in the fortified area yet.
	m_nFortifiedSpawnAmount = 0;

	// Loop through all players.
	for (int i = 0; i < m_aPlayers.Count(); i++)
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(m_aPlayers[i]);
		int nHealth = 0;

		if (!pPlayer || !pPlayer->IsConnected())
			continue;

		// If they are on a valid team pop em out of observer and spawn them.
		if (pPlayer->GetTeamNumber() != TEAM_UNASSIGNED && 
			pPlayer->GetTeamNumber() != TEAM_SPECTATOR)
		{
			int nHealth = 0;

			// If the player is dead, they get doubly punished.
			if (!pPlayer->IsAlive())
				TakeNutrition(2);
			else
				TakeNutrition(1);

			// Stop observer mode for the player.
			if (pPlayer->IsObserver())
				pPlayer->StopObserverMode();

			// Spawn the player.
			pPlayer->Spawn();

			// Calculate the health they can have (starvation).
			nHealth = m_nSpawnHealth + (GetNutritionResources() * sv_fskn_starvinghealthdecrease.GetInt());

			// Cap it off.
			if (nHealth > m_nSpawnHealth)
				nHealth = m_nSpawnHealth;

			// If the civilians are starving, notify everyone.
			if (!IsTeamStarving() && nHealth < m_nSpawnHealth)
				SetTeamStarving(true);

			// Set their max health.
			pPlayer->SetMaxHealth(m_nSpawnHealth);

			// Does the player spawn with health?
			if (nHealth <= 0)
			{
				// Woops, guess not... so stick them in observer.
				pPlayer->StartObserverMode(OBS_MODE_ROAMING);

				ForsakenGameRules()->EnableTeam(GetTeamNumber(), false);
			}
			else
			{
				// Set the player's health.
				pPlayer->SetHealth(nHealth);
			}
		}
	}
}

// Global
bool LessThan_ObjectiveSpawn(CForsakenFortifiedObjectiveEntity *const &rLeft, CForsakenFortifiedObjectiveEntity *const &rRight)
{
	return rLeft < rRight;
}

CForsakenTeam *GetGlobalForsakenTeam(int nIndex)
/*
	
	Pre: 
	Post: 
*/
{
	return (CForsakenTeam*)GetGlobalTeam(nIndex);
}