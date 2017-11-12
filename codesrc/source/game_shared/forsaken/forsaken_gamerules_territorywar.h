#ifndef __FORSAKEN_GAMERULES_H_
#define __FORSAKEN_GAMERULES_H_

#include "teamplay_gamerules.h"
#include "convar.h"
#include "gamevars_shared.h"

#ifdef CLIENT_DLL
#include "c_baseplayer.h"
#include "igameevents.h"

#define CForsakenGameRules C_ForsakenGameRules
#define CForsakenGameRulesProxy C_ForsakenGameRulesProxy
#else
#include "player.h"
class CForsakenObjectiveEntity;
#endif

// Save state file
#define FORSKANE_SAVESTATE					"scripts/map_change_save.txt"

class CForsakenGameRulesProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS(CForsakenGameRulesProxy, CGameRulesProxy);
	DECLARE_NETWORKCLASS();
};

class CForsakenGameRules : public CTeamplayRules, public IGameEventListener2
{
public:
	DECLARE_CLASS(CForsakenGameRules, CTeamplayRules);

	// Public Accessor Functions
	virtual bool IsMultiplayer(void) { return true; }
	virtual bool IsTeamplay(void) { return true; }
	virtual bool ShouldUseRobustRadiusDamage(CBaseEntity *pEntity) { return (pEntity->IsPlayer() ? true : false); }
	virtual int GetRemainingTimeInRound() { return m_nRoundTime; }

	// Public Functions
	virtual bool ShouldCollide(int nCollisionGroup0, int nCollisionGroup1);
	virtual int PlayerRelationship(CBaseEntity *pentPlayer, CBaseEntity *pentTarget);
	virtual void FireGameEvent(IGameEvent *pEvent);

	// Public Variables
	CNetworkVar(int, m_nRoundTime);

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE();

#else

	DECLARE_SERVERCLASS_NOBASE();

	// Constructor & Deconstructor
	CForsakenGameRules();
	virtual ~CForsakenGameRules();

	// Public Accessor Functions
	virtual const char *GetChatPrefix(bool bTeamOnly, CBasePlayer *pPlayer);
	virtual const char *GetGameDescription();
	virtual void SetIntermissionTime(float fTime) { m_flIntermissionEndTime = fTime; }

	// Public Functions
	virtual bool FPlayerCanRespawn(CBasePlayer *pPlayer);
	virtual bool ClientCommand(const char *czCommand, CBaseEntity *pentEdict);
	virtual bool ClientConnected(edict_t *pEntity, const char *czName, const char *czAddress, 
		char *czReject, int nMaxRejectLen);
	virtual CBaseEntity *GetPlayerSpawnSpot(CBasePlayer *pPlayer);
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	virtual void ClientDisconnected(edict_t *pClient);
	virtual void EnableTeam(int nTeam, bool bEnable);
	virtual void LevelShutdown();
	virtual void LoadState();
	virtual void SaveState();
	virtual void OnTeamChange(CBasePlayer *pPlayer);
	virtual void PlayerKilled(CBasePlayer *pVictim, const CTakeDamageInfo &rInfo);
	virtual void Precache();
	virtual void RadiusDamage(const CTakeDamageInfo &rtdiDamageInf, const Vector &rvecSource, 
		float fRadius, int nClassIgnore);
	virtual void Think();
	void EndCurrentRound(int nWinner);
	void EndMap(int nWinner);
	void EndWar(int nWinner);
	void HandleRespawn(CBasePlayer *pPlayer);
	void RemoveAllEntityDecals();
	void ResetMap();
	void StartNewRound();

	// Public Variables

private:
	// Private Functions
	int GetRoundRemainTime();
	void RadiusDamage(const CTakeDamageInfo &rtdiDamageInf, const Vector &rvecSource, 
		float fRadius, int nClassIgnore, bool bIgnoreWorld);

protected:
	// Protected Functions
	virtual bool CanDoNewRound();
	virtual void AurasThink();
	virtual void ChangeLevel();
	void NotifyEntitiesOfNewRound();
	void ObjectiveAdd(IGameEvent *pEvent);
	void ObjectiveCompleted(IGameEvent *pEvent);
	void ObjectiveDestroyed(IGameEvent *pEvent);
	void ObjectiveTakingDamage(IGameEvent *pEvent);

	// Protected Variables
	bool m_bInRound;
	bool m_bWarOver;
	CUtlVector<CForsakenObjectiveEntity*> m_vecCompletedObjectives;
	float m_fNextAuraThinkTime;
	float m_fRoundFinalizeTime;
	float m_fRoundStartTime;
	int m_nRoundCount;
	int m_nRoundDuration;

#endif
};

inline CForsakenGameRules *ForsakenGameRules()
{
	return dynamic_cast<CForsakenGameRules*>(g_pGameRules);
}

#endif