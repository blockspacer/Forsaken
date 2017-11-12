/*
	forsaken_roundwin.cpp
	Forsaken round win message panel.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/ImagePanel.h>
#include <cl_dll/iviewport.h>
#include <vgui/KeyCode.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <filesystem.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include "IGameUIFuncs.h"
#include "igameresources.h"
#include <cl_dll/iviewport.h>
#include <stdlib.h>
#include <stdio.h>
#include "forsaken/c_forsaken_player.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken_roundwin.h"
#include "tier0/memdbgon.h"

using namespace vgui;

// Constructor & Deconstructor
CRoundWinMessage::CRoundWinMessage(IViewPort *pViewport) : Frame(NULL, PANEL_ROUNDWIN)
{
	m_fCloseTime = 0;
	m_nTeamWinner = TEAM_UNASSIGNED;
	m_pViewport = pViewport;

	// Initial values.
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	LoadControlSettings("Resource/UI/RoundWinMsg.res");
}

CRoundWinMessage::~CRoundWinMessage()
{
}

// Functions
void CRoundWinMessage::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	SetPaintBackgroundType(2);

	BaseClass::ApplySchemeSettings(pScheme);
}

bool CRoundWinMessage::NeedsUpdate()
/*
	
	Pre: 
	Post: 
*/
{
	if (!IsVisible())
		return false;

	if (gpGlobals->curtime >= m_fCloseTime)
		m_pViewport->ShowPanel(PANEL_ROUNDWIN, false);

	return true;
}

void CRoundWinMessage::SetData(KeyValues *pData)
/*
	
	Pre: 
	Post: 
*/
{
	m_nTeamWinner = pData->GetInt("team", 0);
	// Bug Fix ID 0000032: This used to be 2500 seconds.
	m_fCloseTime = gpGlobals->curtime + 5.0f;

	Update();
}

void CRoundWinMessage::ShowPanel(bool bState)
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

void CRoundWinMessage::Update()
/*
	
	Pre: 
	Post: 
*/
{
	char czWinMessage[1024];
	ImagePanel *teamIcon = dynamic_cast<ImagePanel*>(FindChildByName("team_image"));
	Label *winLabel = dynamic_cast<Label*>(FindChildByName("win_info"));

	// Determine which team won and get the string and image representation of it.
	switch (m_nTeamWinner)
	{
	case TEAM_CIVILIANS_BITMASK:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "The Civilians have won the round!");

		if (teamIcon)
			teamIcon->SetImage("teamlogo_civ");

		break;

	case TEAM_GOVERNORS_BITMASK:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "The Governors have won the round!");

		if (teamIcon)
			teamIcon->SetImage("teamlogo_gov");

		break;

	case TEAM_RELIGIOUS_BITMASK:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "The Religious have won the round!");

		if (teamIcon)
			teamIcon->SetImage("teamlogo_rel");

		break;

	default:
		Q_snprintf(czWinMessage, sizeof(czWinMessage), "The round is a draw.");

		if  (teamIcon)
			teamIcon->SetImage("teamlogo_civ");

		break;
	}

	if (winLabel)
		winLabel->SetText(czWinMessage);

	MoveToCenterOfScreen();
}