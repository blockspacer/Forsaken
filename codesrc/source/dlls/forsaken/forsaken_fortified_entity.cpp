/*
	forsaken_fortified_entity.cpp
	The fortified entity in Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken_player.h"
#include "forsaken_fortified_entity.h"
#include "forsaken_objective_entity.h"
#include "forsaken_capture_entity.h"
#include "forsaken_team.h"
#include "forsaken_util.h"

// BETA DISABLE: Disable fortification entities for release.
#ifdef BETA_FORSAKEN
// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_fortified_entity, CForsakenFortifiedObjectiveEntity);

BEGIN_DATADESC(CForsakenFortifiedObjectiveEntity)
	// Fields
	DEFINE_AUTO_ARRAY(m_czCivModel, FIELD_CHARACTER),
	DEFINE_AUTO_ARRAY(m_czGovModel, FIELD_CHARACTER),
	DEFINE_AUTO_ARRAY(m_czRelModel, FIELD_CHARACTER),
	DEFINE_AUTO_ARRAY(m_czObjectiveName, FIELD_CHARACTER),

	DEFINE_FIELD(m_nHealth, FIELD_INTEGER),
	DEFINE_FIELD(m_nTeamOwner, FIELD_INTEGER),

	DEFINE_KEYFIELD(m_nHealth, FIELD_INTEGER, "fortification_health"),
	DEFINE_KEYFIELD(m_nTeamOwner, FIELD_INTEGER, "fortification_teamowner"),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_INTEGER, "SetTeamOwner", SetTeamOwner),

	// Outputs
	DEFINE_OUTPUT(m_FortificationComplete, "FortificationComplete"),
	DEFINE_OUTPUT(m_FortificationDestroyed, "FortificationDestroyed"),
	DEFINE_OUTPUT(m_FortificationUpgraded, "FortificationUpgraded"),

	// Think Functions
	DEFINE_THINKFUNC(DamageThink),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CForsakenFortifiedObjectiveEntity, DT_ForsakenFortifiedObjectiveEntity)
	SendPropBool(SENDINFO(m_bTakingDamage)),
	SendPropEHandle(SENDINFO(m_hObjectiveEntity)),
	SendPropInt(SENDINFO(m_nHealth)),
	SendPropInt(SENDINFO(m_nTeamOwner)),
	SendPropInt(SENDINFO(m_nLevel)),
	SendPropInt(SENDINFO(m_nFortificationStatus))
END_SEND_TABLE()

// Global Variable Decleration
ConVar sv_fskn_fortificationcost_lvl1("sv_fskn_fortificationcost_lvl1", "25", 
	FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, "Sets the cost of a level 1 upgrade.");
ConVar sv_fskn_fortificationcost_lvl2("sv_fskn_fortificationcost_lvl2", "50", 
	FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, "Sets the cost of a level 2 upgrade.");
ConVar sv_fskn_fortificationcost_lvl3("sv_fskn_fortificationcost_lvl3", "75", 
	FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_NOTIFY, "Sets the cost of a level 3 upgrade.");
extern ConVar sv_fskn_ffobjective;

// Constructor & Deconstructor
CForsakenFortifiedObjectiveEntity::CForsakenFortifiedObjectiveEntity()
{
	m_bTakingDamage = false;
	m_nCivModel = m_nGovModel = m_nRelModel = 0;
	m_hObjectiveEntity = NULL;
	m_nHealth = 0;
	m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_NONE;
	m_nDesiredLevel = m_nLevel = 0;
	m_nRoundBuildNumber = 0;
	m_nTeamOwner = 0;
}

CForsakenFortifiedObjectiveEntity::~CForsakenFortifiedObjectiveEntity()
{
}

// Functions
bool CForsakenFortifiedObjectiveEntity::CanAffordUpgrade(int nLevel)
/*
	
	Pre: 
	Post: 
*/
{
	int nTeam = ForsakenUtil_TeamBitmaskToTeam(m_nTeamOwner);
	CForsakenTeam *pForsakenTeam = dynamic_cast<CForsakenTeam*>(g_Teams[nTeam]);

	// Determine if the team owner can afford the upgrade.
	if (pForsakenTeam)
	{
		int nResources = pForsakenTeam->GetNutritionResources();
		int nUpgradeCost = GetUpgradeCost(nLevel);

		// If we have enough resources for the upgrade, return true.
		if (nResources >= nUpgradeCost)
			return true;
	}

	return false;
}

bool CForsakenFortifiedObjectiveEntity::FindObjectiveEntity()
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity *pFoundEntity = gEntList.FindEntityByName(NULL, m_czObjectiveName);
	CForsakenObjectiveEntity *pObjectiveEntity = dynamic_cast<CForsakenObjectiveEntity*>(pFoundEntity);

	// Did we find the entity?
	if (pObjectiveEntity)
	{
		m_hObjectiveEntity = pObjectiveEntity;

		return true;
	}

	return false;
}

