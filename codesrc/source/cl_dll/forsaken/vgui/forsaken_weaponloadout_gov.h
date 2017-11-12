#ifndef __FORSAKEN_VGUI_WEAPONLOADOUT_GOV_H_
#define __FORSAKEN_VGUI_WEAPONLOADOUT_GOV_H_

namespace vgui
{
	class RichText;
	class HTML;
}

class CWeaponLoadout_Gov : public CWeaponLoadout
{
public:
	DECLARE_CLASS_SIMPLE(CWeaponLoadout_Gov, CWeaponLoadout);

	// Constructor & Deconstructor
	CWeaponLoadout_Gov(IViewPort *pViewport);
	virtual ~CWeaponLoadout_Gov();

	// Public Accessor Functions
	virtual const char *GetName() { return PANEL_WEAPLOADOUT_GOV; }

	// Public Functions
	virtual void ShowPanel(bool bState);
	virtual void Update();

	// Public Variables

protected:
	// Protected Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	// Protected Variables
};

#endif