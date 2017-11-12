/*
	forsaken_overview.cpp
	Forsaken panel for war overview.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include <vgui_controls/Frame.h>
#include <cl_dll/iviewport.h>
#include "forsaken_overview.h"
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
CForsakenMapOverview::CForsakenMapOverview(IViewPort *pViewport) : Frame(NULL, PANEL_MAPOVERVIEW)
{
	// Initial values.
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);
	LoadControlSettings("Resource/UI/MapOverview.res");

	// Save the viewport.
	m_pViewport = pViewport;
}

CForsakenMapOverview::~CForsakenMapOverview()
{
}

// Functions
void CForsakenMapOverview::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenMapOverview::OnCommand(const char *czCommand)
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
	else if (Q_stricmp(czCommand, "vguicancel") == 0)
	{
		engine->ClientCmd(const_cast<char*>(czCommand));

		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);

		// Forsaken Addition: We don't want the panel showing back up again.
		m_pViewport->ShowPanel(this, false);
	}
}

void CForsakenMapOverview::SetImage(const char *czImagePanel, const char *czPath)
/*
	
	Pre: 
	Post: 
*/
{
	ImagePanel *pEntry = dynamic_cast<ImagePanel*>(FindChildByName(czImagePanel));

	if (pEntry)
		pEntry->SetImage(czPath);
}

void CForsakenMapOverview::SetLabelText(const char *czLabel, const char *czText)
/*
	
	Pre: 
	Post: 
*/
{
	Label *pEntry = dynamic_cast<Label*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetText(czText);
}

void CForsakenMapOverview::SetMapThumb()
/*
	
	Pre: 
	Post: 
*/
{
	char *czMapName = NULL;
	const char *czMapNameTemp = engine->GetLevelName();
	int nLen = 0;

	if (czMapNameTemp != NULL && czMapNameTemp[0] != 0)
	{
		// Allocate memory for the map name.
		nLen = Q_strlen(czMapNameTemp) + 1;
		czMapName = new char[nLen];

		// We don't want no stinking crashes.
		if (czMapName != NULL)
		{
			char *czMapNameOffs = czMapName;

			// Copy the map name.
			Q_strcpy(czMapName, czMapNameTemp);

			// Get rid of the 'maps/'
			czMapNameOffs += 5;

			// Get rid of the .bsp.
			for (int i = 0; ; i++)
			{
				// Find the .?
				if (czMapNameOffs[i] == '.')
				{
					czMapNameOffs[i] = 0;

					break;
				}
			}

			char *czImageName = NULL;
			int nLenImage = 0;

			// Determine the length of the map name.
			nLenImage = Q_strlen(czMapName) + 12;

			// Allocate memory for the image name.
			// maps/thumb_<map_name>.vmt
			czImageName = new char[nLenImage];

			// Generate the image name.
			if (czImageName != NULL)
			{
				Q_snprintf(czImageName, nLenImage * sizeof(char), "maps/thumb_%s", czMapNameOffs);

				SetImage("map_thumb", czImageName);
			}

			delete [] czImageName;
		}

		delete [] czMapName;
	}
}

void CForsakenMapOverview::ShowPanel(bool bState)
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

void CForsakenMapOverview::Update()
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = C_ForsakenPlayer::GetLocalForsakenPlayer();
	C_ForsakenTeam *pCivTeam = dynamic_cast<C_ForsakenTeam*>(g_Teams[TEAM_CIVILIANS]);
	C_ForsakenTeam *pGovTeam = dynamic_cast<C_ForsakenTeam*>(g_Teams[TEAM_GOVERNORS]);
	C_ForsakenTeam *pRelTeam = dynamic_cast<C_ForsakenTeam*>(g_Teams[TEAM_RELIGIOUS]);
	char czText[128] = "";

	// Display team-specific heads.
	switch (pPlayer->GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		SetImage("leader_head", "civleader_head");
		break;

	case TEAM_GOVERNORS:
		SetImage("leader_head", "govleader_head");
		break;

	case TEAM_RELIGIOUS:
		SetImage("leader_head", "relleader_head");
		break;

	default:
		SetImage("leader_head", "blank_head");
		break;
	}

	// Set the map thumb.
	SetMapThumb();

	// Update team resource totals.
	Q_snprintf(czText, 128, "%d", pCivTeam->GetNutritionResources());
	SetLabelText("resources_civ_food", czText);
	Q_snprintf(czText, 128, "%d", pCivTeam->GetOrdinanceResources());
	SetLabelText("resources_civ_weap", czText);
	Q_snprintf(czText, 128, "%d", pGovTeam->GetNutritionResources());
	SetLabelText("resources_gov_food", czText);
	Q_snprintf(czText, 128, "%d", pGovTeam->GetOrdinanceResources());
	SetLabelText("resources_gov_weap", czText);
	Q_snprintf(czText, 128, "%d", pRelTeam->GetNutritionResources());
	SetLabelText("resources_rel_food", czText);
	Q_snprintf(czText, 128, "%d", pRelTeam->GetOrdinanceResources());
	SetLabelText("resources_rel_weap", czText);
}