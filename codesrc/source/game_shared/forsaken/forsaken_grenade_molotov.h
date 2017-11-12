#ifndef __FORSAKEN_GRENADE_MOLOTOV_H_
#define __FORSAKEN_GRENADE_MOLOTOV_H_

#include "forsaken_weapon_base.h"

#ifdef CLIENT_DLL
#define CForsakenGrenadeMolotov C_ForsakenGrenadeMolotov
#endif

class CForsakenGrenadeMolotov : public CForsakenGrenadeBase
{
public:
	DECLARE_CLASS(CForsakenGrenadeMolotov, CForsakenGrenadeBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenGrenadeMolotov() { }

	// Public Accessor Functions
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_MOLOTOVGREN; }
	virtual float GetPrimedCountdownTime() { return 10.0f; }

#ifndef CLIENT_DLL
	DECLARE_DATADESC();

	// Public Functions
	virtual void EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, 
		AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime = -1.0f);
#endif
};

#endif