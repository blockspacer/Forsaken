#ifndef __FORSAKEN_WEAPON_GUN_BASE_H_
#define __FORSAKEN_WEAPON_GUN_BASE_H_

#ifdef CLIENT_DLL
#define CForsakenWeaponGunBase C_ForsakenWeaponGunBase
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#endif

class CForsakenWeaponGunBase : public CForsakenWeaponBase
{
public:
	DECLARE_CLASS(CForsakenWeaponGunBase, CForsakenWeaponBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenWeaponGunBase();

	// Public Accessor Functions

	// Public Functions
	virtual bool Reload();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void WeaponIdle();

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
};

#endif