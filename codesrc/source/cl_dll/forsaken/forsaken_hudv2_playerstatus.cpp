/*
forsaken_hudv2_playerstatus.cpp
The player status HUD v2.0 element for Forsaken.

Copyright© 2005 Forsaken Mod Team.
All Rights Reserved.
*/

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "iclientvehicle.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
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
#define MAX_OBJECTIVES	5
#define OBJECTIVE_SCAN	"%f %f %f %f %f"
#define OBJECTIVE_TEXTURES 25				// 8 for each team + 1 for blank
#define OFFSET_EMPTY 24						// blank texture is last
#define OFFSET_CIV 0						// Civ starts at index 0
#define OFFSET_GOV 8						// Gov starts at index 8
#define OFFSET_REL 16						// Rel starts at index 16
#define OFFSET_LEVEL 2						// Offset to move to next fort level
#define OFFSET_SECURED 1					// Offset to move to secured status

// Internal Classes
class CForsakenHudV2_PlayerStatus : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CForsakenHudV2_PlayerStatus, vgui::Panel);

public:
	// Constructor & Deconstructor
	CForsakenHudV2_PlayerStatus(const char *czElementName);

	// Public Accessor Functions

	// Public Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void ApplySettings(KeyValues *pResourceData);
	virtual void Init();
	virtual void Paint();
	virtual void Reset();
	virtual void VidInit();

	// Public Variables

protected:
	// Protected Functions
	void PaintCaptureObjective(float fXPos, float fYPos, C_ForsakenObjectiveEntity *pObjective);
	void PaintDestroyObjective(float fXPos, float fYPos, C_ForsakenObjectiveEntity *pObjective);
	void PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, wchar_t *czText);

	// Protected Variables
	float m_fObjectiveXPositions[MAX_OBJECTIVES];
	float m_fObjectiveYPositions[MAX_OBJECTIVES];
	int m_textureAuraEmpty;
	int m_textureBGCiv, m_textureBGGov, m_textureBGRel;
	int m_textureBGCivLdr, m_textureBGGovLdr, m_textureBGRelLdr;
	int m_textureHealth;
	int m_textureLeader;
	int m_textureSprint;

	int m_textureObjectives[OBJECTIVE_TEXTURES];

	Color m_ObjectiveNormalColor;
	Color m_PauseCaptureColor;
	Color m_OpposingCaptureColor;
	Color m_FriendlyCaptureColor;

	CPanelAnimationVar(vgui::HFont, m_hTextFont, "ObjectiveFont", "Default");

	// Aura & Background Positioning
	CPanelAnimationVarAliasType(float, m_fAuraBlockSize, "aura_size", "20", "proportional_float");
	CPanelAnimationVarAliasType(float, m_fAuraXPos, "aura_xpos", "30", "proportional_float");
	CPanelAnimationVarAliasType(float, m_fAuraYPos, "aura_ypos", "0", "proportional_float");
	CPanelAnimationVarAliasType(float, m_fBGBlockSize, "bg_size", "80", "proportional_float");
	CPanelAnimationVarAliasType(float, m_fBGXPos, "bg_xpos", "0", "proportional_float");
	CPanelAnimationVarAliasType(float, m_fBGYPos, "bg_ypos", "20", "proportional_float");

	// Objective Positioning
	CPanelAnimationVarAliasType(float, m_fObjectiveBlockSize, "objective_size", "20", 
		"proportional_float");
};

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudV2_PlayerStatus);

// Global Variable Declerations
bool g_bDisplayObjectiveInfo = false;

