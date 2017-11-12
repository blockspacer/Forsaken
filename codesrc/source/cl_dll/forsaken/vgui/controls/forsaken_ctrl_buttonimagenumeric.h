#ifndef __FORSAKEN_CONTROL_BUTTONIMAGENUMERIC_H_
#define __FORSAKEN_CONTROL_BUTTONIMAGENUMERIC_H_

#include <vgui_controls/TextImage.h>
#include "forsaken/vgui/controls/forsaken_ctrl_buttonimage.h"

class CForsakenButtonImageNumeric : public CForsakenButtonImage
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenButtonImageNumeric, CForsakenButtonImage);

	// Constructor & Deconstructor
	CForsakenButtonImageNumeric(vgui::Panel *pParent, const char *czName, const char *czText);
	~CForsakenButtonImageNumeric();

	// Public Accessor Functions
	bool IsActive() { return m_bIsSelected; }
	int GetCost() { return m_nCost; }
	int GetCurrentValue() { return m_nCurrentValue; }
	vgui::MouseCode GetLastMouseEvent() { return _mouseClickCode; }
	void ResetValue()
	{
		char czText[64] = "";

		// Reset the active value.
		m_nCurrentValue = m_nDefaultValue;

		// Set the text image's text.
		Q_snprintf(czText, sizeof(czText), "%d", m_nCurrentValue);
		SetText(czText);
	}
	void SetActive(bool state) { m_bIsSelected = state; }
	void SetCurrentValue(int nNewValue)
	{
		char czText[64] = "";

		// Clip the new value if necessary, if not set it to the new value.
		if (nNewValue < m_nMinRange)
			m_nCurrentValue = m_nMinRange;
		else if (nNewValue > m_nMaxRange)
			m_nCurrentValue = m_nMaxRange;
		else
			m_nCurrentValue = nNewValue;

		// Set the text image's text.
		Q_snprintf(czText, sizeof(czText), "%d", m_nCurrentValue);
		SetText(czText);
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
	virtual void DoClick();
	virtual void DrawFocusBorder(int tx0, int ty0, int tx1, int ty1);
	virtual void GetSettings(KeyValues *pResources);
	virtual void Paint();
	virtual void PaintBackground();
	void Init();

	// Protected Variables
	int m_nCost;
	int m_nCurrentValue;
	int m_nDefaultValue;
	int m_nMaxRange;
	int m_nMinRange;
};

#endif