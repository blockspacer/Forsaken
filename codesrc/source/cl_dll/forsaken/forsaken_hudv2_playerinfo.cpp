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
#include "c_baseplayer.h"
#include "c_forsaken_player.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "tier0/memdbgon.h"

// Internal Classes
class CForsakenHudV2_PlayerInfo : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CForsakenHudV2_PlayerInfo, vgui::Panel);

public:
	// Constructor & Deconstructor
	CForsakenHudV2_PlayerInfo(const char *czElementName);

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
	void UpdateAmmo(int &nAmmo, int &nClip);
	void PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText);

	// Protected Variables
	int m_textureBackground;

	CPanelAnimationVar(Color, m_AmmoBulletsColor, "AmmoColor", "Orange");
	CPanelAnimationVar(Color, m_AmmoClipsColor, "ClipsColor", "Orange");
	CPanelAnimationVar(Color, m_RoundTimerColor, "RoundTimeColor", "Orange");
	CPanelAnimationVar(vgui::HFont, m_hNumberFont, "NumberFont", "HudNumbers");
	CPanelAnimationVar(vgui::HFont, m_hSmallNumberFont, "SmallNumberFont", "HudNumbersSmall");
	CPanelAnimationVarAliasType(float, m_fRoundTimerXPos, "roundtimer_xpos", "25", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fRoundTimerYPos, "roundtimer_ypos", "5", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fAmmoBulletsXPos, "bullets_xpos", "25", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fAmmoBulletsYPos, "bullets_ypos", "35", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fAmmoClipsXPos, "clips_xpos", "45", 
		"proportional_float");
	CPanelAnimationVarAliasType(float, m_fAmmoClipsYPos, "clips_ypos", "35", 
		"proportional_float");
};

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudV2_PlayerInfo);

// Constructor & Deconstructor
CForsakenHudV2_PlayerInfo::CForsakenHudV2_PlayerInfo(const char *czElementName) :
	CHudElement(czElementName), BaseClass(NULL, "ForsakenHudV2_PlayerInfo")
{
	// Set visibility rules and the parent.
	SetHiddenBits(HIDEHUD_WEAPONSELECTION | HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT | HIDEHUD_CREDITS);
	SetParent(g_pClientMode->GetViewport());

	// Load the background texture.
	m_textureBackground = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_textureBackground, "hudv2/playerinfo_background", 
		true, true);
}

// Functions
void CForsakenHudV2_PlayerInfo::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenHudV2_PlayerInfo::Init()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_PlayerInfo::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
	int nHeight, nWidth;

	if (!pPlayer)
		return;

	// Get the panel size.
	GetSize(nWidth, nHeight);

	// Initial paint state.
	SetPaintBackgroundEnabled(false);

	// Draw the background.
	vgui::surface()->DrawSetTexture(m_textureBackground);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawTexturedRect(0, 0, nWidth, nHeight);

	// Draw the round-timer.
	{
		int nMinutes = 0, nSeconds = 0;
		int nRoundTime = ForsakenGameRules()->GetRemainingTimeInRound();
		wchar_t czRoundTime[32];

		// Figure out the time in mortal-readable format.
		nMinutes = nRoundTime / 60;
		nSeconds = nRoundTime % 60;

		// Round time string.
		swprintf(czRoundTime, L"%d:%.2d", nMinutes, nSeconds);

		// Paint the current round time.
		vgui::surface()->DrawSetTextColor(m_RoundTimerColor);
		PaintString(m_hNumberFont, m_fRoundTimerXPos, m_fRoundTimerYPos, czRoundTime);
	}

	// Draw the ammo.
	{
		int nAmmo = 0, nClip = 0;
		wchar_t czAmmo[10];

		// Get the ammo.
		UpdateAmmo(nAmmo, nClip);

		// Convert the ammo to a string and print it.
		swprintf(czAmmo, L"%d", nAmmo);
		vgui::surface()->DrawSetTextColor(m_AmmoBulletsColor);
		PaintString(m_hNumberFont, m_fAmmoBulletsXPos, m_fAmmoBulletsYPos, czAmmo);

		// Convert the clip to a string and print it.
		swprintf(czAmmo, L"%d", nClip);
		vgui::surface()->DrawSetTextColor(m_AmmoClipsColor);
		PaintString(m_hSmallNumberFont, m_fAmmoClipsXPos, m_fAmmoClipsYPos, czAmmo);
	}
}

void CForsakenHudV2_PlayerInfo::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, 
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

void CForsakenHudV2_PlayerInfo::Reset()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_PlayerInfo::UpdateAmmo(int &nAmmo, int &nClip)
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
	C_BaseCombatWeapon *pWeapon = GetActiveWeapon();

	if (!pPlayer || !pWeapon)
		return;

	// Get the ammo.
	nAmmo = pWeapon->Clip1();

	// Calculate the ammo remaining and clips remaining.
	if (nAmmo < 0)
	{
		nAmmo = pPlayer->GetAmmoCount(pWeapon->GetPrimaryAmmoType());
		nClip = 0;
	}
	else
		nClip = pPlayer->GetAmmoCount(pWeapon->GetPrimaryAmmoType());
}

void CForsakenHudV2_PlayerInfo::VidInit()
/*
	
	Pre: 
	Post: 
*/
{
}