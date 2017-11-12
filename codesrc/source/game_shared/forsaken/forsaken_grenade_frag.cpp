/*
	forsaken_grenade_frag.cpp
	Forsaken weapon code for the frag grenade.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_grenade_base.h"
#include "gamerules.h"
#include "engine/IEngineSound.h"
#include "forsaken_grenade_frag.h"

#ifndef CLIENT_DLL
#include "forsaken/forsaken_player.h"
#include "items.h"
#include "forsaken_grenade_projectile.h"
#endif

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenGrenadeFrag, DT_ForsakenGrenadeFrag)

BEGIN_NETWORK_TABLE(CForsakenGrenadeFrag, DT_ForsakenGrenadeFrag)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenGrenadeFrag)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_grenade_frag, CForsakenGrenadeFrag);
PRECACHE_WEAPON_REGISTER(forsaken_grenade_frag);

#ifndef CLIENT_DLL
// Internal Classes
class CForsakenGrenadeFragProjectile : public CForsakenGrenadeProjectile
{
public:
	DECLARE_CLASS(CForsakenGrenadeFragProjectile, CForsakenGrenadeProjectile);

	// Public Accessor Functions
	virtual float GetShakeAmplitude(void) { return 5.0f; }

	// Public Functions
	static CForsakenGrenadeFragProjectile *Create(Vector vecSource, QAngle angles, 
		Vector vecVelocity, AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTimer)
	{
		CForsakenGrenadeFragProjectile *pNewGrenade = 
			(CForsakenGrenadeFragProjectile*)CBaseEntity::Create("forsaken_grenade_projectile_frag", 
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

	virtual void Precache()
	{
		PrecacheModel("models/weapons/w_fraggrenade_thrown.mdl");

		BaseClass::Precache();
	}

	virtual void Spawn()
	{
		Precache();

		SetModel("models/weapons/w_fraggrenade_thrown.mdl");

		BaseClass::Spawn();
	}
};

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_grenade_projectile_frag, CForsakenGrenadeFragProjectile);
PRECACHE_WEAPON_REGISTER(forsaken_grenade_projectile_frag);

BEGIN_DATADESC(CForsakenGrenadeFrag)
END_DATADESC()

// Functions
void CForsakenGrenadeFrag::EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, 
	AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime)
{
	if (fTime == -1.0f)
		fTime = 3.0f;

	CForsakenGrenadeFragProjectile::Create(vecSource, angles, vecVelocity, angImpulse, pPlayer, fTime);
}
#endif