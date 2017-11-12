/*
	forsaken_fortifiedsupport_entity.cpp
	The fortified support entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken_player.h"
#include "forsaken_fortified_entity.h"
#include "forsaken_fortifiedsupport_entity.h"
#include "forsaken_objective_entity.h"
#include "forsaken_capture_entity.h"

// BETA DISABLE: Disable fortification support entities for release.
#ifdef BETA_FORSAKEN
// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_fortifiedsupport_entity, CForsakenFortifiedSupportEntity);

BEGIN_DATADESC(CForsakenFortifiedSupportEntity)
	// Fields
	DEFINE_AUTO_ARRAY(m_czCivModel, FIELD_CHARACTER),
	DEFINE_AUTO_ARRAY(m_czGovModel, FIELD_CHARACTER),
	DEFINE_AUTO_ARRAY(m_czRelModel, FIELD_CHARACTER),
	DEFINE_AUTO_ARRAY(m_czObjectiveName, FIELD_CHARACTER),

	DEFINE_FIELD(m_nHealth, FIELD_INTEGER),
	DEFINE_FIELD(m_nLevel, FIELD_INTEGER),
	DEFINE_FIELD(m_nTeamOwner, FIELD_INTEGER),

	DEFINE_KEYFIELD(m_nHealth, FIELD_INTEGER, "support_health"),
	DEFINE_KEYFIELD(m_nLevel, FIELD_INTEGER, "support_level"),
	DEFINE_KEYFIELD(m_nTeamOwner, FIELD_INTEGER, "support_teamowner"),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_INTEGER, "FortificationUpgraded", FortificationUpgraded),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "SetTeamOwner", SetTeamOwner),

	// Outputs

	// Think Functions
	DEFINE_THINKFUNC(DamageThink),
END_DATADESC()

// Global Variable Decleration
extern ConVar sv_fskn_ffobjective;

// Constructor & Deconstructor
CForsakenFortifiedSupportEntity::CForsakenFortifiedSupportEntity()
{
	m_bTakingDamage = false;
	m_nHealth = 0;
	m_nLevel = 0;
	m_nTeamOwner = 0;
}

CForsakenFortifiedSupportEntity::~CForsakenFortifiedSupportEntity()
{
}

// Functions
bool CForsakenFortifiedSupportEntity::FindFortificationEntity()
/*
	
	Pre: 
	Post: 
*/
{
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
			Q_stricmp(m_czObjectiveName, pFortifiedObjectiveEntity->GetObjectiveName()) == 0)
		{
			// The entity was found.
			bFound = true;

			// Obtain a pointer to the fortified entity.
			m_hFortifiedEntity = pFortifiedObjectiveEntity;

			break;
		}
	}

	return bFound;
}

bool CForsakenFortifiedSupportEntity::KeyValue(const char *czKey, const char *czValue)
/*
	
	Pre: 
	Post: 
*/
{
	if (FStrEq(czKey, "civ_model"))
		Q_strncpy(m_czCivModel.GetForModify(), czValue, MAX_PATH);
	else if (FStrEq(czKey, "gov_model"))
		Q_strncpy(m_czGovModel.GetForModify(), czValue, MAX_PATH);
	else if (FStrEq(czKey, "rel_model"))
		Q_strncpy(m_czRelModel.GetForModify(), czValue, MAX_PATH);
	else if (FStrEq(czKey, "objective_name"))
		Q_strncpy(m_czObjectiveName.GetForModify(), czValue, OBJECTIVE_NAME_LENGTH);
	else
		return BaseClass::KeyValue(czKey, czValue);

	return true;
}

int CForsakenFortifiedSupportEntity::OnTakeDamage(const CTakeDamageInfo &rDmgInfo)
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity *pAttacker = rDmgInfo.GetAttacker();
	int nTeam = pAttacker->GetTeamNumber();
	int nBitTeam = 0;

	// Don't allow capturing if we aren't in a round.
	if (ForsakenGameRules()->GetRemainingTimeInRound() == 0)
		return 1;

	// Get the team bit mask of the attacker.
	switch (nTeam)
	{
	case TEAM_CIVILIANS:
		nBitTeam = TEAM_CIVILIANS_BITMASK;
		break;

	case TEAM_GOVERNORS:
		nBitTeam = TEAM_GOVERNORS_BITMASK;
		break;

	case TEAM_RELIGIOUS:
		nBitTeam = TEAM_RELIGIOUS_BITMASK;
		break;
	}

	// Are they on the same team and is objective friendly fire off?
	if (pAttacker->IsPlayer() && (nBitTeam & m_nTeamOwner) && !sv_fskn_ffobjective.GetBool())
		return 1;

	// Let the clients know we are taking damage.
	m_bTakingDamage = true;

	// They shouldn't care if we are taking damage after 3s.
	SetNextThink(gpGlobals->curtime + 3000.0f);
	SetThink(&CForsakenFortifiedSupportEntity::DamageThink);

	// Keep on taking damage.
	return BaseClass::OnTakeDamage(rDmgInfo);
}

