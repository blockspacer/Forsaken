/*
	forsaken_winmsg.cpp
	Forsaken win message panel.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include <vgui_controls/Frame.h>
#include <cl_dll/iviewport.h>
#include <vgui/KeyCode.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <filesystem.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include "IGameUIFuncs.h"
#include "igameresources.h"
#include <cl_dll/iviewport.h>
#include <stdlib.h>
#include <stdio.h>
#include "forsaken/c_forsaken_player.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken_mapwin.h"
#include "tier0/memdbgon.h"

using namespace vgui;

// Constructor & Deconstructor
CMapWinMessage::CMapWinMessage(IViewPort *pViewport) : Frame(NULL, PANEL_MAPWIN)
{
	m_pViewport = pViewport;

	// Initial values.
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	LoadControlSettings("Resource/UI/MapWinMsg.res");
}

CMapWinMessage::~CMapWinMessage()
{
}

// Functions
void CMapWinMessage::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	SetPaintBackgroundType(2);

	BaseClass::ApplySchemeSettings(pScheme);
}

bool CMapWinMessage::NeedsUpdate()
/*
	
	Pre: 
	Post: 
*/
{
	if (!IsVisible())
		return false;

	if (gpGlobals->curtime >= m_fCloseTime)
		gViewPortInterface->ShowPanel(PANEL_MAPWIN, false);

	return true;
}

void CMapWinMessage::SetData(KeyValues *pData)
/*
	
	Pre: 
	Post: 
*/
{
	m_nTeamWinner = pData->GetInt("team", 0);

	m_fCloseTime = gpGlobals->curtime + 2500.0f;

	Update();
}

void CMapWinMessage::SetImage(const char *czImagePanel, const char *czPath)
/*
	
	Pre: 
	Post: 
*/
{
	ImagePanel *pEntry = dynamic_cast<ImagePanel*>(FindChildByName(czImagePanel));

	if (pEntry)
		pEntry->SetImage(czPath);
}

void CMapWinMessage::SetLabelText(const char *czLabel, const char *czText)
/*
	
	Pre: 
	Post: 
*/
{
	Label *pEntry = dynamic_cast<Label*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetText(czText);
}

void CMapWinMessage::ShowPanel(bool bState)
/*
	
	Pre: 
	Post: 
*/
{
	if (BaseClass::IsVisible() == bState)
		return;

	// Setup the initial look of the dialog.
	if (bState)
	{
		Reset();

		Activate();

		MoveToCenterOfScreen();
	}
	else
	{
		BaseClass::SetVisible(false);

		// Disable mouse and keyboard input.
		SetMouseInputEnabled(false);
		SetKeyBoardInputEnabled(false);
	}
}

void CMapWinMessage::Update()
/*
	
	Pre: 
	Post: 
*/
{
	char czWinMessage[1024];

	// Determine which team won and get the string and image representation of it.
	switch (m_nTeamWinner)
	{
	case TEAM_CIVILIANS_BITMASK:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "The Civilians have won the map!");

		SetImage("team_image", "minilogo_civ");

		break;

	case TEAM_GOVERNORS_BITMASK:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "The Governors have won the map!");

		SetImage("team_image", "minilogo_gov");

		break;

	case TEAM_RELIGIOUS_BITMASK:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "The Religious have won the map!");

		SetImage("team_image", "minilogo_rel");

		break;

	default:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "Nobody won the map, neutral.");

		SetImage("team_image", "minilogo_none");

		break;
	}

	// Set the map thumb.
	SetImage("map_thumb", engine->GetLevelName());

	SetLabelText("win_info", czWinMessage);

	MoveToCenterOfScreen();
}