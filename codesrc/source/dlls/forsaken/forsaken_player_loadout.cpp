/*
forsaken_player.cpp
Server-side forsaken player.

Copyright© 2005 Forsaken Mod Team.
All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_player.h"
#include "forsaken_fortified_entity.h"
#include "forsaken_team.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken/forsaken_weapon_base.h"
#include "forsaken/forsaken_objective_entity.h"
#include "predicted_viewmodel.h"
#include "iservervehicle.h"
#include "viewport_panel_names.h"

// Function Prototypes
void CC_Forsaken_RemoveNonKilldrop(CForsakenPlayer *pPlayer);

// Ammo
void CC_Forsaken_Ammo_12Gauge();
void CC_Forsaken_Ammo_223();
void CC_Forsaken_Ammo_300();
void CC_Forsaken_Ammo_44Cal();
void CC_Forsaken_Ammo_45CalACP();
void CC_Forsaken_Ammo_45CalColt();
void CC_Forsaken_Ammo_9mm();
void CC_Forsaken_Ammo_RPG();

// Resources
void CC_Forsaken_Resource_1201FP();
void CC_Forsaken_Resource_Frag();
void CC_Forsaken_Resource_Molotov();
void CC_Forsaken_Resource_Stun();

// Civ
void CC_Forsaken_WeaponKit_Civ_710Ammo();
void CC_Forsaken_WeaponKit_Civ_710HKMK23();
void CC_Forsaken_WeaponKit_Civ_710Molotov();
void CC_Forsaken_WeaponKit_Civ_HKMK23Ammo();
void CC_Forsaken_WeaponKit_Civ_HKMK23Frag();
void CC_Forsaken_WeaponKit_Civ_HKMK23RagingBull();
void CC_Forsaken_WeaponKit_Civ_RagingBullAmmo();
void CC_Forsaken_WeaponKit_Civ_RagingBullHKMK23();
void CC_Forsaken_WeaponKit_Civ_RagingBullFrag();

void CC_Forsaken_WeaponKit_Civ_710RagingBullLdr();
void CC_Forsaken_WeaponKit_Civ_RPGRagingBullLdr();
void CC_Forsaken_WeaponKit_Civ_RPGHKMK23Ldr();

// Gov
void CC_Forsaken_WeaponKit_Gov_1201FPAmmo();
void CC_Forsaken_WeaponKit_Gov_1201FPFrag();
void CC_Forsaken_WeaponKit_Gov_1201FPHKMK23();
void CC_Forsaken_WeaponKit_Gov_HKMK23Ammo();
void CC_Forsaken_WeaponKit_Gov_HKMK231201FP();
void CC_Forsaken_WeaponKit_Gov_HKMK23Frag();
void CC_Forsaken_WeaponKit_Gov_SA80Ammo();
void CC_Forsaken_WeaponKit_Gov_SA80HKMK23();
void CC_Forsaken_WeaponKit_Gov_SA80Frag();

void CC_Forsaken_WeaponKit_Gov_1201FPHKMK23Ldr();
void CC_Forsaken_WeaponKit_Gov_1201FPFragsLdr();
void CC_Forsaken_WeaponKit_Gov_SA80FragsLdr();

// Rel

// Global Variable Declerations
// Ammo
static ConCommand cc_weaponammo_12gauge("weapon_resource_ammo_12gauge", 
	CC_Forsaken_Ammo_12Gauge, "Loads the player out with additional 12 Gauge ammo.");
static ConCommand cc_weaponammo_223("weapon_resource_ammo_223", 
	CC_Forsaken_Ammo_223, "Loads the player out with additional .223 ammo.");
static ConCommand cc_weaponammo_300("weapon_resource_ammo_300", 
	CC_Forsaken_Ammo_300, "Loads the player out with additional .300 ammo.");
static ConCommand cc_weaponammo_44cal("weapon_resource_ammo_44cal", 
	CC_Forsaken_Ammo_44Cal, "Loads the player out with additional 44 caliber ammo.");
static ConCommand cc_weaponammo_45calacp("weapon_resource_ammo_45calacp", 
	CC_Forsaken_Ammo_45CalACP, "Loads the player out with additional 45 caliber ACP ammo.");
static ConCommand cc_weaponammo_45calcolt("weapon_resource_ammo_45calcolt", 
	CC_Forsaken_Ammo_45CalColt, "Loads the player out with additional 45 caliber colt ammo.");
static ConCommand cc_weaponammo_9mm("weapon_resource_ammo_9mm", 
	CC_Forsaken_Ammo_9mm, "Loads the player out with additional 9mm ammo.");
static ConCommand cc_weaponammo_rpg("weapon_resource_ammo_rpg",
	CC_Forsaken_Ammo_RPG, "Loads the player out with additional RPG ammo.");


// Resources
static ConCommand cc_weaponresource_1201fp("weapon_resource_1201fp", 
	CC_Forsaken_Resource_1201FP, "Loads the player out with a 1201FP.");
static ConCommand cc_weaponresource_frag("weapon_resource_frag", 
	CC_Forsaken_Resource_Frag, "Loads the player out with an additional grenade.");
static ConCommand cc_weaponresource_molotov("weapon_resource_molotov", 
	CC_Forsaken_Resource_Molotov, "Loads the player out with an additional molotov cocktail.");
static ConCommand cc_weaponresource_stun("weapon_resource_stun", 
	CC_Forsaken_Resource_Stun, "Loads the player out with an additional stun grenade.");

// Civ
static ConCommand cc_weaponloadout_civ_710ammo("weapon_kit_civ_710ammo",
	CC_Forsaken_WeaponKit_Civ_710Ammo, "Loads the player out with the 710 Remington.");
static ConCommand cc_weaponloadout_civ_710hkmk23("weapon_kit_civ_710hkmk23",
	CC_Forsaken_WeaponKit_Civ_710HKMK23, "Loads the player out with the 710 Remington and H&K MK23.");
static ConCommand cc_weaponloadout_civ_710molotov("weapon_kit_civ_710molotov",
	CC_Forsaken_WeaponKit_Civ_710Molotov, "Loads the player out with the 710 Remington and molotov cocktail.");
static ConCommand cc_weaponloadout_civ_hkmk23ammo("weapon_kit_civ_hkmk23ammo",
	CC_Forsaken_WeaponKit_Civ_HKMK23Ammo, "Loads the player out with the H&K MK23.");
static ConCommand cc_weaponloadout_civ_hkmk23frag("weapon_kit_civ_hkmk23frag",
	CC_Forsaken_WeaponKit_Civ_HKMK23Frag, "Loads the player out with the H&K MK23 and a frag grenade.");
static ConCommand cc_weaponloadout_civ_hkmk23ragingbull("weapon_kit_civ_hkmk23ragingbull",
	CC_Forsaken_WeaponKit_Civ_HKMK23RagingBull, "Loads the player out with the H&K MK23 and a Raging Bull.");
static ConCommand cc_weaponloadout_civ_ragingbullammo("weapon_kit_civ_ragingbullammo",
	CC_Forsaken_WeaponKit_Civ_RagingBullAmmo, "Loads the player out with the Raging Bull.");
static ConCommand cc_weaponloadout_civ_ragingbullfrag("weapon_kit_civ_ragingbullfrag",
	CC_Forsaken_WeaponKit_Civ_RagingBullFrag, "Loads the player out with the Raging Bull and a frag grenade.");
static ConCommand cc_weaponloadout_civ_ragingbullhkmk23("weapon_kit_civ_ragingbullhkmk23",
	CC_Forsaken_WeaponKit_Civ_RagingBullHKMK23, "Loads the player out with the Raging Bull and a H&K MK23.");

static ConCommand cc_weaponloadout_civ_710ragingbull_ldr("weapon_kit_civ_710ragingbull_ldr",
	CC_Forsaken_WeaponKit_Civ_710RagingBullLdr, "Loads the player out with the 710 Remington and Raging Bull.");
static ConCommand cc_weaponloadout_civ_rpghkmk23_ldr("weapon_kit_civ_rpghkmk23_ldr", 
	CC_Forsaken_WeaponKit_Civ_RPGHKMK23Ldr, "Loads the player out with the RPG and H&K MK23.");
static ConCommand cc_weaponloadout_civ_rpgragingbull_ldr("weapon_kit_civ_rpgragingbull_ldr", 
	CC_Forsaken_WeaponKit_Civ_RPGRagingBullLdr, "Loads the player out with the RPG and Raging Bull.");

// Gov
static ConCommand cc_weaponloadout_gov_1201fpammo("weapon_kit_gov_1201fpammo", 
	CC_Forsaken_WeaponKit_Gov_1201FPAmmo, "Loads the player out with the 1201FP.");
static ConCommand cc_weaponloadout_gov_1201fphkmk23("weapon_kit_gov_1201fphkmk23", 
	CC_Forsaken_WeaponKit_Gov_1201FPHKMK23, "Loads the player out with the 1201FP and H&K MK23.");
static ConCommand cc_weaponloadout_gov_1201fpfrag("weapon_kit_gov_1201fpfrag", 
	CC_Forsaken_WeaponKit_Gov_1201FPFrag, "Loads the player out with the 1201FP and a frag grenade.");
static ConCommand cc_weaponloadout_gov_hkmk23ammo("weapon_kit_gov_hkmk23ammo", 
	CC_Forsaken_WeaponKit_Gov_HKMK23Ammo, "Loads the player out with the H&K MK23.");
static ConCommand cc_weaponloadout_gov_hkmk231201fp("weapon_kit_gov_hkmk231201fp", 
	CC_Forsaken_WeaponKit_Gov_HKMK231201FP, "Loads the player out with the H&K MK23 and a 1201FP.");
static ConCommand cc_weaponloadout_gov_hkmk23frag("weapon_kit_gov_hkmk23frag", 
	CC_Forsaken_WeaponKit_Gov_HKMK23Frag, "Loads the player out with the H&K MK23 and a frag grenade.");
static ConCommand cc_weaponloadout_gov_sa80ammo("weapon_kit_gov_sa80ammo", 
	CC_Forsaken_WeaponKit_Gov_SA80Ammo, "Loads the player out with the SA80.");
static ConCommand cc_weaponloadout_gov_sa80hkmk23("weapon_kit_gov_sa80hkmk23", 
	CC_Forsaken_WeaponKit_Gov_SA80HKMK23, "Loads the player out with the SA80 and H&K MK23.");
static ConCommand cc_weaponloadout_gov_sa80frag("weapon_kit_gov_sa80frag", 
	CC_Forsaken_WeaponKit_Gov_SA80Frag, "Loads the player out with the SA80 and a frag grenade.");

static ConCommand cc_weaponloadout_gov_1201fpfrags_ldr("weapon_kit_gov_1201fpfrags_ldr", 
	CC_Forsaken_WeaponKit_Gov_1201FPFragsLdr, "Loads the player out with the 1201FP and two frag grenades.");
static ConCommand cc_weaponloadout_gov_1201fphkmk23_ldr("weapon_kit_gov_1201fphkmk23_ldr", 
	CC_Forsaken_WeaponKit_Gov_1201FPHKMK23Ldr, "Loads the player out with the 1201FP and H&K MK23.");
static ConCommand cc_weaponloadout_gov_sa80frags_ldr("weapon_kit_gov_sa80frags_ldr", 
	CC_Forsaken_WeaponKit_Gov_SA80FragsLdr, "Loads the player out with the SA80 and two frag grenades.");

// Rel

// Functions
void CC_Forsaken_RemoveNonKilldrop(CForsakenPlayer *pPlayer)
/*
	
	Pre: 
	Post: 
*/
{
	if (pPlayer && pPlayer->IsAlive())
	{
		pPlayer->ClearActiveWeapon();

		// Loop through all player weapons.
		for (int i = 0; i < pPlayer->WeaponCount(); i++)
		{
			CForsakenWeaponBase *pForsakenWeapon = dynamic_cast<CForsakenWeaponBase*>(pPlayer->GetWeapon(i));

			// If it's a valid weapon and it wasn't a kill drop weapon, remove it.
			if (pForsakenWeapon && !pForsakenWeapon->m_bKillDrop)
				pPlayer->RemoveWeapon(pForsakenWeapon);
		}

		// Switch the player to the next best weapon.
		pPlayer->SwitchToNextBestWeapon(NULL);
	}
}

