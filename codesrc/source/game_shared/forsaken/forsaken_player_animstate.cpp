/*
	forsaken_player_animstate.cpp
	Forsaken player animation code.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "base_playeranimstate.h"
#include "tier0/vprof.h"
#include "animation.h"
#include "studio.h"
#include "apparent_velocity_helper.h"
#include "utldict.h"

#include "forsaken/forsaken_player_animstate.h"
#include "forsaken/forsaken_weapon_base.h"
#include "forsaken/forsaken_grenade_base.h"

#ifdef CLIENT_DLL
#include "forsaken/c_forsaken_player.h"
#include "bone_setup.h"
#include "interpolatedvar.h"
#else
#include "forsaken/forsaken_player.h"
#endif

// Pre-Processor Macros
#define ANIM_TOPSPEED_CROUCH	75
#define ANIM_TOPSPEED_RUN		150
#define ANIM_TOPSPEED_WALK		100

#define CROUCH_IDLE_NAME		"crouch_idle_"
#define CROUCH_WALK_NAME		"crouch_walk_"
#define STAND_IDLE_NAME			"stand_idle_"
#define STAND_RUN_NAME			"stand_run_"
#define STAND_WALK_NAME			"stand_walk_"
#define STAND_KICK_NAME			"kick_upper_"
#define STAND_SPRINT_NAME		"sprint_upper_"

#define CROUCH_IDLE_FIRE_NAME	(CROUCH_IDLE_NAME "shoot_")
#define CROUCH_WALK_FIRE_NAME	(CROUCH_WALK_NAME "shoot_")
#define STAND_IDLE_FIRE_NAME	(STAND_IDLE_NAME "shoot_")
#define STAND_RUN_FIRE_NAME		(STAND_RUN_NAME "shoot_")
#define STAND_WALK_FIRE_NAME	(STAND_WALK_NAME "shoot_")

#define CROUCH_IDLE_MELEE_NAME	(CROUCH_IDLE_NAME "melee_")
#define CROUCH_WALK_MELEE_NAME	(CROUCH_WALK_NAME "melee_")
#define STAND_IDLE_MELEE_NAME	(STAND_IDLE_NAME "melee_")
#define STAND_RUN_MELEE_NAME	(STAND_RUN_NAME "melee_")
#define STAND_WALK_MELEE_NAME	(STAND_WALK_NAME "melee_")

#define LAYER_FIRE_SEQUENCE		(AIMSEQUENCE_LAYER + NUM_AIMSEQUENCE_LAYERS)
#define LAYER_RELOAD_SEQUENCE	(LAYER_FIRE_SEQUENCE + 1)
#define LAYER_GRENADE_SEQUENCE	(LAYER_RELOAD_SEQUENCE + 1)
#define LAYER_NUMWANTED			(LAYER_GRENADE_SEQUENCE + 1)

// Internal Classes
class CForsakenPlayerAnimState : public CBasePlayerAnimState, public IForsakenPlayerAnimState
{
public:
	DECLARE_CLASS(CForsakenPlayerAnimState, CBasePlayerAnimState);

	// Constructor & Deconstructor
	CForsakenPlayerAnimState();

	// Public Accessor Functions
	virtual Activity CalcMainActivity();
	virtual bool CanThePlayerMove();
	virtual bool IsThrowingGrenade();
	virtual float GetCurrentMaxGroundSpeed();

	// Public Functions
	virtual int CalcAimLayerSequence(float *fCycle, float *fAimSequenceWeight, bool bForceIdle);
	virtual void ClearAnimationLayers();
	virtual void ClearAnimationState();
	virtual void ComputeSequences(CStudioHdr *pStudioHdr);
	virtual void DebugShowAnimState(int iStartLine);
	virtual void DoAnimationEvent(ForsakenPlayerAnimEvent_t animEvent);
	void InitForsaken(CBaseAnimatingOverlay *pOverlay, 
		IForsakenPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, 
		bool bUseAimSequences);

	// Public Variables

private:
	// Private Functions

	// Private Variables
	bool m_bFiring;
	bool m_bFirstJumpFrame;
	bool m_bJumping;
	bool m_bPriming;
	bool m_bReloading;
	bool m_bThrowing;
	float m_fFireCycle;
	float m_fJumpStartTime;
	float m_fReloadCycle;
	float m_fThrowCycle;
	IForsakenPlayerAnimStateHelpers *m_pHelpers;
	int m_nFireSequence;
	int m_nLastThrowCounter;
	int m_nReloadSequence;
	int m_nThrowSequence;

protected:
	// Protected Accessor Functions
	bool IsGrenadePrimed();
	const char *GetWeaponSuffix();
	int GetThrowCounter();

	// Protected Functions
	bool HandleJumping();
	int CalcFireLayerSequences(ForsakenPlayerAnimEvent_t animEvent);
	int CalcMeleeLayerSequences(ForsakenPlayerAnimEvent_t animEvent);
	int CalcPrimeSequence();
	int CalcReloadLayerSequences();
	int CalcThrowSequence();
	void ComputeAttackSequence(CStudioHdr *pStudioHdr);
	void ComputeReloadSequence(CStudioHdr *pStudioHdr);
	void ComputeThrowSequence(CStudioHdr *pStudioHdr);
	void UpdateLayerSequenceGeneric(CStudioHdr *pStudioHdr, int nLayer, bool &bEnabled, 
		float &fCurrCycle, int &nSequence, bool bWaitAtEnd);

	// Protected Variables
};

// Constructor & Deconstructor
CForsakenPlayerAnimState::CForsakenPlayerAnimState()
{
	m_bReloading = false;
	m_pHelpers = NULL;
	m_pOuter = NULL;
}

// Functions
// CForsakenPlayerAnimState
Activity CForsakenPlayerAnimState::CalcMainActivity()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(m_pOuter);
	float fOuterSpeed = GetOuterXYSpeed();

	if (pPlayer->IsKicking() || pPlayer->m_fKickEndTime > gpGlobals->curtime)
		return ACT_FSKN_KICK;
	else if (HandleJumping())
		return ACT_HOP;
	else if (pPlayer->IsLeaning())
	{
		if (pPlayer->GetLeaningDirection() == LEAN_DIR_RIGHT)
			return ACT_FSKN_LEANRIGHT;
		else if (pPlayer->GetLeaningDirection() == LEAN_DIR_LEFT)
			return ACT_FSKN_LEANLEFT;
		else
			return ACT_IDLE;
	}
	else
	{
		Activity idealActivity = ACT_IDLE;

		if (m_pOuter->GetFlags() & FL_DUCKING)
		{
			if (fOuterSpeed > MOVING_MINIMUM_SPEED)
				idealActivity = ACT_RUN_CROUCH;
			else
				idealActivity = ACT_CROUCHIDLE;
		}
		else
		{
			if (fOuterSpeed > MOVING_MINIMUM_SPEED)
			{
				if (pPlayer->IsSprinting() && pPlayer->m_fForwardSpeed > 0)
					idealActivity = ACT_FSKN_SPRINT;
				else if (fOuterSpeed > ARBITRARY_RUN_SPEED)
					idealActivity = ACT_RUN;
				else
					idealActivity = ACT_WALK;
			}
			else
				idealActivity = ACT_IDLE;
		}

		return idealActivity;
	}
}

bool CForsakenPlayerAnimState::CanThePlayerMove()
/*
	
	Pre: 
	Post: 
*/
{
	return m_pHelpers->ForsakenAnim_CanMove();
}

