#ifndef __FORSAKEN_VGUI_WAROVERVIEW_H_
#define __FORSAKEN_VGUI_WAROVERVIEW_H_

namespace vgui
{
	class RichText;
	class HTML;
}

class CForsakenMapOverview : public vgui::Frame, public IViewPortPanel
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenMapOverview, vgui::Frame);

	// Constructor & Deconstructor
	CForsakenMapOverview(IViewPort *pViewport);
	virtual ~CForsakenMapOverview();

	// Public Accessor Functions
	vgui::VPANEL GetVPanel() { return BaseClass::GetVPanel(); }
	virtual bool HasInputElements() { return true; }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual const char *GetName() { return PANEL_MAPOVERVIEW; }
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
	void SetImage(const char *czImagePanel, const char *czPath);
	void SetLabelText(const char *czLabel, const char *czText);
	void SetMapThumb();

	// Protected Variables
	IViewPort *m_pViewport;
};

#endif