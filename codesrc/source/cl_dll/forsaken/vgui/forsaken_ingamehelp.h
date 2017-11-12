#ifndef __FORSAKEN_VGUI_INGAMEHELP_H_
#define __FORSAKEN_VGUI_INGAMEHELP_H_

namespace vgui
{
	class RichText;
	class HTML;
}

class CInGameHelp : public vgui::Frame, public IViewPortPanel
{
public:
	DECLARE_CLASS_SIMPLE(CInGameHelp, vgui::Frame);

	// Constructor & Deconstructor
	CInGameHelp(IViewPort *pViewport);
	virtual ~CInGameHelp();

	// Public Accessor Functions
	vgui::VPANEL GetVPanel() { return BaseClass::GetVPanel(); }
	virtual bool HasInputElements() { return true; }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual const char *GetName() { return PANEL_INGAMEHELP; }
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

	// Protected Variables
	IViewPort *m_pViewport;
	vgui::HTML *m_pHelpHTML;
};

#endif