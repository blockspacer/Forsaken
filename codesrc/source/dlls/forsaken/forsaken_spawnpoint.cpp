/*
	forsaken_spawnpoint.cpp
	All player spawn points for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "entitylist.h"
#include "entityoutput.h"
#include "player.h"
#include "eventqueue.h"
#include "gamerules.h"
#include "forsaken_player.h"
#include "forsaken_fortified_entity.h"
#include "forsaken_team.h"
#include "forsaken_spawnpoint.h"
#include "tier0/memdbgon.h"

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_civspawn, CForsakenSpawnPoint_Civ);
LINK_ENTITY_TO_CLASS(forsaken_govspawn, CForsakenSpawnPoint_Gov);
LINK_ENTITY_TO_CLASS(forsaken_relspawn, CForsakenSpawnPoint_Rel);
LINK_ENTITY_TO_CLASS(forsaken_joinspawn, CForsakenSpawnPoint_Join);
LINK_ENTITY_TO_CLASS(forsaken_objspawn, CForsakenSpawnPoint_Objective);

BEGIN_DATADESC(CForsakenSpawnPoint_Civ)
	DEFINE_OUTPUT(m_OnPlayerSpawn, "OnPlayerSpawn"),
END_DATADESC()

BEGIN_DATADESC(CForsakenSpawnPoint_Gov)
	DEFINE_OUTPUT(m_OnPlayerSpawn, "OnPlayerSpawn"),
END_DATADESC()

BEGIN_DATADESC(CForsakenSpawnPoint_Rel)
	DEFINE_OUTPUT(m_OnPlayerSpawn, "OnPlayerSpawn"),
END_DATADESC()

BEGIN_DATADESC(CForsakenSpawnPoint_Join)
	DEFINE_OUTPUT(m_OnPlayerSpawn, "OnPlayerSpawn"),
END_DATADESC()

BEGIN_DATADESC(CForsakenSpawnPoint_Objective)
	DEFINE_FIELD(m_czObjectiveName, FIELD_STRING),
	DEFINE_KEYFIELD(m_czObjectiveName, FIELD_STRING, "objective_name"),
END_DATADESC()

// Constructor & Deconstructor
CForsakenSpawnPoint_Objective::CForsakenSpawnPoint_Objective()
{
}

CForsakenSpawnPoint_Civ::~CForsakenSpawnPoint_Civ()
{
	CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(GetGlobalTeam(TEAM_CIVILIANS));

	if (pTeam)
		pTeam->RemoveForsakenSpawn(this);
}

CForsakenSpawnPoint_Gov::~CForsakenSpawnPoint_Gov()
{
	CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(GetGlobalTeam(TEAM_GOVERNORS));

	if (pTeam)
		pTeam->RemoveForsakenSpawn(this);
}

CForsakenSpawnPoint_Rel::~CForsakenSpawnPoint_Rel()
{
	CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(GetGlobalTeam(TEAM_RELIGIOUS));

	if (pTeam)
		pTeam->RemoveForsakenSpawn(this);
}

CForsakenSpawnPoint_Join::~CForsakenSpawnPoint_Join()
{
}

CForsakenSpawnPoint_Objective::~CForsakenSpawnPoint_Objective()
{
}

// Functions
void CForsakenSpawnPoint_Civ::Activate()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Activate();

	CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(GetGlobalTeam(TEAM_CIVILIANS));

	pTeam->AddForsakenSpawn(this);
}

void CForsakenSpawnPoint_Gov::Activate()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Activate();

	CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(GetGlobalTeam(TEAM_GOVERNORS));

	pTeam->AddForsakenSpawn(this);
}

void CForsakenSpawnPoint_Rel::Activate()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Activate();

	CForsakenTeam *pTeam = static_cast<CForsakenTeam*>(GetGlobalTeam(TEAM_RELIGIOUS));

	pTeam->AddForsakenSpawn(this);
}

void CForsakenSpawnPoint_Join::Activate()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Activate();
}

void CForsakenSpawnPoint_Objective::Activate()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Activate();

	bool bFound = false;
	CBaseEntity *pEntity = NULL;

	// Search through all fortified entities.
	while (!bFound && (pEntity = gEntList.FindEntityByClassname(pEntity, 
		"forsaken_fortified_entity")) != NULL)
	{
		CForsakenFortifiedObjectiveEntity *pFortifiedObjectiveEntity = 
			dynamic_cast<CForsakenFortifiedObjectiveEntity*>(pEntity);

		// Is this the entity we are looking for?
		if (pFortifiedObjectiveEntity && 
			Q_stricmp(STRING(m_czObjectiveName), pFortifiedObjectiveEntity->GetObjectiveName()) == 0)
		{
			// The entity was found.
			bFound = true;

			// Add us to the objective's spawn list.
			pFortifiedObjectiveEntity->AddObjectiveSpawn(this);

			break;
		}
	}
}