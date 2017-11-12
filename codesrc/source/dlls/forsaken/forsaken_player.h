#ifndef __FORSAKEN_PLAYER_H_
#define __FORSAKEN_PLAYER_H_

#include "player.h"
#include "server_class.h"
#include "forsaken/forsaken_player_animstate.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_objective_entity.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken/forsaken_shareddefs.h"

class CForsakenTeam;

#define POINTID_NONE				0
#define POINTID_PLAYER_ASSIST		1
#define POINTID_PLAYER_DAMAGE		2
#define POINTID_PLAYER_KILL			3
#define POINTID_PLAYER_LEADERDFND	4
#define POINTID_PLAYER_SUICIDE		5
#define POINTID_OBJECTIVE_ASSIST	6
#define POINTID_OBJECTIVE_COMPLETE	7

#define VOICECMD_NONE			0
#define VOICECMD_DIRECTIONAL	1
#define VOICECMD_GENERAL		2
#define VOICECMD_TEAM			3

struct tagPointsSummary
{
	char *czName;
	int nPointID;
	int nPoints;
};

struct tagPlayerAttacked
{
	char czSteamID[MAX_STEAM_ID_LENGTH];
	float fDamage;
	int nEntIndex;
};

struct tagPlayerAttackers
{
	char czSteamID[MAX_STEAM_ID_LENGTH];
	float fAttackTime;
	int nEntIndex;
};

class CForsakenPlayer : public CBasePlayer, public IForsakenPlayerAnimStateHelpers
{
public:
	DECLARE_CLASS(CForsakenPlayer, CBasePlayer);
	DECLARE_SERVERCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenPlayer();
	~CForsakenPlayer();

	// Public Accessor Functions
	bool CanLoadout()
	{
		// Bug Fix ID 0000079: If we aren't in a round, allow loadouts.
		if (ForsakenGameRules()->GetRemainingTimeInRound() == 0)
			return true;

		return m_fLoadoutTime >= gpGlobals->curtime;
	}
	bool IsAuthorized() { return m_bHasAuth; }
	bool IsPlayingCredits() { return m_bInCredits; }
	bool IsKicking() { return m_bIsKicking; }
	bool IsLeader() { return m_bIsLeader; }
	bool IsLeaning() { return m_bIsLeaning; }
	bool IsSelectingSpawn() { return m_bSelectingSpawn; }
	bool IsSprinting() { return m_bIsSprinting; }
	bool IsSprintLow() { return m_bIsLowSprint; }
	bool IsStunned() { return m_bIsStunned; }
	CForsakenObjectiveEntity *GetObjectiveEntity() { return m_hObjectiveEntity.Get(); }
	float GetAuthEndTime() { return m_fEndAuthTime; }
	float GetMaxSprint() { return m_fMaxSprint; }
	float GetSprint() { return m_fSprint; }
	float GetStunEndTime() { return m_fStunEndTime; }
	ForsakenMovementID GetMovementType();
	int GetLeaderChoice() { return m_nLeaderChoice; }
	int GetLeaningDirection() { return m_nLean; }
	int GetScore() { return m_nScore; }
	int GetSharedResources() { return m_nSharedResources; }
	int GetVoiceCommandState() { return m_nVoiceCmdState; }
	LeaderAuras GetAura() { return m_laCurrentAura; }
	virtual const QAngle &EyeAngles();
	virtual Vector EyePosition();
	virtual void SetPlayerLowSprint(bool bState);
	virtual void SetPlayerSprinting(bool bState);
	void IncrementResources(int nNewVal) { m_nSharedResources += nNewVal; }
	void IncrementScore(int nNewVal) { m_nScore += nNewVal; }
	void IncrementSprint(float fNewVal) { m_fSprint += fNewVal; }
	void SetPlayerAuthorized(bool bHasAuth) { m_bHasAuth = bHasAuth; }
	void SetPlayerEndAuthTime(float fEndTime) { m_fEndAuthTime = fEndTime; }
	void SetAura(LeaderAuras laNewAura) { m_laCurrentAura = laNewAura; }
	void SetAuthEndTime(float fAuthEndTime) { m_fEndAuthTime = fAuthEndTime; }
	void SetLeader(bool bNewValue) { m_bIsLeader = bNewValue; }
	void SetLeaderChoice(int nChoice) { m_nLeaderChoice = nChoice; }
	void SetLeaningDirection(float fSideMovement)
	{
		if (fSideMovement > 0)
			m_nLean = LEAN_DIR_RIGHT;
		else if (fSideMovement < 0)
			m_nLean = LEAN_DIR_LEFT;
		else
			m_nLean = LEAN_DIR_NONE;
	}
	void SetObjectiveEntity(CForsakenObjectiveEntity *pEntity) { m_hObjectiveEntity = pEntity; }
	void SetSharedResources(int nResources) { m_nSharedResources = nResources; }
	void SetSpawnSelection(bool bSelectingSpawn) { m_bSelectingSpawn = bSelectingSpawn; }
	void SetStunned(bool bIsStunned, float fPercentage);
	void SetVoiceCommandState(int nNewState) { m_nVoiceCmdState = nNewState; }

