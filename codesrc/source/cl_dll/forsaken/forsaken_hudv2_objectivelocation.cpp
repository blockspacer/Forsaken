/*
	forsaken_hudv2_objectivelocation.cpp
	The objective location HUD v2.0 element for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include <vgui_controls/AnimationController.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include "c_baseplayer.h"
#include "c_forsaken_player.h"
#include "utlvector.h"
#include "c_forsaken_objective_entity.h"
#include "c_forsaken_capture_entity.h"
#include "c_forsaken_destroy_entity.h"
#include "c_forsaken_objective_list.h"
#include "c_forsaken_fortification_list.h"
#include "tier0/memdbgon.h"

// Internal Defines
#define OBJECTIVE_TEXTURES 25				// 8 for each team + 1 for blank
#define OFFSET_EMPTY 24						// blank texture is last
#define OFFSET_CIV 0						// Civ starts at index 0
#define OFFSET_GOV 8						// Gov starts at index 8
#define OFFSET_REL 16						// Rel starts at index 16
#define OFFSET_LEVEL 2						// Offset to move to next fort level
#define OFFSET_SECURED 1					// Offset to move to secured status

// Internal Classes
class CForsakenHudV2_ObjectiveLocation : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CForsakenHudV2_ObjectiveLocation, vgui::Panel);

public:
	// Constructor & Deconstructor
	CForsakenHudV2_ObjectiveLocation(const char *czElementName);

	// Public Accessor Functions

	// Public Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Init();
	virtual void Paint();
	virtual void Reset();
	virtual void VidInit();

	// Public Variables

protected:
	// Protected Functions
	void PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText);

	// Protected Variables
	int m_textureObjectives[OBJECTIVE_TEXTURES];

	CPanelAnimationVar(Color, m_ObjectiveFontColor, "ObjectiveFontColor", "Orange");
	CPanelAnimationVar(vgui::HFont, m_hObjectiveFont, "ObjectiveFont", "Default");

	Color m_ObjectiveNormalColor;
	Color m_PauseCaptureColor;
	Color m_OpposingCaptureColor;
	Color m_FriendlyCaptureColor;

	// Objective Positioning
	CPanelAnimationVarAliasType(float, m_fObjectiveBlockSize, "objective_size", "20", 
		"proportional_float");
};

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudV2_ObjectiveLocation);

// Global Variable Declerations
extern bool g_bDisplayObjectiveInfo;

// Function Prototypes
int ScreenTransform(const Vector& point, Vector& screen);

// Constructor & Deconstructor
CForsakenHudV2_ObjectiveLocation::CForsakenHudV2_ObjectiveLocation(const char *czElementName) :
	CHudElement(czElementName), BaseClass(NULL, "ForsakenHudV2_ObjectiveLocation")
{
	// Set visibility rules and the parent.
	SetHiddenBits(HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT | HIDEHUD_CREDITS);
	SetParent(g_pClientMode->GetViewport());

	// Allocate memory for the objective textures.
	for(int i = 0; i < OBJECTIVE_TEXTURES; i++)
	{
		m_textureObjectives[i] = vgui::surface()->CreateNewTextureID();
	}

	// Setup colors
	m_ObjectiveNormalColor = Color(255,255,255,255);
	m_PauseCaptureColor = Color(255,255,0,255);
	m_OpposingCaptureColor = Color(255,0,0,255);
	m_FriendlyCaptureColor = Color(0,255,0,255);

	// Load the objective textures. (Don't feel like using sprintf() just to build static path names)
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[0], "vgui/objective/civ_captured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[1], "vgui/objective/civ_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[2], "vgui/objective/civ_L1_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[3], "vgui/objective/civ_L1_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[4], "vgui/objective/civ_L2_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[5], "vgui/objective/civ_L2_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[6], "vgui/objective/civ_L3_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[7], "vgui/objective/civ_L3_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[8], "vgui/objective/gov_captured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[9], "vgui/objective/gov_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[10], "vgui/objective/gov_L1_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[11], "vgui/objective/gov_L1_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[12], "vgui/objective/gov_L2_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[13], "vgui/objective/gov_L2_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[14], "vgui/objective/gov_L3_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[15], "vgui/objective/gov_L3_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[16], "vgui/objective/rel_captured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[17], "vgui/objective/rel_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[18], "vgui/objective/rel_L1_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[19], "vgui/objective/rel_L1_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[20], "vgui/objective/rel_L2_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[21], "vgui/objective/rel_L2_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[22], "vgui/objective/rel_L3_unsecured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[23], "vgui/objective/rel_L3_secured", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureObjectives[24], "hudv2/objective_empty", true, true);
}

// Functions
void CForsakenHudV2_ObjectiveLocation::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenHudV2_ObjectiveLocation::Init()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_ObjectiveLocation::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	if (!g_bDisplayObjectiveInfo)
		return;

	// Set panel size to hud size
	int nHudWidth, nHudHeight;
	GetHudSize(nHudWidth, nHudHeight);
	SetSize(nHudWidth, nHudHeight);	// sets size of panel

	// Set the text color
	vgui::surface()->DrawSetTextColor(m_ObjectiveFontColor);

	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
	wchar_t czText[512] = L"";

	// Initial paint state.
	SetPaintBackgroundEnabled(false);

	for (int i = 0; i < g_pObjectiveList->GetObjectiveCount(); i++)
	{
		// Set default HUD coloring
		Color renderColor(m_ObjectiveNormalColor);

		C_ForsakenObjectiveEntity *pObjective = g_pObjectiveList->GetObjective(i);

		// This code is currently only for capture objectives.
		if (pObjective->GetObjectiveType() != OBJECTIVE_TYPE_CAPTURE)
		{
			continue;
		}

		C_ForsakenCaptureObjectiveEntity *pCapObjective = 
			static_cast<C_ForsakenCaptureObjectiveEntity *>(pObjective);
		C_ForsakenFortifiedObjectiveEntity *pFortification = NULL;
		int nObjectiveTexture = m_textureObjectives[OFFSET_EMPTY]; // This index is the blank objective texture
		int nObjectiveTextureIndex = 0; // Started at 0 for easy use of offsets to determine index
		int nXS = 0, nYS = 0, nTextWidth = 0, nXoffset = 0;

		// If we are not going to draw it, do early culling.
		if (!GetTargetInScreenSpace(pCapObjective, nXS, nYS))
			continue;

		// Get the matching fortified entity.
		for (int i = 0; i < g_pFortificationList->GetFortificationCount(); i++)
		{
			C_ForsakenFortifiedObjectiveEntity *pFortificationTemp = g_pFortificationList->GetFortification(i);
			if (pFortificationTemp->GetObjectiveEntity() == pObjective)
			{
				pFortification = pFortificationTemp;
				break;
			}
		}

		// Handle text output
		swprintf(czText, L"%hs", pCapObjective->GetObjectiveName());
		nTextWidth = UTIL_ComputeStringWidth(m_hObjectiveFont, czText);

		// Center text output
		nXoffset = nTextWidth / 2;
		PaintString(m_hObjectiveFont, nXS - nXoffset, nYS, czText);

		///////////////////////////////////
		// Determine the texture to use
		///////////////////////////////////

		// Who owns the objective?
		switch (pCapObjective->GetTeamOwner())
		{
		case TEAM_CIVILIANS:
			nObjectiveTextureIndex += OFFSET_CIV;
			break;

		case TEAM_GOVERNORS:
			nObjectiveTextureIndex += OFFSET_GOV;
			break;

		case TEAM_RELIGIOUS:
			nObjectiveTextureIndex += OFFSET_REL;
			break;

		default:
			nObjectiveTextureIndex = OFFSET_EMPTY;
		}

		// Fortification status
		if (pFortification)
		{
			// What level is the fortification?
			nObjectiveTextureIndex += pFortification->GetFortificationLevel() * OFFSET_LEVEL;

			// Is the fortification secured?
			if (pFortification->GetFortificationStatus() == FORSAKEN_FORTIFIEDSTATUS_COMPLETE)
			{
				nObjectiveTextureIndex += OFFSET_SECURED;
			}
		}

		// Finally, set the texture id.
		nObjectiveTexture = m_textureObjectives[nObjectiveTextureIndex];

		///////////////////////////////////
		// Determine animation sequence
		///////////////////////////////////

		// Capturing can only occur if the fort is not secured
		if (!pFortification || pFortification->GetFortificationStatus() != FORSAKEN_FORTIFIEDSTATUS_COMPLETE)
		{
			// Do we have someone capturing the objective that isn't the owner?
			if (pCapObjective->GetTeamCapturing() != TEAM_UNASSIGNED && pCapObjective->GetTeamCapturing() != pCapObjective->GetTeamOwner())
			{
				// Multiple teams trying to capture this objective
				if(pCapObjective->GetTeamCapturing() == TEAM_SPECTATOR)
				{
					renderColor = m_PauseCaptureColor;
				}
				// We are capturing the objective
				else if(pCapObjective->GetTeamCapturing() == pPlayer->GetTeamNumber())
				{
					renderColor = m_FriendlyCaptureColor;
				}
				// Other team is capturing the objective
				else
				{
					renderColor = m_OpposingCaptureColor;
				}
			}		
		}

		// Adjust alpha for pulsing
		if (renderColor != m_ObjectiveNormalColor)
		{
			renderColor[3] = 255 * ( 0.5f + 0.5f * ( cos( gpGlobals->curtime*10 ) ) );
		}

		vgui::surface()->DrawSetColor(renderColor);

		// Center texture output
		nXoffset = m_fObjectiveBlockSize / 2;

		// Notify the user who currently owns this objective.
		vgui::surface()->DrawSetTexture(nObjectiveTexture);
		vgui::surface()->DrawTexturedSubRect(nXS - nXoffset, nYS + 30, nXS - nXoffset + m_fObjectiveBlockSize, 
			nYS + 30 + m_fObjectiveBlockSize, 0.0f, 0.0f, 1.0f, 1.0f);
	}
}

void CForsakenHudV2_ObjectiveLocation::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, 
	wchar_t *czText)
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

void CForsakenHudV2_ObjectiveLocation::Reset()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_ObjectiveLocation::VidInit()
/*
	
	Pre: 
	Post: 
*/
{
}