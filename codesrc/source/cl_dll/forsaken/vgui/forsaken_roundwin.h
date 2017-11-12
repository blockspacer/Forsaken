#ifndef __FORSAKEN_VGUI_ROUNDWINMSG_H_
#define __FORSAKEN_VGUI_ROUNDWINMSG_H_

namespace vgui
{
	class RichText;
	class HTML;
}

class CRoundWinMessage : public vgui::Frame, public IViewPortPanel
{
public:
	DECLARE_CLASS_SIMPLE(CRoundWinMessage, vgui::Frame);

	// Constructor & Deconstructor
	CRoundWinMessage(IViewPort *pViewport);
	virtual ~CRoundWinMessage();

	// Public Accessor Functions
	vgui::VPANEL GetVPanel() { return BaseClass::GetVPanel(); }
	virtual bool HasInputElements() { return true; }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual const char *GetName() { return PANEL_ROUNDWIN; }
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

	// Protected Variables
	float m_fCloseTime;
	int m_nTeamWinner;
	IViewPort *m_pViewport;
};

#endif