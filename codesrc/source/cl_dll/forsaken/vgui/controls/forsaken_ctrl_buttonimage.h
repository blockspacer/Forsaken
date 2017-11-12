#ifndef __FORSAKEN_CONTROL_BUTTONIMAGE_H_
#define __FORSAKEN_CONTROL_BUTTONIMAGE_H_

#include <vgui_controls/Button.h>
#include <vgui_controls/Image.h>
#include <KeyValues.h>

class KeyValues;

class CForsakenButtonImage : public vgui::Button
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenButtonImage, vgui::Button);

	// Constructor & Deconstructor
	CForsakenButtonImage(vgui::Panel *pParent, const char *czName, const char *czText);
	~CForsakenButtonImage();

	// Public Accessor Functions
	bool IsActive() { return m_bIsSelected; }
	const char *GetKitName()
	{
		if (m_pKitName)
			return m_pKitName->GetString("kitname", "none");

		return "none";
	}
	void SetActive(bool state) { m_bIsSelected = state; }
	void SetKitName(const char *czName)
	{
		SetKitName(new KeyValues("KitName", "kitname", czName));
	}

	void SetKitName(KeyValues *pNewName)
	{
		if (m_pKitName)
			m_pKitName->deleteThis();

		m_pKitName = pNewName;
	}

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
	void Init();

	// Protected Variables
	bool m_bIsSelected;
	char *m_czActiveImageName;
	char *m_czArmedImageName;
	char *m_czDepressedImageName;
	char *m_czDisabledImageName;
	char *m_czEnabledImageName;
	Color m_colorNonSelectedFocus;
	Color m_colorSelectedFocus;
	KeyValues *m_pKitName;
	vgui::IImage *m_pActiveImage;
	vgui::IImage *m_pArmedImage;
	vgui::IImage *m_pDepressedImage;
	vgui::IImage *m_pDisabledImage;
	vgui::IImage *m_pEnabledImage;
};

#endif