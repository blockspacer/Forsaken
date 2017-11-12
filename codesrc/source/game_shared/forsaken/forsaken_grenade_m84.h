#ifndef __FORSAKEN_GRENADE_M84STUN_H_
#define __FORSAKEN_GRENADE_M84STUN_H_

#include "forsaken_weapon_base.h"

#ifdef CLIENT_DLL
#define CForsakenGrenadeM84Stun C_ForsakenGrenadeM84Stun
#endif

class CForsakenGrenadeM84Stun : public CForsakenGrenadeBase
{
public:
	DECLARE_CLASS(CForsakenGrenadeM84Stun, CForsakenGrenadeBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenGrenadeM84Stun() { }

	// Public Accessor Functions
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_M84STUN; }
	virtual float GetPrimedCountdownTime() { return 3.0f; }

#ifndef CLIENT_DLL
	DECLARE_DATADESC();

	// Public Functions
	virtual void EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, 
		AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime = -1.0f);
#endif
};

#endif