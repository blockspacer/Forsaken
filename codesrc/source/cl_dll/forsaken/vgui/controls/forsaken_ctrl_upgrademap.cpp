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
#include "forsaken/vgui/controls/forsaken_ctrl_upgrademap.h"
#include "forsaken/c_forsaken_player.h"
#include "forsaken/c_forsaken_objective_list.h"
#include "forsaken/c_forsaken_capture_entity.h"
#include "forsaken/c_forsaken_fortification_list.h"
#include "iclientmode.h"
#include <vgui_controls/AnimationController.h>

#include <tier0/memdbgon.h>

using namespace vgui;

// Constructor & Deconstructor
CForsakenUpgradeMap::CForsakenUpgradeMap(vgui::Panel *pParent, const char *czName, 
	const char *czText) : BaseClass(pParent, czName, czText)
{
	m_czCurrentMapName = NULL;
	m_pMapImage = NULL;

	Init();
}

CForsakenUpgradeMap::~CForsakenUpgradeMap()
{
}
void CForsakenUpgradeMap::Update()
/*
	
	Pre: 
	Post: 
*/
{
	UpdateObjectiveButtons();
}
void CForsakenUpgradeMap::UpdateObjectiveButtons()
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());

	// Update each objective button
	// Indicies are assumed to still match up from when they were created!!!!
	for(int butIndex = 0; butIndex < m_vecObjectiveButtons.Count(); butIndex++)
	{
		// Assume no upgrade priviledge
		bool enabled = false;

		// Set default HUD coloring
		Color renderColor(m_ObjectiveNormalColor);

		int nObjectiveTextureIndex = 0; // Started at 0 for easy use of offsets to determine index

		// Empty textures
		vgui::IImage* normal = m_textureObjectives[OFFSET_EMPTY];
		vgui::IImage* selected = m_textureObjectives[OFFSET_EMPTY];

		// What type objective is this?
		if (m_vecObjectives[butIndex].nLocationType == LOCATION_TYPE_SPAWN)
		{
			// Spawn points can't be upgraded.
			enabled = false;
		}
		else if (m_vecObjectives[butIndex].nLocationType == LOCATION_TYPE_OBJECTIVE)
		{
			C_ForsakenObjectiveEntity *pObjective = NULL;

			// Find the matching objective in the globals list
			for (int objIndex = 0; objIndex < g_pObjectiveList->GetObjectiveCount(); objIndex++)
			{
				C_ForsakenObjectiveEntity *pObjectiveTemp = g_pObjectiveList->GetObjective(objIndex);

				if ( Q_strcmp(m_vecObjectives[butIndex].czObjectiveName, pObjectiveTemp->GetObjectiveName()) == 0)
				{
					pObjective = pObjectiveTemp;
					break;
				}
			}

			// Something went wrong
			if (!pObjective) continue;

			C_ForsakenCaptureObjectiveEntity *pCapObjective = 
				static_cast<C_ForsakenCaptureObjectiveEntity *>(pObjective);
			C_ForsakenFortifiedObjectiveEntity *pFortification = NULL;
			
			// Get the matching fortified entity.
			for (int fortIndex = 0; fortIndex < g_pFortificationList->GetFortificationCount(); fortIndex++)
			{
				C_ForsakenFortifiedObjectiveEntity *pFortificationTemp = g_pFortificationList->GetFortification(fortIndex);
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
				if (pFortification->GetFortificationStatus() == FORSAKEN_FORTIFIEDSTATUS_COMPLETE ||
					pFortification->GetFortificationStatus() == FORSAKEN_FORTIFIEDSTATUS_UPGRADING)
				{
					nObjectiveTextureIndex += OFFSET_SECURED;
				}

				// If we own it and there is a tent and its not at max LVL, we can upgrade it.
				if (pFortification->GetFortificationStatus() == FORSAKEN_FORTIFIEDSTATUS_COMPLETE &&
					pPlayer->GetTeamNumber() == pObjective->GetTeamOwner() &&
					pFortification->GetFortificationLevel() < FORSAKEN_FORTIFIEDLEVEL_THREE)
				{
						enabled = true;
				}	
			}

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
		}

		// Finally, set the textures.
		normal = m_textureObjectives[nObjectiveTextureIndex];
		selected = m_textureObjectives[nObjectiveTextureIndex];

		if (nObjectiveTextureIndex < OFFSET_EMPTY)
		{
			selected = m_textureObjectives[nObjectiveTextureIndex + OFFSET_SELECTED];
		}

		// Adjust alpha for pulsing
		if (renderColor != m_ObjectiveNormalColor)
		{
			renderColor[3] = 255 * ( 0.5f + 0.5f * ( cos( gpGlobals->curtime*10 ) ) );
		}

		// Create message for radio image to send to leaderoptions panel
		KeyValues *msg = new KeyValues("RadioImageSelected");

		msg->SetString("objectiveName", m_vecObjectiveButtons[butIndex]->GetName());
		msg->SetInt("selectionType", SELECT_TYPE_UPGRADE);

		m_vecObjectiveButtons[butIndex]->SetActionSignalMessage(msg);	// RadioImage handles the deallocation
		m_vecObjectiveButtons[butIndex]->SetAnimateColor(renderColor);
		m_vecObjectiveButtons[butIndex]->SetEnabled(enabled);
		m_vecObjectiveButtons[butIndex]->SetNormalImage(normal);
		m_vecObjectiveButtons[butIndex]->SetSelectedImage(selected);
	}
}