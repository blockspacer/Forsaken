/*
	forsaken_hudv2_playerstatus.cpp
	The player status HUD v2.0 element for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include <vgui_controls/AnimationController.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include "igameresources.h"
#include "c_playerresource.h"
#include "c_baseplayer.h"
#include "c_forsaken_player.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "c_forsaken_team.h"
#include "tier0/memdbgon.h"

// Internal Classes
class CForsakenHudV2_Notifications : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CForsakenHudV2_Notifications, vgui::Panel);

public:
	// Constructor & Deconstructor
	CForsakenHudV2_Notifications(const char *czElementName);

	// Public Accessor Functions

	// Public Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Init();
	virtual void Paint();
	virtual void Reset();
	virtual void VidInit();

	// Public Variables

protected:
	// Protected Functions
	void PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText);

	// Protected Variables
	CPanelAnimationVar(vgui::HFont, m_hTextFont, "NotificationFont", "Default");
	CPanelAnimationVar(Color, m_NotificationColor, "NotificationPulseColor", "255 0 0 255");
	CPanelAnimationVar(int, m_Alpha, "Alpha", "255");

	bool NewRound;

	int m_textureBackground;
};

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudV2_Notifications);

// Constructor & Deconstructor
CForsakenHudV2_Notifications::CForsakenHudV2_Notifications(const char *czElementName) :
	CHudElement(czElementName), BaseClass(NULL, "ForsakenHudV2_Notifications")
{
	// Set visibility rules and the parent.
	SetHiddenBits(0);
	SetParent(g_pClientMode->GetViewport());

	NewRound = true;
}

// Functions
void CForsakenHudV2_Notifications::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenHudV2_Notifications::Init()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_Notifications::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	// Initial paint state.
	SetPaintBackgroundEnabled(false);

	// Who is starving
	C_ForsakenTeam *pCivTeam = static_cast<C_ForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	C_ForsakenTeam *pGovTeam = static_cast<C_ForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	C_ForsakenTeam *pRelTeam = static_cast<C_ForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);

	// How many are starving?
	int nStarving = pCivTeam->IsTeamStarving() + pGovTeam->IsTeamStarving() + pRelTeam->IsTeamStarving();
	int nYOffset = 0;

    // Show status for a new round only
	if (NewRound)
	{
		m_Alpha = 255;
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("NotificationPulse");
		NewRound = false;
	}

	// Get player info
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());

	if (!pPlayer)
		return;

	// Setup sizes and colors, etc.
	int width, height, nTextWidth, nTextHeight;
	SetAlpha(m_Alpha);
	vgui::surface()->GetScreenSize(width, height);
	SetSize(width, height);
	Color textColor;

	if(pCivTeam->IsTeamStarving())
	{
		if (pPlayer->GetTeamNumber() == TEAM_CIVILIANS)
			textColor = Color(255,0,0,255);
		else
			textColor = Color(0,255,0,255);

		vgui::surface()->DrawSetTextColor(textColor);
		wchar_t czText[256] = L"The civilian faction is starving!";
		vgui::surface()->GetTextSize(m_hTextFont, czText, nTextWidth, nTextHeight);

		PaintString(m_hTextFont, width/2.0f - nTextWidth/2.0f, height/2.0f + nYOffset*nTextHeight, czText);
		nYOffset++;
	}

	if(pGovTeam->IsTeamStarving())
	{
		if (pPlayer->GetTeamNumber() == TEAM_GOVERNORS)
			textColor = Color(255,0,0,255);
		else
			textColor = Color(0,255,0,255);

		vgui::surface()->DrawSetTextColor(textColor);
		wchar_t czText[256] = L"The governor faction is starving!";
		vgui::surface()->GetTextSize(m_hTextFont, czText, nTextWidth, nTextHeight);

		PaintString(m_hTextFont, width/2.0f - nTextWidth/2.0f, height/2.0f + nYOffset*nTextHeight, czText);
		nYOffset++;
	}

	if(pRelTeam->IsTeamStarving())
	{
		if (pPlayer->GetTeamNumber() == TEAM_RELIGIOUS)
			textColor = Color(255,0,0,255);
		else
			textColor = Color(0,255,0,255);

		vgui::surface()->DrawSetTextColor(textColor);
		wchar_t czText[256] = L"The religious faction is starving!";
		vgui::surface()->GetTextSize(m_hTextFont, czText, nTextWidth, nTextHeight);

		PaintString(m_hTextFont, width/2.0f - nTextWidth/2.0f, height/2.0f + nYOffset*nTextHeight, czText);
		nYOffset++;
	}
}

void CForsakenHudV2_Notifications::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc,
	wchar_t *czText)
/*
	
	Pre: 
	Post: 
*/
{
	// Set the font and position then draw the character.
	vgui::surface()->DrawSetTextFont(hFont);
	vgui::surface()->DrawSetTextPos(nXLoc, nYLoc);

	// Draw the string.
	for (wchar_t *cChar = czText; *cChar != 0; cChar++)
		vgui::surface()->DrawUnicodeChar(*cChar);
}

void CForsakenHudV2_Notifications::Reset()
/*
	
	Pre: 
	Post: 
*/
{
	NewRound = true;
}

void CForsakenHudV2_Notifications::VidInit()
/*
	
	Pre: 
	Post: 
*/
{
	NewRound = true;
}