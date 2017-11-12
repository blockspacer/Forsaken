/*
	forsaken_melee_relknife.cpp
	Forsaken weapon code for the Governor's knife.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_melee.h"

#ifdef CLIENT_DLL
#define CForsakenMeleeRelKnife C_ForsakenMeleeRelKnife
#endif

// Internal Classes
class CForsakenMeleeRelKnife : public CForsakenWeaponBase
{
	DECLARE_CLASS(CForsakenMeleeRelKnife, CForsakenWeaponBase);
	DECLARE_PREDICTABLE();
	DECLARE_NETWORKCLASS();

	// Constructor & Deconstructor
	CForsakenMeleeRelKnife();

	// Public Functions
	virtual bool Reload();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
};

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenMeleeRelKnife, DT_ForsakenMeleeRelKnife)

BEGIN_NETWORK_TABLE(CForsakenMeleeRelKnife, DT_ForsakenMeleeRelKnife)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenMeleeRelKnife)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_melee_relknife, CForsakenMeleeRelKnife);
PRECACHE_WEAPON_REGISTER(forsaken_melee_relknife);

// Constructor & Deconstructor
CForsakenMeleeRelKnife::CForsakenMeleeRelKnife()
{
	m_bFiresUnderwater = true;
	m_bMeleeHit = false;
	m_fReloadEndTime = 0.0f;
}

// Functions
bool CForsakenMeleeRelKnife::Reload()
/*
	
	Pre:
	Post: 
*/
{
	return false;
}

void CForsakenMeleeRelKnife::PrimaryAttack()
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

void CForsakenMeleeRelKnife::SecondaryAttack()
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