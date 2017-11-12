/*
	forsaken_weapon_base.cpp
	Base weapon class used for all Forsaken weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "in_buttons.h"
#include "takedamageinfo.h"
#include "forsaken_weapon_base.h"
#include "ammodef.h"

#ifdef CLIENT_DLL
#include "forsaken/c_forsaken_player.h"
#include "eventlist.h"
#include "fx.h"
#include "particles_localspace.h"
#include "tier0/vprof.h"
#else
#include "ilagcompensationmanager.h"
#include "te_effect_dispatch.h"
#include "forsaken/forsaken_player.h"
#endif

#include "effect_dispatch_data.h"

// Global Variable Declarations
static const char *g_szWeaponAliases[] =
{
	"None",
	"1201fp",
	"hk23",
	"dualm10",
	"sa80",
	"fraggren",
	"ragingbull",
	"dualp99",
	"molotov",
	"710",
	"rpg",
	"revolver",
	"870remington",
	"m8rstun",
	NULL
};
static const Vector g_bludgeonMins(-BLUDGEON_HULL_DIM, -BLUDGEON_HULL_DIM, -BLUDGEON_HULL_DIM);
static const Vector g_bludgeonMaxs(BLUDGEON_HULL_DIM, BLUDGEON_HULL_DIM, BLUDGEON_HULL_DIM);

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponBase, DT_ForsakenWeaponBase)

BEGIN_NETWORK_TABLE(CForsakenWeaponBase, DT_ForsakenWeaponBase)
#ifdef CLIENT_DLL
	RecvPropBool(RECVINFO(m_bIsLeaderWeapon)),
	RecvPropBool(RECVINFO(m_bKillDrop)),
	RecvPropFloat(RECVINFO(m_fReloadEndTime)),
#else
	SendPropBool(SENDINFO(m_bIsLeaderWeapon)),
	SendPropBool(SENDINFO(m_bKillDrop)),
	SendPropFloat(SENDINFO(m_fReloadEndTime)),
//	SendPropExclude("DT_AnimTimeMustBeFirst", "m_flAnimTime"),
//	SendPropExclude("DT_BaseAnimating", "m_nSequence"),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CForsakenWeaponBase)
	DEFINE_PRED_FIELD(m_flTimeWeaponIdle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_NOERRORCHECK)
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS(forsaken_weapon_base, CForsakenWeaponBase);

// Constructor & Deconstructor
CForsakenWeaponBase::CForsakenWeaponBase()
{
	SetPredictionEligible(true);

	AddSolidFlags(FSOLID_TRIGGER);

	m_bAttackDisabled = false;
	m_bKillDrop = false;
}

CForsakenWeaponBase::~CForsakenWeaponBase()
{
}

// Functions
// CForsakenWeaponBase
bool CForsakenWeaponBase::CanFire()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());

	if (!pPlayer)
		return false;

	// If the attack button has been released and it's after the attack time, re-enable fire.
	if (SemiAuto() && pPlayer->m_afButtonReleased & IN_ATTACK)
		m_bAttackDisabled = false;
	else if (!SemiAuto())
		m_bAttackDisabled = false;

	return !m_bAttackDisabled;
}

bool CForsakenWeaponBase::CanHolster()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());

	if (pPlayer && ((gpGlobals->curtime > pPlayer->m_fKickEndTime) || (!pPlayer->IsKicking())))
		return true;
	else
		return false;
}

bool CForsakenWeaponBase::Deploy()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());
	if (!pPlayer)
		return false;

	// Reset the number of shots fired.
	pPlayer->m_nShotsFired = 0;

	if (!BaseClass::Deploy())
		return false;

	CBaseViewModel *pViewModel = pPlayer->GetViewModel();
	if (!pViewModel)
		return false;

	switch (pPlayer->GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		// Civilians have the no-glove skin (2nd in the list).
		pViewModel->m_nSkin = 1;
		break;

	case TEAM_GOVERNORS:
		// Governors have the glove skin (1st in the list).
		pViewModel->m_nSkin = 0;
		break;

	case TEAM_RELIGIOUS:
		// Religious have their own spiffy skin (3rd in the list).
		pViewModel->m_nSkin = 2;
		break;
	}

	return true;
}

bool CForsakenWeaponBase::DidImpactWater(const Vector &start, const Vector &end)
/*
	
	Pre: 
	Post: 
*/
{
	// We must start outside the water
	if (UTIL_PointContents(start) & (CONTENTS_WATER | CONTENTS_SLIME))
		return false;

	// We must end inside of water
	if (!(UTIL_PointContents(end) & (CONTENTS_WATER | CONTENTS_SLIME)))
		return false;

	trace_t	waterTrace;

	UTIL_TraceLine(start, end, (CONTENTS_WATER | CONTENTS_SLIME), GetOwner(), 
		COLLISION_GROUP_NONE, &waterTrace);

	if (waterTrace.fraction < 1.0f)
	{
		CEffectData	data;

		data.m_fFlags  = 0;
		data.m_vOrigin = waterTrace.endpos;
		data.m_vNormal = waterTrace.plane.normal;
		data.m_flScale = 8.0f;

		// See if we hit slime
		if (waterTrace.contents & CONTENTS_SLIME)
			data.m_fFlags |= FX_WATER_IN_SLIME;

#ifdef GAME_DLL
		DispatchEffect("watersplash", data);
#endif
	}

	return true;
}

