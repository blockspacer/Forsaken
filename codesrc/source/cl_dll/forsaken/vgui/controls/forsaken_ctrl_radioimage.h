#ifndef __FORSAKEN_CONTROL_RADIOIMAGE_H_
#define __FORSAKEN_CONTROL_RADIOIMAGE_H_

#include <vgui_controls/RadioButton.h>
#include <vgui_controls/Image.h>
#include <KeyValues.h>

class KeyValues;

class CForsakenRadioImage : public vgui::RadioButton
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenRadioImage, vgui::RadioButton);

	// Constructor & Deconstructor
	CForsakenRadioImage(vgui::Panel *pParent, const char *czName, const char *czText);
	~CForsakenRadioImage();

	// Public Accessor Functions
	bool IsActive() { return m_bIsSelected; }
	void SetActive(bool state) { m_bIsSelected = state; }

	// Public Functions
	void SetNormalImage(vgui::IImage *pImage);
	void SetSelectedImage(vgui::IImage *pImage);
	void SetAnimateColor(Color color) {m_colorAnimate = color;}
	void SetActionSignalMessage(KeyValues * msg);

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
	virtual void Paint();
	virtual void SetSelected(bool state);
	void Init();

	// Protected Variables
	bool m_bIsSelected;
	Color m_colorNonSelectedFocus;
	Color m_colorSelectedFocus;
	Color m_colorAnimate;
	KeyValues *msgToSend;
	vgui::IImage *m_pNormalImage;
	vgui::IImage *m_pSelectedImage;
};

#endif