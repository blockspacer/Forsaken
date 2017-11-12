#ifndef __FORSAKEN_WEAPON_BASE_H_
#define __FORSAKEN_WEAPON_BASE_H_

#include "forsaken_shareddefs.h"
#include "forsaken_player_animstate.h"
#include "forsaken_weapon_parse.h"

#ifdef CLIENT_DLL
#define CForsakenWeaponBase C_ForsakenWeaponBase
#endif

class CForsakenPlayer;

// Ammo Definitions
#define AMMO_12GAUGE	"AMMO_12GAUGE"
#define AMMO_223		"AMMO_223"
#define AMMO_300		"AMMO_300"
#define AMMO_44CAL		"AMMO_44CAL"
#define AMMO_45CAL_ACP	"AMMO_45CAL_ACP"
#define AMMO_45CAL_COLT	"AMMO_45CAL_COLT"
#define AMMO_9MM		"AMMO_9MM"
#define AMMO_GRENADE	"AMMO_GRENADE"
#define AMMO_MOLOTOV	"AMMO_MOLOTOV"
#define AMMO_RPGMISSILE	"AMMO_RPGMISSILE"
#define AMMO_STUN		"AMMO_STUNGRENADE"

// Melee Hull
#define BLUDGEON_HULL_DIM		16

// Forsaken Weapon IDs
typedef enum
{
	FORSAKEN_WEAPON_NONE = 0,

	FORSAKEN_WEAPON_1201FP, 
	FORSAKEN_WEAPON_HK23,
	FORSAKEN_WEAPON_DUALM10,
	FORSAKEN_WEAPON_SA80,
	FORSAKEN_WEAPON_FRAGGREN,
	FORSAKEN_WEAPON_RAGINGBULL,
	FORSAKEN_WEAPON_DUALP99,
	FORSAKEN_WEAPON_MOLOTOVGREN,
	FORSAKEN_WEAPON_710,
	FORSAKEN_WEAPON_RPG,
	FORSAKEN_WEAPON_REVOLVER,
	FORSAKEN_WEAPON_870REMINGTON,
	FORSAKEN_WEAPON_M84STUN,

	FORSAKEN_WEAPON_MAX
} ForsakenWeaponID;

// Forsaken Muzzle Flash Types
typedef enum
{
	FSKN_MUZZLEFLASH_NONE = -1,
	FSKN_MUZZLEFLASH_1201FP = 0,
	FSKN_MUZZLEFLASH_710,
	FSKN_MUZZLEFLASH_HK23,
	FSKN_MUZZLEFLASH_DUALM10,
	FSKN_MUZZLEFLASH_DUALP99,
	FSKN_MUZZLEFLASH_RAGINGBULL,
	FSKN_MUZZLEFLASH_REVOLVER,
	FSKN_MUZZLEFLASH_SA80,
	FSKN_MUZZLEFLASH_RPG,
	FSKN_MUZZLEFLASH_870REMINGTON
} ForsakenMuzzleFlashType;

// Forsaken Shell Types
typedef enum
{
	FSKN_SHELL_NONE = -1,
	FSKN_SHELL_1201FP = 0,
	FSKN_SHELL_HK23,
	FSKN_SHELL_MAC10,
	FSKN_SHELL_P99,
	FSKN_SHELL_RAGINGBULL,
	FSKN_SHELL_REMINGTON,
	FSKN_SHELL_REVOLVER,
	FSKN_SHELL_SA80
} ForsakenShellType;

typedef enum
{
	FORSAKEN_WEAPON_PRIMARY_MODE = 0,
	FORSAKEN_WEAPON_SECONDARY_MODE
} ForsakenWeaponMode;

class CForsakenWeaponBase : public CBaseCombatWeapon
{
public:
	DECLARE_CLASS(CForsakenWeaponBase, CBaseCombatWeapon);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenWeaponBase();
	virtual ~CForsakenWeaponBase();

	// Public Accessor Functions
	CForsakenPlayer *GetPlayerOwner() const;
	const CForsakenWeaponInfo &GetForsakenWeaponData() const;
	virtual bool CanHolster();
	virtual bool IsDualie() { return false; }
	virtual bool IsLeaderWeapon() { return m_bIsLeaderWeapon; }
	virtual bool IsPredicted() const { return true; }
	virtual bool IsWeaponZoomed() const { return false; }
	virtual char *GetDeathNoticeName();
	virtual ForsakenMuzzleFlashType GetMuzzleFlashType() const { return FSKN_MUZZLEFLASH_NONE; }
	virtual ForsakenShellType GetShellType() const { return FSKN_SHELL_NONE; }
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_NONE; }
	virtual void SetLeaderWeapon(bool bIsLeaderWeap) { m_bIsLeaderWeapon = bIsLeaderWeap; }

	// Public Functions
	virtual bool Deploy();
	virtual bool PlayEmptySound();
	virtual void ItemPreFrame();
	virtual void ProcessMuzzleFlashEvent();
	void DoKick();

#ifdef CLIENT_DLL
	virtual bool ShouldPredict()
	{
		if (GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer())
			return true;

		return BaseClass::ShouldPredict();
	}
#endif

#ifdef GAME_DLL
	virtual void SendReloadEvents();
#endif

	// Public Variables
	CNetworkVar(bool, m_bIsLeaderWeapon);
	CNetworkVar(bool, m_bKillDrop);
	CNetworkVar(float, m_fReloadEndTime);

private:
	// Constructor & Deconstructor
	CForsakenWeaponBase(const CForsakenWeaponBase &);

	// Private Functions

	// Private Variables

protected:
	// Protected Functions
	virtual bool CanFire();
	virtual bool DidImpactWater(const Vector &start, const Vector &end);
	virtual bool SemiAuto() { return false; }
	virtual const Vector &GetBulletSpread(ForsakenMovementID moveType);
	virtual QAngle GetRecoilAmount(ForsakenMovementID moveType)
	{
		QAngle newAngle;

		newAngle.Init();

		return newAngle;
	}
	virtual void AlterPlayerAccuracy(ForsakenMovementID moveType, Vector &rvecAccuracy);
	virtual void AlterPlayerRecoil(ForsakenMovementID moveType, QAngle &rangRecoil);
	virtual void ImpactEffect(trace_t &traceHit, int nDamageType);
	void ChooseIntersectionPoint(trace_t &hitTrace, const Vector &mins, const Vector &maxs, 
		CBasePlayer *pOwner);
	void MeleeHit(float fDamage, trace_t &traceHit, int nDamageType);
	Activity PerformMeleeAttack(float fDamage, float fRange, int nDamageType, bool bSlash);
	void PerformRecoil(bool bSnapEyeAngles = false);

	// Protected Variables
	bool m_bAttackDisabled;
	bool m_bMeleeHit;
};

const char *WeaponIDToAlias(int nWeaponID);
int AliasToWeaponID(const char *czAlias);

#endif