bool CForsakenPlayerAnimState::HandleJumping()
/*
	
	Pre: 
	Post: 
*/
{
	Vector vecUp;

	AngleVectors(m_pOuter->GetAbsAngles(), NULL, NULL, &vecUp);

	if (m_bJumping)
	{
		if (m_bFirstJumpFrame)
		{
			m_bFirstJumpFrame = false;

			RestartMainSequence();
		}

		if (gpGlobals->curtime - m_fJumpStartTime > 0.2f)
		{
			if (m_pOuter->GetFlags() & FL_ONGROUND)
			{
				m_bJumping = false;

				RestartMainSequence();
			}
		}
	}
	else if (!m_bJumping && !(m_pOuter->GetFlags() & FL_ONGROUND) && 
		m_pOuter->GetAbsVelocity().z > 0.0f)
	{
		m_bJumping = true;
		m_bFirstJumpFrame = true;
		m_fJumpStartTime = gpGlobals->curtime;
	}

	return m_bJumping;
}

bool CForsakenPlayerAnimState::IsGrenadePrimed()
/*
	
	Pre: 
	Post: 
*/
{
	CBaseCombatCharacter *pCharacter = m_pOuter->MyCombatCharacterPointer();

	if (pCharacter)
	{
		CForsakenGrenadeBase *pGrenade = 
			dynamic_cast<CForsakenGrenadeBase*>(pCharacter->GetActiveWeapon());

		return pGrenade && pGrenade->IsPinPulled();
	}

	return false;
}

