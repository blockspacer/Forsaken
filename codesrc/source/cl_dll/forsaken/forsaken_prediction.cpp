/*
	forsaken_prediction.cpp
	Forsaken prediction code.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "prediction.h"
#include "c_baseplayer.h"
#include "igamemovement.h"

// Internal Classes
class CForsakenPrediction : public CPrediction
{
	DECLARE_CLASS(CForsakenPrediction, CPrediction);

public:
	virtual void FinishMove(C_BasePlayer *pPlayer, CUserCmd *pCommand, CMoveData *pMoveData);
	virtual void SetupMove(C_BasePlayer *pPlayer, CUserCmd *pCommand, IMoveHelper *pHelper, 
		CMoveData *pMoveData);
};

// Global Variable Declerations
static CForsakenPrediction g_Prediction;
CPrediction *prediction = &g_Prediction;
static CMoveData g_MoveData;
CMoveData *g_pMoveData = &g_MoveData;

// HL2 Class Macros
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CForsakenPrediction, IPrediction, 
		VCLIENT_PREDICTION_INTERFACE_VERSION, g_Prediction);

// Functions
void CForsakenPrediction::FinishMove(C_BasePlayer *pPlayer, CUserCmd *pCommand, CMoveData *pMoveData)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::FinishMove(pPlayer, pCommand, pMoveData);
}

void CForsakenPrediction::SetupMove(C_BasePlayer *pPlayer, CUserCmd *pCommand, IMoveHelper *pHelper, 
		CMoveData *pMoveData)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::SetupMove(pPlayer, pCommand, pHelper, pMoveData);
}