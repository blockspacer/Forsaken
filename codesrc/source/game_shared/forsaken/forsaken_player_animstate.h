#ifndef __FORSAKEN_PLAYER_ANIMSTATE_H_
#define __FORSAKEN_PLAYER_ANIMSTATE_H_

#include "convar.h"
#include "iplayeranimstate.h"
#include "base_playeranimstate.h"

#ifdef CLIENT_DLL
class C_BaseAnimatingOverlay;
class C_ForsakenWeaponBase;
#define CBaseAnimatingOverlay C_BaseAnimatingOverlay
#define CForsakenWeaponBase C_ForsakenWeaponBase
#define CForsakenPlayer C_ForsakenPlayer
#else
class CBaseAnimatingOverlay;
class CForsakenWeaponBase;
class CForsakenPlayer;
#endif

#define ARBITRARY_RUN_SPEED		175.0f

enum ForsakenPlayerAnimEvent_t
{
	FORSAKEN_PLAYERANIMEVENT_GUN_FIRE_PRIMARY,
	FORSAKEN_PLAYERANIMEVENT_GUN_FIRE_SECONDARY,
	FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_PRIMARY,
	FORSAKEN_PLAYERANIMEVENT_MELEE_FIRE_SECONDARY,
	FORSAKEN_PLAYERANIMEVENT_JUMP,
	FORSAKEN_PLAYERANIMEVENT_CROUCH,
	FORSAKEN_PLAYERANIMEVENT_RELOAD,
	FORSAKEN_PLAYERANIMEVENT_THROW,

	FORSAKEN_PLAYERANIMEVENT_COUNT
};

class IForsakenPlayerAnimState : virtual public IPlayerAnimState
{
public:
	virtual bool IsThrowingGrenade() = 0;
	virtual void DoAnimationEvent(ForsakenPlayerAnimEvent_t animEvent) = 0;
};

class IForsakenPlayerAnimStateHelpers
{
public:
	virtual bool ForsakenAnim_CanMove() = 0;
	virtual CForsakenWeaponBase *ForsakenAnim_GetActiveWeapon() = 0;
};

IForsakenPlayerAnimState *CreatePlayerAnimState(CBaseAnimatingOverlay *pOverlay, 
												IForsakenPlayerAnimStateHelpers *pHelpers, 
												LegAnimType_t legAnimType, bool bUseAimSequences);

extern ConVar cl_showanimstate;

#endif