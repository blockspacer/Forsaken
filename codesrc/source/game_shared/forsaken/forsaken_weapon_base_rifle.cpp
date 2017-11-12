/*
	forsaken_weapon_base_melee.cpp
	Forsaken weapon code for all melee weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_gun.h"
#include "forsaken_weapon_base_rifle.h"
#include "forsaken_fx_shared.h"
#include "in_buttons.h"

#ifndef CLIENT_DLL
#include "forsaken/te_firebullets.h"
#endif

// HL2 Class Macros
BEGIN_NETWORK_TABLE(CForsakenWeaponRifleBase, DT_ForsakenWeaponRifleBase)
#ifdef CLIENT_DLL
	RecvPropBool(RECVINFO(m_bWantsToReload)),
	RecvPropFloat(RECVINFO(m_fEndZoomAnim)),
	RecvPropFloat(RECVINFO(m_fNextZoom)),
	RecvPropFloat(RECVINFO(m_fReloadTime)),
	RecvPropInt(RECVINFO(m_nZoomLevel)),
#else
	SendPropBool(SENDINFO(m_bWantsToReload)),
	SendPropFloat(SENDINFO(m_fEndZoomAnim)),
	SendPropFloat(SENDINFO(m_fNextZoom)),
	SendPropFloat(SENDINFO(m_fReloadTime)),
	SendPropInt(SENDINFO(m_nZoomLevel)),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeaponRifleBase)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_riflebase, CForsakenWeaponRifleBase);
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponRifleBase, DT_ForsakenWeaponRifleBase)

#ifdef GAME_DLL
BEGIN_DATADESC(CForsakenWeaponRifleBase)
	DEFINE_FIELD(m_fNextZoom, FIELD_FLOAT),
	DEFINE_FIELD(m_fReloadTime, FIELD_FLOAT),
	DEFINE_FIELD(m_nZoomLevel, FIELD_INTEGER),
END_DATADESC()
#endif

// Constructor & Deconstructor
CForsakenWeaponRifleBase::CForsakenWeaponRifleBase()
{
	m_bIsOnLadder = false;
	m_bResetZoom = false;
	m_fEndZoomAnim = 0.0f;
	m_fNextZoom = 0.0f;
	m_fReloadTime = 0.0f;
	m_nZoomLevel = 0;
}

// Functions
bool CForsakenWeaponRifleBase::Holster(CBaseCombatWeapon *pSwitchingTo)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());

	if (!pPlayer)
		return false;

	m_bResetZoom = false;
	m_nZoomLevel = 0;

#ifndef CLIENT_DLL
	// Reset the FOV.
	pPlayer->SetFOV(this, GetFOVForZoom(m_nZoomLevel), 0);
#endif

	return BaseClass::Holster(pSwitchingTo);
}

bool CForsakenWeaponRifleBase::Reload()
/*
	
	Pre: 
	Post: 
*/
{
	if (m_fReloadTime == 0.0f && m_nZoomLevel != 0)
	{
		// Reset the zoom.
		ResetZoom(0.0f);

		// We will play the zoom-out anim, and then reload.
		m_bWantsToReload = true;

		return false;
	}
	else if (m_fReloadTime <= gpGlobals->curtime && m_nZoomLevel == 0)
		return BaseClass::Reload();
}

