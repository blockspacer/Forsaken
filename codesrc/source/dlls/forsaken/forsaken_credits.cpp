/*
	forsaken_credits.cpp
	The point entity in Forsaken that handles all credits.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_shareddefs.h"

// Internal Class Declarations
class CCredits : public CPointEntity
{
	DECLARE_CLASS(CCredits, CPointEntity);
	DECLARE_DATADESC();

public:
	// Public Accessor Functions

	// Public Functions
	virtual void OnRestore();
	void InputRollCredits(inputdata_t &inputdata);
	void InputRollOutroCredits(inputdata_t &inputdata);
	void InputShowLogo(inputdata_t &inputdata);
	void InputSetLogoLength(inputdata_t &inputdata);
	void RollIntroCreditsThink();
	void RollOutroCreditsThink();
	void ShowLogoThink();
	void Spawn();

	// Public Variables
	COutputEvent m_OnCreditsDone;

private:
	// Private Functions
	void RollIntroCredits();
	void RollOutroCredits();
	void ShowLogo();

	// Private Variables
	bool m_bRolledOutroCredits;
	float m_flLogoLength;
};

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(env_credits, CCredits);

BEGIN_DATADESC( CCredits )
	DEFINE_INPUTFUNC(FIELD_VOID, "RollCredits", InputRollCredits),
	DEFINE_INPUTFUNC(FIELD_VOID, "RollOutroCredits", InputRollOutroCredits),
	DEFINE_INPUTFUNC(FIELD_VOID, "ShowLogo", InputShowLogo),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetLogoLength", InputSetLogoLength),
	DEFINE_OUTPUT(m_OnCreditsDone, "OnCreditsDone"),

	DEFINE_FIELD(m_bRolledOutroCredits, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flLogoLength, FIELD_FLOAT),

	DEFINE_THINKFUNC(RollIntroCreditsThink),
	DEFINE_THINKFUNC(RollOutroCreditsThink),
	DEFINE_THINKFUNC(ShowLogoThink),
END_DATADESC()

// Function Prototypes
static void StopCredits();

// Global Variable Decleration
static ConCommand creditsdone("stop_credits", StopCredits);

// Functions
// CCredits
void CCredits::Spawn()
/*
	Description.
	Pre: 
	Post: 
*/
{
	SetSolid(SOLID_NONE);
	SetMoveType(MOVETYPE_NONE);
}

void CCredits::OnRestore()
/*
	Description.
	Pre: 
	Post: 
*/
{
	BaseClass::OnRestore();

	if (m_bRolledOutroCredits)
	{
		// Roll them again so that the client .dll will send the "creditsdone" message and we'll
		//  actually get back to the main menu
		RollOutroCredits();
	}
}

void CCredits::RollIntroCreditsThink()
/*
	Description.
	Pre: 
	Post: 
*/
{
	RollIntroCredits();
}

void CCredits::RollOutroCreditsThink()
/*
	Description.
	Pre: 
	Post: 
*/
{
	RollOutroCredits();
}

void CCredits::ShowLogoThink()
/*
	Description.
	Pre: 
	Post: 
*/
{
	ShowLogo();
}

void CCredits::ShowLogo()
/*
	Description.
	Pre: 
	Post: 
*/
{
	CBasePlayer *pPlayer = UTIL_PlayerByIndex(1);

	if (!pPlayer)
	{
		SetNextThink(gpGlobals->curtime);
		SetThink(&CCredits::ShowLogoThink);

		return;
	}

	CSingleUserRecipientFilter filter(pPlayer);

	// Make the filter reliable.
	filter.MakeReliable();

	if (m_flLogoLength)
	{
		UserMessageBegin(filter, "LogoTimeMsg");
			WRITE_FLOAT(m_flLogoLength);
		MessageEnd();
	}
	else
	{
		UserMessageBegin(filter, "CreditsMsg");
			WRITE_BYTE(CREDITS_LOGO);
		MessageEnd();
	}
}

void CCredits::RollIntroCredits()
/*
	Description.
	Pre: 
	Post: 
*/
{
	CBasePlayer *pPlayer = UTIL_PlayerByIndex(1);

	if (!pPlayer)
	{
		SetNextThink(gpGlobals->curtime);
		SetThink(&CCredits::RollIntroCreditsThink);

		return;
	}

	CSingleUserRecipientFilter filter(pPlayer);

	// Make the filter reliable.
	filter.MakeReliable();

	UserMessageBegin(filter, "CreditsMsg");
		WRITE_BYTE(CREDITS_INTRO);
	MessageEnd();
}

void CCredits::RollOutroCredits()
/*
	Description.
	Pre: 
	Post: 
*/
{
	CBasePlayer *pPlayer = UTIL_PlayerByIndex(1);

	if (!pPlayer)
	{
		SetNextThink(gpGlobals->curtime);
		SetThink(&CCredits::RollOutroCreditsThink);

		return;
	}

	CSingleUserRecipientFilter filter(pPlayer);

	// Make the filter reliable.
	filter.MakeReliable();

	// Send the message to roll outro credits.
	UserMessageBegin(filter, "CreditsMsg");
		WRITE_BYTE(CREDITS_OUTRO);
	MessageEnd();

	engine->ClientCommand(pPlayer->edict(), "play credits.mp3");
}

void CCredits::InputRollOutroCredits(inputdata_t &inputdata)
/*
	Description.
	Pre: 
	Post: 
*/
{
	RollOutroCredits();

	// In case we save restore
	m_bRolledOutroCredits = true;
}

void CCredits::InputShowLogo(inputdata_t &inputdata)
/*
	Description.
	Pre: 
	Post: 
*/
{
	ShowLogo();
}

void CCredits::InputSetLogoLength(inputdata_t &inputdata)
/*
	Description.
	Pre: 
	Post: 
*/
{
	m_flLogoLength = inputdata.value.Float();
}

void CCredits::InputRollCredits(inputdata_t &inputdata)
/*
	Description.
	Pre: 
	Post: 
*/
{
	RollIntroCredits();
}

// Global
static void StopCredits()
{
	CCredits *pCredits = (CCredits*)gEntList.FindEntityByClassname(NULL, "env_credits");

	if (pCredits)
		pCredits->m_OnCreditsDone.FireOutput(pCredits, pCredits);
}