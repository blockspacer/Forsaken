/*
	forsaken_grenade_molotov.cpp
	Forsaken weapon code for the molotov cocktail.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_grenade_base.h"
#include "gamerules.h"
#include "engine/IEngineSound.h"
#include "forsaken_grenade_molotov.h"

#ifndef CLIENT_DLL
#include "fire.h"
#include "forsaken/forsaken_player.h"
#include "items.h"
#include "forsaken_grenade_projectile.h"
#include "soundent.h"
#include "EntityFlame.h"
#include "props.h"
#endif

#define DEG_TO_RAD 0.0174532925199444

extern short	g_sModelIndexFireball;

void CreateEntityFlame(CBaseEntity *pEnt);

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenGrenadeMolotov, DT_ForsakenGrenadeMolotov)

BEGIN_NETWORK_TABLE(CForsakenGrenadeMolotov, DT_ForsakenGrenadeMolotov)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenGrenadeMolotov)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_grenade_molotov, CForsakenGrenadeMolotov);
PRECACHE_WEAPON_REGISTER(forsaken_grenade_molotov);

#ifndef CLIENT_DLL
// Internal Classes
class CForsakenGrenadeMolotovProjectile : public CPhysicsProp
{
public:
	DECLARE_CLASS(CForsakenGrenadeMolotovProjectile, CPhysicsProp);

	// Public Functions
	static CForsakenGrenadeMolotovProjectile *Create(Vector vecSource, QAngle angles, 
		Vector vecVelocity, AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime)
	{
		CForsakenGrenadeMolotovProjectile *pNewGrenade = 
			(CForsakenGrenadeMolotovProjectile*)CBaseEntity::Create("forsaken_grenade_projectile_molotov", 
			vecSource, angles, pPlayer);

		pNewGrenade->SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM);
		pNewGrenade->SetAbsVelocity(vecVelocity);
		pNewGrenade->m_vecInitialVelocity = vecVelocity;
		pNewGrenade->ChangeTeam(pPlayer->GetTeamNumber());
		pNewGrenade->ApplyLocalAngularVelocityImpulse(angImpulse);
		pNewGrenade->SetOwnerEntity(pPlayer);
		
		return pNewGrenade;
	}

	virtual void Detonate(trace_t &rTrace, Vector &rvecVelocity)
	{
		// Break the bottle into pieces
		Break( this, CTakeDamageInfo() );

		// Move out from whatever we hit that made Molotov explode
		trace_t trace;

		if (rTrace.plane.normal.y > 0.5f || rTrace.plane.normal.y < -0.5f || rTrace.plane.normal.x > 0.5f || rTrace.plane.normal.x < -0.5f)
		{
			// If we hit a mostly vertical wall, don't move out as much
			UTIL_TraceLine ( rTrace.endpos, rTrace.endpos + (rTrace.plane.normal * 32),  MASK_SOLID_BRUSHONLY, 
				this, COLLISION_GROUP_NONE, &trace);
		}
		else
		{	// For more horizontal things, move out a lot
			UTIL_TraceLine ( rTrace.endpos, rTrace.endpos + (rTrace.plane.normal * 128),  MASK_SOLID_BRUSHONLY, 
				this, COLLISION_GROUP_NONE, &trace);
		}

		//debugoverlay->AddLineOverlay(rTrace.endpos, trace.endpos, 0, 0, 255, false, 5.0f);

		// Set the final position. If we hit something else, pick the half way point
		Vector finalPosition = trace.endpos;
		if ( trace.fraction != 1.0 )
		{
			Vector	ofsDir = ( rTrace.endpos - trace.endpos );
			float	length = VectorLength( ofsDir ) / 2.0f;
			finalPosition = trace.endpos + (trace.plane.normal * length);
			//debugoverlay->AddLineOverlay(trace.endpos, finalPosition, 0, 255, 0, false, 10.0f);
		}

		int contents = UTIL_PointContents ( GetAbsOrigin() );

		// Ignore water
		if ( (contents & MASK_WATER) )
		{
			UTIL_Remove( this );
			return;
		}

		EmitSound("Weapon_Forsaken_Molotov.Explode");

		// Start some fires
		int i, fireCount = 0, maxTries = 500;
		QAngle vecTraceAngles;
		Vector vecTraceDir;
		trace_t firetrace;

		for( i = 0 ; fireCount < 25 && i < maxTries ; i++)
		{
			// build a little ray
			vecTraceAngles[PITCH]	= random->RandomFloat(0, 180);
			vecTraceAngles[YAW]		= random->RandomFloat(0, 360);
			vecTraceAngles[ROLL]	= 0.0f;

			AngleVectors( vecTraceAngles, &vecTraceDir );

			Vector vecStart, vecEnd;

			vecStart = finalPosition;
			vecEnd = vecStart + vecTraceDir * 256;

			UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, 
				&firetrace );

			int contents = UTIL_PointContents ( firetrace.endpos );

			// Ignore water
			if ( (contents & MASK_WATER) )
			{
				continue;
			}
			if( firetrace.fraction == 1.0 )
			{
				//if we didn't hit a surface, no need to continue
				continue;
			}
			
			//debugoverlay->AddLineOverlay(vecStart, firetrace.endpos, 255, 0, 0, false, 5.0f);

			// Get length of the trace
			Vector	ofsDir = ( firetrace.endpos - vecStart );
			float	length = VectorLength( ofsDir );

			// Check length
			if ( length < 0 )
				length = 0;
			if ( length > 256 )
				length = 256;

			//Get our scale based on distance
			float scale	 = 70.00f + (70.0f * (1.0f - (length / 256.0f)));
			float growth = 0.5f + (0.75f * (length / 256.0f));

			// Make the fire and scorch the surface
			bool fireStarted = FireSystem_StartFire( firetrace.endpos, scale, growth, RandomFloat(10.0f, 15.0f), 
				(SF_FIRE_START_ON | SF_FIRE_SMOKELESS | SF_FIRE_DONT_DROP | SF_FIRE_DIE_PERMANENT), GetOwnerEntity(), FIRE_NATURAL );

			if (fireStarted)
			{
				fireCount++;
				UTIL_DecalTrace( &firetrace, "Scorch" );
			}
		}
		// End Start some fires
	}

	virtual void Precache()
	{
		PrecacheModel("models/weapons/w_molotov_thrown.mdl");
		
		PrecacheScriptSound( "Weapon_Forsaken_Molotov.Explode" );

		BaseClass::Precache();
	}

	virtual void Spawn()
	{
		Precache();

		SetModel("models/weapons/w_molotov_thrown.mdl");

		BaseClass::Spawn();
	}

	virtual void ResolveFlyCollisionCustom(trace_t &rTrace, Vector &rvecVelocity)
	{
		bool bBreakthrough = false;

		if (rTrace.m_pEnt && (FClassnameIs(rTrace.m_pEnt, "func_breakable") || 
			FClassnameIs(rTrace.m_pEnt, "func_breakable_surf")) && RandomInt(1, 10) > 6)
		{
			bBreakthrough = true;
		}

		if (bBreakthrough)
		{
			rTrace.m_pEnt->DispatchTraceAttack(CTakeDamageInfo(this, GetOwnerEntity(), 10, DMG_CLUB), GetAbsVelocity(), &rTrace);
			ApplyMultiDamage();

			// Update the velocity.
			if (rTrace.m_pEnt->m_iHealth <= 0)
			{
				SetAbsVelocity(GetAbsVelocity() * 0.4f);
				
				return;
			}
			else
			{
				Detonate(rTrace, rvecVelocity);
			}
		}
		else
		{
			Detonate(rTrace, rvecVelocity);
		}
	}

private:
	Vector m_vecInitialVelocity;
};

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_grenade_projectile_molotov, CForsakenGrenadeMolotovProjectile);
PRECACHE_WEAPON_REGISTER(forsaken_grenade_projectile_molotov);

BEGIN_DATADESC(CForsakenGrenadeMolotov)
END_DATADESC()

// Functions
void CForsakenGrenadeMolotov::EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, 
	AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime)
{
	if(pPlayer != NULL)
	{
		pPlayer->StopSound("Weapon_Forsaken_Molotov.BurnIdle");
	}

	CForsakenGrenadeMolotovProjectile::Create(vecSource, angles, vecVelocity, angImpulse, pPlayer, fTime);
}
#endif
