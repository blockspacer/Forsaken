/*
	forsaken_grenade_base.cpp
	Forsaken weapon code for all grenades.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_grenade_base.h"
#include "gamerules.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#include "items.h"
#endif

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenGrenadeBase, DT_ForsakenBaseGrenade)
LINK_ENTITY_TO_CLASS(forsaken_grenade_base, CForsakenGrenadeBase);

#ifdef GAME_DLL
BEGIN_DATADESC(CForsakenGrenadeBase)
	DEFINE_FIELD(m_bRedraw, FIELD_BOOLEAN),
END_DATADESC()
#endif

BEGIN_NETWORK_TABLE(CForsakenGrenadeBase, DT_ForsakenBaseGrenade)
#ifdef CLIENT_DLL
	RecvPropBool(RECVINFO(m_bPinPulled)),
	RecvPropBool(RECVINFO(m_bRedraw)),
	RecvPropFloat(RECVINFO(m_fThrowTime)),
	RecvPropFloat(RECVINFO(m_fPinPullEndTime)),
	RecvPropFloat(RECVINFO(m_fExplodeInHandTime)),
#else
	SendPropBool(SENDINFO(m_bPinPulled)),
	SendPropBool(SENDINFO(m_bRedraw)),
	SendPropFloat(SENDINFO(m_fThrowTime)),
	SendPropFloat(SENDINFO(m_fPinPullEndTime)),
	SendPropFloat(SENDINFO(m_fExplodeInHandTime)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CForsakenGrenadeBase)
	DEFINE_PRED_FIELD(m_bRedraw, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()
#endif

// Constructor & Deconstructor
CForsakenGrenadeBase::CForsakenGrenadeBase()
{
	// Reset our state variables.
	m_bPinPulled = false;
	m_bRedraw = false;
	m_fThrowTime = 0.0f;
	m_fPinPullEndTime = 0.0f;
	m_fExplodeInHandTime = 0.0f;
}

// Functions
bool CForsakenGrenadeBase::Deploy()
/*
	
	Pre: 
	Post: 
*/
{
	// Reset our state variables.
	m_bPinPulled = false;
	m_bRedraw = false;
	m_fThrowTime = 0.0f;
	m_fExplodeInHandTime = 0.0f;

	return BaseClass::Deploy();
}

bool CForsakenGrenadeBase::Holster(CForsakenWeaponBase *pSwitchingTo)
/*
	
	Pre: 
	Post: 
*/
{
	// Reset our state variables.
	m_bPinPulled = false;
	m_bRedraw = false;
	m_fThrowTime = 0.0f;
	m_fExplodeInHandTime = 0.0f;

#ifndef CLIENT_DLL
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	// If we don't have ammo, remove the grenade from our inventory.
	if (pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		pPlayer->Weapon_Drop(this, NULL, NULL);
		UTIL_Remove(this);
	}
#endif

	return BaseClass::Holster(pSwitchingTo);
}

bool CForsakenGrenadeBase::Reload()
/*
	
	Pre: 
	Post: 
*/
{
	// If we can redraw and can attack, reload.
	if (m_bRedraw && (m_flNextPrimaryAttack <= gpGlobals->curtime) && 
		(m_flNextSecondaryAttack <= gpGlobals->curtime))
	{
		// Update our view model animation.
		SendWeaponAnim(ACT_VM_DRAW);

		// Update our next attack times.
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = 
			gpGlobals->curtime + GetViewModelSequenceDuration();

		SetWeaponIdleTime(m_flNextPrimaryAttack);

		// Update our state variables.
		m_bPinPulled = false;
		m_bRedraw = false;
		m_fThrowTime = 0.0f;
		m_fExplodeInHandTime = 0.0f;
	}

	return true;
}

