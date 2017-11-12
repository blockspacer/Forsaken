/*
	forsaken_melee_chisel.cpp
	Forsaken weapon code for the chisel.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_melee.h"

#ifdef CLIENT_DLL
#define CForsakenMeleeChisel C_ForsakenMeleeChisel
#endif

// Internal Classes
class CForsakenMeleeChisel : public CForsakenWeaponBase
{
	DECLARE_CLASS(CForsakenMeleeChisel, CForsakenWeaponBase);
	DECLARE_PREDICTABLE();
	DECLARE_NETWORKCLASS();

	// Constructor & Deconstructor
	CForsakenMeleeChisel();

	// Public Functions
	virtual bool Reload();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
};

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenMeleeChisel, DT_ForsakenMeleeChisel)

BEGIN_NETWORK_TABLE(CForsakenMeleeChisel, DT_ForsakenMeleeChisel)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenMeleeChisel)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_melee_chisel, CForsakenMeleeChisel);
PRECACHE_WEAPON_REGISTER(forsaken_melee_chisel);

// Constructor & Deconstructor
CForsakenMeleeChisel::CForsakenMeleeChisel()
{
	m_bFiresUnderwater = true;
	m_bMeleeHit = false;
	m_fReloadEndTime = 0.0f;
}

// Functions
bool CForsakenMeleeChisel::Reload()
/*
	
	Pre:
	Post: 
*/
{
	return false;
}

void CForsakenMeleeChisel::PrimaryAttack()
/*
	
	Pre:
	Post: 
*/
{
	Activity hitActivity;
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	hitActivity = PerformMeleeAttack(25.0f, 50.0f, DMG_SLASH, true);

	GetPlayerOwner()->SetAnimation(PLAYER_MELEE_ATTACK1);
	GetPlayerOwner()->DoAnimationEvent(FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_PRIMARY);
	SendWeaponAnim(hitActivity);

	m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = 
		gpGlobals->curtime + GetViewModelSequenceDuration();

	switch (hitActivity)
	{
	case ACT_VM_MELEE_SLASH:
		WeaponSound(SINGLE);
		break;

	case ACT_VM_MELEE_STAB:
		WeaponSound(WPN_DOUBLE);
		break;

	case ACT_VM_MELEE_MISS:
		WeaponSound(SPECIAL1);
		break;

	case ACT_VM_MELEE_STAB_MISS:
		WeaponSound(SPECIAL2);
		break;
	}
}

void CForsakenMeleeChisel::SecondaryAttack()
/*
	
	Pre:
	Post: 
*/
{
	Activity hitActivity;
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	hitActivity = PerformMeleeAttack(50.0f, 50.0f, DMG_CLUB, false);

	GetPlayerOwner()->SetAnimation(PLAYER_MELEE_ATTACK2);
	GetPlayerOwner()->DoAnimationEvent(FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_SECONDARY);
	SendWeaponAnim(hitActivity);

	switch (hitActivity)
	{
	case ACT_VM_MELEE_SLASH:
		WeaponSound(SINGLE);
		break;

	case ACT_VM_MELEE_STAB:
		WeaponSound(WPN_DOUBLE);
		break;

	case ACT_VM_MELEE_MISS:
		WeaponSound(SPECIAL1);
		break;

	case ACT_VM_MELEE_STAB_MISS:
		WeaponSound(SPECIAL2);
		break;
	}

	m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = 
		gpGlobals->curtime + GetViewModelSequenceDuration();
}