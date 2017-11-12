/*
	forsaken_grenade_projectile.cpp
	The projectile entity for grenades.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_grenade_projectile.h"

#ifdef CLIENT_DLL
#include "forsaken/c_forsaken_player.h"
#endif

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenGrenadeProjectile, DT_ForsakenGrenadeProjectile)

BEGIN_NETWORK_TABLE(CForsakenGrenadeProjectile, DT_ForsakenGrenadeProjectile)
#ifdef CLIENT_DLL
	RecvPropVector(RECVINFO(m_vecInitialVelocity))
#else
	SendPropVector(SENDINFO(m_vecInitialVelocity), 20, 0, -3000, 3000)
#endif
END_NETWORK_TABLE()

#ifndef CLIENT_DLL
BEGIN_DATADESC(CForsakenGrenadeProjectile)
	DEFINE_THINKFUNC(DetonationThink)
END_DATADESC()
#endif

// Global Variable Decleration
extern ConVar sv_gravity;

// Functions
#ifdef CLIENT_DLL
int CForsakenGrenadeProjectile::DrawModel(int nFlags)
/*
	
	Pre: 
	Post: 
*/
{
	if (GetThrower() != C_BasePlayer::GetLocalPlayer())
	{
		if (gpGlobals->curtime - m_fSpawnTime < 0.5f)
		{
			C_ForsakenPlayer *pPlayer = dynamic_cast<C_ForsakenPlayer*>(GetThrower());

			if (pPlayer && pPlayer->m_statePlayerAnimation->IsThrowingGrenade())
				return 0;
		}
	}

	return BaseClass::DrawModel(nFlags);
}

void CForsakenGrenadeProjectile::PostDataUpdate(DataUpdateType_t dutUpdate)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::PostDataUpdate(dutUpdate);

	if (dutUpdate == DATA_UPDATE_CREATED)
	{
		CInterpolatedVar<Vector> &rInterpolation = GetOriginInterpolator();
		float fChangeTime = GetLastChangeTime(LATCH_SIMULATION_VAR);
		Vector vecSample1, vecSample2;

		rInterpolation.ClearHistory();

		// Setup two samples.
		vecSample1 = GetLocalOrigin() - m_vecInitialVelocity;
		vecSample2 = GetLocalOrigin();

		// Add the two samples.
		rInterpolation.AddToHead(fChangeTime - 1.0, &vecSample1, false);
		rInterpolation.AddToHead(fChangeTime, &vecSample2, false);
	}
}

void CForsakenGrenadeProjectile::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	m_fSpawnTime = gpGlobals->curtime;

	BaseClass::Spawn();
}
#else
void CForsakenGrenadeProjectile::DetonationThink(void)
/*
	
	Pre: 
	Post: 
*/
{
	if (m_flDetonateTime >= gpGlobals->curtime)
		Detonate();
}

void CForsakenGrenadeProjectile::ResolveFlyCollisionCustom(trace_t &rTrace, Vector &rvecVelocity)
/*
	
	Pre: 
	Post: 
*/
{
	bool bBreakthrough = false;
	float fElasticity = 1.0f, fSpeed = 0.0f, fTotalElasticity = 0.0f;
	Vector vecAbsVelocity;

	// If it isn't a player it isn't fully elastic.
	if (rTrace.m_pEnt && rTrace.m_pEnt->IsPlayer())
		fElasticity = 0.3f;

	if (rTrace.m_pEnt && (FClassnameIs(rTrace.m_pEnt, "func_breakable") || 
						  FClassnameIs(rTrace.m_pEnt, "func_breakable_surf")))
		bBreakthrough = true;

	if (bBreakthrough)
	{
		CTakeDamageInfo dmgInfo(this, this, 10, DMG_CLUB);

		rTrace.m_pEnt->DispatchTraceAttack(dmgInfo, GetAbsVelocity(), &rTrace);
		ApplyMultiDamage();

		// Update the velocity.
		if (rTrace.m_pEnt->m_iHealth <= 0)
		{
			SetAbsVelocity(GetAbsVelocity() * 0.4f);

			return;
		}
	}

	// Clamp the elasticity.
	fTotalElasticity = clamp(GetElasticity() * fElasticity, 0.0f, 0.9f);

	// Clip the velocity.
	PhysicsClipVelocity(GetAbsVelocity(), rTrace.plane.normal, vecAbsVelocity, 2.0f);

	// Factor in elasticity.
	vecAbsVelocity *= fTotalElasticity;

	// Add in the base velocity and our calculated velocity.
	VectorAdd(vecAbsVelocity, GetBaseVelocity(), rvecVelocity);

	// Calculate the square of the speed.
	fSpeed = DotProduct(rvecVelocity, rvecVelocity);

	if (rTrace.plane.normal.z > 0.7f)
	{
		SetAbsVelocity(vecAbsVelocity);

		if (fSpeed < 900)
		{
			QAngle angle;

			if (rTrace.m_pEnt->IsStandable())
				SetGroundEntity(rTrace.m_pEnt);

			SetAbsVelocity(vec3_origin);
			SetLocalAngularVelocity(vec3_angle);

			VectorAngles(rTrace.plane.normal, angle);
			angle[1] = random->RandomFloat(0, 360);
			SetAbsAngles(angle);
		}
		else
		{
			Vector vecDirection = GetBaseVelocity(), vecDelta = vecDirection - vecAbsVelocity;
			float fScale;

			VectorNormalize(vecDirection);

			fScale = vecDelta.Dot(vecDirection);

			VectorScale(vecAbsVelocity, (1.0f - rTrace.fraction) * gpGlobals->frametime, rvecVelocity);
			VectorMA(rvecVelocity, (1.0f - rTrace.fraction) * gpGlobals->frametime, vecDirection * fScale, rvecVelocity);
			PhysicsPushEntity(rvecVelocity, &rTrace);
		}
	}
	else
	{
		if (fSpeed < 900)
		{
			SetAbsVelocity(vec3_origin);
			SetLocalAngularVelocity(vec3_angle);
		}
		else
			SetAbsVelocity(vecAbsVelocity);
	}

	EmitSound( "Grenade.Bounce" );
	//BounceSound();
}

void CForsakenGrenadeProjectile::SetupInterpolationHistory(const Vector &rvecVelocity)
{
	m_vecInitialVelocity = rvecVelocity;
}

void CForsakenGrenadeProjectile::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Spawn();

	SetSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM);
	SetSolid(SOLID_BBOX);
	SetSize(Vector(-2, -2, -2), Vector(2, 2, 2));
}
#endif