// Constructor & Deconstructor
CForsakenHudV2_PlayerStatus::CForsakenHudV2_PlayerStatus(const char *czElementName) :
CHudElement(czElementName), vgui::Panel(NULL, "ForsakenHudV2_PlayerStatus")
{
	// Set visibility rules and the parent.
	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT | HIDEHUD_CREDITS);
	SetParent(g_pClientMode->GetViewport());

	// Allocate memory for the background/faction textures.
	m_textureBGCiv = vgui::surface()->CreateNewTextureID();
	m_textureBGCivLdr = vgui::surface()->CreateNewTextureID();
	m_textureBGGov = vgui::surface()->CreateNewTextureID();
	m_textureBGGovLdr = vgui::surface()->CreateNewTextureID();
	m_textureBGRel = vgui::surface()->CreateNewTextureID();
	m_textureBGRelLdr = vgui::surface()->CreateNewTextureID();

	// Allocate memory for the active aura textures.
	m_textureAuraEmpty = vgui::surface()->CreateNewTextureID();

	// Allocate memory for the objective textures.
	for(int i = 0; i < OBJECTIVE_TEXTURES; i++)
	{
		m_textureObjectives[i] = vgui::surface()->CreateNewTextureID();
	}

	// Allocate memory for the health, leader, and sprint textures.
	m_textureHealth = vgui::surface()->CreateNewTextureID();
	m_textureLeader = vgui::surface()->CreateNewTextureID();
	m_textureSprint = vgui::surface()->CreateNewTextureID();

	// Setup colors
	m_ObjectiveNormalColor = Color(255,255,255,255);
	m_PauseCaptureColor = Color(255,255,0,255);
	m_OpposingCaptureColor = Color(255,0,0,255);
	m_FriendlyCaptureColor = Color(0,255,0,255);

	// Load the background/faction textures.
	vgui::surface()->DrawSetTextureFile(m_textureBGCiv, "hudv2/faction_civ", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureBGCivLdr, "hudv2/faction_civ_ldr", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureBGGov, "hudv2/faction_gov", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureBGGovLdr, "hudv2/faction_gov_ldr", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureBGRel, "hudv2/faction_rel", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureBGRelLdr, "hudv2/faction_rel_ldr", true, true);

	// Load the active aura textures.
	vgui::surface()->DrawSetTextureFile(m_textureAuraEmpty, "hudv2/aura_empty", true, true);

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

	// Load the health, leader, and sprint textures.
	vgui::surface()->DrawSetTextureFile(m_textureHealth, "hudv2/health_bar", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureLeader, "hudv2/leader_icon", true, true);
	vgui::surface()->DrawSetTextureFile(m_textureSprint, "hudv2/sprint_bar", true, true);
}

// Functions
// CForsakenHudV2_PlayerStatus
void CForsakenHudV2_PlayerStatus::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CForsakenHudV2_PlayerStatus::ApplySettings(KeyValues *pResourceData)
/*
	
	Pre: 
	Post: 
*/
{
	const char *czObjectiveXPositions = pResourceData->GetString("objectives_xpos", NULL);
	const char *czObjectiveYPositions = pResourceData->GetString("objectives_ypos", NULL);

	// Initialize values.
	for (int i = 0; i < MAX_OBJECTIVES; i++)
		m_fObjectiveXPositions[i] = m_fObjectiveYPositions[i] = -1.0f;

	// Do we have the x and y positions?
	if (czObjectiveXPositions == NULL || czObjectiveYPositions == NULL)
	{
		Msg("Invalid objective x and y positions.");

		return;
	}

	sscanf(czObjectiveXPositions, OBJECTIVE_SCAN, &m_fObjectiveXPositions[0], 
		&m_fObjectiveXPositions[1], &m_fObjectiveXPositions[2], &m_fObjectiveXPositions[3], 
		&m_fObjectiveXPositions[4]);
	sscanf(czObjectiveYPositions, OBJECTIVE_SCAN, &m_fObjectiveYPositions[0], 
		&m_fObjectiveYPositions[1], &m_fObjectiveYPositions[2], &m_fObjectiveYPositions[3], 
		&m_fObjectiveYPositions[4]);

	BaseClass::ApplySettings(pResourceData);
}