void CForsakenFortifiedSupportEntity::DamageThink()
/*
	
	Pre: 
	Post: 
*/
{
	// Reset the taking damage flag.
	m_bTakingDamage = false;
}

void CForsakenFortifiedSupportEntity::Event_Killed(const CTakeDamageInfo &rDmgInfo)
/*
	
	Pre: 
	Post: 
*/
{
	// Notify the attacker that they killed us.
	if (rDmgInfo.GetAttacker())
		rDmgInfo.GetAttacker()->Event_KilledOther(this, rDmgInfo);

	// We no longer take damage and we need to be marked as dead.
	m_takedamage = DAMAGE_NO;
	m_lifeState = LIFE_DEAD;
}

void CForsakenFortifiedSupportEntity::FortificationUpgraded(inputdata_t &rInput)
/*
	
	Pre: 
	Post: 
*/
{
	// Find our fortification entity.
	FindFortificationEntity();

	int nLevel = rInput.value.Int();

	// This isn't targeted towards our level.
	if (nLevel > m_nLevel)
	{
		AddEffects(EF_NODRAW);
		SetSolid(SOLID_NONE);

		return;
	}

	// We are now upgraded, so update the model and set to draw.
	switch (m_nTeamOwner)
	{
	case TEAM_CIVILIANS_BITMASK:
		SetModel(m_czCivModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	case TEAM_GOVERNORS_BITMASK:
		SetModel(m_czGovModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	case TEAM_RELIGIOUS_BITMASK:
		SetModel(m_czRelModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	default:
		return;
	}

	// Set the models health.
	SetHealth(m_nHealth);
	SetMaxHealth(m_nHealth);

	// The model can take damage.
	m_takedamage = DAMAGE_NO;
	m_lifeState = LIFE_ALIVE;
}

void CForsakenFortifiedSupportEntity::Precache()
/*
	
	Pre: 
	Post: 
*/
{
	// Precache the models.
	PrecacheModel(m_czCivModel);
	PrecacheModel(m_czGovModel);
	PrecacheModel(m_czRelModel);

	BaseClass::Precache();
}

void CForsakenFortifiedSupportEntity::SetTeamMapOwner(int nTeamOwner)
/*
	
	Pre: 
	Post: 
*/
{
	// Find our fortification entity.
	FindFortificationEntity();

	// Save the new team owner.
	m_nTeamOwner = nTeamOwner;
}

void CForsakenFortifiedSupportEntity::SetTeamOwner(inputdata_t &rInput)
/*
	
	Pre: 
	Post: 
*/
{
	int nNewTeamOwner = rInput.value.Int();

	// Updated team owner, so update the model and set to draw or not draw.
	switch (nNewTeamOwner)
	{
	case TEAM_CIVILIANS_BITMASK:
		SetModel(m_czCivModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	case TEAM_GOVERNORS_BITMASK:
		SetModel(m_czGovModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	case TEAM_RELIGIOUS_BITMASK:
		SetModel(m_czRelModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	default:
		SetModel(m_czCivModel);
		AddEffects(EF_NODRAW);
		SetSolid(SOLID_NONE);
		break;
	}

	// Set the models health.
	SetHealth(m_nHealth);
	SetMaxHealth(m_nHealth);

	// The model can take damage.
	m_takedamage = DAMAGE_NO;
	m_lifeState = LIFE_ALIVE;

	// Save the new team owner.
	m_nTeamOwner = nNewTeamOwner;
}

void CForsakenFortifiedSupportEntity::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	Precache();

	switch (m_nTeamOwner)
	{
	case TEAM_CIVILIANS_BITMASK:
		SetModel(m_czCivModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	case TEAM_GOVERNORS_BITMASK:
		SetModel(m_czGovModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	case TEAM_RELIGIOUS_BITMASK:
		SetModel(m_czRelModel);
		RemoveEffects(EF_NODRAW);
		SetSolid(SOLID_BBOX);
		break;

	default:
		SetModel(m_czCivModel);
		AddEffects(EF_NODRAW);
		SetSolid(SOLID_NONE);
		break;
	}

	// No movement and an axis-aligned bounding box.
	SetAbsVelocity(vec3_origin);
	SetMoveType(MOVETYPE_NONE);

	// Set the models health.
	SetHealth(m_nHealth);
	SetMaxHealth(m_nHealth);

	// Let the game know we can't take damage.
	m_takedamage = DAMAGE_NO;

	BaseClass::Spawn();
}
#endif