bool CForsakenWeaponBase::PlayEmptySound()
/*
	
	Pre: 
	Post: 
*/
{
	return true;
}

CForsakenPlayer *CForsakenWeaponBase::GetPlayerOwner() const
/*
	
	Pre: 
	Post: 
*/
{
	return dynamic_cast<CForsakenPlayer*>(GetOwner());
}

char *CForsakenWeaponBase::GetDeathNoticeName()
/*
	
	Pre: 
	Post: 
*/
{
#ifndef CLIENT_DLL
	return const_cast<char *>(GetForsakenWeaponData().szClassName);
#else
	return "unknown";
#endif
}

const CForsakenWeaponInfo &CForsakenWeaponBase::GetForsakenWeaponData() const
/*
	
	Pre: 
	Post: 
*/
{
	return *dynamic_cast<const CForsakenWeaponInfo*>(&GetWpnData());
}

const Vector &CForsakenWeaponBase::GetBulletSpread(ForsakenMovementID moveType)
/*
	
	Pre: 
	Post: 
*/
{
	static Vector retVal = VECTOR_CONE_1DEGREES;

	return retVal;
}

void CForsakenWeaponBase::AlterPlayerAccuracy(ForsakenMovementID moveType, Vector &rvecAccuracy)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());

	if (!pPlayer)
		return;

	// Governors get a 25% accuracy increase if they are standing still or crouched.
	if (pPlayer->GetTeamNumber() == TEAM_GOVERNORS && 
		(moveType == FORSAKEN_MOVEMENT_NONE || moveType == FORSAKEN_MOVEMENT_CROUCH))
		rvecAccuracy -= rvecAccuracy * 0.25f;
}

void CForsakenWeaponBase::AlterPlayerRecoil(ForsakenMovementID moveType, QAngle &rangRecoil)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());

	if (!pPlayer)
		return;

	// Governors get a 25% accuracy increase if they are standing still or crouched.
	if (pPlayer->GetTeamNumber() == TEAM_GOVERNORS && 
		(moveType == FORSAKEN_MOVEMENT_NONE || moveType == FORSAKEN_MOVEMENT_CROUCH))
		rangRecoil -= rangRecoil * 0.25f;
}

void CForsakenWeaponBase::ChooseIntersectionPoint(trace_t &hitTrace, const Vector &mins, 
	const Vector &maxs, CBasePlayer *pOwner)
