/*
	forsaken_player_shared.cpp
	Shared Forsaken player code (client & server).

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#ifdef CLIENT_DLL
#include "forsaken/c_forsaken_player.h"

char TEXTURETYPE_Find( trace_t *ptr );
#else
#include "forsaken/forsaken_player.h"
#endif

#include "decals.h"
#include "gamevars_shared.h"
#include "takedamageinfo.h"
#include "effect_dispatch_data.h"
#include "engine/ivdebugoverlay.h"
#include "forsaken/forsaken_weapon_base.h"

// Function Prototypes
void DispatchEffect( const char *pName, const CEffectData &data );

// Global Variable Decleration
ConVar sv_showimpacts("sv_showimpacts", "0", FCVAR_REPLICATED, 
		"Shows client (red) and server (blue) impact point.");

// HL2 Class Macros
#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CForsakenPlayer)
	DEFINE_PRED_FIELD(m_hObjectiveEntity, FIELD_EHANDLE, FTYPEDESC_INSENDTABLE),
	DEFINE_PRED_FIELD(m_flCycle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK)
END_PREDICTION_DATA()
#endif

// Functions
bool CForsakenPlayer::ForsakenAnim_CanMove()
/*
	
	Pre: 
	Post: 
*/
{
	return true;
}

CForsakenWeaponBase *CForsakenPlayer::ForsakenAnim_GetActiveWeapon()
/*
	
	Pre: 
	Post: 
*/
{
	return GetActiveForsakenWeapon();
}

const QAngle &CForsakenPlayer::EyeAngles()
/*
	Modifies the eye angles if the player is stunned or leaning.
	Pre: None.
	Post: The eye angles are modified randomly if the player is stunned, and 
		in the lean direction if the player is leaning.
*/
{
	static QAngle angEyes;

	// Determine the default eye angles.
	angEyes = BaseClass::EyeAngles();

	// Are we stunned?
	if (IsStunned())
	{
		float fEyeYaw = angEyes[YAW], fEyePitch = angEyes[PITCH], fEyeRoll = angEyes[ROLL];
		float fCos = cos(gpGlobals->curtime);
		float fSin = sin(gpGlobals->curtime), f2Sin = sin(gpGlobals->curtime * 2);
		float fTime = 1.0 - ((m_fStunEndTime - gpGlobals->curtime) / 
			FORSAKEN_M84STUN_TIME);
		Vector lerpFrom(20, 20, 10), lerpTo(0, 0, 0), lerpResult;

		// We need to decrease the angle amount over time.
		VectorLerp(lerpFrom, lerpTo, fTime, lerpResult);

		// Calculate the additional yaw, pitch, and roll.
		fEyeYaw += lerpResult.x * fSin;
		fEyePitch += lerpResult.y * f2Sin;
		fEyeRoll += lerpResult.z * fCos * fSin;

		// Update the yaw, pitch, and roll.
		angEyes[YAW] = fEyeYaw;
		angEyes[PITCH] = fEyePitch;
		angEyes[ROLL] = fEyeRoll;
	}/*
	// Are we leaning?
	else if (IsLeaning())
	{
		switch (GetLeaningDirection())
		{
		case LEAN_DIR_LEFT:
			fEyeRoll -= 0.1f;
			break;

		case LEAN_DIR_RIGHT:
			fEyeRoll += 0.1f;
			break;
		}

		if (fEyeRoll > 5.0f)
			fEyeRoll = 5.0f;
		else if (fEyeRoll < -5.0f)
			fEyeRoll = -5.0f;

		angEyes[ROLL] = fEyeRoll;
	}*/
	else
	{
		/*fEyeRoll = 0;
		fTime = 0;*/
	}

	return angEyes;
}

ForsakenMovementID CForsakenPlayer::GetMovementType()
{
	//float fMoveSpeed = GetMoveType
	int nFlags = GetFlags();

	// Are we on the ground?
	if (nFlags & FL_ONGROUND)
	{
		// Are we ducking?
		if (nFlags & FL_DUCKING)
		{
			// Are we moving while ducking?
			if (IsMoving())
				return FORSAKEN_MOVEMENT_CROUCHWALK;
			else
				return FORSAKEN_MOVEMENT_CROUCH;
		}
		else
		{
			// Are we moving while standing?
			if (IsMoving())
				return FORSAKEN_MOVEMENT_RUN;
			else
				return FORSAKEN_MOVEMENT_NONE;
		}
	}
	else
		return FORSAKEN_MOVEMENT_JUMP;
}

