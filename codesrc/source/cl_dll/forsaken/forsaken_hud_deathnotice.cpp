/*
	forsaken_hud_weaponselection.cpp
	Weapon selection code for the Forsaken HUD.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "iclientmode.h"
#include "history_resource.h"
#include "input.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/Panel.h>
#include "vgui/ILocalize.h"
#include "igameresources.h"
#include "hud_macros.h"
#include "c_playerresource.h"
#include "clientmode_forsaken.h"
#include "tier0/memdbgon.h"

#define NOTICE_TYPE_DEATH	1
#define NOTICE_TYPE_POINTS	2

#define POINT_NOTICE_LENGTH	256

// Internal Classes
struct tagNoticeItem
{
	float fDisplayTime;
	int nNoticeType;

	// Death Notice
	bool bSuicide;
	bool bTeamKill;
	char czKiller[MAX_PLAYER_NAME_LENGTH];
	char czVictim[MAX_PLAYER_NAME_LENGTH];
	CHudTexture *deathIcon;
	int nKillerTeam;
	int nVictimTeam;

	// Points
	char czMessage[POINT_NOTICE_LENGTH];
};

class CForsakenHudDeathNotice : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CForsakenHudDeathNotice, vgui::Panel);

public:
	// Constructor & Deconstructor
	CForsakenHudDeathNotice(const char *czElementName);

	// Public Accessor Functions

	// Public Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void FireGameEvent(IGameEvent *event);
	virtual void Init();
	virtual void Paint();
	virtual void Reset();
	virtual void VidInit();
	void MsgFunc_PointsNotice(bf_read &rMessage);

	// Public Variables

protected:
	// Protected Functions
	void PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText);
	void RenderDeathNotice(tagNoticeItem *pDeathNotice, int nOffset);
	void RenderPointNotice(tagNoticeItem *pPointsNotice, int nOffset);

	// Protected Variables
	CHudTexture *m_pSuicideTexture;
	CHudTexture *m_pTeamKillTexture;
	CPanelAnimationVar(bool, m_bRightJustify, "RightJustify", "1");
	CPanelAnimationVar(Color, m_colIcon, "IconColor", "SelectionTextFg");
	CPanelAnimationVar(int, m_nItemHeight, "ItemHeight", "15");
	CPanelAnimationVar(int, m_nMaxDeathNotices, "MaxDisplayed", "3");
	CPanelAnimationVar(vgui::HFont, m_hTextFont, "TextFont", "Default");
	CUtlVector<tagNoticeItem> m_vecDeathNotices;
};

using namespace vgui;

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudDeathNotice);
DECLARE_HUD_MESSAGE(CForsakenHudDeathNotice, PointsNotice);
static ConVar fskn_deathnotice_time("fskn_deathnotice_time", "5", 0, 
	"The amount of time each death notice is displayed (0.5s-8.0s).", true, 0.5f, true, 8.0f);

// Constructor & Deconstructor
CForsakenHudDeathNotice::CForsakenHudDeathNotice(const char *czElementName) :
	CHudElement(czElementName), BaseClass(NULL, "ForsakenHudDeathNotice")
{
	// Set the parent and hidden flags.
	SetHiddenBits(HIDEHUD_MISCSTATUS | HIDEHUD_CREDITS);
	SetParent(g_pClientMode->GetViewport());

	// Set the default icons.
	m_pSuicideTexture = NULL;
	m_pTeamKillTexture = NULL;
}

// Functions
void CForsakenHudDeathNotice::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBackgroundEnabled(false);
}

void CForsakenHudDeathNotice::FireGameEvent(IGameEvent *event)
/*
	
	Pre: 
	Post: 
*/
{
	char czDeathMsg[1024] = "";
	const char *czKillerName = NULL, *czVictimName = NULL;
	int nKiller = 0, nVictim = 0;
	tagNoticeItem newDeathNotice;

	// Set the notice to a death notice.
	newDeathNotice.nNoticeType = NOTICE_TYPE_DEATH;

	// Fetch the killer and victim.
	if (engine)
	{
		nKiller = engine->GetPlayerForUserID(event->GetInt("attacker"));
		nVictim = engine->GetPlayerForUserID(event->GetInt("userid"));
	}

	// Fetch the player and victim usernames.
	if (GameResources())
	{
		czKillerName = GameResources()->GetPlayerName(nKiller);
		czVictimName = GameResources()->GetPlayerName(nVictim);
	}

	// Valid killer name?
	if (!czKillerName)
		czKillerName = "";

	// Valid victim name?
	if (!czVictimName)
		czVictimName = "";

	// The death notice isn't for this player.
	/*if (Q_stricmp(czKillerName, C_BasePlayer::GetLocalPlayer()->GetPlayerName()) == 0 || 
		Q_stricmp(czVictimName, C_BasePlayer::GetLocalPlayer()->GetPlayerName()))
		return;*/

	// Copy the killer and victim name.
	Q_strncpy(newDeathNotice.czKiller, czKillerName, MAX_PLAYER_NAME_LENGTH);
	Q_strncpy(newDeathNotice.czVictim, czVictimName, MAX_PLAYER_NAME_LENGTH);

	// Fetch the players teams.
	if (GameResources())
	{
		newDeathNotice.nKillerTeam = GameResources()->GetTeam(nKiller);
		newDeathNotice.nVictimTeam = GameResources()->GetTeam(nVictim);
	}

	// Was this a suicide?
	newDeathNotice.bSuicide = (!nKiller || nKiller == nVictim);

	// How about a team kill?
	newDeathNotice.bTeamKill = (newDeathNotice.nKillerTeam == newDeathNotice.nVictimTeam);

	// Set the player names.
	newDeathNotice.fDisplayTime = gpGlobals->curtime + fskn_deathnotice_time.GetFloat();

	// Set the death icon texture.
	if (newDeathNotice.bSuicide)
		newDeathNotice.deathIcon = m_pSuicideTexture;
	else if (newDeathNotice.bTeamKill)
		newDeathNotice.deathIcon = m_pTeamKillTexture;
	else
	{
		char czWeaponIconName[256] = "";

		// Make the name for the death icon.
		Q_snprintf(czWeaponIconName, 256, "fskn_death_%s", event->GetString("weapon", "generic"));

		// Fetch the death icon.
		newDeathNotice.deathIcon = gHUD.GetIcon(czWeaponIconName);
	}

	// Add the death notice to the list.
	m_vecDeathNotices.AddToTail(newDeathNotice);

	// Prepare the death message echoed out to console.
	Q_snprintf(czDeathMsg, sizeof(czDeathMsg), "%s killed %s with %s.\n", czKillerName, 
		czVictimName, event->GetString("weapon", "generic"));

	// Echo out the death message to console.
	Msg(czDeathMsg);
}