	// Public Functions
	bool SendPointSummary(CBaseEntity *pEntity, tagPointsSummary &rPointSum);
	CForsakenWeaponBase *GetActiveForsakenWeapon() const;
	static CForsakenPlayer *CreatePlayer(const char *czName, edict_t *pEdict);
	static CForsakenPlayer *Instance(int nEntity);
	virtual bool ClientCommand(const char *czCommand);
	virtual bool ForsakenAnim_CanMove();
	virtual CForsakenWeaponBase *ForsakenAnim_GetActiveWeapon();
	virtual int OnTakeDamage(const CTakeDamageInfo &rDmgInfo);
	virtual void CheatImpulseCommands(int nImpulse);
	virtual void CreateViewModel(int nViewModelIndex = 0);
	virtual void Event_Killed(const CTakeDamageInfo &rInfo);
	virtual void InitialSpawn();
	virtual void PostThink();
	virtual void Precache();
	virtual void PreThink();
	virtual void TouchThink();
	virtual void TraceAttack(const CTakeDamageInfo &rInfo, const Vector &vecDirection, trace_t *pTrace);
	virtual void Spawn();
	virtual	void Weapon_Drop(CBaseCombatWeapon *pWeapon, const Vector *pvecTarget, 
		const Vector *pVelocity);
	void DisplayPointSummary();
	void DoAnimationEvent(ForsakenPlayerAnimEvent_t eAnimEvent);
	void FireBullet(Vector vecSource, const QAngle &angShootAngles, float fSpread, int nDamage, 
		int nBulletType, CBaseEntity *pentAttacker, bool bDoEffects, float fX, float fY);
	void FireRicochet(Vector vecSource, const QAngle &angShootAngles, float fSpread, 
		int nDamage, int nBulletType, CBaseEntity *pentAttacker, bool bDoEffects, float fX, 
		float fY, int numRicochets);
	void HandleCustomMovement();
	void UpdateModel();

	// Public Variables
	CNetworkHandle(CBaseEntity, m_hRagdoll);
	CNetworkHandle(CForsakenObjectiveEntity, m_hObjectiveEntity);
	CNetworkQAngle(m_angEyeAngles);
	CNetworkVar(bool, m_bHasSpawned);
	CNetworkVar(bool, m_bIsKicking);
	CNetworkVar(bool, m_bIsLeader);
	CNetworkVar(bool, m_bIsLeaning);
	CNetworkVar(bool, m_bIsLowSprint);
	CNetworkVar(bool, m_bIsSprinting);
	CNetworkVar(bool, m_bIsStunned);
	CNetworkVar(float, m_fForwardSpeed);
	CNetworkVar(float, m_fKickEndTime);
	CNetworkVar(float, m_fMaxSprint);
	CNetworkVar(float, m_fSprint);
	CNetworkVar(float, m_fStunEndTime);
	CNetworkVar(int, m_nLeaderChoice);
	CNetworkVar(int, m_nLean);
	CNetworkVar(int, m_nScore);
	CNetworkVar(int, m_nSharedResources);
	CNetworkVar(int, m_nShotsFired);
	CNetworkVar(int, m_nThrowCounter);
	CUtlVector<tagPlayerAttacked> m_attacked;
	CUtlVector<tagPlayerAttackers> m_attackers;
	CUtlVector<tagPointsSummary> m_pointsummary;

private:
	// Private Functions
	int AutoJoin(int nTeamA, int nTeamB, CForsakenTeam *pTeamA, CForsakenTeam *pTeamB);
	int HandleAutoJoin();
	void AuraThink();
	void CreateRagdollEntity();

	// Private Variables
	bool m_bHasAuth;
	bool m_bInCredits;
	bool m_bSelectingSpawn;
	float m_fEndAuthTime;
	float m_fLoadoutTime;
	IForsakenPlayerAnimState *m_pPlayerAnimState;
	int m_nVoiceCmdState;
	LeaderAuras m_laCurrentAura;

protected:
	// Protected Functions
	virtual void OnBulletHit(bool bDoEffects, CBaseEntity *pentAttacker, float fDamage, 
			float fDistance, int nBulletType, trace_t *pTrace, Vector vecDirection, 
			Vector vecStartPos);
};

inline CForsakenPlayer *ToForsakenPlayer(CBaseEntity *pentPlayer)
{
	return dynamic_cast<CForsakenPlayer*>(pentPlayer);
}

#endif