// Ammo
void CC_Forsaken_Ammo_12Gauge()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 10, AMMO_12GAUGE);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Ammo_223()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 30, AMMO_223);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Ammo_300()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 4, AMMO_300);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Ammo_44Cal()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout()
		&& pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 5, AMMO_44CAL);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Ammo_45CalACP()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 12, AMMO_45CAL_ACP);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Ammo_45CalColt()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 5, AMMO_45CAL_COLT);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Ammo_9mm()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 12, AMMO_9MM);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Ammo_RPG()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (15 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give the player the ammo they want.
		pPlayer->GiveAmmo(nCount * 12, AMMO_RPGMISSILE);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(15 * nCount));
		pTeam->IncreaseOrdinanceResources(-(15 * nCount));
	}
}

// Resources
void CC_Forsaken_Resource_1201FP()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > 15)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// Give them the 1201FP if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_1201fp"))
		{
			pPlayer->GiveNamedItem("forsaken_weapon_1201fp");

			pPlayer->SetAmmo(15, AMMO_12GAUGE);
		}

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-15);
		pTeam->IncreaseOrdinanceResources(-15);
	}
}

void CC_Forsaken_Resource_Frag()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// If they own the weapon already
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag", 0))
		{
			pPlayer->GiveNamedItem("forsaken_grenade_frag");

			// Decrease the sahred resource count.
			pPlayer->IncrementResources(-5);
			pTeam->IncreaseOrdinanceResources(-5);

			nCount--;
		}

		// Give them any additional amounts they want.
		if (nCount > 0)
			pPlayer->GiveAmmo(nCount, AMMO_GRENADE);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Resource_Molotov()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// If they own the weapon already
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_molotov", 0))
		{
			pPlayer->GiveNamedItem("forsaken_grenade_molotov");

			// Decrease the sahred resource count.
			pPlayer->IncrementResources(-5);
			pTeam->IncreaseOrdinanceResources(-5);

			nCount--;
		}

		// Give them any additional amounts they want.
		if (nCount > 0)
			pPlayer->GiveAmmo(nCount, AMMO_MOLOTOV);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

