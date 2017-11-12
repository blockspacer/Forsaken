#ifndef __FORSAKEN_CONTROL_OBJECTIVEMAP_H_
#define __FORSAKEN_CONTROL_OBJECTIVEMAP_H_

#include <vgui_controls/Button.h>
#include <vgui_controls/Image.h>
#include <KeyValues.h>
#include <utlvector.h>
#include "FileSystem.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/vgui/controls/forsaken_ctrl_radioimage.h"

class KeyValues;

#define LOCATION_TYPE_UNKNOWN		0
#define LOCATION_TYPE_SPAWN			1
#define LOCATION_TYPE_OBJECTIVE		2

#define SELECT_TYPE_UNKNOWN			0
#define SELECT_TYPE_SPAWN			1
#define SELECT_TYPE_UPGRADE			2

#define OBJECTIVE_TEXTURES 49				// 8 for each team + 1 for blank
#define OFFSET_EMPTY 48						// blank texture is last
#define OFFSET_CIV 0						// Civ starts at index 0
#define OFFSET_GOV 16						// Gov starts at index 8
#define OFFSET_REL 32						// Rel starts at index 16
#define OFFSET_LEVEL 4						// Offset to move to next fort level
#define OFFSET_SECURED 2					// Offset to move to secured status
#define OFFSET_SELECTED 1					// Offset to move to selected status

struct tagLocationItem
{
	char czObjectiveName[OBJECTIVE_NAME_LENGTH];
	int nX, nY;
	int nTeamBitmask;
	int nLocationType;
};

class CForsakenObjectiveMap : public vgui::Button
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenObjectiveMap, vgui::Button);

	// Constructor & Deconstructor
	CForsakenObjectiveMap(vgui::Panel *pParent, const char *czName, const char *czText);
	~CForsakenObjectiveMap();

	// Public Accessor Functions
	const char *GetActiveObjectiveName() { return NULL; }

	// Public Functions

	// Public Variables

private:
	// Private Functions

	// Private Variables

protected:
	// Protected Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void ApplySettings(KeyValues *pResources);
	virtual void DrawFocusBorder(int tx0, int ty0, int tx1, int ty1);
	virtual void GetSettings(KeyValues *pResources);
	virtual void PaintBackground();
	virtual void Paint();
	void PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText);
	void Init();
	void LoadMapImage(char *czMapName);
	void LoadMapObjectives(char *czMapName);
	void CreateObjectiveButtons();

	// Protected Variables
	char *m_czCurrentMapName;
	Color m_colorNonSelectedFocus;
	Color m_colorSelectedFocus;
	CUtlVector<tagLocationItem> m_vecObjectives;
	CUtlVector<CForsakenRadioImage *> m_vecObjectiveButtons;
	vgui::IImage *m_pMapImage;

	Color m_ObjectiveNormalColor;
	Color m_PauseCaptureColor;
	Color m_OpposingCaptureColor;
	Color m_FriendlyCaptureColor;

	CPanelAnimationVar(Color, m_ObjectiveFontColor, "ObjectiveFontColor", "Orange");
	CPanelAnimationVar(vgui::HFont, m_hObjectiveFont, "ObjectiveFont", "Default");

	// Radio Button Images
	vgui::IImage * m_textureObjectives[OBJECTIVE_TEXTURES];
};

#endif