bool CForsakenFortifiedObjectiveEntity::KeyValue(const char *czKey, const char *czValue)
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

bool CForsakenFortifiedObjectiveEntity::Upgrade(int nDesiredLevel)
/*
	
	Pre: 
	Post: 
*/
{
	// We can only upgrade if we are in a 'complete' phase.
	if (m_nFortificationStatus == FORSAKEN_FORTIFIEDSTATUS_COMPLETE)
	{
		// Can we afford the desired upgrade?
		if (CanAffordUpgrade(nDesiredLevel))
		{
			/*char czZomg[1024];

			Q_snprintf(czZomg, 1024, "%s fortification is now being upgraded to %d.", m_czObjectiveName, nDesiredLevel);
			DevMsg(czZomg);*/

			// We are now upgrading and we want to hit level X.
			m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_UPGRADING;
			m_nDesiredLevel = nDesiredLevel;

			return true;
		}
	}

	return false;
}

int CForsakenFortifiedObjectiveEntity::GetUpgradeCost(int nLevel)
/*
	
	Pre: 
	Post: 
*/
{
	int nLevelCosts[3];
	int nUpgradeCost = 0;

	// Fetch the costs into an array.
	nLevelCosts[0] = sv_fskn_fortificationcost_lvl1.GetInt();
	nLevelCosts[1] = sv_fskn_fortificationcost_lvl2.GetInt();
	nLevelCosts[2] = sv_fskn_fortificationcost_lvl3.GetInt();

	// Determine the upgrade cost.
	for (int i = m_nLevel; i < nLevel; i++)
		nUpgradeCost += nLevelCosts[i];

	return nUpgradeCost;
}

int CForsakenFortifiedObjectiveEntity::OnTakeDamage(const CTakeDamageInfo &rDmgInfo)
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
	SetThink(&CForsakenFortifiedObjectiveEntity::DamageThink);

	// Keep on taking damage.
	return BaseClass::OnTakeDamage(rDmgInfo);
}

void CForsakenFortifiedObjectiveEntity::DamageThink()
/*
	
	Pre: 
	Post: 
*/
{
	// Reset the taking damage flag.
	m_bTakingDamage = false;
}

void CForsakenFortifiedObjectiveEntity::Event_Killed(const CTakeDamageInfo &rDmgInfo)
/*
	
	Pre: 
	Post: 
*/
{
	// We got destroyed.
	m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_DESTROYED;

	// Fire the output.
	m_FortificationDestroyed.Set(TEAM_UNASSIGNED, rDmgInfo.GetAttacker(), this);

	// The fortification has been destroyed, they can capture the objective now.
	SetObjectiveCaptureable(true);

	// Notify the attacker that they killed us.
	if (rDmgInfo.GetAttacker())
		rDmgInfo.GetAttacker()->Event_KilledOther(this, rDmgInfo);

	// We no longer take damage and we need to be marked as dead.
	m_takedamage = DAMAGE_NO;
	m_lifeState = LIFE_DEAD;
}

void CForsakenFortifiedObjectiveEntity::FireGameEvent(IGameEvent *pEvent)
{
	// Is this a new round event?
	if (Q_stricmp(pEvent->GetName(), "forsaken_newround") == 0)
	{
		// Does someone own us?
		if (m_nTeamOwner != TEAM_UNASSIGNED)
		{
			// Determine our status.
			switch (m_nFortificationStatus)
			{
			case FORSAKEN_FORTIFIEDSTATUS_BUILDING:
				// If we haven't had a full round of building yet, count this new round.
				if (m_nRoundBuildNumber == 0)
					m_nRoundBuildNumber = pEvent->GetInt("roundnumber");
				else
				{
					/*char czZomg[1024];

					Q_snprintf(czZomg, 1024, "%s fortification has completed building.", m_czObjectiveName);
					DevMsg(czZomg);*/

					// We've had a full round of building without being destroyed, mark ourselves 
					// as complete.
					m_nRoundBuildNumber = 0;
					m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_COMPLETE;
					m_FortificationComplete.Set(m_nTeamOwner, this, this);
					m_FortificationUpgraded.Set(m_nLevel, this, this);
				}

				break;

			case FORSAKEN_FORTIFIEDSTATUS_UPGRADING:
				// If we haven't had a full round of building yet, count this new round.
				if (m_nRoundBuildNumber == 0)
					m_nRoundBuildNumber = pEvent->GetInt("roundnumber");
				else
				{
					// We've had a full round of building without being destroyed, mark ourselves 
					// as upgraded to the desired level.
					m_nRoundBuildNumber = 0;
					m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_COMPLETE;

					// If we can afford the upgrade, do it.
					if (CanAffordUpgrade(m_nDesiredLevel) && m_nDesiredLevel != 0)
					{
						/*char czZomg[1024];

						Q_snprintf(czZomg, 1024, "%s fortification has completed upgrading to %d.", m_czObjectiveName, m_nDesiredLevel);
						DevMsg(czZomg);*/

						// Upgrade to the level we want.
						m_nLevel = m_nDesiredLevel;
						m_nDesiredLevel = 0;
						m_FortificationComplete.Set(m_nTeamOwner, this, this);
						m_FortificationUpgraded.Set(m_nLevel, this, this);

						FortificationUpgraded();
					}
/*
					// Do we have a desired level we are trying to hit?
					if (m_nDesiredLevel != 0)
					{
						// Have we hit that desired level?
						if (m_nDesiredLevel < m_nLevel)
						{
							// Guess not, we are going to upgrade again.
							m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_UPGRADING;
							m_nRoundBuildNumber = 0;
						}
						else
							// We did, so stop upgrading.
							m_nDesiredLevel = 0;
					}*/
				}

				break;

			default:
				return;
			}
		}
	}
}

