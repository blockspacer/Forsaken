#ifndef __FORSAKEN_WEAPON_SHOTGUN_BASE_H_
#define __FORSAKEN_WEAPON_SHOTGUN_BASE_H_

#ifdef CLIENT_DLL
#define CForsakenWeaponShotgunBase C_ForsakenWeaponShotgunBase
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#endif

class CForsakenWeaponShotgunBase : public CForsakenWeaponGunBase
{
public:
	DECLARE_CLASS(CForsakenWeaponShotgunBase, CForsakenWeaponGunBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenWeaponShotgunBase();

	// Public Accessor Functions

	// Public Functions
	virtual bool Reload();
	virtual void PrimaryAttack();
	virtual void WeaponIdle();

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	CNetworkVar(int, m_nInSpecialReload);
};

#endif