bool CForsakenPlayerAnimState::IsThrowingGrenade()
/*
	
	Pre: 
	Post: 
*/
{
	if (m_bThrowing)
		return m_fThrowCycle < 0.25;
	else
		return (m_nLastThrowCounter != GetThrowCounter()) || IsGrenadePrimed();
}

const char *CForsakenPlayerAnimState::GetWeaponSuffix()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenWeaponBase *pWeapon = m_pHelpers->ForsakenAnim_GetActiveWeapon();
	if (!pWeapon)
		return 0;

	return pWeapon->GetForsakenWeaponData().m_czAnimExtension;
}

float CForsakenPlayerAnimState::GetCurrentMaxGroundSpeed()
/*
	
	Pre: 
	Post: 
*/
{
	Activity currActivity = m_pOuter->GetSequenceActivity(m_pOuter->GetSequence());

	if (currActivity == ACT_WALK || currActivity == ACT_IDLE)
		return ANIM_TOPSPEED_WALK;
	else if (currActivity == ACT_RUN)
		return ANIM_TOPSPEED_RUN;
	else if (currActivity == ACT_RUN_CROUCH)
		return ANIM_TOPSPEED_CROUCH;
	else
		return 0.0f;
}

int CForsakenPlayerAnimState::CalcAimLayerSequence(float *fCycle, float *fAimSequenceWeight, 
		bool bForceIdle)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(m_pOuter);
	const char *pSuffix = GetWeaponSuffix();

	if (!pSuffix)
		pSuffix = "1201fp";

	if (pPlayer->IsKicking() || pPlayer->m_fKickEndTime > gpGlobals->curtime)
		return CalcSequenceIndex("%s%s", STAND_KICK_NAME, pSuffix);
	else if (GetOuterXYSpeed() > MOVING_MINIMUM_SPEED && pPlayer->IsSprinting())
		return CalcSequenceIndex("%s%s", STAND_SPRINT_NAME, pSuffix);

	if (bForceIdle)
	{
		switch (GetCurrentMainSequenceActivity())
		{
		case ACT_CROUCHIDLE:
			return CalcSequenceIndex("%s%s", CROUCH_IDLE_NAME, pSuffix);

		default:
			return CalcSequenceIndex("%s%s", STAND_IDLE_NAME, pSuffix);
		}
	}
	else
	{
		switch (GetCurrentMainSequenceActivity())
		{
		case ACT_RUN:
			return CalcSequenceIndex("%s%s", STAND_RUN_NAME, pSuffix);

		case ACT_WALK:
		case ACT_IDLETORUN:
		case ACT_RUNTOIDLE:
			return CalcSequenceIndex("%s%s", STAND_WALK_NAME, pSuffix);

		case ACT_CROUCHIDLE:
			return CalcSequenceIndex("%s%s", CROUCH_IDLE_NAME, pSuffix);

		case ACT_RUN_CROUCH:
			return CalcSequenceIndex("%s%s", CROUCH_WALK_NAME, pSuffix);

		case ACT_IDLE:
		default:
			return CalcSequenceIndex("%s%s", STAND_IDLE_NAME, pSuffix);
		}
	}
}

