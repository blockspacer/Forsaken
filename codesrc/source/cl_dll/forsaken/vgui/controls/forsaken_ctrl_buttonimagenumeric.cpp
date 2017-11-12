/*
	forsaken_ctrl_buttonimagenumeric.cpp
	Custom button control that allows for images and a numeric counter.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <vgui/IBorder.h>
#include <vgui/IInput.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <vgui/MouseCode.h>
#include <vgui/KeyCode.h>
#include <KeyValues.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/FocusNavGroup.h>
#include "forsaken/vgui/controls/forsaken_ctrl_buttonimagenumeric.h"

#include <tier0/memdbgon.h>

using namespace vgui;

// Constructor & Deconstructor
CForsakenButtonImageNumeric::CForsakenButtonImageNumeric(vgui::Panel *pParent, 
	const char *czName, const char *czText) : BaseClass(pParent, czName, czText)
{
	Init();
}

CForsakenButtonImageNumeric::~CForsakenButtonImageNumeric()
{
}

// Functions
void CForsakenButtonImageNumeric::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenButtonImageNumeric::ApplySettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySettings(pResources);

	// Fetch the values.
	m_nCost = pResources->GetInt("item_cost", m_nCost);
	m_nMaxRange = pResources->GetInt("max_value_range", 1);
	m_nMinRange = pResources->GetInt("min_value_range", 0);
	m_nDefaultValue = pResources->GetInt("default_value", m_nMinRange);

	// Set the current value to the default.
	SetCurrentValue(m_nDefaultValue);
}

void CForsakenButtonImageNumeric::DoClick()
/*
	
	Pre: 
	Post: 
*/
{
	char czText[64] = "";

	if (_mouseClickCode == MOUSE_LEFT)
	{
		// Increase the current value.
		m_nCurrentValue++;

		// If the current value is above the max, clip it.
		if (m_nCurrentValue > m_nMaxRange)
		{
			m_nCurrentValue = m_nMaxRange;

			return;
		}
	}
	else if (_mouseClickCode == MOUSE_RIGHT)
	{
		// Decrease the current value.
		m_nCurrentValue--;

		// If the current value is below the min, clip it.
		if (m_nCurrentValue < m_nMinRange)
		{
			m_nCurrentValue = m_nMinRange;

			return;
		}
	}

	// Set the text image's text.
	Q_snprintf(czText, sizeof(czText), "%d", m_nCurrentValue);
	SetText(czText);

	BaseClass::DoClick();
}

void CForsakenButtonImageNumeric::DrawFocusBorder(int tx0, int ty0, int tx1, int ty1)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenButtonImageNumeric::GetSettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::GetSettings(pResources);

	// Save the current values.
	pResources->SetInt("max_value_range", m_nMaxRange);
	pResources->SetInt("min_value_range", m_nMinRange);
	pResources->SetInt("default_value", m_nDefaultValue);
	pResources->SetInt("item_cost", m_nCost);
}

void CForsakenButtonImageNumeric::Init()
/*
	
	Pre: 
	Post: 
*/
{
	m_nMaxRange = m_nMinRange = m_nDefaultValue = m_nCurrentValue = 0;

	SetMouseClickEnabled(MOUSE_LEFT, true);
	SetMouseClickEnabled(MOUSE_RIGHT, true);

	Label::SetFgColor(Color(255, 255, 255, 255));
}

void CForsakenButtonImageNumeric::Paint()
{
	Label::SetFgColor(Color(255, 255, 255, 255));

	Button::Paint();
}

void CForsakenButtonImageNumeric::PaintBackground()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::PaintBackground();
}