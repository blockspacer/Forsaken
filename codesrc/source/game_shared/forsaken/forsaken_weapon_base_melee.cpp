/*
	forsaken_weapon_base_melee.cpp
	Forsaken weapon code for all melee weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_melee.h"
#include "engine/IEngineSound.h"
#include "takedamageinfo.h"
#include "in_buttons.h"

#define BLUDGEON_HULL_DIM		16

static const Vector g_bludgeonMins(-BLUDGEON_HULL_DIM,-BLUDGEON_HULL_DIM,-BLUDGEON_HULL_DIM);
static const Vector g_bludgeonMaxs(BLUDGEON_HULL_DIM,BLUDGEON_HULL_DIM,BLUDGEON_HULL_DIM);

// HL2 Class Macros
BEGIN_NETWORK_TABLE(CForsakenWeaponMeleeBase, DT_ForsakenWeaponMeleeBase)
#ifndef CLIENT_DLL/*
	SendPropExclude("DT_AnimTimeMustBeFirst", "m_flAnimTime"),
	SendPropExclude("DT_BaseAnimating", "m_nSequence"),*/
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeaponMeleeBase)
END_PREDICTION_DATA()

#ifndef CLIENT_DLL
BEGIN_DATADESC(CForsakenWeaponMeleeBase)
END_DATADESC()
#endif

IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponMeleeBase, DT_ForsakenWeaponMeleeBase)
LINK_ENTITY_TO_CLASS(forsaken_weapon_base_melee, CForsakenWeaponMeleeBase);

// Constructor & Deconstructor
CForsakenWeaponMeleeBase::CForsakenWeaponMeleeBase()
{
	m_bFiresUnderwater = true;
	m_bHit = false;
	m_fReloadEndTime = 0.0f;
}

// Functions
void CForsakenWeaponMeleeBase::ItemPostFrame()
/*
	
	Pre: 
	Post: 
*/
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	m_bHit = false;

	if (pPlayer->m_nButtons & IN_ATTACK && m_flNextPrimaryAttack <= gpGlobals->curtime)
		PrimaryAttack();
	else if (pPlayer->m_nButtons & IN_ATTACK2 && m_flNextSecondaryAttack <= gpGlobals->curtime)
		SecondaryAttack();
	else if (m_flTimeWeaponIdle < gpGlobals->curtime)
		WeaponIdle();
}