int CForsakenPlayerAnimState::CalcFireLayerSequences(ForsakenPlayerAnimEvent_t animEvent)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenWeaponBase *pWeapon = m_pHelpers->ForsakenAnim_GetActiveWeapon();
	const char *pSuffix = GetWeaponSuffix();
	char czTmp[256] = "";

	if (!pWeapon || !pSuffix)
		return -1;

	if (animEvent == FORSAKEN_PLAYERANIMEVENT_THROW)
		pSuffix = "throw";
	else if (animEvent == FORSAKEN_PLAYERANIMEVENT_GUN_FIRE_SECONDARY)
	{
		Q_snprintf(czTmp, sizeof(czTmp), "alt_%s", pSuffix);

		pSuffix = czTmp;
	}

	if (pWeapon->GetWeaponID() == FORSAKEN_WEAPON_DUALM10 || 
		pWeapon->GetWeaponID() == FORSAKEN_WEAPON_DUALP99)
	{
		CForsakenPlayer *pPlayer = dynamic_cast<CForsakenPlayer*>(m_pOuter);

		if (pPlayer->m_nShotsFired & 0x1)
			Q_snprintf(czTmp, sizeof(czTmp), "%s_l", pSuffix);
		else
			Q_snprintf(czTmp, sizeof(czTmp), "%s_r", pSuffix);

		pSuffix = czTmp;
	}

	switch (GetCurrentMainSequenceActivity())
	{
	case ACT_PLAYER_RUN_FIRE:
	case ACT_RUN:
		return CalcSequenceIndex("%s%s", STAND_RUN_FIRE_NAME, pSuffix);

	case ACT_PLAYER_WALK_FIRE:
	case ACT_WALK:
		return CalcSequenceIndex("%s%s", STAND_WALK_FIRE_NAME, pSuffix);

	case ACT_PLAYER_CROUCH_FIRE:
	case ACT_CROUCHIDLE:
		return CalcSequenceIndex("%s%s", CROUCH_IDLE_FIRE_NAME, pSuffix);

	case ACT_PLAYER_CROUCH_WALK_FIRE:
	case ACT_RUN_CROUCH:
		return CalcSequenceIndex("%s%s", CROUCH_WALK_FIRE_NAME, pSuffix);

	case ACT_FSKN_SPRINT:
		return CalcSequenceIndex(STAND_SPRINT_NAME);

	case ACT_PLAYER_IDLE_FIRE:
	default:
		return CalcSequenceIndex("%s%s", STAND_IDLE_FIRE_NAME, pSuffix);
	}
}

int CForsakenPlayerAnimState::CalcMeleeLayerSequences(ForsakenPlayerAnimEvent_t animEvent)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenWeaponBase *pWeapon = m_pHelpers->ForsakenAnim_GetActiveWeapon();
	const char *pSuffix = GetWeaponSuffix();
	char czTmp[256] = "";

	if (!pWeapon || !pSuffix)
		return -1;

	if (animEvent == FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_SECONDARY)
	{
		Q_snprintf(czTmp, sizeof(czTmp), "alt_%s", pSuffix);

		pSuffix = czTmp;
	}

	switch (GetCurrentMainSequenceActivity())
	{
	case ACT_PLAYER_RUN_FIRE:
	case ACT_RUN:
		return CalcSequenceIndex("%s%s", STAND_RUN_MELEE_NAME, pSuffix);

	case ACT_PLAYER_WALK_FIRE:
	case ACT_WALK:
		return CalcSequenceIndex("%s%s", STAND_WALK_MELEE_NAME, pSuffix);

	case ACT_PLAYER_CROUCH_FIRE:
	case ACT_CROUCHIDLE:
		return CalcSequenceIndex("%s%s", CROUCH_IDLE_MELEE_NAME, pSuffix);

	case ACT_PLAYER_CROUCH_WALK_FIRE:
	case ACT_RUN_CROUCH:
		return CalcSequenceIndex("%s%s", CROUCH_WALK_MELEE_NAME, pSuffix);

	case ACT_FSKN_SPRINT:
		return CalcSequenceIndex(STAND_SPRINT_NAME);

	case ACT_PLAYER_IDLE_FIRE:
	default:
		return CalcSequenceIndex("%s%s", STAND_IDLE_MELEE_NAME, pSuffix);
	}
}

int CForsakenPlayerAnimState::CalcPrimeSequence()
/*
	
	Pre: 
	Post: 
*/
{
	return CalcSequenceIndex("idle_prime");
}

int CForsakenPlayerAnimState::CalcReloadLayerSequences()
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenWeaponBase *pWeapon = m_pHelpers->ForsakenAnim_GetActiveWeapon();
	char czName[512] = "";
	const char *pSuffix = GetWeaponSuffix();
	int nReloadSequence = -1;

	if (!pSuffix || !pWeapon)
		return -1;

	Q_snprintf(czName, sizeof(czName), "reload_%s", pSuffix);

	nReloadSequence = m_pOuter->LookupSequence(czName);

	return nReloadSequence;
}

