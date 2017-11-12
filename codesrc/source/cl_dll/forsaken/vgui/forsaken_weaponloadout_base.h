#ifndef __FORSAKEN_VGUI_WEAPONLOADOUT_H_
#define __FORSAKEN_VGUI_WEAPONLOADOUT_H_

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Panel.h>

#include "forsaken/vgui/controls/forsaken_ctrl_buttonimage.h"
#include "forsaken/vgui/controls/forsaken_ctrl_buttonimagenumeric.h"

#include <cl_dll/iviewport.h>

#include <vgui/KeyCode.h>
#include <UtlVector.h>

namespace vgui
{
	class RichText;
	class HTML;
}

class CWeaponLoadout : public vgui::Frame, public IViewPortPanel
{
public:
	DECLARE_CLASS_SIMPLE(CWeaponLoadout, vgui::Frame);

	// Constructor & Deconstructor
	CWeaponLoadout(IViewPort *pViewport, Panel *parent, const char *panelName, bool showTaskbarIcon = true);
	virtual ~CWeaponLoadout();

	// Public Accessor Functions
	vgui::Panel *CreateControlByName(const char *czControlName);
	vgui::VPANEL GetVPanel() { return BaseClass::GetVPanel(); }
	virtual bool HasInputElements() { return true; }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual const char *GetName() { return "panel_weaponloadout"; }
	virtual void SetData(KeyValues *pData) { }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	// Public Functions
	virtual bool NeedsUpdate() { return true; }
	virtual void Reset() { }

	// Public Variables

protected:
	// Protected Functions
	const char *GetKitName(const char *czLabel);
	int GetResourceValue(const char *czLabel);
	void CheckPack();
	void DoLoadout();
	void OnCommand(const char *czCommand);
	void SetPlayerImage(const char *czImageName);
	void SetLabelText(const char *czLabel, const char *czText);
	void SetWeaponEnabled(const char *czLabel, bool bEnabled);
	void SetWeaponSelected(const char *czLabel, bool bSelected);
	void UpdateResourceLabel();

	// Protected Variables
	int m_nLeaderPackCount;
	int m_nSelectedWeaponPack;
	int m_nSharedResourcesAmt;
	int m_nSharedResourcesRemaining;
	int m_nSharedResourcesFlag;
	IViewPort *m_pViewport;
};

#endif