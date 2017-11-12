/*
	forsaken_weapon_base_melee.cpp
	Forsaken weapon code for all melee weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_gun.h"
#include "forsaken_weapon_base_shotgun.h"
#include "forsaken_fx_shared.h"
#include "in_buttons.h"

#ifndef CLIENT_DLL
#include "forsaken/te_firebullets.h"
#endif

// HL2 Class Macros

BEGIN_NETWORK_TABLE(CForsakenWeaponShotgunBase, DT_ForsakenWeaponShotgunBase)
#ifdef CLIENT_DLL
	RecvPropInt(RECVINFO(m_nInSpecialReload)),
#else
	SendPropInt(SENDINFO(m_nInSpecialReload), 2, SPROP_UNSIGNED),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeaponShotgunBase)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_shotgunbase, CForsakenWeaponShotgunBase);
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponShotgunBase, DT_ForsakenWeaponShotgunBase)

// Constructor & Deconstructor
CForsakenWeaponShotgunBase::CForsakenWeaponShotgunBase()
{
	m_nInSpecialReload = 0;
}

// Functions
bool CForsakenWeaponShotgunBase::Reload()
/*
	
	Pre:
	Post: 
*/
{
	CForsakenPlayer *pPlayer = GetPlayerOwner();
	const CForsakenWeaponInfo &rWeaponInf = GetForsakenWeaponData();

	if (!pPlayer || pPlayer->IsSprinting())
		return false;

	// Do we have any ammo to reload or is our clip already full?
	if (pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0 || m_iClip1 == GetMaxClip1())
		return false;

	// If we are still in recoil, wait on the reload.
	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return false;

	if (m_nInSpecialReload == 0)
	{
		// Update the animations and set the next weapon idle time.
		pPlayer->SetAnimation(PLAYER_RELOAD);
		SendWeaponAnim(ACT_SHOTGUN_RELOAD_START);
		SetWeaponIdleTime(gpGlobals->curtime + rWeaponInf.m_fCycleTime);

		// Update the new attack times and move to the next reload stage.
		pPlayer->m_flNextAttack = m_flNextPrimaryAttack = m_flNextSecondaryAttack = 
			GetWeaponIdleTime();
		m_fReloadEndTime = GetWeaponIdleTime();
		m_nInSpecialReload = 1;
	}
	else if (m_nInSpecialReload == 1)
	{
		// Wait for it...
		if (GetWeaponIdleTime() > gpGlobals->curtime)
			return true;

		// Move to the next stage of reload.
		m_nInSpecialReload = 2;

		// Update the animations and set the next weapon idle time.
		SendWeaponAnim(ACT_VM_RELOAD);
		SetWeaponIdleTime(gpGlobals->curtime + GetViewModelSequenceDuration());

		pPlayer->m_flNextAttack = m_flNextPrimaryAttack = m_flNextSecondaryAttack = 
			GetWeaponIdleTime();
		m_fReloadEndTime = GetWeaponIdleTime();

		// Let all clients know we are reloading.
#ifdef GAME_DLL
		SendReloadEvents();
#endif
	}
	else
	{
		// Add ammo to the clip and set the reload stage.
		m_iClip1++;
		m_nInSpecialReload = 1;

		// Decrease the total ammo count.
		pPlayer->RemoveAmmo(1, m_iPrimaryAmmoType);
	}

	return true;
}

void CForsakenWeaponShotgunBase::PrimaryAttack()
/*
	
	Pre:
	Post: 
*/
{
	const CForsakenWeaponInfo &rWeaponInf = GetForsakenWeaponData();

	BaseClass::PrimaryAttack();

	if (m_iClip1 > 0)
		SetWeaponIdleTime(gpGlobals->curtime + (rWeaponInf.m_fCycleTime * 5.0f));
	else
		SetWeaponIdleTime(gpGlobals->curtime + rWeaponInf.m_fCycleTime);
}

void CForsakenWeaponShotgunBase::WeaponIdle()
/*
	
	Pre:
	Post: 
*/
{
	CForsakenPlayer *pPlayer = GetPlayerOwner();
	const CForsakenWeaponInfo &rWeaponInf = GetForsakenWeaponData();

	if (GetWeaponIdleTime() <= gpGlobals->curtime)
	{
		// Are we out of chambered ammunition, have remaining ammunition, and reloading?
		if (m_iClip1 == 0 && m_nInSpecialReload == 0 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType))
			Reload();
		// We're in the process of reloading.
		else if (m_nInSpecialReload > 0)
		{
			// We aren't at a full-clip yet and we still have ammo.
			if (m_iClip1 < rWeaponInf.iMaxClip1 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) > 0)
				Reload();
			else
			{
				// Update the animations and weapon-idle time.
				SendWeaponAnim(ACT_SHOTGUN_RELOAD_FINISH);
				SetWeaponIdleTime(gpGlobals->curtime + (rWeaponInf.m_fCycleTime * 5));

				// Re-set the reload flag.
				m_fReloadEndTime = 0.0f;
				m_nInSpecialReload = 0;
			}
		}
		// We be idling.
		else
			SendWeaponAnim(ACT_VM_IDLE);
	}

	// Decrease the shots fired count.
	if (pPlayer && pPlayer->m_nShotsFired > 3)
		pPlayer->m_nShotsFired -= 3;
	else if (pPlayer && pPlayer->m_nShotsFired > 0)
		pPlayer->m_nShotsFired = 0;
}