int CForsakenPlayerAnimState::CalcThrowSequence()
/*
	
	Pre: 
	Post: 
*/
{
	return CalcSequenceIndex("idle_throw");
}

int CForsakenPlayerAnimState::GetThrowCounter()
{
	CForsakenPlayer *pPlayer = dynamic_cast<CForsakenPlayer*>(m_pOuter);

	if (pPlayer)
		return pPlayer->m_nThrowCounter;

	return 0;
}

void CForsakenPlayerAnimState::ClearAnimationLayers()
/*
	
	Pre: 
	Post: 
*/
{
	if (!m_pOuter)
		return;

	m_pOuter->SetNumAnimOverlays(LAYER_NUMWANTED);

	for (int i = 0; i < m_pOuter->GetNumAnimOverlays(); i++)
		m_pOuter->GetAnimOverlay(i)->SetOrder(CBaseAnimatingOverlay::MAX_OVERLAYS);
}

void CForsakenPlayerAnimState::ClearAnimationState()
/*
	
	Pre: 
	Post: 
*/
{
	m_bFiring = false;
	m_bFirstJumpFrame = false;
	m_bJumping = false;
	m_bPriming = false;
	m_bReloading = false;
	m_bThrowing = false;
	m_nLastThrowCounter = GetThrowCounter();

	BaseClass::ClearAnimationState();
}

void CForsakenPlayerAnimState::ComputeAttackSequence(CStudioHdr *pStudioHdr)
/*
	
	Pre: 
	Post: 
*/
{
#ifdef CLIENT_DLL
	UpdateLayerSequenceGeneric(pStudioHdr, LAYER_FIRE_SEQUENCE, m_bFiring, m_fFireCycle, m_nFireSequence, 
		false);
#endif
}

void CForsakenPlayerAnimState::ComputeReloadSequence(CStudioHdr *pStudioHdr)
/*
	
	Pre: 
	Post: 
*/
{
#ifdef CLIENT_DLL
	UpdateLayerSequenceGeneric(pStudioHdr, LAYER_RELOAD_SEQUENCE, m_bReloading, m_fReloadCycle, 
		m_nReloadSequence, false);
#endif
}

void CForsakenPlayerAnimState::ComputeThrowSequence(CStudioHdr *pStudioHdr)
/*
	
	Pre: 
	Post: 
*/
{
#ifdef CLIENT_DLL
	if (m_bThrowing)
		UpdateLayerSequenceGeneric(pStudioHdr, LAYER_GRENADE_SEQUENCE, m_bThrowing, m_fThrowCycle, 
			m_nThrowSequence, false);
	else
	{
		bool bThrowPending = (m_nLastThrowCounter != GetThrowCounter());

		if (IsGrenadePrimed() || bThrowPending)
		{
			if (!m_bPriming)
			{
				if (TimeSinceLastAnimationStateClear() < 0.4f)
					m_fThrowCycle = 1.0;
				else
					m_fThrowCycle = 0.0;

				m_bPriming = true;
				m_nThrowSequence = CalcPrimeSequence();
			}

			UpdateLayerSequenceGeneric(pStudioHdr, LAYER_GRENADE_SEQUENCE, m_bPriming, m_fThrowCycle, 
				m_nThrowSequence, true);

			if (bThrowPending && m_fThrowCycle == 1.0)
			{
				m_nLastThrowCounter = GetThrowCounter();
				m_nThrowSequence = CalcThrowSequence();

				if (m_nThrowSequence != -1)
				{
					m_bPriming = false;
					m_bThrowing = true;
					m_fThrowCycle = 0;
				}
			}
		}
		else
			m_bPriming = false;
	}
#endif
}

void CForsakenPlayerAnimState::ComputeSequences(CStudioHdr *pStudioHdr)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ComputeSequences(pStudioHdr);

	ComputeAttackSequence(pStudioHdr);
	ComputeReloadSequence(pStudioHdr);
	ComputeThrowSequence(pStudioHdr);
}

