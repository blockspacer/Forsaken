/*
	forsaken_leaderoptions.cpp
	Forsaken panel for leader options.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertySheet.h>
#include <cl_dll/iviewport.h>
#include "forsaken/vgui/controls/forsaken_ctrl_objectivemap.h"
#include "forsaken/vgui/controls/forsaken_ctrl_spawnmap.h"
#include "forsaken/vgui/controls/forsaken_ctrl_upgrademap.h"
#include "forsaken_leaderoptions.h"
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <filesystem.h>
#include "IGameUIFuncs.h"
#include "igameresources.h"
#include <cl_dll/iviewport.h>
#include <stdlib.h>
#include <stdio.h>
#include "forsaken/c_forsaken_player.h"
#include "forsaken/c_forsaken_team.h"
#include "forsaken/c_forsaken_fortified_entity.h"
#include "forsaken/c_forsaken_fortification_list.h"
#include "forsaken/c_forsaken_objective_entity.h"
#include "forsaken/c_forsaken_objective_list.h"
#include "tier0/memdbgon.h"

// Global Variable Declerations
static ConVar cl_leader_preference("cl_fskn_leaderpreference", "2", FCVAR_USERINFO | FCVAR_ARCHIVE, 
	"Sets your preference on being leader. 0 = Yes, 1 = No, 2 = Don't Care");

using namespace vgui;

// Constructor & Deconstructor
CLeaderOptionsPage::CLeaderOptionsPage(vgui::Panel *pParent, const char *czPanelName) : 
	vgui::PropertyPage(pParent, czPanelName)
{
}

CLeaderOptionsPage::~CLeaderOptionsPage()
{
}

CLeaderOptions::CLeaderOptions(IViewPort *pViewport) : Frame(NULL, PANEL_LEADEROPTIONS)
{
	// Initial values.
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);

	// Create our tabs.
	m_pTabSheet = new PropertySheet(this, "Leader_Sheet");

	// Create our pages.
	// Options
	m_pOptionsTab = new CLeaderOptionsPage(this, "Options_Tab");
	m_pOptionsTab->SetScheme("ClientScheme");
	m_pOptionsTab->SetProportional(false);

	// Spawn
	m_pSpawnTab = new CLeaderOptionsPage(this, "Spawn_Tab");
	m_pSpawnTab->SetScheme("ClientScheme");
	m_pSpawnTab->SetProportional(false);

	// Upgrades
	m_pUpgradeTab = new CLeaderOptionsPage(this, "Upgrade_Tab");
	m_pUpgradeTab->SetScheme("ClientScheme");
	m_pUpgradeTab->SetProportional(false);

	// Add the tabs.
	m_pTabSheet->AddPage(m_pOptionsTab, "Options");
	m_pTabSheet->AddPage(m_pSpawnTab, "Select Spawn");
	m_pTabSheet->AddPage(m_pUpgradeTab, "Upgrade Fortification");
	m_pTabSheet->SetActivePage(m_pUpgradeTab);
	m_pTabSheet->SetEnabled(true);
	m_pTabSheet->SetVisible(true);

	// Create our buttons.
	m_pOK = new CForsakenButtonImage(this, "OKButton", "OK");
	m_pCancel = new CForsakenButtonImage(this, "CancelButton", "Cancel2");

	// Create our objective maps.
	m_pUpgradeObjectiveMap = new CForsakenUpgradeMap(m_pUpgradeTab, "upgrade_map", "");
	m_pSpawnObjectiveMap = new CForsakenSpawnMap(m_pSpawnTab, "spawn_map", "");

	// Create our upgrade buttons.
	for (int i = 1; i < UPGRADE_OBJECTIVE_MAX; i++)
	{
		char czUpgradeLabel[128] = "upgradeBtn_lvl";

		// Determine the name of the button.
		Q_snprintf(czUpgradeLabel, sizeof(czUpgradeLabel), "%s%d", czUpgradeLabel, i);

		// Create the upgrade button.
		m_pUpgradeObjectives[i-1] = new CForsakenButtonImage(m_pUpgradeTab, czUpgradeLabel, "");
	}

	// Load our controls.
	LoadControlSettings("Resource/UI/LeaderOptions.res");
	m_pOptionsTab->LoadControlSettings("Resource/UI/LeaderOptions_OptionsPage.res");
	m_pSpawnTab->LoadControlSettings("Resource/UI/LeaderOptions_SpawnPage.res");
	m_pUpgradeTab->LoadControlSettings("Resource/UI/LeaderOptions_UpgradePage.res");

	// Bring the controls up-top.
	m_pTabSheet->MoveToFront();
	m_pOK->MoveToFront();
	m_pCancel->MoveToFront();

	// Update the sizing.
	UpdatePropertySheetSize();

	// Save the viewport.
	m_pViewport = pViewport;

	m_czLastSelectedSpawn[0] = m_czLastSelectedUpgrade[0] = m_czSelectedSpawn[0] = 
		m_czSelectedUpgrade[0] = '\0';
	m_nSpawnLocationType = LOCATION_TYPE_UNKNOWN;
	m_nUpgradeLevel = 0;
}

CLeaderOptions::~CLeaderOptions()
{
}

// Functions
// CLeaderOptionsPage
vgui::Panel *CLeaderOptionsPage::CreateControlByName(const char *czControlName)
{
	if (Q_stricmp("ButtonImage", czControlName) == 0)
		return new CForsakenButtonImage(this, "ButtonImage", NULL);
	else
		return BaseClass::CreateControlByName(czControlName);
}

// CLeaderOptions
bool CLeaderOptions::IsRadioSelected(const char *czRadioName)
/*
	
	Pre: 
	Post: 
*/
{
	RadioButton *pEntry = dynamic_cast<RadioButton*>(FindChildByName(czRadioName, true));

	if (pEntry)
		return pEntry->IsSelected();

	return false;
}

