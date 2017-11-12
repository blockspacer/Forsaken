/*
	forsaken_playermove.cpp
	Forsaken player movement.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "player_command.h"
#include "igamemovement.h"
#include "in_buttons.h"
#include "ipredictionsystem.h"
#include "forsaken_player.h"
#include "iservervehicle.h"

// Function Prototypes
CPlayerMove *PlayerMove();

// Internal Classes
class CForsakenPlayerMove : public CPlayerMove
{
	DECLARE_CLASS(CForsakenPlayerMove, CPlayerMove);

public:
	// Constructor & Deconstructor

	// Public Accessor Functions

	// Public Functions
	virtual void FinishMove(CBasePlayer *pPlayer, CUserCmd *pUserCommand, CMoveData *pMove);
	virtual void SetupMove(CBasePlayer *pPlayer, CUserCmd *pUserCommand, IMoveHelper *pHelper, 
		CMoveData *pMove);
	virtual void StartCommand(CBasePlayer *pPlayer, CUserCmd *pUserCommand);

	// Public Variables
};

// Global Variable Decleration
static CMoveData g_MoveData;
static CForsakenPlayerMove g_PlayerMove;
CMoveData *g_pMoveData = &g_MoveData;
IPredictionSystem *IPredictionSystem::g_pPredictionSystems = NULL;

// Functions
// CForsakenPlayerMove
void CForsakenPlayerMove::FinishMove(CBasePlayer *pPlayer, CUserCmd *pUserCommand, CMoveData *pMove)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::FinishMove(pPlayer, pUserCommand, pMove);
}

void CForsakenPlayerMove::SetupMove(CBasePlayer *pPlayer, CUserCmd *pUserCommand, 
		IMoveHelper *pHelper, CMoveData *pMove)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::SetupMove(pPlayer, pUserCommand, pHelper, pMove);
}

void CForsakenPlayerMove::StartCommand(CBasePlayer *pPlayer, CUserCmd *pUserCommand)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::StartCommand(pPlayer, pUserCommand);
}

// Global
CPlayerMove *PlayerMove()
/*
	
	Pre: 
	Post: 
*/
{
	return &g_PlayerMove;
}