void CForsakenHudV2_PlayerStatus::Init()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_PlayerStatus::Paint()
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
	float fHealthHeight = 0, fSprintHeight = 0;
	float fHealthPercentage = 1.0f, fSprintPercentage = 1.0f;
	int nHeight, nTeamTexture, nWidth;

	if (!pPlayer)
		return;

	// Get the panel size.
	GetSize(nWidth, nHeight);

	// Initial paint state.
	SetPaintBackgroundEnabled(false);

	// Calculate the health height and percentage.
	fHealthPercentage = (float)pPlayer->GetHealth() / (float)pPlayer->GetMaxHealth();
	fHealthHeight = (nHeight - m_fBGYPos) - (fHealthPercentage * (nHeight - m_fBGYPos)) + m_fBGYPos;

	// Calculate the sprint percentage.
	if (pPlayer->GetMaxSprint() > 0)
		fSprintPercentage = (float)pPlayer->GetSprint() / (float)pPlayer->GetMaxSprint();
	else
		fSprintPercentage = 0.0f;

	// Calculate the sprint height.
	fSprintHeight = (nHeight - m_fBGYPos) - (fSprintPercentage * (nHeight - m_fBGYPos)) + m_fBGYPos;

	// Determine which team we are on and if we are a leader or not.
	switch (pPlayer->GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		if (pPlayer->IsLeader())
			nTeamTexture = m_textureBGCivLdr;
		else
			nTeamTexture = m_textureBGCiv;

		break;

	case TEAM_GOVERNORS:
		if (pPlayer->IsLeader())
			nTeamTexture = m_textureBGGovLdr;
		else
			nTeamTexture = m_textureBGGov;

		break;

	case TEAM_RELIGIOUS:
		if (pPlayer->IsLeader())
			nTeamTexture = m_textureBGRelLdr;
		else
			nTeamTexture = m_textureBGRel;

		break;

	default:
		// No team, so no need drawing the HUD.
		return;
	}

	// Draw the faction information.
	vgui::surface()->DrawSetTexture(nTeamTexture);
	vgui::surface()->DrawSetColor(255, 255, 255, 255);
	vgui::surface()->DrawTexturedRect(m_fBGXPos, m_fBGYPos, m_fBGXPos + m_fBGBlockSize, 
		m_fBGYPos + m_fBGBlockSize);

	// Draw the health bar.
	vgui::surface()->DrawSetTexture(m_textureHealth);
	vgui::surface()->DrawTexturedSubRect(m_fBGXPos, fHealthHeight, (m_fBGXPos + m_fBGBlockSize) / 2, 
		m_fBGYPos + m_fBGBlockSize, 0.0f, 1.0f - fHealthPercentage, 1.0f, 1.0f);

	// Draw the sprint bar.
	vgui::surface()->DrawSetTexture(m_textureSprint);
	vgui::surface()->DrawTexturedSubRect((m_fBGXPos + m_fBGBlockSize) / 2, fSprintHeight, 
		m_fBGXPos + m_fBGBlockSize, m_fBGYPos + m_fBGBlockSize, 0.0f, 1.0f - fSprintPercentage, 
		1.0f, 1.0f);

	// Draw the aura image.
	vgui::surface()->DrawSetTexture(m_textureAuraEmpty);
	vgui::surface()->DrawTexturedSubRect(m_fAuraXPos, m_fAuraYPos, m_fAuraXPos + m_fAuraBlockSize, 
		m_fAuraYPos + m_fAuraBlockSize, 0.0f, 0.0f, 1.0f, 1.0f);

	// Paint the objectives.
	for (int i = 0; i < g_pObjectiveList->GetObjectiveCount() && i < MAX_OBJECTIVES; i++)
	{
		C_ForsakenObjectiveEntity *pObjective = g_pObjectiveList->GetObjective(i);
		float fXPos = vgui::scheme()->GetProportionalScaledValue(m_fObjectiveXPositions[i]);
		float fYPos = vgui::scheme()->GetProportionalScaledValue(m_fObjectiveYPositions[i]);

		// Draw the objective depending on the type.
		switch (pObjective->GetObjectiveType())
		{
		case OBJECTIVE_TYPE_CAPTURE:
			PaintCaptureObjective(fXPos, fYPos, pObjective);
			break;

		case OBJECTIVE_TYPE_DESTROY:
			PaintDestroyObjective(fXPos, fYPos, pObjective);
			break;

		default:
			break;
		}
	}
}

