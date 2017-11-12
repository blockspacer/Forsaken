/*
	forsaken_grenade_m84.cpp
	Forsaken weapon code for the M84 stun grenade.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_grenade_base.h"
#include "gamerules.h"
#include "engine/IEngineSound.h"
#include "forsaken_grenade_m84.h"

#ifndef CLIENT_DLL
#include "recipientfilter.h"
#include "forsaken/forsaken_player.h"
#include "items.h"
#include "forsaken_grenade_projectile.h"
#endif

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenGrenadeM84Stun, DT_ForsakenGrenadeM84Stun)

BEGIN_NETWORK_TABLE(CForsakenGrenadeM84Stun, DT_ForsakenGrenadeM84Stun)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenGrenadeM84Stun)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_grenade_m84stun, CForsakenGrenadeM84Stun);
PRECACHE_WEAPON_REGISTER(forsaken_grenade_m84stun);

#ifndef CLIENT_DLL
// Internal Classes
class CForsakenGrenadeM84StunProjectile : public CForsakenGrenadeProjectile
{
public:
	DECLARE_CLASS(CForsakenGrenadeM84StunProjectile, CForsakenGrenadeProjectile);

	// Public Accessor Functions
	virtual float GetShakeAmplitude(void) { return 5.0f; }

	// Public Functions
	static CForsakenGrenadeM84StunProjectile *Create(Vector vecSource, QAngle angles, 
		Vector vecVelocity, AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTimer)
	{
		CForsakenGrenadeM84StunProjectile *pNewGrenade = 
			(CForsakenGrenadeM84StunProjectile*)CBaseEntity::Create("forsaken_grenade_projectile_m84stun", 
			vecSource, angles, pPlayer);

		pNewGrenade->m_flDetonateTime = gpGlobals->curtime + fTimer;
		pNewGrenade->SetAbsVelocity(vecVelocity);
		pNewGrenade->SetupInterpolationHistory(vecVelocity);
		pNewGrenade->SetThrower(pPlayer);
		pNewGrenade->SetGravity(BaseClass::GetGrenadeGravity());
		pNewGrenade->SetFriction(BaseClass::GetGrenadeFriction());
		pNewGrenade->SetElasticity(BaseClass::GetGrenadeElasticity());
		pNewGrenade->m_flDamage = 200.0f;
		pNewGrenade->m_DmgRadius = pNewGrenade->m_flDamage * 3.0f;
		pNewGrenade->ChangeTeam(pPlayer->GetTeamNumber());
		pNewGrenade->ApplyLocalAngularVelocityImpulse(angImpulse);

		pNewGrenade->SetThink(&CForsakenGrenadeProjectile::DetonationThink);
		pNewGrenade->SetNextThink(pNewGrenade->m_flDetonateTime);

		return pNewGrenade;
	}

	virtual void Detonate();

	virtual void Precache()
	{
		PrecacheModel("models/weapons/w_m84stun_thrown.mdl");

		BaseClass::Precache();
	}

	virtual void Spawn()
	{
		Precache();

		SetModel("models/weapons/w_m84stun_thrown.mdl");

		BaseClass::Spawn();
	}
};

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_grenade_projectile_m84stun, CForsakenGrenadeM84StunProjectile);
PRECACHE_WEAPON_REGISTER(forsaken_grenade_projectile_m84stun);

BEGIN_DATADESC(CForsakenGrenadeM84Stun)
END_DATADESC()

// Function Prototypes
void TE_DynamicLight( IRecipientFilter& filter, float delay, const Vector* org, 
	int r, int g, int b, int exponent, float radius, float time, float decay );

// Functions
// CForsakenGrenade
void CForsakenGrenadeM84StunProjectile::Detonate()
{
	CBaseEntity *ent = NULL;
	CPVSFilter filter(GetAbsOrigin());
	int nFlashMask = CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS;

	// Broadcast a white light for those that will be unaffected by the stun.
	TE_DynamicLight(filter, 0.0f, &GetAbsOrigin(), 255, 255, 255, 5, 1024, 0.5, 
		20480);

	for (CEntitySphereQuery sphere(GetAbsOrigin(), FORSAKEN_M84STUN_FADERADIUS_MAX); 
		(ent = sphere.GetCurrentEntity()) != NULL; sphere.NextEntity())
	{
		CBaseEntity *pEnt = ent;

		// Set the player to a stunned state.
		if (pEnt && pEnt->IsPlayer())
		{
			bool bIsHidden = false;
			CForsakenPlayer *pPlayer = ToForsakenPlayer(pEnt);
			float fDistance = pEnt->GetAbsOrigin().DistTo(GetAbsOrigin());
			float fFadeDist = fDistance - FORSAKEN_M84STUN_FADERADIUS_MIN;
			float fPercentage = 1.0f;
			trace_t	tr;
			Vector vecToPlayer = pEnt->BodyTarget(GetAbsOrigin(), false);

			// Are we in the 100% blind radius (min fade radius)?
			if (fDistance < FORSAKEN_M84STUN_FADERADIUS_MIN)
			{
				// Do a trace to see if there is anything blocking our way.
				UTIL_TraceLine(GetAbsOrigin(), vecToPlayer, nFlashMask, 
					pEnt, COLLISION_GROUP_NONE, &tr);

				// We need to do a more complex trace.
				if (tr.fraction != 1.0f)
					bIsHidden = true;
			}
			// Are we in the fade radius and still in the complex check radius?
			else if (bIsHidden || 
				(fDistance >= FORSAKEN_M84STUN_FADERADIUS_MIN && 
				fDistance <= FORSAKEN_M84STUN_SIMPLERADIUS))
			{
				/*
					Okay, if we hit this block we have two possible states.
					Either we are in the absolute blind radius and we are ducked 
					behind something, or we are just in the fade radius and need 
					a more complex check of how we will be blinded.
					*/
				// Do a trace to see if there is anything blocking our way.
				UTIL_TraceLine(GetAbsOrigin(), vecToPlayer, nFlashMask, 
					pEnt, COLLISION_GROUP_NONE, &tr);

				// Calculate how much they are going to be flashed.
				fPercentage = ((FORSAKEN_M84STUN_FADERADIUS_MAX - fFadeDist) / 
					FORSAKEN_M84STUN_FADERADIUS_MAX);

				// Determine if we hit an entity or the world.
				if (tr.fraction != 1.0f && tr.DidHitWorld() && tr.m_pEnt)
				{
					Vector vecToTarget = vecToPlayer - tr.endpos;
					Vector vecObjectUp, vecDeflection;

					// Normalize the vector to the target.
					VectorNormalize(vecToTarget);

					// Find the major angle to determine the light deflection.
					CrossProduct(vecToTarget, tr.plane.normal, vecObjectUp);
					CrossProduct(tr.plane.normal, vecObjectUp, vecDeflection);
					VectorNormalize(vecDeflection);

					// Trace along the deflection to see if we can find a nearby edge...
					UTIL_TraceLine(tr.endpos, 
						tr.endpos + vecDeflection * FORSAKEN_M84STUN_DEFLECTIONTRACE, 
						nFlashMask, pEnt, COLLISION_GROUP_NONE, &tr);

					// So that the player will be flashed even if they are hiding 
					// behind a small object.
					UTIL_TraceLine(tr.endpos, vecToTarget, nFlashMask, pEnt, 
						COLLISION_GROUP_NONE, &tr);

					// Since they were smart enough to hide, reduce the flash amount.
					if (tr.fraction == 1.0f)
						fPercentage -= (bIsHidden ? 0.1f : 0.25f);
					else
						fPercentage = 0.0f;
				}
			}
			// Are we in the fade radius but outside the complex check radius?
			else if (fDistance >= FORSAKEN_M84STUN_SIMPLERADIUS && 
				fDistance <= FORSAKEN_M84STUN_FADERADIUS_MAX)
			{
				// Do a trace to see if there is anything blocking our way.
				UTIL_TraceLine(GetAbsOrigin(), vecToPlayer, CONTENTS_SOLID, 
					pEnt, COLLISION_GROUP_NONE, &tr);

				// If nothing is blocking us, put us into a stun state... 
				// otherwise, we can't be hit.
				if (tr.fraction == 1.0f)
					fPercentage = ((FORSAKEN_M84STUN_FADERADIUS_MAX - fFadeDist) / 
					FORSAKEN_M84STUN_FADERADIUS_MAX);
				else
					fPercentage = 0.0f;
			}

			// If the player is in our radius, set them to stunned.
			pPlayer->SetStunned(true, fPercentage);
		}
	}

	// Play the sound of it exploding.
	EmitSound("Weapon_Forsaken_M84.Explode");

	// Remove ourselves after detonation.
	UTIL_Remove(this);
}

// CForsakenGrenadeM84Stun
void CForsakenGrenadeM84Stun::EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, 
	AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime)
{
	if (fTime == -1.0f)
		fTime = 3.0f;

	CForsakenGrenadeM84StunProjectile::Create(vecSource, angles, vecVelocity, angImpulse, pPlayer, fTime);
}
#endif