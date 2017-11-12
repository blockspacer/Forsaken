/*
	forsaken_weapon_rpg.cpp
	Forsaken weapon code for the RPG.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "forsaken_weapon_rpg.h"
#include "forsaken_shareddefs.h"

#ifdef CLIENT_DLL
#include "model_types.h"
#include "beamdraw.h"
#include "fx_line.h"
#include "view.h"
#include "forsaken/c_forsaken_player.h"
#else
#include "basecombatcharacter.h"
#include "movie_explosion.h"
#include "soundent.h"
#include "player.h"
#include "rope.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "explode.h"
#include "util.h"
#include "in_buttons.h"
#include "shake.h"
#include "te_effect_dispatch.h"
#include "triggers.h"
#include "smoke_trail.h"
#include "collisionutils.h"
#include "forsaken/forsaken_player.h"
#endif

#include "tier0/memdbgon.h"

// HL2 Class Macros
#ifndef CLIENT_DLL
BEGIN_DATADESC(CForsakenRPGMissile)
	// Fields
	DEFINE_FIELD(m_fDamage, FIELD_FLOAT),
	DEFINE_FIELD(m_hRocketTrail, FIELD_EHANDLE),

	// Think Functions
	DEFINE_FUNCTION(IgniteThink),
	DEFINE_FUNCTION(MissileTouch),
END_DATADESC()

LINK_ENTITY_TO_CLASS(forsaken_rpg_missile, CForsakenRPGMissile);

#define RPG_MISSILE_EXPLODESOUND	"ForsakenRPG.Explode"
#define RPG_MISSILE_IGNITESOUND		"ForsakenRPG.Ignite"
#define RPG_MISSILE_MODEL			"models/weapons/w_rocket.mdl"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponRPG, DT_ForsakenWeaponRPG)

BEGIN_NETWORK_TABLE(CForsakenWeaponRPG, DT_ForsakenWeaponRPG)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeaponRPG)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_rpg, CForsakenWeaponRPG);
PRECACHE_WEAPON_REGISTER(forsaken_weapon_rpg);

// Constructor & Deconstructor
// CForsakenRPGMissile
#ifndef CLIENT_DLL
CForsakenRPGMissile::CForsakenRPGMissile()
{
}

CForsakenRPGMissile::~CForsakenRPGMissile()
{
}
#endif

// CForsakenWeaponRPG
CForsakenWeaponRPG::CForsakenWeaponRPG()
{
}

CForsakenWeaponRPG::~CForsakenWeaponRPG()
{
}

// Functions
// CForsakenRPGMissile
#ifndef CLIENT_DLL
CForsakenRPGMissile *CForsakenRPGMissile::Create(const Vector &rvecOrigin, const QAngle &rAngles, 
	edict_t *pOwner)
/*
	Description.
	Pre: 
	Post: 
*/
{
	CForsakenRPGMissile *pMissile = (CForsakenRPGMissile*)CBaseEntity::Create("forsaken_rpg_missile", 
		rvecOrigin, rAngles, Instance(pOwner));
	Vector vecForward;

	pMissile->AddEffects(EF_NOSHADOW);
	pMissile->SetOwnerEntity(Instance(pOwner));
	pMissile->Spawn();

	AngleVectors(rAngles, &vecForward);

	pMissile->SetAbsVelocity(vecForward * 300 + Vector(0, 0, 128));

	return pMissile;
}

void CForsakenRPGMissile::CreateSmokeTrail()
/*
	Description.
	Pre: 
	Post: 
*/
{
	if (m_hRocketTrail)
		return;

	// Create a rocket trail.
	if ((m_hRocketTrail = RocketTrail::CreateRocketTrail()) != NULL)
	{
		m_hRocketTrail->m_Opacity = 0.2f;
		m_hRocketTrail->m_SpawnRate = 100;
		m_hRocketTrail->m_ParticleLifetime = 0.5f;
		m_hRocketTrail->m_StartColor.Init(0.65f, 0.65f , 0.65f);
		m_hRocketTrail->m_EndColor.Init(0.0, 0.0, 0.0);
		m_hRocketTrail->m_StartSize = 8;
		m_hRocketTrail->m_EndSize = 32;
		m_hRocketTrail->m_SpawnRadius = 4;
		m_hRocketTrail->m_MinSpeed = 2;
		m_hRocketTrail->m_MaxSpeed = 16;

		// Follow the grenade.
		m_hRocketTrail->FollowEntity(this, "Smoke");
		m_hRocketTrail->SetLifetime(999);
	}
}

