/*
	vgui_rootpanel_forsaken.cpp
	Base VGUI panel class for all Forsaken panels.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "vgui_int.h"
#include "ienginevgui.h"
#include "vgui_rootpanel_forsaken.h"
#include "vgui/IVGui.h"
#include "tier0/memdbgon.h"

// Function Prototypes
vgui::VPANEL VGui_GetClientDLLRootPanel(void);
void VGUI_CreateClientDLLRootPanel(void);
void VGUI_DestroyClientDLLRootPanel(void);

// Global Variable Declerations
C_ForsakenRootPanel *g_pRootPanel = NULL;

// Constructor & Deconstructor
C_ForsakenRootPanel::C_ForsakenRootPanel(vgui::VPANEL parent) : 
		BaseClass(NULL, "Forsaken Root Panel")
{
	SetParent(parent);
	SetPaintEnabled(false);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);

	SetPostChildPaintEnabled(true);
	SetBounds(0, 0, ScreenWidth(), ScreenHeight());
	vgui::ivgui()->AddTickSignal(GetVPanel());
}

C_ForsakenRootPanel::~C_ForsakenRootPanel()
{
}

// Functions
// C_ForsakenRootPanel
void C_ForsakenRootPanel::LevelInit(void)
/*
	
	Pre: 
	Post: 
*/
{
}

void C_ForsakenRootPanel::LevelShutdown(void)
/*
	
	Pre: 
	Post: 
*/
{
}

void C_ForsakenRootPanel::OnTick(void)
/*
	
	Pre: 
	Post: 
*/
{
}

void C_ForsakenRootPanel::PostChildPaint()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::PostChildPaint();

	RenderPanelEffects();
}

void C_ForsakenRootPanel::RenderPanelEffects(void)
/*
	
	Pre: 
	Post: 
*/
{
}

// Global
vgui::VPANEL VGui_GetClientDLLRootPanel(void)
/*
	
	Pre: 
	Post: 
*/
{
	return g_pRootPanel->GetVPanel();
}

void VGUI_CreateClientDLLRootPanel(void)
/*
	
	Pre: 
	Post: 
*/
{
	g_pRootPanel = new C_ForsakenRootPanel(enginevgui->GetPanel(PANEL_CLIENTDLL));
}

void VGUI_DestroyClientDLLRootPanel(void)
/*
	
	Pre: 
	Post: 
*/
{
	delete g_pRootPanel;

	g_pRootPanel = NULL;
}