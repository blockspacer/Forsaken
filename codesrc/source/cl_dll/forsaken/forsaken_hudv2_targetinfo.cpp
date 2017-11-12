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
#include "tier0/memdbgon.h"

ConVar cl_fskn_playeridlocation("cl_fskn_playeridlocation", "1", FCVAR_ARCHIVE, "Sets the location of the player identify. 0 = bottom right, 1 = center, 2 = follow entity");

// Internal Classes
class CForsakenHudV2_TargetInfo : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CForsakenHudV2_TargetInfo, vgui::Panel);

public:
	// Constructor & Deconstructor
	CForsakenHudV2_TargetInfo(const char *czElementName);

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
	CPanelAnimationVar(vgui::HFont, m_hTextFont, "TargetFont", "Default");

	int m_textureBackground;
};

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudV2_TargetInfo);

// Constructor & Deconstructor
CForsakenHudV2_TargetInfo::CForsakenHudV2_TargetInfo(const char *czElementName) :
	CHudElement(czElementName), BaseClass(NULL, "ForsakenHudV2_TargetInfo")
{
	// Set visibility rules and the parent.
	SetHiddenBits(0);
	SetParent(g_pClientMode->GetViewport());
}

// Functions
void CForsakenHudV2_TargetInfo::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenHudV2_TargetInfo::Init()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_TargetInfo::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	Color fontColor;

	// Get pointer to our information
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());

	if (!pPlayer)
		return;

	// Are we looking at anyone?
	int lookPlayerId = pPlayer->GetLookPlayerIndex();

	if (lookPlayerId == -1)
	{
		return;
	}

	// Get player we are looking at
	C_ForsakenPlayer *pLookPlayer = ToForsakenPlayer(UTIL_PlayerByIndex(lookPlayerId));
	
	// Get team of us and player
	int theirTeam = pLookPlayer->GetTeamNumber();
	int ourTeam = pPlayer->GetTeamNumber();

	// Don't show info for certain spectator modes.
	if ( pPlayer->GetObserverMode() == OBS_MODE_IN_EYE || 
		pPlayer->GetObserverMode() == OBS_MODE_CHASE )
	{
		return;
	}

	// Get player position
	int iX, iY;

	// Set the font color to that of the player we are looking at
	fontColor = GameResources()->GetTeamColor(pLookPlayer->GetTeamNumber());

	// Set panel size to hud size
	int nHudWidth, nHudHeight;
	GetHudSize(nHudWidth, nHudHeight);
	SetSize(nHudWidth, nHudHeight);	// sets size of panel

	// Reset color
	vgui::surface()->DrawSetTextColor(fontColor);

	// Convert player name.
	wchar_t czTextName[MAX_PLAYER_NAME_LENGTH] = L"";
	mbstowcs(czTextName, pLookPlayer->GetPlayerName(), MAX_PLAYER_NAME_LENGTH);

	wchar_t czTextHealth[32] = L"";
	swprintf(czTextHealth, L"Health: %d", pLookPlayer->GetHealth());

	// Calculate the location.
	switch (cl_fskn_playeridlocation.GetInt())
	{
	case 0:
		{
			// bottom right
			iX = nHudWidth - 40 - UTIL_ComputeStringWidth(m_hTextFont, czTextName);
			iY = nHudHeight - 110;
			break;
		}
	case 1:
		{
			//center
			iX = (nHudWidth / 2) - (UTIL_ComputeStringWidth(m_hTextFont, czTextName) / 2);
			iY = (nHudHeight / 2) + 12;
			break;
		}
	case 2:
		{
			// follow
			if (!GetTargetInScreenSpace(pLookPlayer, iX, iY)) return;
			break;
		}
	default:
		{
			//center
			iX = (nHudWidth / 2) - (UTIL_ComputeStringWidth(m_hTextFont, czTextName) / 2);
			iY = (nHudHeight / 2) + 12;
		}
	}

	//Paint the player name
	PaintString(m_hTextFont, iX, iY, czTextName);

	// Paint health only if they are on the same team.
	if (ourTeam == theirTeam)
		PaintString(m_hTextFont, iX, iY+12, czTextHealth);
}

void CForsakenHudV2_TargetInfo::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc,
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

void CForsakenHudV2_TargetInfo::Reset()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_TargetInfo::VidInit()
/*
	
	Pre: 
	Post: 
*/
{
}