Vector CForsakenPlayer::EyePosition()
{
	static float fLeanPercentage = 0.0f;
	QAngle angEyeAngles;
	Vector vecEyePos;

	if (IsLeaning())
	{
		Vector vecForward, vecRight, vecUp;
		Vector vecLean;

		switch (GetLeaningDirection())
		{
		case LEAN_DIR_LEFT:
			fLeanPercentage += 0.005f;
			vecLean = FORSAKEN_PLAYER_LEANLEFT_OFFSET;
			break;

		case LEAN_DIR_RIGHT:
			fLeanPercentage += 0.005f;
			vecLean = FORSAKEN_PLAYER_LEANRIGHT_OFFSET;
			break;
		}

		if (fLeanPercentage > 1.0f)
			fLeanPercentage = 1.0f;

		EyePositionAndVectors(&vecEyePos, &vecForward, &vecRight, &vecUp);

		vecEyePos.x += (vecForward.x * vecLean.y * fLeanPercentage);
		vecEyePos.y += (vecForward.y * vecLean.y * fLeanPercentage);

		vecEyePos.x += (vecRight.x * vecLean.x * fLeanPercentage);
		vecEyePos.y += (vecRight.y * vecLean.x * fLeanPercentage);

		return vecEyePos;
	}
	else
	{
		fLeanPercentage = 0.0f;

		return BaseClass::EyePosition();
	}

	return vecEyePos;
}

void CForsakenPlayer::FireBullet(Vector vecSource, const QAngle &angShootAngles, float fSpread, 
		int nDamage, int nBulletType, CBaseEntity *pentAttacker, bool bDoEffects, float fX, 
		float fY)