void CForsakenRPGMissile::DoExplosion()
/*
	Description.
	Pre: 
	Post: 
*/
{
	EmitSound(RPG_MISSILE_EXPLODESOUND);

	ExplosionCreate(GetAbsOrigin(), GetAbsAngles(), GetOwnerEntity(), GetDamage(), 
		GetDamage() * 3.0f, 0, 0.0f, this);
}

void CForsakenRPGMissile::Explode()
/*
	Description.
	Pre: 
	Post: 
*/
{
	trace_t tr;
	Vector vecForward;

	// Get the forward vectors.
	GetVectors(&vecForward, NULL, NULL);

	// Trace ahead of us.
	UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + vecForward * 16, MASK_SHOT, this, 
		COLLISION_GROUP_NONE, &tr);

	// We don't take damage and are no longer solid.
	m_takedamage = DAMAGE_NO;
	SetSolid(SOLID_NONE);

	// If we hit the end of the trace or haven't hit the sky explode.
	if (tr.fraction == 1.0f || !(tr.surface.flags & SURF_SKY))
		DoExplosion();

	// Kill the rocket trail.
	if (m_hRocketTrail)
	{
		m_hRocketTrail->SetLifetime(0.1f);
		m_hRocketTrail = NULL;
	}

	// We don't know our owner.
	if (m_hOwner)
	{
		m_hOwner = NULL;
	}

	// Stop the sound and remove us.
	StopSound(RPG_MISSILE_IGNITESOUND);
	UTIL_Remove(this);
}

void CForsakenRPGMissile::IgniteThink()
/*
	Description.
	Pre: 
	Post: 
*/
{
	Vector vecForward;

	SetMoveType(MOVETYPE_FLY);
	SetModel(RPG_MISSILE_MODEL);
	UTIL_SetSize(this, vec3_origin, vec3_origin);
 	RemoveSolidFlags(FSOLID_NOT_SOLID);
	EmitSound(RPG_MISSILE_IGNITESOUND);
	AngleVectors(GetLocalAngles(), &vecForward);
	SetAbsVelocity(vecForward * 3000);

	if (m_hOwner && m_hOwner->GetOwner())
	{
		CBasePlayer *pPlayer = ToBasePlayer(m_hOwner->GetOwner());

		color32 white = {255, 225, 205, 64};
		UTIL_ScreenFade(pPlayer, white, 0.1f, 0.0f, FFADE_IN);
	}

	CreateSmokeTrail();
}

void CForsakenRPGMissile::MissileTouch(CBaseEntity *pOther)
/*
	Description.
	Pre: 
	Post: 
*/
{
	if (pOther->IsSolidFlagSet(FSOLID_TRIGGER | FSOLID_VOLUME_CONTENTS) &&
		pOther->GetCollisionGroup() != COLLISION_GROUP_WEAPON)
		return;

	Explode();
}

void CForsakenRPGMissile::Precache()
/*
	Description.
	Pre: 
	Post: 
*/
{
	PrecacheModel(RPG_MISSILE_MODEL);

	PrecacheScriptSound(RPG_MISSILE_EXPLODESOUND);
	PrecacheScriptSound(RPG_MISSILE_IGNITESOUND);
}

void CForsakenRPGMissile::SetGracePeriod(float fGracePeriod)
/*
	Description.
	Pre: 
	Post: 
*/
{
	// Set our grace period.
	m_fEndGracePeriod = gpGlobals->curtime + fGracePeriod;

	// We aren't solid till this grace period ends.
	AddSolidFlags(FSOLID_NOT_SOLID);
}

void CForsakenRPGMissile::Spawn()
/*
	Description.
	Pre: 
	Post: 
*/
{
	// Precache all models.
	Precache();

	// Define movement behaviour, current model, and size.
	AddFlag(FL_OBJECT);
	SetSolid(SOLID_BBOX);
	SetModel(RPG_MISSILE_MODEL);
	SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE);
	UTIL_SetSize(this, -Vector(4, 4, 4), Vector(4, 4, 4));

	// Set the think time, think, and touch.
	SetNextThink(gpGlobals->curtime + 0.3f);
	SetThink(&CForsakenRPGMissile::IgniteThink);
	SetTouch(&CForsakenRPGMissile::MissileTouch);

	// Define bleeding, damage ability, and health.
	m_bloodColor = DONT_BLEED;
	m_takedamage = DAMAGE_YES;
	m_iHealth = m_iMaxHealth = 20;
}
#endif

