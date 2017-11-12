#ifndef __FORSAKEN_VIEWPORT_H_
#define __FORSAKEN_VIEWPORT_H_

#include "forsaken/forsaken_shareddefs.h"
#include "baseviewport.h"

using namespace vgui;

namespace vgui
{
	class Panel;
}

class CForsakenViewport : public CBaseViewport
{
	DECLARE_CLASS_SIMPLE(CForsakenViewport, CBaseViewport);

public:
	// Constructor & Deconstructor

	// Public Accessor Functions
	int GetDeathMessageStartHeight(void);

	// Public Functions
	IViewPortPanel *CreatePanelByName(const char *czPanelName);
	virtual void ApplySchemeSettings(IScheme *pScheme);
	void CreateDefaultPanels(void);

	// Public Variables
};

#endif