/*
	
	Pre: 
	Post: 
*/
{
	float fCurrentDamage = nDamage;
	float fCurrentDistance = 0.0;
	IHandleEntity *pPass = this;
	trace_t tr;
	Vector vecDirShooting, vecRight, vecUp;
	unsigned char ucSurfaceMat = 0;

	AngleVectors(angShootAngles, &vecDirShooting, &vecRight, &vecUp);

	// We are the default attacker.
	if (!pentAttacker)
		pentAttacker = this;

	// Randomize some spray in there.
	bool bStop = false;
	Vector vecDir = vecDirShooting +
		fX * fSpread * vecRight +
		fY * fSpread * vecUp;
	Vector vecTotalTrace = Vector(0.0f, 0.0f, 0.0f);
	Vector vecEnd = vecSource + vecDir * MAX_TRACE_LENGTH;
	Vector vecLastTrace = vecSource, vecStart = vecSource;

	// Normalize the direction, we only want it affecting the path not adding to the distance.
	VectorNormalize(vecDir);

	// Perform a loop on the trace.
	for (int i = 0; i < 100 && !bStop; i++)
	{
		// Save the last trace.
		vecLastTrace = vecSource;

		// Trace the bullet path.
		UTIL_TraceLine(vecSource, vecEnd, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, pPass, 
			COLLISION_GROUP_NONE, &tr);

		// We are still at the same position... so just stop here.
		if (tr.endpos == vecLastTrace)
			break;

		// Get the surface material and the total trace length.
		ucSurfaceMat = TEXTURETYPE_Find(&tr);
		vecTotalTrace = vecTotalTrace + ((vecEnd - vecSource) * tr.fraction);
		fCurrentDistance += vecTotalTrace.Length();

		// Is it passable or are we inside a object?
		if ((tr.fraction < 1.0f) || (tr.startsolid && i != 0))
		{
			if (tr.contents & CONTENTS_WINDOW || tr.contents & CONTENTS_GRATE)
			{
				// Bullet hit code.
				OnBulletHit(bDoEffects, pentAttacker, fCurrentDamage, fCurrentDistance, nBulletType, &tr, 
					vecDir, vecStart);

				// Weaken the damage since we hit something.
				fCurrentDamage *= RandomFloat(0.8f, 0.98f);

				// It's passable, so ignore this entity and set the new bullet position.
				pPass = tr.m_pEnt;
				vecSource = tr.endpos;
			}
			else
			{
				// We will pass through these materials.
				switch (ucSurfaceMat)
				{
				case CHAR_TEX_VENT:
				case CHAR_TEX_GRATE:
				case CHAR_TEX_WOOD:
				case CHAR_TEX_COMPUTER:
				case CHAR_TEX_GLASS:
				case CHAR_TEX_FLESH:
				case CHAR_TEX_BLOODYFLESH:
				case CHAR_TEX_FOLIAGE:
				case CHAR_TEX_PLASTIC:
					// Bullet hit code.
					OnBulletHit(bDoEffects, pentAttacker, fCurrentDamage, fCurrentDistance, nBulletType, &tr, 
						vecDir, vecStart);

					// Weaken the damage since we hit something.
					fCurrentDamage *= RandomFloat(0.8f, 0.98f);

					// It's passable, so ignore this entity and set the new bullet position.
					pPass = tr.m_pEnt;
					vecSource = tr.endpos;

					break;

				default:
					// Set the stop bit.
					bStop = true;

					break;
				}
			}
		}
		// It isn't, so the bullet stops here.
		else
			break;
	}

	// Nothing to hit.
	if (tr.fraction == 1.0f)
		return;

	// If we hit something metal, ricochet.
	// Trace along the ricochet line and add effects.
	if(!tr.allsolid && ucSurfaceMat == CHAR_TEX_METAL)
	{
		// Setup Memory (C style)
		Vector vecDirection;
		Vector vecNormal;
		Vector vecReflection;
		float fDot;
		QAngle angReflection;
		int numRicochets = random->RandomInt(1, 2);

		// Validate
		if( !vecDir.IsValid() || !tr.plane.normal.IsValid() )
		{
			return;
		}

		// Retrieve Normalized Vectors
		vecDirection = vecDir;
		vecNormal = tr.plane.normal;
		VectorNormalize( vecDirection );
		VectorNormalize( vecNormal );

		// Dot Product
		fDot = -DotProduct( vecDirection, vecNormal );

		// R = 2.0 * (-I DOT N) * N + I
		vecReflection = 2.0f * ( vecNormal * fDot ) + vecDirection;
		VectorNormalize( vecReflection );
		VectorAngles(vecReflection, angReflection);

		// decrease damage
		float ricochetDamage = fCurrentDamage * RandomFloat(0.8f, 0.98f);

		FireRicochet(tr.endpos, angReflection, 0.4f, ricochetDamage, nBulletType, pentAttacker, bDoEffects, fX, fY, random->RandomInt(3,10));
	}

	// Bullet hit code.
	OnBulletHit(bDoEffects, pentAttacker, fCurrentDamage, fCurrentDistance, nBulletType, &tr, 
		vecDir, vecStart);
}

void CForsakenPlayer::FireRicochet(Vector vecSource, const QAngle &angShootAngles, float fSpread, 
		int nDamage, int nBulletType, CBaseEntity *pentAttacker, bool bDoEffects, float fX, 
		float fY, int numRicochets)