// CForsakenWeaponRPG
bool CForsakenWeaponRPG::CanHolster()
/*
	Description.
	Pre: 
	Post: 
*/
{
	if (m_hMissile != NULL)
		return false;

	return CForsakenWeaponBase::CanHolster();
}

bool CForsakenWeaponRPG::HasAnyAmmo()
/*
	Description.
	Pre: 
	Post: 
*/
{
	if (m_hMissile != NULL)
		return true;

	return CForsakenWeaponBase::HasAnyAmmo();
}

bool CForsakenWeaponRPG::Reload()
/*
	Description.
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = GetPlayerOwner();
	
	if (!pPlayer || pPlayer->IsSprinting())
		return false;

	if (pPlayer->GetAmmoCount(GetPrimaryAmmoType()) <= 0)
		return false;

	// Perform the reload.
	if (!DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD))
		return false;

	// Update the reload end time.
	m_fReloadEndTime = gpGlobals->curtime + GetViewModelSequenceDuration();
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();

	// Update the player animations.
#ifdef GAME_DLL
	SendReloadEvents();
#endif

	return true;
}

void CForsakenWeaponRPG::Precache()
/*
	Description.
	Pre: 
	Post: 
*/
{
	BaseClass::Precache();

#ifndef CLIENT_DLL
	UTIL_PrecacheOther("forsaken_rpg_missile");
#endif
}

void CForsakenWeaponRPG::PrimaryAttack()
/*
	Description.
	Pre: 
	Post: 
*/
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	const CForsakenWeaponInfo &rWeaponInf = GetForsakenWeaponData();
	Vector vecOrigin;
	Vector vecForward, vecRight, vecUp;
	Vector vecMuzzlePoint;

	// If we aren't owned, have a rocket out there, or are reloading don't fire.
	if (!pPlayer || m_hMissile)
		return;

	// Decrease the clip size.
	m_iClip1--;

	// Increase the next primary attack time.
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;

	// Get the forward, right, and up vectors.
	pPlayer->EyeVectors(&vecForward, &vecRight, &vecUp);

	// Get the muzzle point.
	vecMuzzlePoint = pPlayer->Weapon_ShootPosition() + vecForward * 12.0f + 
		vecRight * 6.0f + vecUp * -3.0f;

#ifndef CLIENT_DLL
	QAngle vecAngles;
	trace_t	tr;
	Vector vecEye = pPlayer->EyePosition();

	// Calculate the angles.
	VectorAngles(vecForward, vecAngles);

	// Create the missile.
	CForsakenRPGMissile *pMissile = CForsakenRPGMissile::Create(vecMuzzlePoint, vecAngles, 
		pPlayer->edict());

	// Set us as the owners.
	pMissile->m_hOwner = this;

	UTIL_TraceLine(vecEye, vecEye + vecForward * 128, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);

	// Give the missile a grace period if it is clear at the firing frame.
	if (tr.fraction == 1.0)
		pMissile->SetGracePeriod(0.3);

	// Set the damage.
	pMissile->SetDamage(GetForsakenWeaponData().m_nDamage);

	// Set the missile handle.
	m_hMissile = pMissile;
#endif

	// Send the animation, play the sound.
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	WeaponSound(SINGLE);
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	// Muzzle flash effect.
	pPlayer->DoMuzzleFlash();

	// Update the next attack and weapon idle times.
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
	SetWeaponIdleTime(gpGlobals->curtime + GetViewModelSequenceDuration());
}

void CForsakenWeaponRPG::WeaponIdle()
/*
	Description.
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = GetPlayerOwner();

	if (GetWeaponIdleTime() <= gpGlobals->curtime)
	{
		if (m_iClip1 <= 0 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) > 0)
			Reload();
		else if (m_iClip1 <= 0 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
			pPlayer->Weapon_Switch(g_pGameRules->GetNextBestWeapon(pPlayer, this));
	}
}