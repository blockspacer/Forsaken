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

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_objective_entity, CForsakenObjectiveEntity);

BEGIN_DATADESC(CForsakenObjectiveEntity)
	// Fields
	DEFINE_AUTO_ARRAY(m_czObjectiveName, FIELD_CHARACTER),

	// Outputs
	DEFINE_OUTPUT(m_ObjectiveCompleted, "ObjectiveCompleted"),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CForsakenObjectiveEntity, DT_ForsakenObjectiveEntity)
	SendPropInt(SENDINFO(m_nTeamOwner)),
	SendPropString(SENDINFO(m_czObjectiveName)),
END_SEND_TABLE()

// Global Variable Decleration

// Constructor & Deconstructor
CForsakenObjectiveEntity::CForsakenObjectiveEntity() : CBaseAnimating()
{
	SetPredictionEligible(true);

	m_nTeamOwner = TEAM_UNASSIGNED;
}

CForsakenObjectiveEntity::~CForsakenObjectiveEntity()
{
}

// Functions
bool CForsakenObjectiveEntity::KeyValue(const char *czKeyName, const char *czValue)
{
	if (FStrEq(czKeyName, "objective_name"))
	{
		Q_strncpy(m_czObjectiveName.GetForModify(), czValue, OBJECTIVE_NAME_LENGTH);

		return true;
	}

	return BaseClass::KeyValue(czKeyName, czValue);
}