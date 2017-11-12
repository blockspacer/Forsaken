#ifndef __CLIENT_FORSAKEN_CLIENTMODE_H_
#define __CLIENT_FORSAKEN_CLIENTMODE_H_

#include "clientmode_shared.h"
#include "forsaken/vgui/forsaken_viewport.h"

#define SCREEN_FILE	"scripts/vgui_screens.txt"

class CClientModeForsakenNormal : public ClientModeShared
{
public:
	DECLARE_CLASS(CClientModeForsakenNormal, ClientModeShared);

	// Constructor & Deconstructor
	CClientModeForsakenNormal();
	virtual ~CClientModeForsakenNormal();

	// Public Accessor Functions
	int GetDeathMessageStartHeight();
	virtual float GetViewModelFOV(void);

	// Public Functions
	virtual void InitViewport();
	virtual void PostRenderVGui();

	// Public Variables
};

extern CClientModeForsakenNormal *GetClientModeForsakenNormal();
extern IClientMode *GetClientModeNormal();

#endif