void CForsakenGrenadeBase::ItemPostFrame()
/*
	
	Pre: 
	Post: 
*/
{
	CBaseViewModel *pViewModel = NULL;
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	// Do we have a view model?
	pViewModel = pPlayer->GetViewModel(m_nViewModelIndex);
	if (!pViewModel)
		return;

	// Here we go, the maze of amazing if/else-if statements!

	// This specifically checks to see if we are THROWING the grenade.
	// If the pin is pulled, we aren't holding the attack button, and the pin-pull anim has 
	// finished playing... throw that bitch.
	if (m_bPinPulled && !(pPlayer->m_nButtons & IN_ATTACK) && 
		m_fPinPullEndTime <= gpGlobals->curtime)
	{
		// Perform the animation event on the player.
		pPlayer->DoAnimationEvent(FORSAKEN_PLAYERANIMEVENT_THROW);

		// Start throwing the grenade and update our state variables.
		StartGrenadeThrow();
		SetWeaponIdleTime(gpGlobals->curtime + SequenceDuration());

		// The pin is no longer pulled and update our attack times.
		m_bPinPulled = false;
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetWeaponIdleTime();
	}
	// If we had pulled the pin, we're still priming the grenade, and it's time for it to explode 
	// in our hand.... drop the grenade with a immediate explosion time.
	else if (m_bPinPulled && (pPlayer->m_nButtons & IN_ATTACK) && 
		m_fExplodeInHandTime <= gpGlobals->curtime)
	{
		// Drop it!
		DropGrenade(0.0f);
	}
	// If we are throwing the grenade and it's time to release it from our hand...
	else if ((m_fThrowTime > 0) && (m_fThrowTime < gpGlobals->curtime))
	{
		// Throw that shit!
		ThrowGrenade();
	}
	// The 'redraw' state is mainly used to check to see if we can reload.
	else if (m_bRedraw)
	{
#ifdef GAME_DLL
		// If we are idling...
		if (GetWeaponIdleTime() < gpGlobals->curtime)
		{
			// If we have no more ammo, remove the weapon from inventory.
			// Otherwise, reload.
			if (pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
			{
				pPlayer->Weapon_Drop(this, NULL, NULL);

				UTIL_Remove(this);
			}
			else
				Reload();
		}
#endif
	}
	// We're not doing anything special... call our base class.
	else if (!m_bRedraw)
		BaseClass::ItemPostFrame();
}

void CForsakenGrenadeBase::Precache()
/*
	
	Pre: 
	Post: 
*/
{
	// We have nothing special to precache.
	BaseClass::Precache();
}

void CForsakenGrenadeBase::PrimaryAttack()
/*
	
	Pre: 
	Post: 
*/
{
	// If we are re-drawing our weapon or if the pin is pulled, we can't do anything.
	if (m_bRedraw || m_bPinPulled)
		return;

	CForsakenPlayer *pPlayer = GetPlayerOwner();

	// Do we have a player and do they have ammo?
	if (!pPlayer || pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return;

	// Pull the pin and set the explode in hand time.
	m_bPinPulled = true;
	m_fExplodeInHandTime = gpGlobals->curtime + GetPrimedCountdownTime();

	// Update our animations.
	SendWeaponAnim(ACT_VM_PULLPIN);
	SetWeaponIdleTime(gpGlobals->curtime + GetViewModelSequenceDuration());

	// When does the pin-pull end and when can we attack next?
	m_fPinPullEndTime = m_flNextPrimaryAttack = m_flNextSecondaryAttack = 
		GetWeaponIdleTime();
}

void CForsakenGrenadeBase::SecondaryAttack()
/*
	
	Pre: 
	Post: 
*/
{
	// I have no clue what this function is doing. Really.
	// I'll come back and revisit it.

	if (m_bRedraw)
		return;

	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	if (pPlayer->GetFlags() & FL_DUCKING)
		SendWeaponAnim(ACT_VM_SECONDARYATTACK);
	else
		SendWeaponAnim(ACT_VM_HAULBACK);

	SetWeaponIdleTime(gpGlobals->curtime + GetViewModelSequenceDuration());

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetWeaponIdleTime();
}

#ifdef CLIENT_DLL
void CForsakenGrenadeBase::DecrementAmmo(CForsakenPlayer *pSwitchingTo)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenGrenadeBase::DropGrenade(float fTime)
/*
	
	Pre: 
	Post: 
*/
{
	// Reset our state variables.
	m_bPinPulled = false;
	m_bRedraw = true;
	m_fExplodeInHandTime = 0.0f;
	m_fPinPullEndTime = 0.0f;
	m_fThrowTime = 0.0f;
}

void CForsakenGrenadeBase::ThrowGrenade()
/*
	
	Pre: 
	Post: 
*/
{
	// Update our state variables.
	m_bRedraw = true;
	m_fThrowTime = 0.0f;
}

void CForsakenGrenadeBase::StartGrenadeThrow()
/*
	
	Pre: 
	Post: 
*/
{
	// Update our throw time.
	m_fThrowTime = gpGlobals->curtime + 0.1f;
}
#else
bool CForsakenGrenadeBase::AllowsAutoSwitchFrom(void) const
{
	// If the pin is pulled... we don't want to allow switching from this weapon.
	return !m_bPinPulled;
}

int CForsakenGrenadeBase::CapabilitiesGet()
{
	return bits_CAP_WEAPON_RANGE_ATTACK1;
}

void CForsakenGrenadeBase::DecrementAmmo(CForsakenPlayer *pSwitchingTo)
/*
	
	Pre: 
	Post: 
*/
{
	if (!pSwitchingTo)
		return;

	// Decrement our ammo.
	pSwitchingTo->RemoveAmmo(1, m_iPrimaryAmmoType);
}

void CForsakenGrenadeBase::DropGrenade(float fTime)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());
	Vector vecForward, vecSource, vecVelocity;

	if (!pPlayer)
		return;

	// First, calculate our vectors.
	pPlayer->EyeVectors(&vecForward);
	vecSource = pPlayer->GetAbsOrigin() + pPlayer->GetViewOffset() + vecForward * 16;
	vecVelocity = pPlayer->GetAbsVelocity();

	// Decrement our ammo.
	DecrementAmmo(pPlayer);

	// Emit the grenade.
	EmitGrenade(vecSource, vec3_angle, vecVelocity, AngularImpulse(600, RandomInt(-1200, 1200), 0), 
		pPlayer, fTime);

	// Update our state variables.
	// Bug Fix ID 0000047: This fixes the grenades keep on throwin' bug.
	m_bPinPulled = false;
	m_bRedraw = true;
	m_fExplodeInHandTime = 0.0f;
	m_fPinPullEndTime = 0.0f;
	m_fThrowTime = 0.0f;
}