void CC_Forsaken_Resource_Stun()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CForsakenTeam *pTeam = NULL;

	// Do we have enough commands?
	if (engine->Cmd_Argc() != 2)
		return;

	// Fetch the item count.
	int nCount = atoi(engine->Cmd_Argv(1));

	// Is the player alive and do they have enough weapon resources?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetSharedResources() > (5 * nCount) && nCount > 0)
	{
		pTeam = dynamic_cast<CForsakenTeam*>(pPlayer->GetTeam());

		// If they own the weapon already
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_m84stun", 0))
		{
			pPlayer->GiveNamedItem("forsaken_grenade_m84stun");

			// Decrease the sahred resource count.
			pPlayer->IncrementResources(-5);
			pTeam->IncreaseOrdinanceResources(-5);

			nCount--;
		}

		// Give them any additional amounts they want.
		if (nCount > 0)
			pPlayer->GiveAmmo(nCount, AMMO_STUN);

		// Decrease the shared resource count.
		pPlayer->IncrementResources(-(5 * nCount));
		pTeam->IncreaseOrdinanceResources(-(5 * nCount));
	}
}

// Civ
void CC_Forsaken_WeaponKit_Civ_710Ammo()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the 710 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_710"))
			pPlayer->GiveNamedItem("forsaken_weapon_710");

		// Fix: Need differing ammo types.
		pPlayer->SetAmmo(12, AMMO_300);
	}
}

