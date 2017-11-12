#ifndef __FORSAKEN_GRENADE_FRAG_H_
#define __FORSAKEN_GRENADE_FRAG_H_

#include "forsaken_weapon_base.h"

#ifdef CLIENT_DLL
#define CForsakenGrenadeFrag C_ForsakenGrenadeFrag
#endif

class CForsakenGrenadeFrag : public CForsakenGrenadeBase
{
public:
	DECLARE_CLASS(CForsakenGrenadeFrag, CForsakenGrenadeBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenGrenadeFrag() { }

	// Public Accessor Functions
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_FRAGGREN; }
	virtual float GetPrimedCountdownTime() { return 3.0f; }

#ifndef CLIENT_DLL
	DECLARE_DATADESC();

	// Public Functions
	virtual void EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, 
		AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime = -1.0f);
#endif
};

#endif