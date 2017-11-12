/*
	forsaken_ctrl_objectivemap.cpp
	Custom button control that allows for images.

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
#include "forsaken/vgui/controls/forsaken_ctrl_radioimage.h"
#include "forsaken/vgui/controls/forsaken_ctrl_objectivemap.h"
#include "forsaken/c_forsaken_player.h"
#include "forsaken/c_forsaken_objective_list.h"

#include <tier0/memdbgon.h>

using namespace vgui;

// Constructor & Deconstructor
CForsakenObjectiveMap::CForsakenObjectiveMap(vgui::Panel *pParent, const char *czName, 
	const char *czText) : BaseClass(pParent, czName, czText)
{
	m_czCurrentMapName = NULL;
	m_pMapImage = NULL;

	// Load the objective textures. (Don't feel like using sprintf() just to build static path names)
	m_textureObjectives[0] = scheme()->GetImage("objective/civ_captured", false);
	m_textureObjectives[1] = scheme()->GetImage("objective/civ_captured_selected", false);
	m_textureObjectives[2] = scheme()->GetImage("objective/civ_secured", false);
	m_textureObjectives[3] = scheme()->GetImage("objective/civ_secured_selected", false);
	m_textureObjectives[4] = scheme()->GetImage("objective/civ_L1_unsecured", false);
	m_textureObjectives[5] = scheme()->GetImage("objective/civ_L1_unsecured_selected", false);
	m_textureObjectives[6] = scheme()->GetImage("objective/civ_L1_secured", false);
	m_textureObjectives[7] = scheme()->GetImage("objective/civ_L1_secured_selected", false);
	m_textureObjectives[8] = scheme()->GetImage("objective/civ_L2_unsecured", false);
	m_textureObjectives[9] = scheme()->GetImage("objective/civ_L2_unsecured_selected", false);
	m_textureObjectives[10] = scheme()->GetImage("objective/civ_L2_secured", false);
	m_textureObjectives[11] = scheme()->GetImage("objective/civ_L2_secured_selected", false);
	m_textureObjectives[12] = scheme()->GetImage("objective/civ_L3_unsecured", false);
	m_textureObjectives[13] = scheme()->GetImage("objective/civ_L3_unsecured_selected", false);
	m_textureObjectives[14] = scheme()->GetImage("objective/civ_L3_secured", false);
	m_textureObjectives[15] = scheme()->GetImage("objective/civ_L3_secured_selected", false);
	m_textureObjectives[16] = scheme()->GetImage("objective/gov_captured", false);
	m_textureObjectives[17] = scheme()->GetImage("objective/gov_captured_selected", false);
	m_textureObjectives[18] = scheme()->GetImage("objective/gov_secured", false);
	m_textureObjectives[19] = scheme()->GetImage("objective/gov_secured_selected", false);
	m_textureObjectives[20] = scheme()->GetImage("objective/gov_L1_unsecured", false);
	m_textureObjectives[21] = scheme()->GetImage("objective/gov_L1_unsecured_selected", false);
	m_textureObjectives[22] = scheme()->GetImage("objective/gov_L1_secured", false);
	m_textureObjectives[23] = scheme()->GetImage("objective/gov_L1_secured_selected", false);
	m_textureObjectives[24] = scheme()->GetImage("objective/gov_L2_unsecured", false);
	m_textureObjectives[25] = scheme()->GetImage("objective/gov_L2_unsecured_selected", false);
	m_textureObjectives[26] = scheme()->GetImage("objective/gov_L2_secured", false);
	m_textureObjectives[27] = scheme()->GetImage("objective/gov_L2_secured_selected", false);
	m_textureObjectives[28] = scheme()->GetImage("objective/gov_L3_unsecured", false);
	m_textureObjectives[29] = scheme()->GetImage("objective/gov_L3_unsecured_selected", false);
	m_textureObjectives[30] = scheme()->GetImage("objective/gov_L3_secured", false);
	m_textureObjectives[31] = scheme()->GetImage("objective/gov_L3_secured_selected", false);
	m_textureObjectives[32] = scheme()->GetImage("objective/rel_captured", false);
	m_textureObjectives[33] = scheme()->GetImage("objective/rel_captured_selected", false);
	m_textureObjectives[34] = scheme()->GetImage("objective/rel_secured", false);
	m_textureObjectives[35] = scheme()->GetImage("objective/rel_secured_selected", false);
	m_textureObjectives[36] = scheme()->GetImage("objective/rel_L1_unsecured", false);
	m_textureObjectives[37] = scheme()->GetImage("objective/rel_L1_unsecured_selected", false);
	m_textureObjectives[38] = scheme()->GetImage("objective/rel_L1_secured", false);
	m_textureObjectives[39] = scheme()->GetImage("objective/rel_L1_secured_selected", false);
	m_textureObjectives[40] = scheme()->GetImage("objective/rel_L2_unsecured", false);
	m_textureObjectives[41] = scheme()->GetImage("objective/rel_L2_unsecured_selected", false);
	m_textureObjectives[42] = scheme()->GetImage("objective/rel_L2_secured", false);
	m_textureObjectives[43] = scheme()->GetImage("objective/rel_L2_secured_selected", false);
	m_textureObjectives[44] = scheme()->GetImage("objective/rel_L3_unsecured", false);
	m_textureObjectives[45] = scheme()->GetImage("objective/rel_L3_unsecured_selected", false);
	m_textureObjectives[46] = scheme()->GetImage("objective/rel_L3_secured", false);
	m_textureObjectives[47] = scheme()->GetImage("objective/rel_L3_secured_selected", false);
	m_textureObjectives[48] = scheme()->GetImage("objective/objective_empty", false);

	// Setup colors
	m_ObjectiveNormalColor = Color(255,255,255,255);
	m_PauseCaptureColor = Color(255,255,0,255);
	m_OpposingCaptureColor = Color(255,0,0,255);
	m_FriendlyCaptureColor = Color(0,255,0,255);

	Init();
}

CForsakenObjectiveMap::~CForsakenObjectiveMap()
{
	delete [] m_czCurrentMapName;
	for(int i = 0; i < m_vecObjectiveButtons.Count(); i++)
	{
		delete m_vecObjectiveButtons[i];
	}
}

// Functions
void CForsakenObjectiveMap::ApplySchemeSettings(vgui::IScheme *pScheme)
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

void CForsakenObjectiveMap::ApplySettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySettings(pResources);
}

void CForsakenObjectiveMap::DrawFocusBorder(int tx0, int ty0, int tx1, int ty1)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenObjectiveMap::GetSettings(KeyValues *pResources)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::GetSettings(pResources);
}

void CForsakenObjectiveMap::Init()
/*
	
	Pre: 
	Post: 
*/
{
	m_pMapImage = NULL;
}

