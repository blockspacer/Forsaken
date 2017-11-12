#ifndef __FORSAKEN_VGUI_ROOTPANEL_H_
#define __FORSAKEN_VGUI_ROOTPANEL_H_

#include "vgui_controls/Panel.h"
#include "vgui_controls/EditablePanel.h"
#include "utlvector.h"

class CPanelEffect;

typedef unsigned int EFFECT_HANDLE;

class C_ForsakenRootPanel : public vgui::Panel
{
	typedef vgui::Panel BaseClass;

public:
	// Constructor & Deconstructor
	C_ForsakenRootPanel(vgui::VPANEL parent);
	virtual ~C_ForsakenRootPanel();

	// Public Accessor Functions

	// Public Functions
	virtual void LevelInit(void);
	virtual void LevelShutdown(void);
	virtual void PostChildPaint();
	void OnTick(void);

	// Public Variables

private:
	// Private Functions
	void RenderPanelEffects(void);

	// Private Variables
	CUtlVector<CPanelEffect*> m_uvecEffects;
};

#endif