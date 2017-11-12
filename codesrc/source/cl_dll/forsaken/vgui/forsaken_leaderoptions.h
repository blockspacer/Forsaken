#ifndef __FORSAKEN_VGUI_LEADEROPTIONS_H_
#define __FORSAKEN_VGUI_LEADEROPTIONS_H_

namespace vgui
{
	class RichText;
	class HTML;
}

#include "forsaken/vgui/controls/forsaken_ctrl_buttonimage.h"
#include "forsaken/vgui/controls/forsaken_ctrl_objectivemap.h"
#include "forsaken/vgui/controls/forsaken_ctrl_upgrademap.h"
#include "forsaken/vgui/controls/forsaken_ctrl_spawnmap.h"

#define UPGRADE_OBJECTIVE_BASE	0
#define UPGRADE_OBJECTIVE_LVL1	1
#define UPGRADE_OBJECTIVE_LVL2	2
#define UPGRADE_OBJECTIVE_LVL3	3
#define UPGRADE_OBJECTIVE_MAX	4

class CLeaderOptionsPage : public vgui::PropertyPage
{
public:
	DECLARE_CLASS_SIMPLE(CLeaderOptionsPage, vgui::PropertyPage);

	// Constructor & Deconstructor
	CLeaderOptionsPage(vgui::Panel *parent, const char *czPanelName);
	virtual ~CLeaderOptionsPage();

	// Public Accessor Functions

	// Public Functions
	vgui::Panel *CreateControlByName(const char *czControlName);

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
};

class CLeaderOptions : public vgui::Frame, public IViewPortPanel
{
public:
	DECLARE_CLASS_SIMPLE(CLeaderOptions, vgui::Frame);

	// Constructor & Deconstructor
	CLeaderOptions(IViewPort *pViewport);
	virtual ~CLeaderOptions();

	// Public Accessor Functions
	vgui::VPANEL GetVPanel() { return BaseClass::GetVPanel(); }
	virtual bool HasInputElements() { return true; }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual const char *GetName() { return PANEL_LEADEROPTIONS; }
	virtual void SetData(KeyValues *pData);
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

	// Public Functions
	vgui::Panel *CreateControlByName(const char *czControlName);
	virtual bool NeedsUpdate() { return true; }
	virtual void PerformLayout();
	virtual void Reset() { }
	virtual void ShowPanel(bool bState);
	virtual void Update();

	// Public Variables

protected:
	// Protected Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	bool IsRadioSelected(const char *czRadioName);
	void OnCommand(const char *czCommand);
	void SelectRadio(const char *czRadioName);
	void SetImage(const char *czImagePanel, const char *czPath);
	void SetLabelText(const char *czLabel, const char *czText);
	void SetUpgradeEnabled(const char *czLabel, bool bEnabled);
	void SetUpgradeSelected(const char *czLabel, bool bSelected);
	void UpdatePropertySheetSize();

	MESSAGE_FUNC_PARAMS(OnRadioImageSelected, "RadioImageSelected", pData);

	// Protected Variables
	char m_czLastSelectedSpawn[OBJECTIVE_NAME_LENGTH];
	char m_czLastSelectedUpgrade[OBJECTIVE_NAME_LENGTH];
	char m_czSelectedSpawn[OBJECTIVE_NAME_LENGTH];
	char m_czSelectedUpgrade[OBJECTIVE_NAME_LENGTH];
	CForsakenButtonImage *m_pUpgradeObjectives[UPGRADE_OBJECTIVE_MAX];
	CForsakenButtonImage *m_pCancel;
	CForsakenButtonImage *m_pOK;
	CForsakenUpgradeMap *m_pUpgradeObjectiveMap;
	CForsakenSpawnMap *m_pSpawnObjectiveMap;
	int m_nSpawnLocationType;
	int m_nUpgradeLevel;
	IViewPort *m_pViewport;
	CLeaderOptionsPage *m_pOptionsTab;
	CLeaderOptionsPage *m_pSpawnTab;
	CLeaderOptionsPage *m_pUpgradeTab;
	vgui::PropertySheet *m_pTabSheet;
};

#endif