/*
	
	Pre: 
	Post: 
*/
{
	int			i, j, k;
	float		distance = 1e6f;
	const float	*minmaxs[2] = {mins.Base(), maxs.Base()};
	trace_t		tmpTrace;
	Vector		vecHullEnd = hitTrace.endpos;
	Vector		vecEnd;
	Vector vecSrc = hitTrace.startpos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine(vecSrc, vecHullEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace);

	if ( tmpTrace.fraction == 1.0 )
	{
		for ( i = 0; i < 2; i++ )
		{
			for ( j = 0; j < 2; j++ )
			{
				for ( k = 0; k < 2; k++ )
				{
					vecEnd.x = vecHullEnd.x + minmaxs[i][0];
					vecEnd.y = vecHullEnd.y + minmaxs[j][1];
					vecEnd.z = vecHullEnd.z + minmaxs[k][2];

					UTIL_TraceLine( vecSrc, vecEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &tmpTrace );
					if ( tmpTrace.fraction < 1.0 )
					{
						float thisDistance = (tmpTrace.endpos - vecSrc).Length();

						if (thisDistance < distance)
						{
							hitTrace = tmpTrace;
							distance = thisDistance;
						}
					}
				}
			}
		}
	}
	else
		hitTrace = tmpTrace;
}

void CForsakenWeaponBase::DoKick()
/*
	
	Pre: 
	Post: 
*/
{
	Activity hitActivity;
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (!pPlayer)
		return;

	hitActivity = PerformMeleeAttack(30.0f, 50.0f, DMG_CLUB, false);

	switch (hitActivity)
	{
	case ACT_VM_MELEE_SLASH:
	case ACT_VM_MELEE_STAB:
		WeaponSound(KICK_HIT);
		break;

	case ACT_VM_MELEE_MISS:
	case ACT_VM_MELEE_STAB_MISS:
		WeaponSound(KICK_MISS);
		break;
	}
}

void CForsakenWeaponBase::ImpactEffect(trace_t &traceHit, int nDamageType)
/*
	
	Pre: 
	Post: 
*/
{
	if (DidImpactWater(traceHit.startpos, traceHit.endpos))
		return;

	UTIL_ImpactTrace(&traceHit, nDamageType);
}

void CForsakenWeaponBase::ItemPreFrame()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(GetOwner());

	if (!pPlayer)
		return;

	// If the attack button has been released and it's after the attack time, re-enable fire.
	if (pPlayer->m_afButtonReleased & IN_ATTACK)
		m_bAttackDisabled = false;

	BaseClass::ItemPreFrame();
}

void CForsakenWeaponBase::MeleeHit(float fDamage, trace_t &traceHit, int nDamageType)
/*
	
	Pre: 
	Post: 
*/
{
	CBaseEntity	*pHitEntity = traceHit.m_pEnt;
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	//Apply damage to a hit target
	if (pHitEntity != NULL)
	{
		Vector hitDirection;

		pPlayer->EyeVectors(&hitDirection, NULL, NULL);
		VectorNormalize(hitDirection);

#ifndef CLIENT_DLL
		CTakeDamageInfo info(GetOwner(), GetOwner(), fDamage, nDamageType);

		CalculateMeleeDamageForce(&info, hitDirection, traceHit.endpos);
		pHitEntity->DispatchTraceAttack(info, hitDirection, &traceHit);
		ApplyMultiDamage();
		TraceAttackToTriggers(info, traceHit.startpos, traceHit.endpos, hitDirection);
#endif

		ImpactEffect(traceHit, nDamageType);

		m_bMeleeHit = true;
	}
}

Activity CForsakenWeaponBase::PerformMeleeAttack(float fDamage, float fRange, int nDamageType, bool bSlash)
/*
	
	Pre: 
	Post: 
*/
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner)
		return bSlash ? ACT_VM_MELEE_MISS : ACT_VM_MELEE_STAB_MISS;

#if !defined (CLIENT_DLL)
	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation(pOwner, pOwner->GetCurrentCommand());
