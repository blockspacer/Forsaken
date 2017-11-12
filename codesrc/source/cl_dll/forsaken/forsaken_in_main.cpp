/*
	forsaken_in_main.cpp
	Forsaken input handling.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "kbutton.h"
#include "usercmd.h"
#include "in_buttons.h"
#include "input.h"
#include "iviewrender.h"
#include "iclientmode.h"
#include "prediction.h"
#include "bitbuf.h"
#include "checksum_md5.h"
#include "hltvcamera.h"
#include <ctype.h> // isalnum()
#include <voice_status.h>
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/c_forsaken_player.h"

// Internal Classes
class CForsakenInput : public CInput
{
	typedef CInput BaseClass;

public:
	// Constructor & Deconstructor

	// Public Accessor Functions
	virtual int GetButtonBits(int nResetState);

	// Public Functions

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
};

// Function Prototypes
void CalcButtonBits(int& bits, int in_button, int in_ignore, kbutton_t *button, bool reset);
void IN_EndKick();
void IN_EndLean();
void IN_EndObjectiveInfo();
void IN_EndSprint();
void IN_StartKick();
void IN_StartLean();
void IN_StartObjectiveInfo();
void IN_StartSprint();
void KeyDown( kbutton_t *b, bool bIgnoreKey );
void KeyUp( kbutton_t *b, bool bIgnoreKey );

// Global Variable Declarations
extern bool g_bDisplayObjectiveInfo;
extern int s_ClearInputState;
static CForsakenInput g_Input;
static ConCommand startkick("+kick",IN_StartKick);
static ConCommand endkick("-kick",IN_EndKick);
static ConCommand startleanleft("+lean",IN_StartLean);
static ConCommand endleanleft("-lean",IN_EndLean);
static ConCommand startsprint("+sprint",IN_StartSprint);
static ConCommand endsprint("-sprint",IN_EndSprint);
static ConCommand startobjectiveshowinfo("+showobjectiveinfo", IN_StartObjectiveInfo);
static ConCommand endobjectiveshowinfo("-showobjectiveinfo", IN_EndObjectiveInfo);
IInput *input = (IInput*)&g_Input;

// New Key Defines
kbutton_t in_kick;
kbutton_t in_lean;
kbutton_t in_sprint;
kbutton_t in_objectives;

// Functions
int CForsakenInput::GetButtonBits(int nResetState)
/*
	
	Pre: 
	Post: 
*/
{
	int nBits = BaseClass::GetButtonBits(nResetState);

	CalcButtonBits(nBits, IN_KICK, s_ClearInputState, &in_kick, nResetState);
	//CalcButtonBits(nBits, IN_LEAN, s_ClearInputState, &in_lean, nResetState);
	CalcButtonBits(nBits, IN_SPRINT, s_ClearInputState, &in_sprint, nResetState);

	// FUGGIN HORRIBLE HACK OMG WTF BBQ
	// We shouldn't be telling the server that they aren't pressing the strafe button...
	if (nBits & IN_SPRINT && (nBits & IN_MOVELEFT || nBits & IN_MOVERIGHT))
	{
		CForsakenPlayer *pPlayer = ToForsakenPlayer(CBasePlayer::GetLocalPlayer());
		float fDecreaseSprint = (100.0f / 3.0f) * gpGlobals->frametime;

		if (pPlayer->GetSprint() >= fDecreaseSprint)
			nBits &= ~(IN_MOVELEFT|IN_MOVERIGHT);
	}

	return nBits;
}

// Global
void IN_EndKick()
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable the ability to kick for release.
#ifdef BETA_FORSAKEN
	KeyUp(&in_kick);
#endif
}

void IN_EndLean()
/*
	
	Pre: 
	Post: 
*/
{
	KeyUp(&in_lean);
}

void IN_EndObjectiveInfo()
/*
	
	Pre: 
	Post: 
*/
{
	g_bDisplayObjectiveInfo = false;
}

void IN_EndSprint()
/*
	
	Pre: 
	Post: 
*/
{
	KeyUp(&in_sprint);
}

void IN_StartKick()
/*
	
	Pre: 
	Post: 
*/
{
	// BETA DISABLE: Disable the ability to kick for release.
#ifdef BETA_FORSAKEN
	KeyDown(&in_kick);
#endif
}

void IN_StartLean()
/*
	
	Pre: 
	Post: 
*/
{
	KeyDown(&in_lean);
}

void IN_StartObjectiveInfo()
/*
	
	Pre: 
	Post: 
*/
{
	g_bDisplayObjectiveInfo = true;
}

void IN_StartSprint()
/*
	
	Pre: 
	Post: 
*/
{
	KeyDown(&in_sprint);
}