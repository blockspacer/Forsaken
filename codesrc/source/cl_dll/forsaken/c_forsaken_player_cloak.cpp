/*
	c_forsaken_player_cloak.cpp
	Client-side forsaken player ragdoll cloak (for Religious only).

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "c_forsaken_player.h"
#include "c_forsaken_player_cloak.h"

// Cloak Model
#define CLOAK_MODEL		"models/player/cloak.mdl"

// Internal Classes

// HL2 Class Macros

// Global Variable Decleration
static const char *g_czCloakBoneFollowers[] = 
{
	"l_shoulder",
	"neck_base",
	"r_shoulder"
};

static ConVar cl_fskn_ragdollcloak("cl_fskn_ragdollcloak", "1", FCVAR_ARCHIVE, 
	"Enables or disables client-side rendering of ragdoll cloaks.");

// Constructor & Deconstructor
C_ForsakenPlayer_Cloak::C_ForsakenPlayer_Cloak()
{
}

C_ForsakenPlayer_Cloak::~C_ForsakenPlayer_Cloak()
{
	PhysCleanupFrictionSounds(this);
}

// Functions
bool C_ForsakenPlayer_Cloak::CreateVPhysics()
/*
	Creates the VPhysics for the ragdoll cloak..
	Pre: None.
	Post: A physics object for the player cloak has been created.
*/
{
	return true;
}

void C_ForsakenPlayer_Cloak::CreateRagdoll(CBasePlayer *pParentPlayer)
/*
	Creates the ragdoll for the forsaken player's cloak.
	Pre: None.
	Post: A ragdoll for the player cloak has been created.
*/
{
	// We want to use the cloak model.
	SetModel(CLOAK_MODEL);
	SetMoveType(MOVETYPE_PUSH);
	SetSolid(SOLID_VPHYSICS);

	// We're a ragdoll.
	m_nRenderFX = kRenderFxRagdoll;
	m_hPlayer = pParentPlayer;

	// We need to become a ragdoll.
	BecomeRagdollOnClient(false);
}

void C_ForsakenPlayer_Cloak::ImpactTrace(trace_t *pTrace, int nDamageType, 
	char *czCustomImpactName)
/*
	Traces the impact of a force to the ragdoll.
	Pre: None.
	Post: A impact has been traced.
*/
{
	// Just pass the impact trace along to the player.
	ToForsakenPlayer(m_hPlayer)->ImpactTrace(pTrace, nDamageType, czCustomImpactName);
}