void CC_Forsaken_WeaponKit_Civ_710HKMK23()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the 710 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_710"))
			pPlayer->GiveNamedItem("forsaken_weapon_710");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		// Fix: Need differing ammo types.
		pPlayer->SetAmmo(4, AMMO_300);
		pPlayer->SetAmmo(12, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Civ_710Molotov()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the 710 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_710"))
			pPlayer->GiveNamedItem("forsaken_weapon_710");

		// Give them the molotov cocktail.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_molotov"))
			pPlayer->GiveNamedItem("forsaken_grenade_molotov");
		else
			pPlayer->GiveAmmo(1, AMMO_MOLOTOV);

		// Fix: Need differing ammo types.
		pPlayer->SetAmmo(8, AMMO_300);
		pPlayer->SetAmmo(12, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Civ_710RagingBullLdr()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Is the player a leader?
		if (!pPlayer->IsLeader())
			return;

		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the 710 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_710"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_710"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		// Give them the raging bull if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_ragingbull"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_ragingbull"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		pPlayer->SetAmmo(10, AMMO_45CAL_ACP);
		pPlayer->SetAmmo(15, AMMO_45CAL_COLT);
	}
}

void CC_Forsaken_WeaponKit_Civ_HKMK23Ammo()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		pPlayer->SetAmmo(48, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Civ_HKMK23Frag()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		// Give them the frag grenade if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag"))
			pPlayer->GiveNamedItem("forsaken_grenade_frag");
		else
			pPlayer->GiveAmmo(1, AMMO_GRENADE);

		pPlayer->SetAmmo(24, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Civ_HKMK23RagingBull()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		// Give them the Raging Bull if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_ragingbull"))
			pPlayer->GiveNamedItem("forsaken_weapon_ragingbull");

		pPlayer->SetAmmo(24, AMMO_45CAL_ACP);
		pPlayer->SetAmmo(5, AMMO_45CAL_COLT);
	}
}

