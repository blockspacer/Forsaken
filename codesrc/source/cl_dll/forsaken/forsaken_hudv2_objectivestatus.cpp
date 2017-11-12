/*
	forsaken_hudv2_objectivestatus.cpp
	The objective status HUD v2.0 element for Forsaken.

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
#include "c_baseplayer.h"
#include "c_forsaken_player.h"
#include "utlvector.h"
#include "c_forsaken_objective_entity.h"
#include "c_forsaken_capture_entity.h"
#include "c_forsaken_destroy_entity.h"
#include "c_forsaken_objective_list.h"
#include "tier0/memdbgon.h"

// Internal Classes
class CForsakenHudV2_ObjectiveStatus : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CForsakenHudV2_ObjectiveStatus, vgui::Panel);

public:
	// Constructor & Deconstructor
	CForsakenHudV2_ObjectiveStatus(const char *czElementName);

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
	void PaintCapture(C_ForsakenObjectiveEntity *pObjective);
	void PaintDestroy(C_ForsakenObjectiveEntity *pObjective);
	void PaintPercentage(float fPercentage);
	void PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText);

	// Protected Variables
	CPanelAnimationVar(Color, m_ObjectiveStatusColor, "ObjectiveStatusColor", "Orange");
	CPanelAnimationVar(Color, m_PercentageBarColor, "PercentageBarColor", "Orange");
	CPanelAnimationVar(vgui::HFont, m_hCaptureStatusFont, "StatusFont", "Default");
	CPanelAnimationVarAliasType(float, m_fPercentageBarXPos, "percentagebar_xpos", "0", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fPercentageBarYPos, "percentagebar_ypos", "10", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fPercentageBarHeight, "percentagebar_height", "5", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fPercentageBarWidth, "percentagebar_width", "180", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fInfoTextXPos, "infotext_xpos", "25", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fInfoTextYPos, "infotext_ypos", "5", 
		"proportional_float");
};

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudV2_ObjectiveStatus);

// Constructor & Deconstructor
CForsakenHudV2_ObjectiveStatus::CForsakenHudV2_ObjectiveStatus(const char *czElementName) :
	CHudElement(czElementName), BaseClass(NULL, "ForsakenHudV2_ObjectiveStatus")
{
	// Set visibility rules and the parent.
	SetHiddenBits(HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT | HIDEHUD_CREDITS);
	SetParent(g_pClientMode->GetViewport());
}

// Functions
void CForsakenHudV2_ObjectiveStatus::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenHudV2_ObjectiveStatus::Init()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_ObjectiveStatus::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenObjectiveEntity *pObjective = NULL;
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
	int nHeight, nWidth;

	// We need a player.
	if (!pPlayer)
		return;

	// Get the panel size.
	GetSize(nWidth, nHeight);

	// Initial paint state.
	SetPaintBackgroundEnabled(false);

	// Fetch the objective entity.
	pObjective = pPlayer->GetObjectiveEntity();

	// We also need an objective.
	if (!pObjective)
		return;

	// Determine the objective type and go accordingly.
	switch (pObjective->GetObjectiveType())
	{
	case OBJECTIVE_TYPE_CAPTURE:
		PaintCapture(pObjective);
		break;

	case OBJECTIVE_TYPE_DESTROY:
		PaintDestroy(pObjective);
		break;

	case OBJECTIVE_TYPE_BASE:
	default:
		break;
	}
}

void CForsakenHudV2_ObjectiveStatus::PaintCapture(C_ForsakenObjectiveEntity *pObjective)
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
	C_ForsakenCaptureObjectiveEntity *pCapObjective = 
		static_cast<C_ForsakenCaptureObjectiveEntity*>(pObjective);
	float fPercentage = 0.0f;
	wchar_t czText[512] = L"";

	// If the player is on the team that owns the objective, don't paint the bar.
	if (pPlayer->GetTeamNumber() == pCapObjective->GetTeamOwner())
		return;

	// Calculate the percentage ratio.
	switch (pPlayer->GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		fPercentage = 1.0f - 
			(pCapObjective->GetRemainingTimeCiv() / pCapObjective->GetCaptureTime());
		break;

	case TEAM_GOVERNORS:
		fPercentage = 1.0f - 
			(pCapObjective->GetRemainingTimeGov() / pCapObjective->GetCaptureTime());
		break;

	case TEAM_RELIGIOUS:
		fPercentage = 1.0f - 
			(pCapObjective->GetRemainingTimeRel() / pCapObjective->GetCaptureTime());
		break;
	}

	// Draw the capture objective string information.
	swprintf(czText, L"Capturing: %hs", pObjective->GetObjectiveName());
	vgui::surface()->DrawSetTextColor(m_ObjectiveStatusColor);
	PaintString(m_hCaptureStatusFont, m_fInfoTextXPos, m_fInfoTextYPos, czText);

	// Draw the progress bar.
	PaintPercentage(fPercentage);
}

void CForsakenHudV2_ObjectiveStatus::PaintDestroy(C_ForsakenObjectiveEntity *pObjective)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_ObjectiveStatus::PaintPercentage(float fPercentage)
/*
	
	Pre: 
	Post: 
*/
{
	float fPercentageWidth = (m_fPercentageBarXPos + m_fPercentageBarWidth) * fPercentage;

	// Draw the border.
	vgui::surface()->DrawSetColor(m_PercentageBarColor);
	vgui::surface()->DrawOutlinedRect(m_fPercentageBarXPos, m_fPercentageBarYPos, 
		m_fPercentageBarXPos + m_fPercentageBarWidth, m_fPercentageBarYPos + m_fPercentageBarWidth);

	// Draw the percentage.
	vgui::surface()->DrawFilledRect(m_fPercentageBarXPos, m_fPercentageBarYPos, fPercentageWidth, 
		m_fPercentageBarYPos + m_fPercentageBarWidth);
}

void CForsakenHudV2_ObjectiveStatus::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, 
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

void CForsakenHudV2_ObjectiveStatus::Reset()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_ObjectiveStatus::VidInit()
/*
	
	Pre: 
	Post: 
*/
{
}