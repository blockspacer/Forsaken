/*
	forsaken_objective_entity.cpp
	The objective entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken_objective_entity.h"
#include "forsaken_destroy_entity.h"

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_destroyobjective_entity, CForsakenDestroyObjectiveEntity);

BEGIN_DATADESC(CForsakenDestroyObjectiveEntity)
	// Fields
	DEFINE_FIELD(m_nHealth, FIELD_INTEGER),
	DEFINE_FIELD(m_nTeamOwner, FIELD_INTEGER),
	DEFINE_KEYFIELD(m_nHealth, FIELD_INTEGER, "objective_health"),
	DEFINE_KEYFIELD(m_nTeamOwner, FIELD_INTEGER, "objective_teamowner"),

	// Outputs

	// Think Functions
	DEFINE_THINKFUNC(DamageThink),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CForsakenDestroyObjectiveEntity, DT_ForsakenDestroyObjectiveEntity)
	SendPropBool(SENDINFO(m_bTakingDamage)),
	SendPropInt(SENDINFO(m_nHealth)),
END_SEND_TABLE()

// Global Variable Decleration
ConVar sv_fskn_ffobjective("sv_fskn_ffobjective", "0", FCVAR_REPLICATED, 
		"Players on the same team as the objective do no damage to it.");

// Constructor & Deconstructor
CForsakenDestroyObjectiveEntity::CForsakenDestroyObjectiveEntity() : CForsakenObjectiveEntity()
{
	m_bTakingDamage = false;
	m_nHealth = 0;
}

CForsakenDestroyObjectiveEntity::~CForsakenDestroyObjectiveEntity()
{
}

// Functions
int CForsakenDestroyObjectiveEntity::OnTakeDamage(const CTakeDamageInfo &rDmgInfo)
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity *pAttacker = rDmgInfo.GetAttacker();
	int nTeam = pAttacker->GetTeamNumber();
	int nBitTeam = 0;
	IGameEvent *event = gameeventmanager->CreateEvent("forsaken_objective_damaged");

	// Don't allow capturing if we aren't in a round.
	/*if (ForsakenGameRules()->GetRemainingTimeInRound() == 0)
		return 1;*/

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

	// Set the event data.
	if (event)
	{
		event->SetInt("entindex", entindex());
		event->SetInt("health", GetHealth());
		event->SetInt("team", nTeam);

		gameeventmanager->FireEvent(event);
	}

	// Let the clients know we are taking damage.
	m_bTakingDamage = true;

	// They shouldn't care if we are taking damage after 3s.
	SetNextThink(gpGlobals->curtime + 3000.0f);
	SetThink(&CForsakenDestroyObjectiveEntity::DamageThink);

	// Keep on taking damage.
	return BaseClass::OnTakeDamage(rDmgInfo);
}

void CForsakenDestroyObjectiveEntity::DamageThink()
/*
	
	Pre: 
	Post: 
*/
{
	// Reset the taking damage flag.
	m_bTakingDamage = false;
}

void CForsakenDestroyObjectiveEntity::Event_Killed(const CTakeDamageInfo &rDmgInfo)
/*
	
	Pre: 
	Post: 
*/
{
	IGameEvent *event = gameeventmanager->CreateEvent("forsaken_objective_destroyed");

	// Set the event data.
	if (event)
	{
		event->SetInt("entindex", entindex());
		event->SetInt("team", rDmgInfo.GetAttacker()->GetTeamNumber());

		gameeventmanager->FireEvent(event);
	}

	// Fire the output.
	m_ObjectiveCompleted.Set(rDmgInfo.GetAttacker()->GetTeamNumber(), rDmgInfo.GetAttacker(), 
		this);

	// Notify the attacker that they killed us.
	if (rDmgInfo.GetAttacker())
		rDmgInfo.GetAttacker()->Event_KilledOther(this, rDmgInfo);

	// We no longer take damage and we need to be marked as dead.
	m_takedamage = DAMAGE_NO;
	m_lifeState = LIFE_DEAD;
}

void CForsakenDestroyObjectiveEntity::Precache()
/*
	
	Pre: 
	Post: 
*/
{
	PrecacheModel(GetModelName().ToCStr());

	BaseClass::Precache();
}

void CForsakenDestroyObjectiveEntity::SetTeamMapOwner(int nTeamOwner)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenDestroyObjectiveEntity::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	IGameEvent *event = gameeventmanager->CreateEvent("forsaken_objective_add");

	Precache();

	SetModel(STRING(GetModelName()));

	// No movement and an axis-aligned bounding box.
	SetAbsVelocity(vec3_origin);
	SetMoveType(MOVETYPE_NONE);
	SetSolid(SOLID_BBOX);

	// Set the models health.
	SetHealth(m_nHealth);
	SetMaxHealth(m_nHealth);

	// Let the game know we can take damage.
	m_takedamage = DAMAGE_YES;

	// Set the event data.
	if (event)
	{
		event->SetInt("entindex", entindex());

		gameeventmanager->FireEvent(event);
	}

	BaseClass::Spawn();
}