#ifndef __FORSAKEN_WEAPON_RPG_H_
#define __FORSAKEN_WEAPON_RPG_H_

#include "forsaken_weapon_base.h"

#ifndef CLIENT_DLL

class CForsakenWeaponRPG;
class RocketTrail;

class CForsakenRPGMissile : public CBaseCombatCharacter
{
	DECLARE_CLASS(CForsakenRPGMissile, CBaseCombatCharacter);
	DECLARE_DATADESC();

public:
	// Constructor & Deconstructor
	CForsakenRPGMissile();
	virtual ~CForsakenRPGMissile();

	// Public Accessor Functions
	virtual float GetDamage() { return m_fDamage; }
	void SetDamage(float fDamage) { m_fDamage = fDamage; }
	void SetGracePeriod(float fGracePeriod);

	// Public Functions
	static CForsakenRPGMissile *Create(const Vector &rvecOrigin, const QAngle &rAngles, 
		edict_t *pOwner);
	void Explode();
	void IgniteThink();
	void MissileTouch(CBaseEntity *pOther);
	void Precache();
	void Spawn();

	// Public Variables
	CHandle<CForsakenWeaponRPG> m_hOwner;

protected:
	// Protected Functions
	void CreateSmokeTrail();
	virtual void DoExplosion();

	// Protected Variables
	CHandle<RocketTrail> m_hRocketTrail;
	float m_fDamage;
	float m_fEndGracePeriod;
};

#endif

#ifdef CLIENT_DLL
#define CForsakenWeaponRPG C_ForsakenWeaponRPG
#endif

class CForsakenWeaponRPG : public CForsakenWeaponBase
{
	DECLARE_CLASS(CForsakenWeaponRPG, CForsakenWeaponBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

public:
	// Constructor & Deconstructor
	CForsakenWeaponRPG();
	virtual ~CForsakenWeaponRPG();

	// Public Accessor Functions
	CBaseEntity *GetMissile() { return m_hMissile; }
	virtual float GetFireRate() { return 1; }
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_RPG; }
	virtual ForsakenMuzzleFlashType GetMuzzleFlashType() const { return FSKN_MUZZLEFLASH_RPG; }

	// Public Functions
	bool CanHolster();
	bool HasAnyAmmo();
	bool Reload();
	void DecrementAmmo(CBaseCombatCharacter *pOwner);
	void Precache();
	void PrimaryAttack();
	void WeaponIdle();

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	CNetworkHandle(CBaseEntity, m_hMissile);
};

#endif