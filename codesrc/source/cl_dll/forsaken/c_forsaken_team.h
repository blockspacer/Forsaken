#ifndef __CLIENT_FORSAKEN_TEAM_H_
#define __CLIENT_FORSAKEN_TEAM_H_

#include "c_team.h"
#include "shareddefs.h"
#include "c_forsaken_fortified_entity.h"

class C_ForsakenTeam : public C_Team
{
public:
	DECLARE_CLASS(C_ForsakenTeam, C_Team);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor
	C_ForsakenTeam();
	virtual ~C_ForsakenTeam();

	// Public Accessor Functions
	bool IsTeamAvailable() { return m_bTeamAvailable; }
	bool IsTeamStarving() { return m_bStarving; }
	C_ForsakenFortifiedObjectiveEntity *GetSpawn() { return dynamic_cast<C_ForsakenFortifiedObjectiveEntity*>(m_hFortifiedSpawnEntity.Get()); }
	int GetNutritionResources() { return m_nNutritionResources; }
	int GetOrdinanceResources() { return m_nOrdinanceResources; }

	// Public Functions

	// Public Variables
	bool m_bStarving;
	bool m_bTeamAvailable;
	EHANDLE m_hFortifiedSpawnEntity;
	int m_nNutritionResources;
	int m_nOrdinanceResources;
};

#endif