void CForsakenWeaponRifleBase::HandleZoom()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());

	if (!pPlayer)
		return;

	// If we are sprinting, kicking, or jumping kick out of zoom.
	if (((pPlayer->IsSprinting() && pPlayer->m_fForwardSpeed > 0) || 
		pPlayer->IsKicking() || pPlayer->m_nButtons & IN_JUMP) && m_nZoomLevel != 0)
	{
		m_nZoomLevel = 0;

#ifndef CLIENT_DLL
		pPlayer->SetFOV(this, GetFOVForZoom(m_nZoomLevel), GetZoomRate());
#endif

		// VM animations and sounds for zooming out.
		SendWeaponAnim(ACT_VM_FROMZOOM);
		WeaponSound(SCOPE_NORMAL);
	}

	// Do we need to reset the zoom?
	if (m_bResetZoom && (gpGlobals->curtime > m_fNextZoom))
	{
		m_bResetZoom = false;
		m_nZoomLevel = 0;

#ifndef CLIENT_DLL
		// Reset the FOV.
		pPlayer->SetFOV(this, GetFOVForZoom(m_nZoomLevel), GetZoomRate());
#endif

		// VM animations and sounds for zooming out.
		SendWeaponAnim(ACT_VM_FROMZOOM);
		WeaponSound(SCOPE_NORMAL);

		// We want to play the zoom-out anim, and then reload.
		if (m_bWantsToReload)
			m_fReloadTime = gpGlobals->curtime + GetViewModelSequenceDuration();
	}
	// Can we zoom, are we hitting the zoom button, and has it been enough time since the last one?
	else if ((pPlayer->m_afButtonPressed & IN_ATTACK2) && (gpGlobals->curtime > m_fNextZoom))
	{
		int nNextZoom = m_nZoomLevel + 1;

		// Clip the zoom level.
		if (nNextZoom > GetMaxZoomLevel())
			nNextZoom = 0;

#ifdef CLIENT_DLL
		if (nNextZoom == 0)
			WeaponSound(SCOPE_NORMAL);
		else
			WeaponSound(SCOPE);
#endif

#ifndef CLIENT_DLL
		// Are we at the regular zoom level (default FOV)?
		if (m_nZoomLevel == 0)
		{
			// VM animations for zooming in.
			SendWeaponAnim(ACT_VM_TOZOOM);

			// Update the FOV.
			pPlayer->SetFOV(this, GetFOVForZoom(nNextZoom), GetViewModelSequenceDuration());

			// Set the next zoom time.
			m_fEndZoomAnim = m_fNextZoom = gpGlobals->curtime + GetViewModelSequenceDuration();
		}
		// Are we going to the regular zoom level (default FOV)?
		else if (nNextZoom == 0)
		{
			// VM animations for zooming out.
			SendWeaponAnim(ACT_VM_FROMZOOM);

			// Update the FOV.
			pPlayer->SetFOV(this, GetFOVForZoom(nNextZoom), GetViewModelSequenceDuration());

			// Set the next zoom time.
			m_fEndZoomAnim = m_fNextZoom = gpGlobals->curtime + GetViewModelSequenceDuration();
		}
		// Are we just increasing zoom?
		else
		{
			// Update the FOV.
			pPlayer->SetFOV(this, GetFOVForZoom(nNextZoom), GetZoomRate());

			// Set the next zoom time.
			m_fNextZoom = gpGlobals->curtime + GetZoomWait();
		}
#endif

		// Set the zoom wait time and go to the next state.
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_fNextZoom;
		m_nZoomLevel = nNextZoom;
	}

#ifndef CLIENT_DLL
	// If we are climbing, set up a zoom reset.
	// This needs to come after the 'reset zoom' code above.
	if (pPlayer->IsOnLadder() && m_nZoomLevel != 0 && (gpGlobals->curtime > m_fNextZoom))
	{
		if (!m_bIsOnLadder)
		{
			ResetZoom(0.0f);
			m_bIsOnLadder = true;
		}

		m_bIsOnLadder = false;
	}
	else
	{
		m_bIsOnLadder = false;
	}
#endif
}

void CForsakenWeaponRifleBase::ItemBusyFrame()
/*
	
	Pre: 
	Post: 
*/
{
	HandleZoom();

	BaseClass::ItemBusyFrame();
}

void CForsakenWeaponRifleBase::ItemPostFrame()
/*
	
	Pre: 
	Post: 
*/
{
	HandleZoom();

	BaseClass::ItemPostFrame();
}

void CForsakenWeaponRifleBase::ItemPreFrame()
/*
	
	Pre: 
	Post: 
*/
{
	// Are we trying to reload?
	if (m_bWantsToReload && m_fReloadTime != 0.0f && m_fReloadTime <= gpGlobals->curtime)
	{
		Reload();

		m_bWantsToReload = false;
		m_fReloadTime = 0.0f;
	}

	BaseClass::ItemPreFrame();
}

void CForsakenWeaponRifleBase::ResetZoom(float fWaitTime)
/*
	
	Pre: 
	Post: 
*/
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return;

	// We are resetting the zoom.
	m_bResetZoom = true;
	m_fNextZoom = gpGlobals->curtime + fWaitTime;
}

void CForsakenWeaponRifleBase::SecondaryAttack()
/*
	
	Pre: 
	Post: 
*/
{
}