void CForsakenHudDeathNotice::Init()
/*
	
	Pre: 
	Post: 
*/
{
	// We want to pay attention to deaths.
	gameeventmanager->AddListener(this, "player_death", false);

	HOOK_HUD_MESSAGE(CForsakenHudDeathNotice, PointsNotice);
}

void CForsakenHudDeathNotice::MsgFunc_PointsNotice(bf_read &rMessage)
/*
	
	Pre: 
	Post: 
*/
{
	tagNoticeItem newPointNotice;

	newPointNotice.nNoticeType = NOTICE_TYPE_POINTS;

	// The number of points.
	int nPoints = rMessage.ReadShort();
	char czVictimName[MAX_PLAYER_NAME_LENGTH] = "";
	int nNoticeType = rMessage.ReadShort();

	// Player team.
	newPointNotice.nKillerTeam = rMessage.ReadShort();

	// Initial message.
	Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%d points for", nPoints);

	// What kind of point notice?
	switch (nNoticeType)
	{
	case UMSG_POINTS_KILLASSIST:
		rMessage.ReadString(czVictimName, MAX_PLAYER_NAME_LENGTH);
		Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%s kill assist on %s", newPointNotice.czMessage, czVictimName);
		break;

	case UMSG_POINTS_KILL:
		rMessage.ReadString(czVictimName, MAX_PLAYER_NAME_LENGTH);
		Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%s killing %s", newPointNotice.czMessage, czVictimName);
		break;

	case UMSG_POINTS_OBJCAPTURE:
		Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%s objective capture", newPointNotice.czMessage);
		break;

	case UMSG_POINTS_OBJDESTROY:
		Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%s objective destroy", newPointNotice.czMessage);
		break;

	case UMSG_POINTS_OBJASSIST:
		Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%s objective assist", newPointNotice.czMessage);
		break;

	case UMSG_POINTS_PROTECTLDR:
		Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%s protecting leader", newPointNotice.czMessage);
		break;

	case UMSG_POINTS_TEAMKILL:
		Q_snprintf(newPointNotice.czMessage, POINT_NOTICE_LENGTH, "%s team killing", newPointNotice.czMessage);
		break;

	default:
		return;
	}

	// Display time.
	newPointNotice.fDisplayTime = gpGlobals->curtime + fskn_deathnotice_time.GetFloat();

	// Echo out the points message to console.
	Msg(newPointNotice.czMessage);
	Msg("\n");

	m_vecDeathNotices.AddToTail(newPointNotice);
}

void CForsakenHudDeathNotice::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	// Set the color and font.
	surface()->DrawSetTextColor(COLOR_BLUE);
	surface()->DrawSetTextFont(m_hTextFont);

	for (int i = 0; i < m_vecDeathNotices.Count(); i++)
	{
		// If the display time has expired, remove it.
		if (m_vecDeathNotices[i].fDisplayTime < gpGlobals->curtime)
			m_vecDeathNotices.Remove(i--);
	}

	for (int i = m_vecDeathNotices.Count() - 1; i >= 0; i--)
	{
		if ((m_vecDeathNotices.Count() - i) > m_nMaxDeathNotices)
			break;

		switch (m_vecDeathNotices[i].nNoticeType)
		{
		case NOTICE_TYPE_DEATH:
			RenderDeathNotice(&m_vecDeathNotices[i], i);
			break;

		case NOTICE_TYPE_POINTS:
			RenderPointNotice(&m_vecDeathNotices[i], i);
			break;
		}
	}
}

