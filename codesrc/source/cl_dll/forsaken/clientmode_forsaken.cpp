/*
	clientmode_forsaken.cpp
	Forsaken client mode.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "hud.h"
#include "clientmode_forsaken.h"
#include "cdll_client_int.h"
#include "iinput.h"
#include "vgui/isurface.h"
#include "vgui/ipanel.h"
#include <vgui_controls/AnimationController.h>
#include "ivmodemanager.h"
#include "filesystem.h"
#include "vgui/ivgui.h"
#include "keydefs.h"
//#include "hud_chat.h"
#include "view_shared.h"
#include "view.h"
#include "ivrenderview.h"
#include "model_types.h"
#include "iefx.h"
#include "dlight.h"
#include <imapoverview.h>
#include "c_playerresource.h"
#include <keyvalues.h>
#include "text_message.h"
#include "panelmetaclassmgr.h"

// Internal Classes
class CForsakenModeManager : public IVModeManager
{
public:
	// Constructor & Deconstructor

	// Public Accessor Functions

	// Public Functions
	virtual void ActivateMouse(bool bIsActive) { }
	virtual void Init();
	virtual void LevelInit(const char *czNewMap);
	virtual void LevelShutdown(void);
	virtual void SwitchMode(bool bCommander, bool bForce) { }

	// Public Variables
};

// Global Variable Declarations
ConVar default_fov( "default_fov", "90", FCVAR_CHEAT );
CClientModeForsakenNormal g_ClientModeNormal;
IClientMode *g_pClientMode = NULL;
static CForsakenModeManager g_ModeManager;
IVModeManager *modemanager = (IVModeManager*)&g_ModeManager;

// Constructor & Deconstructor
CClientModeForsakenNormal::CClientModeForsakenNormal()
{
}

CClientModeForsakenNormal::~CClientModeForsakenNormal()
{
}

// Functions
// CClientModeForsakenNormal
float CClientModeForsakenNormal::GetViewModelFOV(void)
/*
	
	Pre: 
	Post: 
*/
{
	return 74.0f;
}

int CClientModeForsakenNormal::GetDeathMessageStartHeight()
/*
	
	Pre: 
	Post: 
*/
{
	return m_pViewport->GetDeathMessageStartHeight();
}

void CClientModeForsakenNormal::InitViewport()
/*
	
	Pre: 
	Post: 
*/
{
	m_pViewport = new CForsakenViewport;

	m_pViewport->Start(gameuifuncs, gameeventmanager);
}

void CClientModeForsakenNormal::PostRenderVGui()
/*
	
	Pre: 
	Post: 
*/
{
}

// CForsakenModeManager
void CForsakenModeManager::Init()
/*
	
	Pre: 
	Post: 
*/
{
	g_pClientMode = GetClientModeNormal();

	PanelMetaClassMgr()->LoadMetaClassDefinitionFile(SCREEN_FILE);
}

void CForsakenModeManager::LevelInit(const char *czNewMap)
/*
	
	Pre: 
	Post: 
*/
{
	g_pClientMode->LevelInit(czNewMap);
}

void CForsakenModeManager::LevelShutdown(void)
/*
	
	Pre: 
	Post: 
*/
{
	g_pClientMode->LevelShutdown();
}

// Global Functions
CClientModeForsakenNormal *GetClientModeForsakenNormal()
/*
	
	Pre: 
	Post: 
*/
{
	return dynamic_cast<CClientModeForsakenNormal*>(GetClientModeNormal());
}

IClientMode *GetClientModeNormal()
/*
	
	Pre: 
	Post: 
*/
{
	return &g_ClientModeNormal;
}