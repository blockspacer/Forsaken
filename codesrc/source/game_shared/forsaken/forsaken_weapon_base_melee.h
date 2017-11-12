#ifndef __FORSAKEN_WEAPON_MELEE_BASE_H_
#define __FORSAKEN_WEAPON_MELEE_BASE_H_

#ifdef CLIENT_DLL
#define CForsakenWeaponMeleeBase C_ForsakenWeaponMeleeBase
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#endif

class CForsakenWeaponMeleeBase : public CForsakenWeaponBase
{
public:
	DECLARE_CLASS(CForsakenWeaponMeleeBase, CForsakenWeaponBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
#ifndef CLIENT_DLL
	DECLARE_DATADESC();
#endif

	// Constructor & Deconstructor
	CForsakenWeaponMeleeBase();

	// Public Accessor Functions

	// Public Functions
	virtual bool Reload() { return false; }
	virtual void ItemPostFrame();

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	bool m_bHit;
};

#endif