void CForsakenHudDeathNotice::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, 
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

void CForsakenHudDeathNotice::RenderDeathNotice(tagNoticeItem *pDeathNotice, int nOffset)
/*
	
	Pre: 
	Post: 
*/
{
	// Invalid icon, so fuck it.
	if (!pDeathNotice->deathIcon)
		return;

	int nIconHeight, nIconWidth, nLen, nX, nY;
	int nYOffset = GetClientModeForsakenNormal()->GetDeathMessageStartHeight();
	wchar_t czKillerName[MAX_PLAYER_NAME_LENGTH];
	wchar_t czVictimName[MAX_PLAYER_NAME_LENGTH];

	// Convert the killer/victim name to unicode.
	vgui::localize()->ConvertANSIToUnicode(pDeathNotice->czKiller, czKillerName, sizeof(czKillerName));
	vgui::localize()->ConvertANSIToUnicode(pDeathNotice->czVictim, czVictimName, sizeof(czVictimName));

	// Calculate the length of the victim name and the Y-offset.
	nLen = UTIL_ComputeStringWidth(m_hTextFont, czVictimName);
	nY = nYOffset + (m_nItemHeight * nOffset);

	// Fetch the icon height & width.
	if (pDeathNotice->deathIcon->bRenderUsingFont)
	{
		nIconHeight = surface()->GetFontTall(pDeathNotice->deathIcon->hFont);
		nIconWidth = surface()->GetCharacterWidth(pDeathNotice->deathIcon->hFont, 
			pDeathNotice->deathIcon->cCharacterInFont);
	}
	else
	{
		float fScale = ((float)ScreenHeight() / 480.0f);

		// Apply scaling.
		nIconHeight = (int)(fScale * (float)pDeathNotice->deathIcon->Height());
		nIconWidth = (int)(fScale * (float)pDeathNotice->deathIcon->Width());
	}

	// Calculate the X offset.
	if (m_bRightJustify)
		nX = GetWide() - nLen - nIconWidth;
	else
		nX = 0;

	// We only draw the killer's name for a suicide.
	if (!pDeathNotice->bSuicide)
	{
		if (m_bRightJustify)
			nX -= UTIL_ComputeStringWidth(m_hTextFont, czKillerName);

		// Set the color based off of the killer's team color.
		surface()->DrawSetTextColor(GameResources()->GetTeamColor(pDeathNotice->nKillerTeam));

		// Draw the killer's name.
		PaintString(m_hTextFont, nX, nY, czKillerName);

		// Fetch the active text position.
		surface()->DrawGetTextPos(nX, nY);
	}

	// Draw the death icon.
	pDeathNotice->deathIcon->DrawSelf(nX, nY, nIconWidth, nIconHeight, m_colIcon);

	// Increase the offset by the icon width.
	nX += nIconWidth;

	// Set the color based off of the victim's team color.
	surface()->DrawSetTextColor(GameResources()->GetTeamColor(pDeathNotice->nVictimTeam));

	// Draw the victim's name.
	PaintString(m_hTextFont, nX, nY, czVictimName);
}

void CForsakenHudDeathNotice::RenderPointNotice(tagNoticeItem *pPointsNotice, int nOffset)
/*
	
	Pre: 
	Post: 
*/
{
	int nYOffset = GetClientModeForsakenNormal()->GetDeathMessageStartHeight();
	int nY = nYOffset + (m_nItemHeight * nOffset);
	int nX = 0;
	wchar_t czPointMsg[POINT_NOTICE_LENGTH];

	// Set the color based off of the victim's team color.
	surface()->DrawSetTextColor(GameResources()->GetTeamColor(pPointsNotice->nKillerTeam));

	// Fetch the unicode string version.
	vgui::localize()->ConvertANSIToUnicode(pPointsNotice->czMessage, czPointMsg, sizeof(czPointMsg));

	// Compute the length and the X offset.
	if (m_bRightJustify)
	{
		int nLen = UTIL_ComputeStringWidth(m_hTextFont, czPointMsg);
		nX = GetWide() - nLen;
	}
	else
		nX = 0;

	// Draw the points notice.
	PaintString(m_hTextFont, nX, nY, czPointMsg);
}

void CForsakenHudDeathNotice::Reset()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudDeathNotice::VidInit()
/*
	
	Pre: 
	Post: 
*/
{
	// Fetch some of the icons.
	m_pSuicideTexture = gHUD.GetIcon("fskn_death_suicide");
	m_pTeamKillTexture = gHUD.GetIcon("fskn_death_teamkill");

	// Fetch the death notice.
	m_vecDeathNotices.Purge();
}