vgui::Panel *CLeaderOptions::CreateControlByName(const char *czControlName)
/*
	
	Pre: 
	Post: 
*/
{
	if (Q_stricmp("ButtonImage", czControlName) == 0)
		return new CForsakenButtonImage(this, "ButtonImage", NULL);
	else
		return BaseClass::CreateControlByName(czControlName);
}

void CLeaderOptions::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CLeaderOptions::PerformLayout()
/*
	
	Pre: 
	Post: 
*/
{
	UpdatePropertySheetSize();

	BaseClass::PerformLayout();

	/*
		OMG NASTY HACK FUCK VALVE!!!

		So we were running into a issue where ButtonImage's for the upgrade buttons were getting improperly resized. This 
		nasty little hack hard-codes the values and fixes this. The REAL issue would be to fix it, but the fuckwads at Valve 
		in their infinite wisdom don't truly fix issues.
		*/
	m_pUpgradeObjectives[0]->SetPos(vgui::scheme()->GetProportionalScaledValue(300), vgui::scheme()->GetProportionalScaledValue(150));
	m_pUpgradeObjectives[0]->SetWide(vgui::scheme()->GetProportionalScaledValue(48));
	m_pUpgradeObjectives[0]->SetTall(vgui::scheme()->GetProportionalScaledValue(48));
	m_pUpgradeObjectives[0]->MoveToFront();
	m_pUpgradeObjectives[1]->SetPos(vgui::scheme()->GetProportionalScaledValue(350), vgui::scheme()->GetProportionalScaledValue(150));
	m_pUpgradeObjectives[1]->SetWide(vgui::scheme()->GetProportionalScaledValue(48));
	m_pUpgradeObjectives[1]->SetTall(vgui::scheme()->GetProportionalScaledValue(48));
	m_pUpgradeObjectives[1]->MoveToFront();
	m_pUpgradeObjectives[2]->SetPos(vgui::scheme()->GetProportionalScaledValue(400), vgui::scheme()->GetProportionalScaledValue(150));
	m_pUpgradeObjectives[2]->SetWide(vgui::scheme()->GetProportionalScaledValue(48));
	m_pUpgradeObjectives[2]->SetTall(vgui::scheme()->GetProportionalScaledValue(48));
	m_pUpgradeObjectives[2]->MoveToFront();
}