void CForsakenHudV2_PlayerStatus::PaintCaptureObjective(float fXPos, float fYPos, 
	C_ForsakenObjectiveEntity *pObjective)
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
	C_ForsakenCaptureObjectiveEntity *pCapObjective = 
		static_cast<C_ForsakenCaptureObjectiveEntity *>(pObjective);
	C_ForsakenFortifiedObjectiveEntity *pFortification = NULL;
	Color renderColor = m_ObjectiveNormalColor;
	int nFontHeight = vgui::surface()->GetFontTall(m_hTextFont);
	int nObjectiveTexture = m_textureObjectives[OFFSET_EMPTY]; // This index is the blank objective texture
	int nObjectiveTextureIndex = 0; // Started at 0 for easy use of offsets to determine index

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

	// Set the colors we need for rendering.
	vgui::surface()->DrawSetColor(renderColor);
	vgui::surface()->DrawSetTextColor(renderColor);

	// Notify the user who currently owns this objective.
	vgui::surface()->DrawSetTexture(nObjectiveTexture);
	vgui::surface()->DrawTexturedSubRect(fXPos, fYPos, fXPos + m_fObjectiveBlockSize, 
		fYPos + m_fObjectiveBlockSize, 0.0f, 0.0f, 1.0f, 1.0f);

	// Does the user want extended objective information?
	if (g_bDisplayObjectiveInfo)
	{
		wchar_t czText[256] = L"";

		// Convert the objective name.
		mbstowcs(czText, pObjective->GetObjectiveName(), 256);

		PaintString(m_hTextFont, fXPos + m_fObjectiveBlockSize + 5, 
			fYPos + (m_fObjectiveBlockSize / 2) - (nFontHeight / 2), czText);
	}
}

void CForsakenHudV2_PlayerStatus::PaintDestroyObjective(float fXPos, float fYPos, 
	C_ForsakenObjectiveEntity *pObjective)
/*
	
	Pre: 
	Post: 
*/
{
// THIS SECTION COMMENTED OUT UNTIL DESTROY OBJECTIVES ARE EVEN USED!!!!!!

/*	C_ForsakenDestroyObjectiveEntity *pDestroyObjective = 
		static_cast<C_ForsakenDestroyObjectiveEntity *>(pObjective);
	Color renderColor = m_ObjectiveNormalColor;
	int nFontHeight = vgui::surface()->GetFontTall(m_hTextFont);
	int nObjectiveTexture = m_textureObjectiveEmpty;

	// Setup the animation and colors for objective destruction.
	if (pDestroyObjective->IsTakingDamage())
	{
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("ObjectiveTakingDamage");

		renderColor = m_ObjectiveDestroyColor;
	}
	else
	{
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("ObjectiveDestroyNormal");

		renderColor = m_ObjectiveNormalColor;
	}

	// Set the colors we need for rendering.
	vgui::surface()->DrawSetColor(renderColor);
	vgui::surface()->DrawSetTextColor(renderColor);

	vgui::surface()->DrawSetTexture(nObjectiveTexture);
	vgui::surface()->DrawTexturedSubRect(fXPos, fYPos, fXPos + m_fObjectiveBlockSize, 
		fYPos + m_fObjectiveBlockSize, 0.0f, 0.0f, 1.0f, 1.0f);

	//TODO: Objective animations.
	//"ObjectiveDestroyed", this is played when a objective is destroyed.

	// Draw the objective text.
	if (g_bDisplayObjectiveInfo)
	{
		wchar_t czText[256] = L"";

		// Convert the objective name.
		mbstowcs(czText, pObjective->GetObjectiveName(), 256);

		PaintString(m_hTextFont, fXPos + m_fObjectiveBlockSize + 5, 
			fYPos + (m_fObjectiveBlockSize / 2) - (nFontHeight / 2), czText);
	}*/
}

void CForsakenHudV2_PlayerStatus::PaintString(vgui::HFont hFont, int nXLoc, int nYLoc, 
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

void CForsakenHudV2_PlayerStatus::Reset()
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenHudV2_PlayerStatus::VidInit()
/*
	
	Pre: 
	Post: 
*/
{
}
