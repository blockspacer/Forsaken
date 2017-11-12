/*
	forsaken_ingamehelp.cpp
	Forsaken panel for in-game help.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include <vgui_controls/Frame.h>
#include <cl_dll/iviewport.h>
#include "forsaken_ingamehelp.h"
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <filesystem.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/HTML.h>
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
CInGameHelp::CInGameHelp(IViewPort *pViewport) : Frame(NULL, PANEL_INGAMEHELP)
{
	// Initial values.
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);

	// Create the HTML control.
	m_pHelpHTML = new HTML(this, "HelpHTML");

	// Load the layout.
	LoadControlSettings("Resource/UI/InGameHelp.res");

	// Save the viewport.
	m_pViewport = pViewport;
}

CInGameHelp::~CInGameHelp()
{
}

// Functions
void CInGameHelp::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CInGameHelp::ShowPanel(bool bState)
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

		bool bFoundHTML = false;
		char czHelpFile[MAX_PATH];
		char czUILanguage[64];

		// Fetch the UI language and a path for the default help page.
		engine->GetUILanguage(czUILanguage, sizeof(czUILanguage));
		Q_snprintf(czHelpFile, sizeof(czHelpFile), "help/%s/main.html", czUILanguage);

		if (!vgui::filesystem()->FileExists(czHelpFile))
			Q_snprintf(czHelpFile, sizeof(czHelpFile), "help/english/main.html");
		else
			bFoundHTML = true;

		if(bFoundHTML || vgui::filesystem()->FileExists(czHelpFile))
		{
			// it's a local HTML file
			char czLocalURL[_MAX_PATH + 7];
			char czPathData[_MAX_PATH];

			Q_strncpy(czLocalURL, "file://", sizeof(czLocalURL));

			vgui::filesystem()->GetLocalPath(czHelpFile, czPathData, sizeof(czPathData));
			Q_strncat(czLocalURL, czPathData, sizeof(czLocalURL), COPY_ALL_CHARACTERS);

			// force steam to dump a local copy
			vgui::filesystem()->GetLocalCopy(czPathData);

			// Display the HTML control.
			m_pHelpHTML->SetVisible(true);
			m_pHelpHTML->OpenURL(czLocalURL);

			InvalidateLayout();
			Repaint();		

			return;
		}
	}
	else
	{
		SetVisible(false);
		SetMouseInputEnabled(false);
	}

	m_pViewport->ShowBackGround(bState);
}

void CInGameHelp::OnCommand(const char *czCommand)
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

void CInGameHelp::Update()
/*
	
	Pre: 
	Post: 
*/
{
}