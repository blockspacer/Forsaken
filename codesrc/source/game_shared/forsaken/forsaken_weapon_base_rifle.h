#ifndef __FORSAKEN_WEAPON_RIFLE_BASE_H_
#define __FORSAKEN_WEAPON_RIFLE_BASE_H_

#ifdef CLIENT_DLL
#define CForsakenWeaponRifleBase C_ForsakenWeaponRifleBase
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#endif

class CForsakenWeaponRifleBase : public CForsakenWeaponGunBase
{
public:
	DECLARE_CLASS(CForsakenWeaponRifleBase, CForsakenWeaponGunBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	// Constructor & Deconstructor
	CForsakenWeaponRifleBase();

	// Public Accessor Functions
	float GetZoomAnimTimeEnd() { return m_fEndZoomAnim; }
	int GetZoomLevel() { return m_nZoomLevel; }
	virtual bool IsWeaponZoomed() const { return m_nZoomLevel != 0; }

	// Public Functions
	virtual bool Reload();
	virtual bool Holster(CBaseCombatWeapon *pSwitchingTo = NULL);
	virtual void ItemBusyFrame();
	virtual void ItemPostFrame();
	virtual void ItemPreFrame();
	virtual void SecondaryAttack();

	// Public Variables

private:
	// Private Functions
	void HandleZoom();

	// Private Variables

protected:
	// Protected Functions
	virtual float GetZoomRate() { return 0.0f; }
	virtual float GetZoomWait() { return 0.0f; }
	virtual int GetFOVForZoom(int nZoomLevel) { return 0; }
	virtual int GetMaxZoomLevel() { return 0; }
	void ResetZoom(float fWaitTime);

	// Protected Variables
	bool m_bResetZoom;
	bool m_bIsOnLadder;
	CNetworkVar(bool, m_bWantsToReload);
	CNetworkVar(float, m_fEndZoomAnim);
	CNetworkVar(float, m_fNextZoom);
	CNetworkVar(float, m_fReloadTime);
	CNetworkVar(int, m_nZoomLevel);
};

#endif