void CForsakenGrenadeBase::EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, 
	AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime)
/*
	
	Pre: 
	Post: 
*/
{
	Assert(0);
}

void CForsakenGrenadeBase::ThrowGrenade()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());
	float fVelocity;
	QAngle angThrow;
	Vector vecForward, vecRight, vecUp, vecSource, vecThrow;

	if (!pPlayer)
		return;

	// Calculate our throwing angles.
	angThrow = pPlayer->LocalEyeAngles();

	// We need to clamp our throwing angles.
	if (angThrow.x < 90)
		angThrow.x = -10 + angThrow.x * (100 / 90.0);
	else
	{
		angThrow.x = 360.0f - angThrow.x;
		angThrow.x = -10 + angThrow.x * -(80 / 90.0);
	}

	// Okay, now we need to calculate our velocity scalar.
	fVelocity = (90 - angThrow.x) * 6;

	// We don't want it TOO large.
	if (fVelocity > 750)
		fVelocity = 750;

	// Calculate our base vectors.
	AngleVectors(angThrow, &vecForward, &vecRight, &vecUp);

	// Now update our vectors.
	vecSource = pPlayer->GetAbsOrigin() + pPlayer->GetViewOffset();
	vecSource += vecForward * 16;
	vecThrow = vecForward * fVelocity + pPlayer->GetAbsVelocity();

	// Decrease our ammo.
	DecrementAmmo(pPlayer);

	// Emit the grenade!
	EmitGrenade(vecSource, vec3_angle, vecThrow, AngularImpulse(600, RandomInt(-1200, 1200), 0), 
		pPlayer, m_fExplodeInHandTime - gpGlobals->curtime);

	// Update our state variables.
	m_bPinPulled = false;
	m_bRedraw = true;
	m_fExplodeInHandTime = 0.0f;
	m_fPinPullEndTime = 0.0f;
	m_fThrowTime = 0.0f;
}

void CForsakenGrenadeBase::StartGrenadeThrow()
/*
	
	Pre: 
	Post: 
*/
{
	// Send the animation and say how long until we actually THROW the grenade.
	SendWeaponAnim(ACT_VM_THROW);

	m_fThrowTime = gpGlobals->curtime + 0.1f;
}
#endif