void CForsakenObjectiveMap::LoadMapImage(char *czMapName)
/*
	
	Pre: 
	Post: 
*/
{
	char *czImageName = NULL;
	int nLen = 0, nLenImage = 0;
	int nWide, nTall;

	// Get the size of our button.
	GetSize(nWide, nTall);

	// We don't want to leak memory.
	if (m_pMapImage)
		delete m_pMapImage;

	// Determine the length of the map name.
	nLen = Q_strlen(czMapName);
	nLenImage = Q_strlen(czMapName) + 15;

	// We don't want to leak memory.
	delete [] m_czCurrentMapName;

	// Allocate memory for the path to the map image.
	// maps/overview_<map_name>.vmt
	m_czCurrentMapName = new char[nLen];
	czImageName = new char[nLenImage];

	// Generate the image name.
	if (czImageName != NULL)
	{
		Q_snprintf(czImageName, nLenImage * sizeof(char), "maps/overview_%s", czMapName);

		// We want to load this new map name.
		m_pMapImage = scheme()->GetImage(czImageName, false);

		// We don't want to leak memory.
		delete [] czImageName;

		// Scale the image to fit.
		if (m_pMapImage)
			m_pMapImage->SetSize(nWide, nTall);
	}

	// Copy the map name.
	if (m_czCurrentMapName != NULL)
		Q_strcpy(m_czCurrentMapName, czMapName);
}

void CForsakenObjectiveMap::LoadMapObjectives(char *czMapName)
/*
	
	Pre: 
	Post: 
*/
{
	char *czObjectiveInfoFile = NULL;
	int nLen = 0;
	KeyValues *pLoadData = new KeyValues("ObjectiveInformation");

	// Allocate memory for the objective file.
	// maps/overview_<map_name>.txt;
	nLen = Q_strlen(czMapName) + 19;
	czObjectiveInfoFile = new char[nLen];

	if (czObjectiveInfoFile != NULL)
	{
		// Copy the map name into the objective file.
		Q_snprintf(czObjectiveInfoFile, nLen * sizeof(char), "maps/overview_%s.txt", czMapName);

		// Clear out any existing objective information we have.
		m_vecObjectives.RemoveAll();

		// And clear radio buttons
		for(int i = 0; i < m_vecObjectiveButtons.Count(); i++)
			delete m_vecObjectiveButtons[i];

		m_vecObjectiveButtons.RemoveAll();

		if (pLoadData)
		{
			// Load the save state from the file.
			if (pLoadData->LoadFromFile(vgui::filesystem(), czObjectiveInfoFile, "MOD"))
			{
				KeyValues *pSubKeys = pLoadData->GetFirstSubKey();

				// Loop until there are no more sub-keys.
				while (pSubKeys != NULL)
				{
					tagLocationItem locItem;

					const char *czObjectiveName = pSubKeys->GetString("ObjectiveName");

					// Fetch the location of the locational item.
					locItem.nX = pSubKeys->GetInt("X");
					locItem.nY = pSubKeys->GetInt("Y");

					// What team does this location belong to?
					locItem.nTeamBitmask = pSubKeys->GetInt("Team");
					locItem.nLocationType = pSubKeys->GetInt("LocationType", LOCATION_TYPE_UNKNOWN);

					// Copy the objective name.
					Q_strncpy(locItem.czObjectiveName, czObjectiveName, OBJECTIVE_NAME_LENGTH);

					// Add it to our objective list.
					m_vecObjectives.AddToTail(locItem);

					// Fetch the next sub-key.
					pSubKeys = pSubKeys->GetNextKey();
				}
			}

			pLoadData->deleteThis();
		}

		CreateObjectiveButtons();
	}

	delete [] czObjectiveInfoFile;
}