/*
	
	Pre: 
	Post: 
*/
{
	// stop on ricochet limit
	if(!numRicochets) return;

	float fCurrentDamage = nDamage;
	float fCurrentDistance = 0.0;
	IHandleEntity *pPass = this;
	trace_t tr;
	Vector vecDirShooting, vecRight, vecUp;
	unsigned char ucSurfaceMat = 0;

	AngleVectors(angShootAngles, &vecDirShooting, &vecRight, &vecUp);

	// We are the default attacker.
	if (!pentAttacker)
		pentAttacker = this;

	// Randomize some spray in there.
	bool bStop = false;
	Vector vecDir = vecDirShooting +
		fX * fSpread * vecRight +
		fY * fSpread * vecUp;
	Vector vecTotalTrace = Vector(0.0f, 0.0f, 0.0f);
	Vector vecEnd = vecSource + vecDir * MAX_TRACE_LENGTH;
	Vector vecLastTrace = vecSource, vecStart = vecSource;

	// Normalize the direction, we only want it affecting the path not adding to the distance.
	VectorNormalize(vecDir);

	// Perform a loop on the trace.
	for (int i = 0; i < 100 && !bStop; i++)
	{
		// Save the last trace.
		vecLastTrace = vecSource;

		// Trace the bullet path.
		UTIL_TraceLine(vecSource, vecEnd, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, pPass, 
			COLLISION_GROUP_NONE, &tr);

		//debugoverlay->AddLineOverlay(vecSource, tr.endpos, 255, 0, 0, false, 5.0f);

		// We are still at the same position... so just stop here.
		if (tr.endpos == vecLastTrace)
			break;

		// Get the surface material and the total trace length.
		ucSurfaceMat = TEXTURETYPE_Find(&tr);
		vecTotalTrace = vecTotalTrace + ((vecEnd - vecSource) * tr.fraction);
		fCurrentDistance += vecTotalTrace.Length();

		// Is it passable or are we inside a object?
		if ((tr.fraction < 1.0f) || (tr.startsolid && i != 0))
		{
			if (tr.contents & CONTENTS_WINDOW || tr.contents & CONTENTS_GRATE)
			{
				// Bullet hit code.
				OnBulletHit(bDoEffects, pentAttacker, fCurrentDamage, fCurrentDistance, nBulletType, &tr, 
					vecDir, vecStart);

				// Weaken the damage since we hit something.
				fCurrentDamage *= RandomFloat(0.8f, 0.98f);

				// It's passable, so ignore this entity and set the new bullet position.
				pPass = tr.m_pEnt;
				vecSource = tr.endpos;
			}
			else
			{
				// We will pass through these materials.
				switch (ucSurfaceMat)
				{
				case CHAR_TEX_VENT:
				case CHAR_TEX_GRATE:
				case CHAR_TEX_WOOD:
				case CHAR_TEX_COMPUTER:
				case CHAR_TEX_GLASS:
				case CHAR_TEX_FLESH:
				case CHAR_TEX_BLOODYFLESH:
				case CHAR_TEX_FOLIAGE:
				case CHAR_TEX_PLASTIC:
					// Bullet hit code.
					OnBulletHit(bDoEffects, pentAttacker, fCurrentDamage, fCurrentDistance, nBulletType, &tr, 
						vecDir, vecStart);

					// Weaken the damage since we hit something.
					fCurrentDamage *= RandomFloat(0.8f, 0.98f);

					// It's passable, so ignore this entity and set the new bullet position.
					pPass = tr.m_pEnt;
					vecSource = tr.endpos;

					break;

				default:
					// Set the stop bit.
					bStop = true;

					break;
				}
			}
		}
		// It isn't, so the bullet stops here.
		else
			break;
	}

	// Nothing to hit.
	if (tr.fraction == 1.0f)
		return;

	// If we hit something metal, keep ricocheting.
	// Trace along the ricochet line and add effects.
	if(!tr.allsolid && ucSurfaceMat == CHAR_TEX_METAL)
	{
		// Setup Memory (C style)
		Vector vecDirection;
		Vector vecNormal;
		Vector vecReflection;
		float fDot;
		QAngle angReflection;

		// Validate
		if( !vecDir.IsValid() || !tr.plane.normal.IsValid() )
		{
			return;
		}

		// Retrieve Normalized Vectors
		vecDirection = vecDir;
		vecNormal = tr.plane.normal;
		VectorNormalize( vecDirection );
		VectorNormalize( vecNormal );

		// Dot Product
		fDot = -DotProduct( vecDirection, vecNormal );

		// R = 2.0 * (-I DOT N) * N + I
		vecReflection = 2.0f * ( vecNormal * fDot ) + vecDirection;
		VectorNormalize( vecReflection );
		VectorAngles(vecReflection, angReflection);

		// decrease damage
		float ricochetDamage = fCurrentDamage * RandomFloat(0.8f, 0.98f);

		FireRicochet(tr.endpos, angReflection, fSpread, ricochetDamage, nBulletType, pentAttacker, bDoEffects, fX, fY, --numRicochets);
	}

	// Bullet hit code.
	OnBulletHit(bDoEffects, pentAttacker, fCurrentDamage, fCurrentDistance, nBulletType, &tr, 
		vecDir, vecStart);
}

