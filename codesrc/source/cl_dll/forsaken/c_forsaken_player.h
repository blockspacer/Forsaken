#ifndef __CLIENT_FORSAKEN_PLAYER_H_
#define __CLIENT_FORSAKEN_PLAYER_H_

#include "forsaken/forsaken_player_animstate.h"
#include "c_baseplayer.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/c_forsaken_objective_entity.h"
#include "baseparticleentity.h"

class C_ForsakenPlayer : public C_BasePlayer, public IForsakenPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS(C_ForsakenPlayer, C_BasePlayer);
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_INTERPOLATION();

	// Constructor & Deconstructor
	C_ForsakenPlayer();
	~C_ForsakenPlayer();

	// Public Accessor Functions
	bool IsKicking() { return m_bIsKicking; }
	bool IsLeader() { return m_bIsLeader; }
	bool IsLeaning() { return m_bIsLeaning; }
	bool IsSprinting() { return m_bIsSprinting; }
	bool IsSprintLow() { return m_bIsLowSprint; }
	bool IsStunned() { return m_bIsStunned; }
	C_ForsakenObjectiveEntity *GetObjectiveEntity()
	{
		C_BaseEntity *pObjectiveEntity = m_hObjectiveEntity;

		return dynamic_cast<C_ForsakenObjectiveEntity*>(pObjectiveEntity);
	}
	int GetLookPlayerIndex() { return m_nLookingAtPlayerIndex; }
	float GetMaxSprint() { return m_fMaxSprint; }
	float GetSprint() { return m_fSprint; }
	float GetStunEndTime() { return m_fStunEndTime; }
	ForsakenMovementID GetMovementType();
	int GetLeaderChoice() { return m_nLeaderChoice; }
	int GetLeaningDirection() { return m_nLean; }
	int GetSharedResources() { return m_nSharedResources; }
	virtual float GetMinFOV() const;
	virtual const QAngle &EyeAngles();
	virtual Vector EyePosition();
	virtual void SetPlayerLowSprint(bool bState);
	virtual void SetPlayerSprinting(bool bState);
	void IncrementSprint(float fNewVal) { m_fSprint += fNewVal; }

	// Public Functions
	bool ShouldDraw();
	C_BaseAnimating *BecomeRagdollOnClient(bool bCopyEntity);
	CForsakenWeaponBase *GetActiveForsakenWeapon() const;
	IRagdoll *GetRepresentativeRagdoll() const;
	static C_ForsakenPlayer *GetLocalForsakenPlayer();
	virtual bool ForsakenAnim_CanMove();
	virtual CForsakenWeaponBase* ForsakenAnim_GetActiveWeapon();
	virtual const QAngle &GetRenderAngles();
	virtual ShadowType_t ShadowCastType();
	virtual void OnDataChanged(DataUpdateType_t dutUpdate);
	virtual void PostDataUpdate(DataUpdateType_t dutUpdate);
	virtual void PostThink();
	virtual void PreThink();
	virtual void Spawn();
	virtual void UpdateClientSideAnimation();
	virtual	void Weapon_Drop(CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, 
		const Vector *pVelocity) { }
	void DoAnimationEvent(ForsakenPlayerAnimEvent_t eAnimEvent);
	void FireBullet(Vector vecSource, const QAngle &angShootAngles, float fSpread, int nDamage, 
		int nBulletType, CBaseEntity *pentAttacker, bool bDoEffects, float fX, float fY);
	void FireRicochet(Vector vecSource, const QAngle &angShootAngles, float fSpread, 
		int nDamage, int nBulletType, CBaseEntity *pentAttacker, bool bDoEffects, float fX, 
		float fY, int numRicochets);
	void HandleCustomMovement();

	// Public Variables
	CInterpolatedVar<QAngle> m_interangEyeAngles;
	EHANDLE m_hObjectiveEntity;
	CNetworkVar(bool, m_bIsKicking);
	CNetworkVar(bool, m_bIsLeader);
	CNetworkVar(bool, m_bIsLeaning);
	CNetworkVar(bool, m_bIsSprinting);
	CNetworkVar(bool, m_bIsLowSprint);
	CNetworkVar(bool, m_bIsStunned);
	CNetworkVar(float, m_fForwardSpeed);
	CNetworkVar(float, m_fKickEndTime);
	CNetworkVar(float, m_fMaxSprint);
	CNetworkVar(float, m_fSprint);
	CNetworkVar(float, m_fStunEndTime);
	CNetworkVar(int, m_nLean);
	CNetworkVar(int, m_nSharedResources);
	CNetworkVar(int, m_nShotsFired);
	CNetworkVar(int, m_nThrowCounter);
	EHANDLE m_hRagdoll;
	IForsakenPlayerAnimState *m_statePlayerAnimation;
	QAngle m_angEyeAngles;

private:
	// Constructor & Deconstructor
	C_ForsakenPlayer(const C_ForsakenPlayer &);

protected:
	// Protected Functions
	virtual void OnBulletHit(bool bDoEffects, CBaseEntity *pentAttacker, float fDamage, 
			float fDistance, int nBulletType, trace_t *pTrace, Vector vecDirection, 
			Vector vecStartPos);
	void MouseID();

	// Protected Variables
	bool m_bLowHealthSound;
	float m_fLowHealthEndTime;
	float m_fLookedAtTime;
	int m_nLeaderChoice;
	int m_nLookingAtPlayerIndex;
};

inline C_ForsakenPlayer *ToForsakenPlayer(CBaseEntity *pentPlayer)
{
	return dynamic_cast<C_ForsakenPlayer*>(pentPlayer);
}

#endif