void CLeaderOptions::OnCommand(const char *czCommand)
/*
	
	Pre: 
	Post: 
*/
{
	if (Q_stricmp(czCommand, "upgrade_1") == 0)
	{
		// We want to upgrade to level 1.
		m_nUpgradeLevel = 1;
		SetUpgradeSelected("upgradeBtn_lvl1", true);
		SetUpgradeSelected("upgradeBtn_lvl2", false);
		SetUpgradeSelected("upgradeBtn_lvl3", false);
	}
	else if (Q_stricmp(czCommand, "upgrade_2") == 0)
	{
		// We want to upgrade to level 2.
		m_nUpgradeLevel = 2;
		SetUpgradeSelected("upgradeBtn_lvl1", false);
		SetUpgradeSelected("upgradeBtn_lvl2", true);
		SetUpgradeSelected("upgradeBtn_lvl3", false);
	}
	else if (Q_stricmp(czCommand, "upgrade_3") == 0)
	{
		// We want to upgrade to level 3.
		m_nUpgradeLevel = 3;
		SetUpgradeSelected("upgradeBtn_lvl1", false);
		SetUpgradeSelected("upgradeBtn_lvl2", false);
		SetUpgradeSelected("upgradeBtn_lvl3", true);
	}
	else if (Q_stricmp(czCommand, "vguiok") == 0)
	{
		int nLeaderPreference = LEADEROPTION_UNKNOWN;

		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);

		// Forsaken Addition: We don't want the panel showing back up again.
		m_pViewport->ShowPanel(this, false);

		// Determine which radio box is selected for leader preferences.
		if (IsRadioSelected("yesLeader"))
			nLeaderPreference = LEADEROPTION_YES;
		else if (IsRadioSelected("noLeader"))
			nLeaderPreference = LEADEROPTION_NO;
		else if (IsRadioSelected("maybeLeader"))
			nLeaderPreference = LEADEROPTION_MAYBE;

		// Check to see if we've selected a different spawn.
		if (Q_stricmp(m_czLastSelectedSpawn, m_czSelectedSpawn) != 0)
		{
			char czSpawnCommand[512] = "fskn_leader_choosespawn";

			// Save our spawn.
			Q_strncpy(m_czLastSelectedSpawn, m_czSelectedSpawn, sizeof(m_czLastSelectedSpawn));

			// Setup the command.
			if (m_nSpawnLocationType == LOCATION_TYPE_SPAWN)
				Q_snprintf(czSpawnCommand, sizeof (czSpawnCommand), "%s \"-1\"", czSpawnCommand);
			else
				Q_snprintf(czSpawnCommand, sizeof (czSpawnCommand), "%s \"%s\"", czSpawnCommand, m_czSelectedSpawn);

			// Execute the command.
			engine->ClientCmd(czSpawnCommand);
		}

		// Check to see if we've selected a area for upgrade.
		if (Q_stricmp(m_czLastSelectedUpgrade, m_czSelectedUpgrade) != 0)
		{
			char czUpgradeCommand[512] = "fskn_leader_upgradefortification";

			// Save our upgrade.
			Q_strncpy(m_czLastSelectedUpgrade, m_czSelectedUpgrade, sizeof(m_czLastSelectedUpgrade));

			// Setup the command.
			Q_snprintf(czUpgradeCommand, sizeof(czUpgradeCommand), "%s \"%s\" %d", czUpgradeCommand, m_czSelectedUpgrade, 
				m_nUpgradeLevel);

			// Execute the command if we want to upgrade.
			if (m_nUpgradeLevel > 0)
				engine->ClientCmd(czUpgradeCommand);
		}

		// If it's changed, update our preference and notify the server.
		if (cl_leader_preference.GetInt() != nLeaderPreference)
		{
			char czLeaderPrefCommand[256] = "fskn_leaderchoice";

			// Save our preference.
			cl_leader_preference.SetValue(nLeaderPreference);

			// Setup the command.
			Q_snprintf(czLeaderPrefCommand, sizeof(czLeaderPrefCommand), "%s %d", 
				czLeaderPrefCommand, nLeaderPreference);

			// Execute the command.
			engine->ClientCmd(czLeaderPrefCommand);
		}
	}
	else if (Q_stricmp(czCommand, "vguicancel") == 0)
	{
		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);

		// Forsaken Addition: We don't want the panel showing back up again.
		m_pViewport->ShowPanel(this, false);
	}
}

void CLeaderOptions::SelectRadio(const char *czRadioName)
/*
	
	Pre: 
	Post: 
*/
{
	RadioButton *pEntry = dynamic_cast<RadioButton*>(FindChildByName(czRadioName, true));

	if (pEntry)
		pEntry->SetSelected(true);
}