void CForsakenPlayer::HandleCustomMovement()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenWeaponBase *pWeaponBase = GetActiveForsakenWeapon();
	int nChangedButtons = m_afButtonPressed | m_afButtonReleased;

	if (nChangedButtons & IN_LEAN)
	{
		if (IsLeaning())
			m_bIsLeaning = false;
		else
			m_bIsLeaning = true;
	}
	else if (nChangedButtons & IN_KICK)
	{
		if (IsKicking())
			m_bIsKicking = false;
		else if ((m_fKickEndTime + 0.1f) <= gpGlobals->curtime && 
			(m_fForwardSpeed > -10.0f && m_fForwardSpeed < 10.0f))
		{
			m_bIsKicking = true;

			// Calculate the kick end time.
			if (pWeaponBase)
			{
				pWeaponBase->SendWeaponAnim(ACT_VM_KICK);
				pWeaponBase->DoKick();

				// Kick animation.
				float kickDuration = pWeaponBase->GetViewModelSequenceDuration();
				if (kickDuration > 2.0) kickDuration = 2.0;
				m_fKickEndTime = gpGlobals->curtime + kickDuration;

				pWeaponBase->m_flNextPrimaryAttack = pWeaponBase->m_flNextSecondaryAttack = 
					m_fKickEndTime;
			}
		}
	}
	else if (nChangedButtons & IN_SPRINT)
	{
		float fReloadEndTime = 0.0f;

		if (pWeaponBase)
			fReloadEndTime = pWeaponBase->m_fReloadEndTime;

		if (IsSprinting() || (m_afButtonReleased & IN_SPRINT))
		{
			SetPlayerSprinting(false);

			if (pWeaponBase)
			{
				pWeaponBase->m_flNextPrimaryAttack = pWeaponBase->m_flNextSecondaryAttack = 
					gpGlobals->curtime + 0.1f;
			}
		}
		else
		{
			if (GetSprint() > 0 && m_fForwardSpeed > 0 && !(GetFlags() & FL_DUCKING) && 
				!(m_afButtonReleased & IN_SPRINT) && fReloadEndTime < gpGlobals->curtime &&
				(GetSprint() > (GetMaxSprint() * .15)))
			{
				m_bIsSprinting = true;

				SetPlayerLowSprint(false);

				if (pWeaponBase)
				{
					pWeaponBase->SendWeaponAnim(ACT_VM_SPRINT);

					pWeaponBase->m_flNextPrimaryAttack = 
						pWeaponBase->m_flNextSecondaryAttack = gpGlobals->curtime + 
						pWeaponBase->GetViewModelSequenceDuration();
				}
			}
		}
	}

	// Are we sprinting?
	if (IsSprinting())
	{
		// If we are ducking, we can't be sprinting.
		if (GetFlags() & FL_DUCKING)
		{
			SetPlayerLowSprint(false);
			SetPlayerSprinting(false);
		}
		else
		{
			// Do we have sprint left?
			if (GetMaxSprint() > 0.0f)
			{
				float fDecreaseSprint = (100.0f / 3.0f) * gpGlobals->frametime;

				// Do we have enough sprint to actually sprint?
				if (GetSprint() >= fDecreaseSprint)
				{
					// Decrease the sprint.
					IncrementSprint(-fDecreaseSprint);

					// Update sprint states.
					SetPlayerLowSprint(false);
					SetPlayerSprinting(true);

					// Update weapon animations.
					if (pWeaponBase && pWeaponBase->m_flNextPrimaryAttack <= gpGlobals->curtime)
					{
						pWeaponBase->SendWeaponAnim(ACT_VM_SPRINT);

						pWeaponBase->m_flNextPrimaryAttack = 
							pWeaponBase->m_flNextSecondaryAttack = gpGlobals->curtime + 
							pWeaponBase->GetViewModelSequenceDuration();
					}
				}
				else
				{
					// Well, we don't have enough sprint... so update our states.
					SetPlayerLowSprint(true);
					SetPlayerSprinting(false);

					// Update weapon animations.
					if (pWeaponBase)
						pWeaponBase->SendWeaponAnim(ACT_VM_IDLE);
				}
			}
			else
			{
				// We can't be sprinting at all.
				SetPlayerLowSprint(false);
				SetPlayerSprinting(false);

				// Update weapon animations.
				if (pWeaponBase)
					pWeaponBase->SendWeaponAnim(ACT_VM_IDLE);
			}
		}
	}

	// If we are low on sprint and we now have enough sprint, update our state.
	if (IsSprintLow() && GetSprint() > (GetMaxSprint() * .15))
		SetPlayerLowSprint(false);
}

void CForsakenPlayer::OnBulletHit(bool bDoEffects, CBaseEntity *pentAttacker, float fDamage, 
	float fDistance, int nBulletType, trace_t *pTrace, Vector vecDirection, Vector vecStartPos)
