/*
	forsaken_warsummary.cpp
	Forsaken panel for war summary.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include <vgui_controls/Frame.h>
#include <cl_dll/iviewport.h>
#include "forsaken_warsummary.h"
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
#include "forsaken/c_forsaken_team.h"
#include "tier0/memdbgon.h"

using namespace vgui;

// Constructor & Deconstructor
CWarSummary::CWarSummary(IViewPort *pViewport) : Frame(NULL, PANEL_WARSUMMARY)
{
	// Initial values.
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);
	LoadControlSettings("Resource/UI/WarSummary.res");

	// Save the viewport.
	m_pViewport = pViewport;
}

CWarSummary::~CWarSummary()
{
}

// Functions
void CWarSummary::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CWarSummary::ShowPanel(bool bState)
/*
	
	Pre: 
	Post: 
*/
{
	if (BaseClass::IsVisible() == bState)
		return;

	if (bState)
	{
		// Setup the initial look of the dialog.
		Activate();
		SetMouseInputEnabled(true);
	}
	else
	{
		SetVisible(false);
		SetMouseInputEnabled(false);
	}

	m_pViewport->ShowBackGround(bState);
}

void CWarSummary::OnCommand(const char *czCommand)
/*
	
	Pre: 
	Post: 
*/
{
	if (Q_stricmp(czCommand, "vguiok") == 0)
	{
		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);

		// Forsaken Addition: We don't want the panel showing back up again.
		m_pViewport->ShowPanel(this, false);
	}
}

void CWarSummary::SetData(KeyValues *pData)
/*
	
	Pre: 
	Post: 
*/
{
	m_nTeamWinner = pData->GetInt("team", 0);

	Update();
}

void CWarSummary::SetImage(const char *czImagePanel, const char *czPath)
/*
	
	Pre: 
	Post: 
*/
{
	ImagePanel *pEntry = dynamic_cast<ImagePanel*>(FindChildByName(czImagePanel));

	if (pEntry)
		pEntry->SetImage(czPath);
}

void CWarSummary::SetLabelText(const char *czLabel, const char *czText)
/*
	
	Pre: 
	Post: 
*/
{
	Label *pEntry = dynamic_cast<Label*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetText(czText);
}

void CWarSummary::Update()
/*
	
	Pre: 
	Post: 
*/
{
	char czWinMessage[1024];

	switch (m_nTeamWinner)
	{
	case TEAM_CIVILIANS_BITMASK:
		Q_strcpy(czWinMessage, "The civilians have won the war!");
		SetImage("faction_group", "civ_faction");
		break;

	case TEAM_GOVERNORS_BITMASK:
		Q_strcpy(czWinMessage, "The governors have won the war!");
		SetImage("faction_group", "gov_faction");
		break;

	case TEAM_RELIGIOUS_BITMASK:
		Q_strcpy(czWinMessage, "The religious have won the war!");
		SetImage("faction_group", "rel_faction");
		break;

	default:
		Q_strcpy(czWinMessage, "The war is being reset.");
		break;
	}

	SetLabelText("win_info", czWinMessage);
}