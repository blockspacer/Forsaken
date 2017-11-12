#ifndef __FORSAKEN_VGUI_WEAPONLOADOUT_REL_H_
#define __FORSAKEN_VGUI_WEAPONLOADOUT_REL_H_

namespace vgui
{
	class RichText;
	class HTML;
}

class CWeaponLoadout_Rel : public CWeaponLoadout
{
public:
	DECLARE_CLASS_SIMPLE(CWeaponLoadout_Rel, CWeaponLoadout);

	// Constructor & Deconstructor
	CWeaponLoadout_Rel(IViewPort *pViewport);
	virtual ~CWeaponLoadout_Rel();

	// Public Accessor Functions
	virtual const char *GetName() { return PANEL_WEAPLOADOUT_REL; }

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