void CForsakenFortifiedObjectiveEntity::FortificationUpgraded()
/*
	
	Pre: 
	Post: 
*/
{
	int nTeam = ForsakenUtil_TeamBitmaskToTeam(m_nTeamOwner);
	CForsakenTeam *pForsakenTeam = dynamic_cast<CForsakenTeam*>(g_Teams[nTeam]);

	// Decrease the number of weapon resources of the team owner.
	if (pForsakenTeam)
		pForsakenTeam->IncreaseOrdinanceResources(-GetUpgradeCost(m_nLevel));
}

void CForsakenFortifiedObjectiveEntity::Precache()
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

void CForsakenFortifiedObjectiveEntity::SetObjectiveCaptureable(bool bState)
/*
	
	Pre: 
	Post: 
*/
{
	if (!m_hObjectiveEntity && !FindObjectiveEntity())
	{
		Msg("%s unable to find %s to set capture state\n", GetEntityName(), m_czObjectiveName);

		return;
	}

	CForsakenObjectiveEntity *pObjective = m_hObjectiveEntity.Get();

	// We only update the capture state for capture objectives.
	if (pObjective->GetObjectiveType() == OBJECTIVE_TYPE_CAPTURE)
	{
		CForsakenCaptureObjectiveEntity *pCaptureObjective = 
			dynamic_cast<CForsakenCaptureObjectiveEntity*>(m_hObjectiveEntity.Get());

		// Update it's capture state.
		pCaptureObjective->SetCaptureable(bState);
	}
	else
		Msg("%s is not a capture objective, not setting capture state\n", GetEntityName(), m_czObjectiveName);
}

void CForsakenFortifiedObjectiveEntity::SetTeamMapOwner(int nTeamOwner)
/*
	
	Pre: 
	Post: 
*/
{
	// Updated team owner, so update the model and set to draw or not draw.
	switch (nTeamOwner)
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

	if (nTeamOwner != TEAM_UNASSIGNED)
	{
		// Temporarily, we'll say that the fortification is created instantly.
		m_FortificationComplete.Set(nTeamOwner, this, this);

		// Also, with a created fortification they can't capture the objective anymore.
		SetObjectiveCaptureable(false);
	}

	// Set the models health.
	SetHealth(m_nHealth);
	SetMaxHealth(m_nHealth);

	// The model can take damage.
	m_takedamage = DAMAGE_YES;
	m_lifeState = LIFE_ALIVE;

	// Save the new team owner.
	m_nTeamOwner = nTeamOwner;
	m_nLevel = 0;

	// We owned this objective at the beginning of a map, so it's built.
	m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_COMPLETE;

}

void CForsakenFortifiedObjectiveEntity::SetTeamOwner(inputdata_t &rInput)
/*
	
	Pre: 
	Post: 
*/
{
	int nNewTeamOwner = rInput.value.Int();

	// New team owner, so set the previous fortification to destroyed.
	if (m_nTeamOwner != nNewTeamOwner)
		m_FortificationDestroyed.Set(TEAM_UNASSIGNED, this, this);

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

	// Also, with a created fortification they can't capture the objective anymore.
	SetObjectiveCaptureable(false);

	// Set the models health.
	SetHealth(m_nHealth);
	SetMaxHealth(m_nHealth);

	// The model can take damage.
	m_takedamage = DAMAGE_YES;
	m_lifeState = LIFE_ALIVE;

	// Save the new team owner.
	m_nTeamOwner = nNewTeamOwner;

	// We are being built.
	m_nFortificationStatus = FORSAKEN_FORTIFIEDSTATUS_BUILDING;
	m_nRoundBuildNumber = 0;
	m_nLevel = 0;

	/*char czZomg[1024];

	Q_snprintf(czZomg, 1024, "%s fortification is now being built.", m_czObjectiveName);
	DevMsg(czZomg);*/
}

void CForsakenFortifiedObjectiveEntity::Spawn()
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

	// Let the game know we can take damage.
	m_takedamage = DAMAGE_YES;

	// We want to listen to these events.
	gameeventmanager->AddListener(this, "forsaken_newround", true);

	BaseClass::Spawn();
}
#endif