void CLeaderOptions::SetData(KeyValues *pData)
/*
	
	Pre: 
	Post: 
*/
{
	Update();
}

void CLeaderOptions::SetImage(const char *czImagePanel, const char *czPath)
/*
	
	Pre: 
	Post: 
*/
{
	ImagePanel *pEntry = dynamic_cast<ImagePanel*>(FindChildByName(czImagePanel, true));

	if (pEntry)
		pEntry->SetImage(czPath);
}

void CLeaderOptions::SetLabelText(const char *czLabel, const char *czText)
/*
	
	Pre: 
	Post: 
*/
{
	Label *pEntry = dynamic_cast<Label*>(FindChildByName(czLabel, true));

	if (pEntry)
		pEntry->SetText(czText);
}

void CLeaderOptions::SetUpgradeEnabled(const char *czLabel, bool bEnabled)
/*

Pre: 
Post: 
*/
{
	CForsakenButtonImage *pEntry = dynamic_cast<CForsakenButtonImage*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetEnabled(bEnabled);
}

void CLeaderOptions::SetUpgradeSelected(const char *czLabel, bool bSelected)
/*

Pre: 
Post: 
*/
{
	CForsakenButtonImage *pEntry = dynamic_cast<CForsakenButtonImage*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetActive(bSelected);
}

void CLeaderOptions::ShowPanel(bool bState)
/*
	
	Pre: 
	Post: 
*/
{
	if (BaseClass::IsVisible() == bState)
		return;

	if (bState)
	{
		C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());

		if (pPlayer)
		{
			char czResources[128] = "";
			C_ForsakenTeam *pTeam = dynamic_cast<C_ForsakenTeam*>(pPlayer->GetTeam());
			// Disable/enable pages depending on leader status.
			/*if (pPlayer->IsLeader())
			{*/
				m_pTabSheet->EnablePage("Select Spawn");
				m_pTabSheet->EnablePage("Upgrade Fortification");
			/*}
			else
			{
				m_pTabSheet->DisablePage("Select Spawn");
				m_pTabSheet->DisablePage("Upgrade Fortification");
			}*/

			// Display the number of resources the team has.
			Q_snprintf(czResources, sizeof(czResources), "%d", pTeam->GetOrdinanceResources());
			SetLabelText("upgrade_faction_resources", czResources);

			// We don't want to upgrade!
			m_nUpgradeLevel = 0;

			// Select the 'I don't care' option by default.
			switch (cl_leader_preference.GetInt())
			{
			case LEADEROPTION_YES:
				SelectRadio("yesLeader");
				break;

			case LEADEROPTION_NO:
				SelectRadio("noLeader");
				break;

			case LEADEROPTION_MAYBE:
				SelectRadio("maybeLeader");
				break;
			}
		}

		// Always start at the options tab first.
		m_pTabSheet->SetActivePage(m_pOptionsTab);

		// Setup the initial look of the dialog.
		Activate();
		SetMouseInputEnabled(true);
	}
	else
	{
		SetVisible(false);
		SetMouseInputEnabled(false);
	}

	m_pViewport->ShowBackGround(bState);
}

void CLeaderOptions::Update()
/*
	
	Pre: 
	Post: 
*/
{
	m_pUpgradeObjectiveMap->Update();
	m_pSpawnObjectiveMap->Update();
}

void CLeaderOptions::UpdatePropertySheetSize()
/*
	
	Pre: 
	Post: 
*/
{
	EditablePanel *pSizeCtrl = dynamic_cast<EditablePanel*>(FindChildByName("Leader_Sheet_Sizing", true));

	if (pSizeCtrl)
	{
		int nX, nY, nWide, nTall;

		// Obtain the dimensions.
		pSizeCtrl->GetSize(nWide, nTall);
		pSizeCtrl->GetPos(nX, nY);

		// Set the dimensions.
		m_pTabSheet->SetSize(nWide, nTall);
		m_pTabSheet->SetPos(nX, nY);

		// Tell it to do a layout.
		m_pTabSheet->InvalidateLayout();
	}
}

