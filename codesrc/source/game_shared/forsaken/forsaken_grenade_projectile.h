#ifndef __FORSAKEN_GRENADE_PROJECTILE_H_
#define __FORSAKEN_GRENADE_PROJECTILE_H_

#include "basegrenade_shared.h"

#ifdef CLIENT_DLL
#define CForsakenGrenadeProjectile C_ForsakenGrenadeProjectile
#endif

class CForsakenGrenadeProjectile : public CBaseGrenade
{
public:
	DECLARE_CLASS(CForsakenGrenadeProjectile, CBaseGrenade);
	DECLARE_NETWORKCLASS();

	// Constructor & Deconstructor
#ifdef CLIENT_DLL
	CForsakenGrenadeProjectile() { }
#endif

	// Public Accessor Functions

	// Public Functions
	virtual void Spawn();

	// Public Variables
	CNetworkVector(m_vecInitialVelocity);

#ifdef CLIENT_DLL
	// Public Functions
	virtual int DrawModel(int nFlags);
	virtual void PostDataUpdate(DataUpdateType_t dutUpdate);

	// Public Variables
	float m_fSpawnTime;
#else
	DECLARE_DATADESC();

	// Public Accessor Functions
	static inline float GetGrenadeElasticity() { return 0.45f; }
	static inline float GetGrenadeFriction() { return 0.2f; }
	static inline float GetGrenadeGravity() { return 0.4f; }
	virtual float GetShakeAmplitude(void) { return 0.0f; }

	// Public Functions
	void DetonationThink(void);
	void SetupInterpolationHistory(const Vector &rvecVelocity);

	// Public Variables

protected:
	// Protected Functions
	virtual void ResolveFlyCollisionCustom(trace_t &rTrace, Vector &rvecVelocity);

	// Protected Variables
#endif
};

#endif