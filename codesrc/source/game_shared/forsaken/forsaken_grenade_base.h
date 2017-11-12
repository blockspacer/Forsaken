#ifndef __FORSAKEN_GRENADE_BASE_H_
#define __FORSAKEN_GRENADE_BASE_H_

#include "forsaken_weapon_base.h"

#ifdef CLIENT_DLL
#define CForsakenGrenadeBase C_ForsakenGrenadeBase
#endif

class CForsakenGrenadeBase : public CForsakenWeaponBase
{
public:
	DECLARE_CLASS(CForsakenGrenadeBase, CForsakenWeaponBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenGrenadeBase();

	// Public Accessor Functions
	bool IsPinPulled() const { return m_bPinPulled; }
	virtual float GetPrimedCountdownTime() { return 0.0f; }

	// Public Functions
	bool Deploy();
	bool Holster(CForsakenWeaponBase *pSwitchingTo);
	bool Reload();
	virtual void DropGrenade(float fTime);
	virtual void ItemPostFrame();
	virtual void Precache();
	virtual void StartGrenadeThrow();
	virtual void ThrowGrenade();
	void DecrementAmmo(CForsakenPlayer *pSwitchingTo);
	void PrimaryAttack();
	void SecondaryAttack();

	// Public Variables

#ifndef CLIENT_DLL
	DECLARE_DATADESC();

	// Public Functions
	int CapabilitiesGet();
	virtual bool AllowsAutoSwitchFrom(void) const;
	virtual void EmitGrenade(Vector vecSource, QAngle angles, Vector vecVelocity, AngularImpulse angImpulse, CForsakenPlayer *pPlayer, float fTime = -1.0f);
#endif

protected:
	// Protected Functions

	// Protected Variables
	CNetworkVar(bool, m_bPinPulled);
	CNetworkVar(bool, m_bRedraw);
	CNetworkVar(float, m_fThrowTime);
	CNetworkVar(float, m_fPinPullEndTime);
	CNetworkVar(float, m_fExplodeInHandTime);
};

#endif