void CLeaderOptions::OnRadioImageSelected(KeyValues *pData)
/*
	
	Pre: 
	Post: 
*/
{
	bool bIsSpawn = true;
	char czObjectiveStatus[256] = "";
	char czObjectiveType[256] = "";
	const char *czObjectiveName = pData->GetString("objectiveName", "");
	int nLocationType = pData->GetInt("locationType", LOCATION_TYPE_UNKNOWN);
	int nSelectType = pData->GetInt("selectionType", SELECT_TYPE_UNKNOWN);

	// Did we select a spawn or an upgrade?
	if (nSelectType == SELECT_TYPE_SPAWN)
	{
		m_nSpawnLocationType = nLocationType;
		Q_strncpy(m_czSelectedSpawn, czObjectiveName, sizeof(m_czSelectedSpawn));

		SetLabelText("spawn_objective_name", m_czSelectedSpawn);

		// Find the objective.
		for (int i = 0; i < g_pObjectiveList->GetObjectiveCount(); i++)
		{
			C_ForsakenObjectiveEntity *pObjective = g_pObjectiveList->GetObjective(i);

			// Is this the objective we are looking for?
			if (pObjective && Q_stricmp(pObjective->GetObjectiveName(), czObjectiveName) == 0)
			{
				// If we hit here, we aren't a spawn obviously.
				bIsSpawn = false;

				// What type of objective is this?
				switch (pObjective->GetObjectiveType())
				{
				case OBJECTIVE_TYPE_CAPTURE:
					Q_snprintf(czObjectiveType, sizeof(czObjectiveType), "Capture");
					break;

				case OBJECTIVE_TYPE_DESTROY:
					Q_snprintf(czObjectiveType, sizeof(czObjectiveType), "Destroy");
					break;
				}

				// Update the labels.
				SetLabelText("spawn_objective_type", czObjectiveType);
				SetLabelText("spawn_objective_status", czObjectiveStatus);
			}
		}

		if (bIsSpawn)
			SetLabelText("spawn_objective_type", "Default Spawn");
	}
	else if (nSelectType == SELECT_TYPE_UPGRADE)
	{
		C_ForsakenObjectiveEntity *pFoundObjective = NULL;
		Q_strncpy(m_czSelectedUpgrade, czObjectiveName, sizeof(m_czSelectedUpgrade));

		SetLabelText("upgrade_objective_name", m_czSelectedUpgrade);

		// Find the objective.
		for (int i = 0; i < g_pObjectiveList->GetObjectiveCount(); i++)
		{
			C_ForsakenObjectiveEntity *pObjective = g_pObjectiveList->GetObjective(i);

			// Is this the objective we are looking for?
			if (pObjective && Q_stricmp(pObjective->GetObjectiveName(), czObjectiveName) == 0)
			{
				// What type of objective is this?
				switch (pObjective->GetObjectiveType())
				{
				case OBJECTIVE_TYPE_CAPTURE:
					Q_snprintf(czObjectiveType, sizeof(czObjectiveType), "Capture");
					break;

				case OBJECTIVE_TYPE_DESTROY:
					Q_snprintf(czObjectiveType, sizeof(czObjectiveType), "Destroy");
					break;
				}

				// Update the labels.
				SetLabelText("upgrade_objective_type", czObjectiveType);
				SetLabelText("upgrade_objective_status", czObjectiveStatus);

				pFoundObjective = pObjective;

				break;
			}
		}

		// Find the fortification bound to the objective we selected.
		for (int i = 0; i < g_pFortificationList->GetFortificationCount(); i++)
		{
			C_ForsakenFortifiedObjectiveEntity *pFortification = g_pFortificationList->GetFortification(i);

			// Is this the fortification we are looking for?
			if (pFortification->GetObjectiveEntity() == pFoundObjective)
			{
				int nFortificationLevel = pFortification->GetFortificationLevel();

				// Loop through all of our buttons.
				for (int j = 1; j < UPGRADE_OBJECTIVE_MAX; j++)
				{
					char czUpgradeLabel[128] = "upgradeBtn_lvl";

					// Determine the name of the button.
					Q_snprintf(czUpgradeLabel, sizeof(czUpgradeLabel), "%s%d", czUpgradeLabel, j);

					// We don't want to upgrade this one yet.
					m_nUpgradeLevel = 0;
					SetUpgradeSelected(czUpgradeLabel, false);

					// If we have already upgraded to this level, disable it as an option.
					if (j > nFortificationLevel)
						SetUpgradeEnabled(czUpgradeLabel, true);
					else
						SetUpgradeEnabled(czUpgradeLabel, false);
				}
			}
		}
	}
}