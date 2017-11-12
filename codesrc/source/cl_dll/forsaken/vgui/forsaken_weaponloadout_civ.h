#ifndef __FORSAKEN_VGUI_WEAPONLOADOUT_CIV_H_
#define __FORSAKEN_VGUI_WEAPONLOADOUT_CIV_H_

namespace vgui
{
	class RichText;
	class HTML;
}

class CWeaponLoadout_Civ : public CWeaponLoadout
{
public:
	DECLARE_CLASS_SIMPLE(CWeaponLoadout_Civ, CWeaponLoadout);

	// Constructor & Deconstructor
	CWeaponLoadout_Civ(IViewPort *pViewport);
	virtual ~CWeaponLoadout_Civ();

	// Public Accessor Functions
	virtual const char *GetName() { return PANEL_WEAPLOADOUT_CIV; }

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