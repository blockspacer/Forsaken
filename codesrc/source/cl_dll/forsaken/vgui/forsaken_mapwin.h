#ifndef __FORSAKEN_VGUI_WINMSG_H_
#define __FORSAKEN_VGUI_WINMSG_H_

namespace vgui
{
	class RichText;
	class HTML;
}

class CMapWinMessage : public vgui::Frame, public IViewPortPanel
{
public:
	DECLARE_CLASS_SIMPLE(CMapWinMessage, vgui::Frame);

	// Constructor & Deconstructor
	CMapWinMessage(IViewPort *pViewport);
	virtual ~CMapWinMessage();

	// Public Accessor Functions
	vgui::VPANEL GetVPanel() { return BaseClass::GetVPanel(); }
	virtual bool HasInputElements() { return true; }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual const char *GetName() { return PANEL_MAPWIN; }
	virtual void SetData(KeyValues *pData);
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	// Public Functions
	virtual bool NeedsUpdate();
	virtual void Reset() { }
	virtual void ShowPanel(bool bState);
	virtual void Update();

	// Public Variables

protected:
	// Protected Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	void SetImage(const char *czImagePanel, const char *czPath);
	void SetLabelText(const char *czLabel, const char *czText);

	// Protected Variables
	float m_fCloseTime;
	int m_nTeamWinner;
	int m_nWinType;
	IViewPort *m_pViewport;
};

#endif