void CC_Forsaken_WeaponKit_Civ_RagingBullAmmo()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the raging bull if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_ragingbull"))
			pPlayer->GiveNamedItem("forsaken_weapon_ragingbull");

		pPlayer->SetAmmo(15, AMMO_45CAL_COLT);
	}
}

void CC_Forsaken_WeaponKit_Civ_RagingBullFrag()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the raging bull if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_ragingbull"))
			pPlayer->GiveNamedItem("forsaken_weapon_ragingbull");

		// Give them the frag grenade if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag"))
			pPlayer->GiveNamedItem("forsaken_grenade_frag");
		else
			pPlayer->GiveAmmo(1, AMMO_GRENADE);

		pPlayer->SetAmmo(5, AMMO_45CAL_COLT);
	}
}

void CC_Forsaken_WeaponKit_Civ_RagingBullHKMK23()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		// Give them the Raging Bull if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_ragingbull"))
			pPlayer->GiveNamedItem("forsaken_weapon_ragingbull");

		pPlayer->SetAmmo(12, AMMO_45CAL_ACP);
		pPlayer->SetAmmo(10, AMMO_45CAL_COLT);
	}
}

void CC_Forsaken_WeaponKit_Civ_RPGHKMK23Ldr()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Is the player a leader?
		if (!pPlayer->IsLeader())
			return;

		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the RPG if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_rpg"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_rpg"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_hk23"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		pPlayer->SetAmmo(24, AMMO_45CAL_ACP);
		pPlayer->SetAmmo(2, AMMO_RPGMISSILE);
	}
}

void CC_Forsaken_WeaponKit_Civ_RPGRagingBullLdr()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
	{
		// Is the player a leader?
		if (!pPlayer->IsLeader())
			return;

		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the chisel if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_chisel"))
			pPlayer->GiveNamedItem("forsaken_melee_chisel");

		// Give them the RPG if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_rpg"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_rpg"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		// Give them the raging bull if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_ragingbull"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_ragingbull"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		pPlayer->SetAmmo(15, AMMO_45CAL_COLT);
		pPlayer->SetAmmo(1, AMMO_RPGMISSILE);
	}
}

// Gov
void CC_Forsaken_WeaponKit_Gov_1201FPAmmo()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the 1201FP if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_1201fp"))
			pPlayer->GiveNamedItem("forsaken_weapon_1201fp");

		pPlayer->SetAmmo(15, AMMO_12GAUGE);
	}
}

void CC_Forsaken_WeaponKit_Gov_1201FPFrag()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the grenade if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag"))
			pPlayer->GiveNamedItem("forsaken_grenade_frag");
		else
			pPlayer->GiveAmmo(1, AMMO_GRENADE);

		// Give them the 1201FP if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_1201fp"))
			pPlayer->GiveNamedItem("forsaken_weapon_1201fp");

		pPlayer->SetAmmo(5, AMMO_12GAUGE);
	}
}

