/*
	forsaken_weapon_base_melee.cpp
	Forsaken weapon code for all melee weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_gun.h"
#include "forsaken_fx_shared.h"
#include "in_buttons.h"

#ifndef CLIENT_DLL
#include "forsaken/te_firebullets.h"
#endif

// HL2 Class Macros

BEGIN_NETWORK_TABLE(CForsakenWeaponGunBase, DT_ForsakenWeaponGunBase)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeaponGunBase)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_gunbase, CForsakenWeaponGunBase);
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponGunBase, DT_ForsakenWeaponGunBase)

// Constructor & Deconstructor
CForsakenWeaponGunBase::CForsakenWeaponGunBase()
{
}

// Functions
bool CForsakenWeaponGunBase::Reload()
/*
	
	Pre:
	Post: 
*/
{
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer || pPlayer->IsSprinting())
		return false;

	// Do we have replacement ammo?
	if (pPlayer->GetAmmoCount(GetPrimaryAmmoType()) <= 0)
		return false;

	// Perform the reload.
	if (!DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD))
		return false;

	// Update the reload end time.
	m_fReloadEndTime = gpGlobals->curtime + GetViewModelSequenceDuration();

	// Update the player animations.
#ifdef GAME_DLL
	SendReloadEvents();
#endif

	return true;
}

void CForsakenWeaponGunBase::PrimaryAttack()
/*
	
	Pre:
	Post: 
*/
{
	CBroadcastRecipientFilter filter;
	CForsakenPlayer *pPlayer = GetPlayerOwner();
	const CForsakenWeaponInfo &rWeaponInf = GetForsakenWeaponData();
	float fSpread = VECTOR_CONE_2DEGREES.x;

	if (!pPlayer || !CanFire())
		return;

	// Are we empty?
	if (m_iClip1 <= 0)
	{
		// We're empty, so update the next attack time.
		if (m_bFireOnEmpty)
		{
			PlayEmptySound();

			m_flNextPrimaryAttack = gpGlobals->curtime + rWeaponInf.m_fCycleTime;

			return;
		}
	}

	// Get the spread depending on movement type.
	fSpread = GetBulletSpread(pPlayer->GetMovementType()).x;

	// Play the animations.
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	// If these are dual weapons, check to see which gun should be firing.
	if (!IsDualie() || (IsDualie() && (pPlayer->m_nShotsFired & 0x1)))
		SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	else
		SendWeaponAnim(ACT_VM_SECONDARYATTACK);

	// Decrease the clip size.
	m_iClip1--;

	// Fire the bullet.
	FX_FireBullets(pPlayer->entindex(), pPlayer->Weapon_ShootPosition(), 
		pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), GetWeaponID(), 
		FORSAKEN_WEAPON_PRIMARY_MODE, CBaseEntity::GetPredictionRandomSeed() & 255, fSpread);

	// Muzzle flash effect.
	pPlayer->DoMuzzleFlash();

	// Update the punch angle (recoil).
	PerformRecoil();

	// Disable primary attack.
	m_bAttackDisabled = true;

	// Update shot count.
	pPlayer->m_nShotsFired++;

	// Update the next attack and weapon idle times.
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + rWeaponInf.m_fCycleTime;
	SetWeaponIdleTime(gpGlobals->curtime + (rWeaponInf.m_fCycleTime * 5));
}

void CForsakenWeaponGunBase::SecondaryAttack()
/*
	
	Pre:
	Post: 
*/
{
	Activity hitActivity;
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	hitActivity = PerformMeleeAttack(25.0f, 50.0f, DMG_CLUB, true);

	pPlayer->SetAnimation(PLAYER_MELEE_ATTACK1);
	pPlayer->DoAnimationEvent(FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_PRIMARY);
	SendWeaponAnim(hitActivity);

	switch (hitActivity)
	{
	case ACT_VM_MELEE_SLASH:
		WeaponSound(WPN_DOUBLE);
		break;

	case ACT_VM_MELEE_MISS:
		WeaponSound(SPECIAL1);
		break;
	}

	pPlayer->m_nShotsFired += 3;

	m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = 
		gpGlobals->curtime + GetViewModelSequenceDuration();
}

void CForsakenWeaponGunBase::WeaponIdle()
/*
	
	Pre:
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());
	const CForsakenWeaponInfo &rWeaponInf = GetForsakenWeaponData();

	// Should we be idling?
	if (GetWeaponIdleTime() > gpGlobals->curtime)
		return;

	if (m_iClip1 > 0)
	{
		// Update the weapon anim and weapon idle time.
		SendWeaponAnim(ACT_VM_IDLE);
		SetWeaponIdleTime(gpGlobals->curtime + (rWeaponInf.m_fCycleTime * 5));
	}

	// Decrease the shots fired count.
	if (pPlayer && pPlayer->m_nShotsFired > 3)
		pPlayer->m_nShotsFired -= 3;
	else if (pPlayer && pPlayer->m_nShotsFired > 0)
		pPlayer->m_nShotsFired = 0;
}