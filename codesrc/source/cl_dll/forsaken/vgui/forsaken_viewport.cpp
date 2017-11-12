/*
	forsaken_viewport.cpp
	Forsaken viewport code, VGUI.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include "vgui_controls/Panel.h"
#include "vgui/ISurface.h"
#include "KeyValues.h"
#include "vgui/Cursor.h"
#include "vgui/IScheme.h"
#include "vgui/IVGui.h"
#include "vgui/ILocalize.h"
#include "vgui/VGUI.h"
#include "hud.h"
#include "voice_status.h"
#include "baseviewport.h"
#include "forsaken_viewport.h"
#include "vguicenterprint.h"
#include "text_message.h"

// Constructor & Deconstructor

// Functions
int CForsakenViewport::GetDeathMessageStartHeight(void)
/*
	
	Pre: 
	Post: 
*/
{
	int x = YRES(2);
	IViewPortPanel *pSpectator = gViewPortInterface->FindPanelByName(PANEL_SPECGUI);

	if (pSpectator && pSpectator->IsVisible())
	{
		x += YRES(52);
	}

	return x;
}

IViewPortPanel *CForsakenViewport::CreatePanelByName(const char *czPanelName)
/*
	
	Pre: 
	Post: 
*/
{
	IViewPortPanel *pPanel = NULL;

	pPanel = BaseClass::CreatePanelByName(czPanelName);

	return pPanel;
}

void CForsakenViewport::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);

	gHUD.InitColors(pScheme);

	SetPaintBackgroundEnabled(false);
}

void CForsakenViewport::CreateDefaultPanels(void)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::CreateDefaultPanels();
}