void CC_Forsaken_WeaponKit_Gov_1201FPHKMK23()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the 1201FP if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_1201fp"))
			pPlayer->GiveNamedItem("forsaken_weapon_1201fp");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		pPlayer->SetAmmo(10, AMMO_12GAUGE);
		pPlayer->SetAmmo(12, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Gov_HKMK23Ammo()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		pPlayer->SetAmmo(48, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Gov_HKMK231201FP()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_1201fp"))
			pPlayer->GiveNamedItem("forsaken_weapon_1201fp");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		pPlayer->SetAmmo(5, AMMO_12GAUGE);
		pPlayer->SetAmmo(24, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Gov_HKMK23Frag()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag"))
			pPlayer->GiveNamedItem("forsaken_grenade_frag");
		else
			pPlayer->GiveAmmo(1, AMMO_GRENADE);

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		pPlayer->SetAmmo(24, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Gov_SA80Ammo()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the SA80 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_sa80"))
			pPlayer->GiveNamedItem("forsaken_weapon_sa80");

		pPlayer->SetAmmo(60, AMMO_223);
	}
}

void CC_Forsaken_WeaponKit_Gov_SA80Frag()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them a frag grenade if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag"))
			pPlayer->GiveNamedItem("forsaken_grenade_frag");
		else
			pPlayer->GiveAmmo(1, AMMO_GRENADE);

		// Give them the SA80 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_sa80"))
			pPlayer->GiveNamedItem("forsaken_weapon_sa80");

		pPlayer->SetAmmo(30, AMMO_223);
	}
}

void CC_Forsaken_WeaponKit_Gov_SA80HKMK23()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the SA80 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_sa80"))
			pPlayer->GiveNamedItem("forsaken_weapon_sa80");

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
			pPlayer->GiveNamedItem("forsaken_weapon_hk23");

		pPlayer->SetAmmo(30, AMMO_223);
		pPlayer->SetAmmo(12, AMMO_45CAL_ACP);
	}
}

void CC_Forsaken_WeaponKit_Gov_1201FPFragsLdr()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Is the player a leader?
		if (!pPlayer->IsLeader())
			return;

		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the 1201FP if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_1201fp"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_1201fp"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		// Give them the frag grenade if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_grenade_frag"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);

			pPlayer->SetAmmo(2, AMMO_GRENADE);
		}
		else
			pPlayer->GiveAmmo(2, AMMO_GRENADE);

		pPlayer->GiveAmmo(15, AMMO_12GAUGE);
	}
}

void CC_Forsaken_WeaponKit_Gov_1201FPHKMK23Ldr()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Is the player a leader?
		if (!pPlayer->IsLeader())
			return;

		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the 1201FP if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_1201fp"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_1201fp"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		// Give them the H&K MK23 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_hk23"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_hk23"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		pPlayer->SetAmmo(36, AMMO_45CAL_ACP);
		pPlayer->SetAmmo(15, AMMO_12GAUGE);
	}
}

void CC_Forsaken_WeaponKit_Gov_SA80FragsLdr()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());

	// Reset the player zoom.
	if (pPlayer)
		pPlayer->SetFOV(pPlayer, 0);

	// Is the player alive and on the civilians?
	if (pPlayer && pPlayer->IsAlive() && pPlayer->CanLoadout() && 
		pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
	{
		// Is the player a leader?
		if (!pPlayer->IsLeader())
			return;

		// Remove any weapons the player has that weren't a kill-drop.
		CC_Forsaken_RemoveNonKilldrop(pPlayer);

		// Give them the knife if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_melee_govknife"))
			pPlayer->GiveNamedItem("forsaken_melee_govknife");

		// Give them the SA80 if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_weapon_sa80"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_weapon_sa80"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}

		// Give them the frag grenade if they don't have it.
		if (!pPlayer->Weapon_OwnsThisType("forsaken_grenade_frag"))
		{
			CForsakenWeaponBase *pWeapon = 
				dynamic_cast<CForsakenWeaponBase*>(pPlayer->GiveNamedItem("forsaken_grenade_frag"));

			if (pWeapon)
				pWeapon->SetLeaderWeapon(true);
		}
		else
			pPlayer->GiveAmmo(1, AMMO_GRENADE);

		pPlayer->SetAmmo(120, AMMO_223);
	}
}