void CForsakenObjectiveMap::CreateObjectiveButtons()
/*
	
	Pre: 
	Post: 
*/
{
	// Process each loaded objective
	for(int i = 0; i < m_vecObjectives.Count(); i++)
	{
		// Setup Radio Button
		CForsakenRadioImage *tempButton = new CForsakenRadioImage(this, m_vecObjectives[i].czObjectiveName, "");

		// Rescale coordinates from orignal dimensions to new dimensions.
		// Also take width and height of button into account.
		int nWidth = 32;
		int nHeight = 32;
		int nX = (GetWide() * m_vecObjectives[i].nX / 256.0f) - (nWidth / 2);
		int nY = (GetTall() * m_vecObjectives[i].nY / 256.0f) - (nHeight / 2);

		tempButton->SetPos(nX, nY);
		tempButton->SetVisible(true);
		tempButton->SetSize(nWidth,nHeight);
		tempButton->SetSubTabPosition(i+1); // Must be [1...N]
		tempButton->AddActionSignalTarget(GetParent()->GetParent()->GetParent());

		// Add radio button
		m_vecObjectiveButtons.AddToTail(tempButton);
	}
}

void CForsakenObjectiveMap::PaintBackground()
/*
	
	Pre: 
	Post: 
*/
{
	char *czMapName = NULL;
	const char *czMapNameTemp = engine->GetLevelName();
	int nLen = 0;

	if (czMapNameTemp != NULL && czMapNameTemp[0] != 0)
	{
		// Allocate memory for the map name.
		nLen = Q_strlen(czMapNameTemp) + 1;
		czMapName = new char[nLen];

		// We don't want no stinking crashes.
		if (czMapName != NULL)
		{
			char *czMapNameOffs = czMapName;

			// Copy the map name.
			Q_strcpy(czMapName, czMapNameTemp);

			// Get rid of the 'maps/'
			czMapNameOffs += 5;

			// Get rid of the .bsp.
			for (int i = 0; ; i++)
			{
				// Find the .?
				if (czMapNameOffs[i] == '.')
				{
					czMapNameOffs[i] = 0;

					break;
				}
			}

			// We are on a new map, so load the new settings.
			if (m_czCurrentMapName == NULL || Q_stricmp(czMapNameOffs, m_czCurrentMapName) != 0)
			{
				LoadMapImage(czMapNameOffs);
				LoadMapObjectives(czMapNameOffs);
			}

			delete [] czMapName;
		}
	}

	// Render the map if we got it.
	if (m_pMapImage)
		m_pMapImage->Paint();
}
void CForsakenObjectiveMap::Paint()
{
	for (int i = 0; i < m_vecObjectives.Count(); i++)
	{
		// Rescale coordinates from orignal dimensions to new dimensions.
		// Take into account height of images.
		int nX = (GetWide() * m_vecObjectives[i].nX / 256.0f);
		int nY = (GetTall() * m_vecObjectives[i].nY / 256.0f) + 16;

		// Set the text color
		vgui::surface()->DrawSetTextColor(m_ObjectiveFontColor);

		// Handle text output
		wchar_t czText[512] = L"";
		swprintf(czText, 512, L"%hs", m_vecObjectives[i].czObjectiveName);
		int nTextWidth = UTIL_ComputeStringWidth(m_hObjectiveFont, czText);

		// Center text output
		int nXoffset = nTextWidth / 2;
		PaintString(m_hObjectiveFont, nX - nXoffset, nY, czText);
	}
}

void CForsakenObjectiveMap::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText)
/*
	
	Pre: 
	Post: 
*/
{
	// Set the font and position then draw the character.
	vgui::surface()->DrawSetTextFont(hFont);
	vgui::surface()->DrawSetTextPos(nXLoc, nYLoc);

	// Draw the string.
	for (wchar_t *cChar = czText; *cChar != 0; cChar++)
		vgui::surface()->DrawUnicodeChar(*cChar);
}