/*
	
	Pre: 
	Post: 
*/
{
	int nDamageType = DMG_BULLET | DMG_NEVERGIB;
	CTakeDamageInfo dmgInfo(pentAttacker, pentAttacker, fDamage, nDamageType);

#ifndef CLIENT_DLL
	// Forsaken AURA: Rel leader pain threshold.
	if (GetAura() == RelAura_PainThreshold)
		dmgInfo.SetDamage(dmgInfo.GetDamage() * 0.75f);
#endif

	// Weaken the damage based on distance.
	fDamage *= pow(0.85f, (fDistance / 500));

	// Show impact points?
	if (sv_showimpacts.GetBool())
	{
#ifdef CLIENT_DLL
		debugoverlay->AddBoxOverlay(pTrace->endpos, Vector(-2,-2,-2), Vector(2,2,2), QAngle(0, 0, 0), 
			255, 0, 0,127, 4);

		if (pTrace->m_pEnt && pTrace->m_pEnt->IsPlayer())
		{
			C_BasePlayer *player = ToBasePlayer(pTrace->m_pEnt);

			player->DrawClientHitboxes(4, true);
		}
#else
		NDebugOverlay::Box(pTrace->endpos, Vector(-2,-2,-2), Vector(2,2,2), 0, 0, 255, 127, 4);

		if (pTrace->m_pEnt && pTrace->m_pEnt->IsPlayer())
		{
			CBasePlayer *player = ToBasePlayer(pTrace->m_pEnt);

			player->DrawServerHitboxes(4, true);
		}
#endif
	}

	// Are we doing effects?
	if (bDoEffects)
	{
		// Water effect?
		if (enginetrace->GetPointContents(pTrace->endpos) & (CONTENTS_WATER | CONTENTS_SLIME))
		{
			trace_t trWater;

			UTIL_TraceLine(vecStartPos, pTrace->endpos, (MASK_SHOT | CONTENTS_WATER | CONTENTS_SLIME), 
				this, COLLISION_GROUP_NONE, &trWater);

			if (trWater.allsolid != 1)
			{
				CEffectData data;

				data.m_vOrigin = trWater.endpos;
				data.m_vNormal = trWater.plane.normal;
				data.m_flScale = random->RandomFloat(8, 12);

				if (trWater.contents & CONTENTS_SLIME)
					data.m_fFlags |= FX_WATER_IN_SLIME;

				DispatchEffect("gunshotsplash", data);
			}
		}
		// Do nothing to no-draw surfaces.
		else if (!(pTrace->surface.flags & (SURF_SKY | SURF_NODRAW | SURF_HINT | SURF_SKIP)))
		{
			CBaseEntity *pEntity = pTrace->m_pEnt;

			if (!(!friendlyfire.GetBool() && pEntity && pEntity->IsPlayer() && 
				pEntity->GetTeamNumber() == GetTeamNumber()))
				UTIL_ImpactTrace(pTrace, nDamageType);
		}
	}

	ClearMultiDamage();

	CalculateBulletDamageForce(&dmgInfo, nBulletType, vecDirection, pTrace->endpos);
	pTrace->m_pEnt->DispatchTraceAttack(dmgInfo, vecDirection, pTrace);

	ApplyMultiDamage();

#ifdef GAME_DLL
	TraceAttackToTriggers(dmgInfo, pTrace->startpos, pTrace->endpos, vecDirection);
#endif
}

void CForsakenPlayer::SetPlayerLowSprint(bool bState)
/*
	
	Pre: 
	Post: 
*/
{
	// Is this a state change?
	if (m_bIsLowSprint != bState)
	{
		m_bIsLowSprint = bState;

#ifndef CLIENT_DLL
		// Do we need to emit or stop the sound?
		if (bState)
			EmitSound("Player.SprintNoPower");
		else
			StopSound("Player.SprintNoPower");
#endif
	}
}

void CForsakenPlayer::SetPlayerSprinting(bool bState)
/*
	
	Pre: 
	Post: 
*/
{
	// Is this a state change?
	if (m_bIsSprinting != bState)
	{
		m_bIsSprinting = bState;

#ifndef CLIENT_DLL
		// Do we need to emit or stop the sound?
		if (bState)
			EmitSound("Player.Sprint");
		else
			StopSound("Player.Sprint");
#endif
	}
}