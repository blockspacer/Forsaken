/*
	ButtonImage.cpp
	Custom button control that allows for images.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include <stdio.h>
#include <UtlSymbol.h>

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
#include <vgui_controls/ButtonImage.h>
#include <vgui_controls/FocusNavGroup.h>

#include <tier0/memdbgon.h>

using namespace vgui;

// Constructor & Deconstructor
ButtonImage::ButtonImage(Panel *pParent, const char *czPanelName, const char *czText, 
	Panel *pActionSignalTarget, const char *czCommand) : Button(pParent, czPanelName, 
	czText, pActionSignalTarget, czCommand)
{
	m_bIsSelected = false;
	m_czActiveImageName = NULL;
	m_czArmedImageName = NULL;
	m_czDepressedImageName = NULL;
	m_czDisabledImageName = NULL;
	m_czEnabledImageName = NULL;

	Init();
}

ButtonImage::ButtonImage(Panel *pParent, const char *czPanelName, const wchar_t *czText, 
	Panel *pActionSignalTarget, const char *czCommand) : Button(pParent, czPanelName, 
	czText, pActionSignalTarget, czCommand)
{
	m_bIsSelected = false;
	m_czActiveImageName = NULL;
	m_czArmedImageName = NULL;
	m_czDepressedImageName = NULL;
	m_czDisabledImageName = NULL;
	m_czEnabledImageName = NULL;

	Init();
}

ButtonImage::~ButtonImage()
{
	delete [] m_czActiveImageName;
	delete [] m_czArmedImageName;
	delete [] m_czDepressedImageName;
	delete [] m_czDisabledImageName;
	delete [] m_czEnabledImageName;
}

// Functions
void ButtonImage::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{

	// Load the colors we use.
	m_colorNonSelectedFocus = GetSchemeColor("ButtonImage.NonSelectedFocus", Color(0, 0, 0, 255), pScheme);
	m_colorSelectedFocus = GetSchemeColor("ButtonImage.SelectedFocus", Color(0, 0, 0, 255), pScheme);

	BaseClass::ApplySchemeSettings(pScheme);
}

void ButtonImage::ApplySettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySettings(pResources);

	const char *czActiveImage = pResources->GetString("active_image", NULL);
	const char *czArmedImage = pResources->GetString("armed_image", NULL);
	const char *czDepressedImage = pResources->GetString("depressed_image", NULL);
	const char *czDisabledImage = pResources->GetString("disabled_image", NULL);
	const char *czEnabledImage = pResources->GetString("enabled_image", NULL);
	int nWide, nTall;

	// Get the size of our button.
	GetSize(nWide, nTall);

	if (czActiveImage)
	{
		int nLen = Q_strlen(czActiveImage);

		// Allocate memory for the string.
		m_czActiveImageName = new char[nLen];

		// Copy the string.
		Q_strcpy(m_czActiveImageName, czActiveImage);

		// Get the image.
		m_pActiveImage = scheme()->GetImage(czActiveImage, false);

		if (m_pActiveImage)
			m_pActiveImage->SetSize(nWide, nTall);
	}

	if (czArmedImage)
	{
		int nLen = Q_strlen(czArmedImage);

		// Allocate memory for the string.
		m_czArmedImageName = new char[nLen];

		// Copy the string.
		Q_strcpy(m_czArmedImageName, czArmedImage);

		// Get the image.
		m_pArmedImage = scheme()->GetImage(czArmedImage, false);

		// Scale the image to fit.
		if (m_pArmedImage)
			m_pArmedImage->SetSize(nWide, nTall);
	}

	if (czDepressedImage)
	{
		int nLen = Q_strlen(czDepressedImage);

		// Allocate memory for the string.
		m_czDepressedImageName = new char[nLen];

		// Copy the string.
		Q_strcpy(m_czDepressedImageName, czDepressedImage);

		// Get the image.
		m_pDepressedImage = scheme()->GetImage(czDepressedImage, false);

		// Scale the image to fit.
		if (m_pDepressedImage)
			m_pDepressedImage->SetSize(nWide, nTall);
	}

	if (czDisabledImage)
	{
		int nLen = Q_strlen(czDisabledImage);

		// Allocate memory for the string.
		m_czDisabledImageName = new char[nLen];

		// Copy the string.
		Q_strcpy(m_czDisabledImageName, czDisabledImage);

		// Get the image.
		m_pDisabledImage = scheme()->GetImage(czDisabledImage, false);

		// Scale the image to fit.
		if (m_pDisabledImage)
			m_pDisabledImage->SetSize(nWide, nTall);
	}

	if (czEnabledImage)
	{
		int nLen = Q_strlen(czEnabledImage);

		// Allocate memory for the string.
		m_czEnabledImageName = new char[nLen];

		// Copy the string.
		Q_strcpy(m_czEnabledImageName, czEnabledImage);

		// Get the image.
		m_pEnabledImage = scheme()->GetImage(czEnabledImage, false);

		// Scale the image to fit.
		if (m_pEnabledImage)
			m_pEnabledImage->SetSize(nWide, nTall);
	}
}

void ButtonImage::DrawFocusBorder(int tx0, int ty0, int tx1, int ty1)
/*
	
	Pre: 
	Post: 
*/
{
}

void ButtonImage::GetSettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	if (m_czActiveImageName)
		pResources->SetString("active_image", m_czActiveImageName);

	if (m_czArmedImageName)
		pResources->SetString("armed_image", m_czArmedImageName);

	if (m_czDepressedImageName)
		pResources->SetString("depressed_image", m_czDepressedImageName);

	if (m_czDisabledImageName)
		pResources->SetString("disabled_image", m_czDisabledImageName);

	if (m_czEnabledImageName)
		pResources->SetString("enabled_image", m_czEnabledImageName);

	BaseClass::GetSettings(pResources);
}

void ButtonImage::Init()
/*
	
	Pre: 
	Post: 
*/
{
	m_pActiveImage = NULL;
	m_pArmedImage = NULL;
	m_pDepressedImage = NULL;
	m_pDisabledImage = NULL;
	m_pEnabledImage = NULL;
}

void ButtonImage::PaintBackground()
/*
	
	Pre: 
	Post: 
*/
{
	IImage *pImage = NULL;

	// Try to paint the image that matches our current state.
	if (IsActive())
		pImage = m_pActiveImage;
	else if (IsArmed())
	{
		if (IsDepressed())
			pImage = m_pDepressedImage;
		else
			pImage = m_pArmedImage;
	}
	else if (!IsEnabled())
		pImage = m_pDisabledImage;
	else
		pImage = m_pEnabledImage;

	// Fall back to the enabled image.
	if (!pImage)
		pImage = m_pEnabledImage;

	// If we have a image, paint it... otherwise they have none.
	if (pImage)
		pImage->Paint();
}