#endif

	trace_t traceHit;
	Vector swingStart = pOwner->Weapon_ShootPosition();
	Vector forward;

	pOwner->EyeVectors( &forward, NULL, NULL );

	Vector swingEnd = swingStart + (forward * fRange);

	UTIL_TraceLine(swingStart, swingEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit);

	if (traceHit.fraction == 1.0)
	{
		swingEnd -= forward * 1.732f * BLUDGEON_HULL_DIM;

		UTIL_TraceHull(swingStart, swingEnd, g_bludgeonMins, g_bludgeonMaxs, MASK_SHOT_HULL, 
			pOwner, COLLISION_GROUP_NONE, &traceHit);

		if (traceHit.fraction < 1.0 && traceHit.m_pEnt)
		{
			Vector vecToTarget = traceHit.m_pEnt->GetAbsOrigin() - swingStart;

			VectorNormalize(vecToTarget);

			float dot = vecToTarget.Dot(forward);

			if (dot < 0.70721f)
				traceHit.fraction = 1.0f;
			else
				ChooseIntersectionPoint(traceHit, g_bludgeonMins, g_bludgeonMaxs, pOwner);
		}
	}

#if !defined (CLIENT_DLL)
	lagcompensation->FinishLagCompensation(pOwner);
#endif

	if (traceHit.fraction < 1.0f)
	{
		MeleeHit(fDamage, traceHit, nDamageType);

		if (bSlash)
			return ACT_VM_MELEE_SLASH;
		else
			return ACT_VM_MELEE_STAB;
	}

	return bSlash ? ACT_VM_MELEE_MISS : ACT_VM_MELEE_STAB_MISS;
}

void CForsakenWeaponBase::PerformRecoil(bool bSnapEyeAngles)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = GetPlayerOwner();
	QAngle angDeltaPunch, angSnapPunch;

	if (!pPlayer)
		return;

	// Get the delta punch angle depending on movement type.
	angDeltaPunch = GetRecoilAmount(pPlayer->GetMovementType());

	// Set the snap view punch angle and half it.
	angSnapPunch = pPlayer->GetLocalAngles();
	angSnapPunch.x += angDeltaPunch.x / 2;
	angSnapPunch.y += angDeltaPunch.y / 2;
	angSnapPunch.z = 0;

	// Snap our eye angles.
#ifndef CLIENT_DLL
	if (bSnapEyeAngles)
		pPlayer->SnapEyeAngles(angSnapPunch);
#endif

	// Set the new punch angle.
	pPlayer->ViewPunch(angDeltaPunch);
}

void CForsakenWeaponBase::ProcessMuzzleFlashEvent()
/*
	
	Pre: 
	Post: 
*/
{
#ifdef CLIENT_DLL
	CForsakenPlayer *pPlayer = CForsakenPlayer::GetLocalForsakenPlayer();

	if (pPlayer && pPlayer->IsObserver() && pPlayer->GetObserverMode() == OBS_MODE_IN_EYE 
		&& pPlayer->GetObserverTarget() && pPlayer->GetObserverTarget() == GetOwner())
		return;
	else
		BaseClass::ProcessMuzzleFlashEvent();
#endif
}

#ifndef CLIENT_DLL
void CForsakenWeaponBase::SendReloadEvents()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;

	CPASFilter filter(pPlayer->GetAbsOrigin());

	filter.RemoveRecipient(pPlayer);

	/*UserMessageBegin(filter, "ReloadEffect");
	WRITE_SHORT(pPlayer->entindex());
	MessageEnd();*/

	pPlayer->DoAnimationEvent(FORSAKEN_PLAYERANIMEVENT_RELOAD);
}
#endif

// Global
const char *WeaponIDToAlias(int nWeaponID)
/*
	
	Pre: 
	Post: 
*/
{
	if (nWeaponID >= FORSAKEN_WEAPON_MAX || nWeaponID < FORSAKEN_WEAPON_NONE)
		return NULL;

	return g_szWeaponAliases[nWeaponID];
}

int AliasToWeaponID(const char *czAlias)
/*
	
	Pre: 
	Post: 
*/
{
	if (czAlias)
	{
		for (int i = 0; g_szWeaponAliases[i] != NULL; i++)
		{
			if (Q_stricmp(g_szWeaponAliases[i], czAlias) != 0)
				return i;
		}
	}

	return -1;
}