void CForsakenPlayerAnimState::DoAnimationEvent(ForsakenPlayerAnimEvent_t animEvent)
/*
	
	Pre: 
	Post: 
*/
{
	if (animEvent == FORSAKEN_PLAYERANIMEVENT_GUN_FIRE_PRIMARY || 
		animEvent == FORSAKEN_PLAYERANIMEVENT_GUN_FIRE_SECONDARY)
	{
		m_fFireCycle = 0;
		m_nFireSequence = CalcFireLayerSequences(animEvent);

		if (m_nFireSequence != -1)
			m_bFiring = true;
		else
			m_bFiring = false;
	}
	else if (animEvent == FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_PRIMARY ||
		animEvent == FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_SECONDARY)
	{
		m_fFireCycle = 0;
		m_nFireSequence = CalcMeleeLayerSequences(animEvent);

		if (m_nFireSequence != -1)
			m_bFiring = true;
		else
			m_bFiring = false;
	}
	else if (animEvent == FORSAKEN_PLAYERANIMEVENT_JUMP)
	{
		m_bJumping = true;
		m_bFirstJumpFrame = true;
		m_fJumpStartTime = gpGlobals->curtime;
	}
	else if (animEvent == FORSAKEN_PLAYERANIMEVENT_RELOAD)
	{
		m_nReloadSequence = CalcReloadLayerSequences();

		if (m_nReloadSequence != -1)
		{
			m_bReloading = true;
			m_fReloadCycle = 0;
		}
		else
			m_bReloading = false;
	}
	else
		Assert(!"CForsakenPlayerAnimState::DoAnimationEvent");
}

void CForsakenPlayerAnimState::DebugShowAnimState(int nStartLine)
/*
	
	Pre: 
	Post: 
*/
{
#ifdef CLIENT_DLL
	engine->Con_NPrintf( nStartLine++, "fire  : %s, cycle: %.2f\n", m_bFiring ? GetSequenceName( m_pOuter->GetModelPtr(), m_nFireSequence ) : "[not firing]", m_fFireCycle );
	engine->Con_NPrintf( nStartLine++, "reload: %s, cycle: %.2f\n", m_bReloading ? GetSequenceName( m_pOuter->GetModelPtr(), m_nReloadSequence ) : "[not reloading]", m_fReloadCycle );
	BaseClass::DebugShowAnimState( nStartLine );
#endif
}

void CForsakenPlayerAnimState::InitForsaken(CBaseAnimatingOverlay *pOverlay, 
		IForsakenPlayerAnimStateHelpers *pHelpers, LegAnimType_t legAnimType, 
		bool bUseAimSequences)
/*
	
	Pre: 
	Post: 
*/
{
	CModAnimConfig animConfig;

	animConfig.m_flMaxBodyYawDegrees = 90;
	animConfig.m_LegAnimType = legAnimType;
	animConfig.m_bUseAimSequences = bUseAimSequences;

	m_pHelpers = pHelpers;

	BaseClass::Init(pOverlay, animConfig);
}

void CForsakenPlayerAnimState::UpdateLayerSequenceGeneric(CStudioHdr *pStudioHdr, int nLayer, 
		bool &bEnabled, float &fCurrCycle, int &nSequence, bool bWaitAtEnd)
/*
	
	Pre: 
	Post: 
*/
{
#ifdef CLIENT_DLL
	if (!bEnabled)
		return;

	C_AnimationLayer *pLayer = m_pOuter->GetAnimOverlay(nLayer);

	// Increment the fire sequence's cycle.
	fCurrCycle += m_pOuter->GetSequenceCycleRate(pStudioHdr, nSequence) * gpGlobals->frametime;
	if (fCurrCycle > 1)
	{
		if (bWaitAtEnd)
			fCurrCycle = 1;
		else
		{
			bEnabled = false;
			nSequence = 0;

			return;
		}
	}

	// Dump the state into the animation layer.
	pLayer->m_flCycle = fCurrCycle;
	pLayer->m_flPlaybackRate = 1.0f;
	pLayer->m_flWeight = 1.0f;
	pLayer->m_nSequence = nSequence;
	pLayer->m_nOrder = nLayer;
#endif
}

// Global
IForsakenPlayerAnimState *CreatePlayerAnimState(CBaseAnimatingOverlay *pOverlay, 
												IForsakenPlayerAnimStateHelpers *pHelpers, 
												LegAnimType_t legAnimType, bool bUseAimSequences)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayerAnimState *pAnimState = new CForsakenPlayerAnimState;

	pAnimState->InitForsaken(pOverlay, pHelpers, legAnimType, bUseAimSequences);

	return pAnimState;
}