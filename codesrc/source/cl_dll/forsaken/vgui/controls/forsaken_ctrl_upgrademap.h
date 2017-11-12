#ifndef __FORSAKEN_CONTROL_UPGRADEMAP_H_
#define __FORSAKEN_CONTROL_UPGRADEMAP_H_

#include <utlvector.h>
#include "FileSystem.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/vgui/controls/forsaken_ctrl_radioimage.h"
#include "forsaken/vgui/controls/forsaken_ctrl_objectivemap.h"

class CForsakenUpgradeMap : public CForsakenObjectiveMap
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenUpgradeMap, CForsakenObjectiveMap);

	// Constructor & Deconstructor
	CForsakenUpgradeMap(vgui::Panel *pParent, const char *czName, const char *czText);
	~CForsakenUpgradeMap();

	// Public Functions
	virtual bool NeedsUpdate() { return true; }
	virtual void Update();

	// Public Variables

private:
	// Private Functions

	// Private Variables

protected:
	// Protected Functions
	void UpdateObjectiveButtons();
};

#endif