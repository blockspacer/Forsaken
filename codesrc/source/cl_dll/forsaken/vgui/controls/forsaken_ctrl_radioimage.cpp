/*
	forsaken_ctrl_radioimage.cpp
	Custom radio button control that allows for images.

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
#include <vgui_controls/RadioButton.h>
#include <vgui_controls/FocusNavGroup.h>
#include "forsaken/vgui/controls/forsaken_ctrl_radioimage.h"

#include <tier0/memdbgon.h>

using namespace vgui;

// Constructor & Deconstructor
CForsakenRadioImage::CForsakenRadioImage(vgui::Panel *pParent, const char *czName, 
	const char *czText) : BaseClass(pParent, czName, czText)
{
	msgToSend = NULL;
	Init();
}

CForsakenRadioImage::~CForsakenRadioImage()
{
	if(msgToSend)
	{
		msgToSend->deleteThis();
	}
}

// Functions
void CForsakenRadioImage::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	// Load the colors we use.
	m_colorNonSelectedFocus = GetSchemeColor("RadioImage.NonSelectedFocus", Color(0, 0, 0, 255), pScheme);
	m_colorSelectedFocus = GetSchemeColor("RadioImage.SelectedFocus", Color(0, 0, 0, 255), pScheme);

	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenRadioImage::ApplySettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySettings(pResources);

	const char *czNormalImage = pResources->GetString("normal_image", NULL);
	const char *czSelectedImage = pResources->GetString("selected_image", NULL);
	int nWide, nTall;

	// Get the size of our button.
	GetSize(nWide, nTall);

	if (czNormalImage)
	{
		// Get the image.
		m_pNormalImage = scheme()->GetImage(czNormalImage, false);

		if (m_pNormalImage)
			m_pNormalImage->SetSize(nWide, nTall);
	}

	if (czSelectedImage)
	{
		// Get the image.
		m_pSelectedImage = scheme()->GetImage(czSelectedImage, false);

		// Scale the image to fit.
		if (m_pSelectedImage)
			m_pSelectedImage->SetSize(nWide, nTall);
	}
}

void CForsakenRadioImage::DrawFocusBorder(int tx0, int ty0, int tx1, int ty1)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenRadioImage::GetSettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::GetSettings(pResources);
}

void CForsakenRadioImage::Init()
/*
	
	Pre: 
	Post: 
*/
{
	m_pNormalImage = NULL;
	m_pSelectedImage = NULL;
}

void CForsakenRadioImage::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	IImage *pImage = NULL;
	int nAlpha = 255; // Full opacity

	// Try to paint the image that matches our current state.
	pImage = m_pNormalImage;

	if (!IsEnabled())
	{
		nAlpha = 64; // make it 1/4 opaque
	}

	if(IsSelected())
		pImage = m_pSelectedImage;

	// Fall back to the enabled image.
	if (!pImage)
		pImage = m_pNormalImage;

	// If we have a image, paint it... otherwise they have none.
	if (pImage)
	{
		SetAlpha(nAlpha);
		pImage->SetColor(m_colorAnimate);
		pImage->Paint();

		/*if(IsSelected())
			vgui::surface()->DrawOutlinedCircle(Get*/
	}
}
void CForsakenRadioImage::SetNormalImage(vgui::IImage *pImage)
/*
	
	Pre: 
	Post: 
*/
{
	m_pNormalImage = pImage;

	if (m_pNormalImage)
		m_pNormalImage->SetSize(GetWide(), GetTall());
}
void CForsakenRadioImage::SetSelectedImage(vgui::IImage *pImage)
/*
	
	Pre: 
	Post: 
*/
{
	m_pSelectedImage = pImage;

	if (m_pSelectedImage)
		m_pSelectedImage->SetSize(GetWide(), GetTall());
}

void CForsakenRadioImage::SetSelected(bool state)
/*
	
	Pre: 
	Post: 
*/
{
	if (state && msgToSend)
	{
		PostActionSignal(msgToSend);
		msgToSend = NULL;		
	}

	BaseClass::SetSelected(state);
}

void CForsakenRadioImage::SetActionSignalMessage(KeyValues *msg)
/*
	
	Pre: 
	Post: 
*/
{
	if (msgToSend)
	{
		msgToSend->deleteThis();
		msgToSend = NULL;
	}

	msgToSend = msg;
}