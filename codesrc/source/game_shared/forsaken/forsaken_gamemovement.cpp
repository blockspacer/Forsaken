/*
	forsaken_gamemovement.cpp
	Game movement for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "gamemovement.h"
#include "forsaken_gamerules_territorywar.h"
#include "forsaken_shareddefs.h"
#include "in_buttons.h"
#include "movevars_shared.h"

#ifdef CLIENT_DLL
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#endif

#include "forsaken/forsaken_weapon_base.h"

// Internal Classes
class CForsakenGameMovement : public CGameMovement
{
public:
	DECLARE_CLASS(CForsakenGameMovement, CGameMovement);

	// Constructor & Deconstructor
	CForsakenGameMovement();

	// Public Accessor Functions

	// Public Functions
	virtual void WalkMove();

	// Public Variables

protected:
	// Protected Functions
	virtual bool CanAccelerate();
	virtual void AirMove();

	// Protected Variables
};

// Static Variables
static CForsakenGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = (IGameMovement*)&g_GameMovement;

// HL2 Class Macros
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, 
								  g_GameMovement);

// Constructor & Deconstructor
CForsakenGameMovement::CForsakenGameMovement()
{
}

// Functions
bool CForsakenGameMovement::CanAccelerate()
/*
	
	Pre: 
	Post: 
*/
{
	// If we are dead and not in roam mode (observer) don't accelerate.
	if (player->pl.deadflag)
	{
		if (player->GetObserverMode() != OBS_MODE_ROAMING)
			return false;
	}

	// No acceleration for water.
	if (player->GetWaterJumpTime())
		return false;

	return true;
}

void CForsakenGameMovement::AirMove()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(player);
	CForsakenWeaponBase *pWeaponBase = pPlayer->GetActiveForsakenWeapon();

	if (pPlayer->IsSprinting() && !(player->GetFlags() & FL_DUCKING))
	{
		if (mv->m_flForwardMove > 0.0f && pPlayer->GetMaxSprint() > 0.0f)
		{
			float fDecreaseSprint = (100.0f / 3.0f) * gpGlobals->frametime;

			if (pPlayer->GetSprint() >= fDecreaseSprint)
			{
				// Increase the forward and max move speed while doing no side movement.
				//mv->m_flSideMove = 0.0f;

				pPlayer->IncrementSprint(-fDecreaseSprint);

				if (pWeaponBase && pWeaponBase->m_flNextPrimaryAttack <= gpGlobals->curtime)
				{
					pWeaponBase->SendWeaponAnim(ACT_VM_SPRINT);

					pWeaponBase->m_flNextPrimaryAttack = 
						pWeaponBase->m_flNextSecondaryAttack = gpGlobals->curtime + 
						pWeaponBase->GetViewModelSequenceDuration();
				}
			}
		}
	}

	// Set the players forward speed.
	pPlayer->m_fForwardSpeed = mv->m_flForwardMove;

	BaseClass::AirMove();
}

void CForsakenGameMovement::WalkMove()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(player);

	// Setup max speeds dependent on each team.
	if (pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
		mv->m_flMaxSpeed *= 1.2f;
	else if (pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
		mv->m_flMaxSpeed *= 0.9f;
	else if (pPlayer->GetTeamNumber() == TEAM_RELIGIOUS)
		mv->m_flMaxSpeed *= 0.9f;

	// If the player is stunned, randomize their movement.
	if (pPlayer->IsStunned())
	{
		float fSin = sin(gpGlobals->curtime), f2Sin = sin(gpGlobals->curtime * 2);
		float fTime = 1.0 - ((pPlayer->m_fStunEndTime - gpGlobals->curtime) / 
			FORSAKEN_M84STUN_TIME);
		Vector lerpFrom(100, 50, 0), lerpTo(0, 0, 0), lerpResult;

		// We need to decrease the movement amount over time.
		VectorLerp(lerpFrom, lerpTo, fTime, lerpResult);

		// Calculate the additional forward and side movement from stun.
		mv->m_flForwardMove += lerpResult.y * f2Sin;
		mv->m_flSideMove += lerpResult.x * fSin;
	}
/*
	if (pPlayer->IsLeaning())
	{
		float fEyeYaw = 0.0f, fEyePitch = 0.0f;
		QAngle angEyes = pPlayer->LocalEyeAngles();

		fEyeYaw = AngleNormalize(angEyes[YAW]);
		fEyePitch = AngleNormalize(angEyes[PITCH]);
	}
	else */if (pPlayer->IsKicking() || pPlayer->m_fKickEndTime > gpGlobals->curtime)
	{
		/*
			Custom movement code for kicking will eventually go here.
			For now we just keep ourselves from actually moving.
		*/
	}
	else if (pPlayer->IsSprinting() && !(player->GetFlags() & FL_DUCKING))
	{
		// Increase the forward and max move speed while doing no side movement.
		mv->m_flForwardMove *= 1.5f;
		mv->m_flMaxSpeed *= 1.5f;
		mv->m_flSideMove = 0.0f;

		BaseClass::WalkMove();
	}
	else
		BaseClass::WalkMove();

	// Set the players forward speed.
	pPlayer->m_fForwardSpeed = mv->m_flForwardMove;
}