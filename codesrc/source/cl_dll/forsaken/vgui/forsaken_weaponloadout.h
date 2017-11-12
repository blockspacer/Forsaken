#ifndef __FORSAKEN_VGUI_WEAPONLOADOUT_H_
#define __FORSAKEN_VGUI_WEAPONLOADOUT_H_

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ButtonImage.h>

#include <cl_dll/iviewport.h>

#include <vgui/KeyCode.h>
#include <UtlVector.h>

namespace vgui
{
	class RichText;
	class HTML;
}
class TeamFortressViewport;

class CWeaponLoadout : public vgui::Frame, public IViewPortPanel
{
public:
	DECLARE_CLASS_SIMPLE(CWeaponLoadout, vgui::Frame);

	// Constructor & Deconstructor
	CWeaponLoadout(IViewPort *pViewport);
	virtual ~CWeaponLoadout();

	// Public Accessor Functions
	vgui::VPANEL GetVPanel() { return BaseClass::GetVPanel(); }
	virtual bool HasInputElements() { return true; }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual const char *GetName() { return PANEL_WEAPLOADOUT; }
	virtual void SetData(KeyValues *pData) { }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	// Public Functions
	virtual bool NeedsUpdate() { return true; }
	virtual void Reset() { }
	virtual void ShowPanel(bool bState);
	virtual void Update();

	// Public Variables

protected:
	// Protected Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnCommand(const char *czCommand);
	void SetLabelText(const char *czLabel, const char *czText);
	void SetWeaponEnabled(const char *czLabel, bool bEnabled);
	void SetWeaponSelected(const char *czLabel, bool bSelected);

	// Protected Variables
	int m_nSelectedLeaderWeapon;
	int m_nSelectedStandardWeapon;
	int m_nLastTeam;
	IViewPort *m_pViewport;
};

#endif