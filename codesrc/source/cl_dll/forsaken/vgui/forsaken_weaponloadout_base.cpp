/*
	forsaken_weaponloadout.cpp
	Forsaken weapon loadout panel.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include "forsaken_weaponloadout_base.h"
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/ImageList.h>
#include <filesystem.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/HTML.h>
#include "IGameUIFuncs.h"
#include "igameresources.h"
#include <cl_dll/iviewport.h>
#include <stdlib.h>
#include <stdio.h>
#include "forsaken/c_forsaken_player.h"
#include "tier0/memdbgon.h"

using namespace vgui;

// Constructor & Deconstructor
CWeaponLoadout::CWeaponLoadout(IViewPort *pViewport, Panel *parent, const char *panelName, 
	bool showTaskbarIcon) : vgui::Frame(parent, panelName, showTaskbarIcon)
{
	m_nLeaderPackCount = 0;
	m_nSelectedWeaponPack = -1;
	m_nSharedResourcesFlag = 0;
	m_pViewport = pViewport;
}

CWeaponLoadout::~CWeaponLoadout()
{
}

// Functions
const char *CWeaponLoadout::GetKitName(const char *czLabel)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenButtonImage *pEntry = NULL;

	// Fetch the entry.
	pEntry = dynamic_cast<CForsakenButtonImage*>(FindChildByName(czLabel));

	if (pEntry)
		return pEntry->GetKitName();

	return "none";
}

int CWeaponLoadout::GetResourceValue(const char *czLabel)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenButtonImageNumeric *pEntry = dynamic_cast<CForsakenButtonImageNumeric*>(FindChildByName(czLabel));

	if (pEntry)
		return pEntry->GetCurrentValue();

	return 0;
}

vgui::Panel *CWeaponLoadout::CreateControlByName(const char *czControlName)
/*
	
	Pre: 
	Post: 
*/
{
	if (Q_stricmp("ButtonImage", czControlName) == 0)
		return new CForsakenButtonImage(this, "ButtonImage", NULL);
	else if (Q_stricmp("ButtonImageNumeric", czControlName) == 0)
		return new CForsakenButtonImageNumeric(this, "ButtonImageNumeric", NULL);
	else
		return BaseClass::CreateControlByName(czControlName);
}

void CWeaponLoadout::CheckPack()
/*
	
	Pre: 
	Post: 
*/
{
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());

	// If we aren't a leader and have a leader pack selected...
	if (!pPlayer->IsLeader() && m_nSelectedWeaponPack <= m_nLeaderPackCount && m_nSelectedWeaponPack != -1)
	{
		char czLeaderWeapon[128] = "";

		// De-select it.
		Q_snprintf(czLeaderWeapon, sizeof(czLeaderWeapon), "leader_weap%d", 
			m_nSelectedWeaponPack);
		SetWeaponSelected(czLeaderWeapon, false);

		// And select the closest standard pack.
		m_nSelectedWeaponPack = m_nLeaderPackCount + 1;

		Q_snprintf(czLeaderWeapon, sizeof(czLeaderWeapon), "standard_weap%d", 
			m_nSelectedWeaponPack - m_nLeaderPackCount);
		SetWeaponSelected(czLeaderWeapon, true);
	}
	else if (m_nSelectedWeaponPack == -1)
	{
		// If we don't have a weapon pack selected at all, select the first available weapon pack.
		if (pPlayer->IsLeader())
		{
			m_nSelectedWeaponPack = 1;

			SetWeaponSelected("leader_weap1", true);
		}
		else
		{
			m_nSelectedWeaponPack = m_nLeaderPackCount + 1;

			SetWeaponSelected("standard_weap1", true);
		}
	}
}

void CWeaponLoadout::DoLoadout()
/*
	
	Pre: 
	Post: 
*/
{
	char czLoadoutCommand[256] = "";
	char czLabel[256] = "";
	const char *czFaction;

	switch (CBasePlayer::GetLocalPlayer()->GetTeamNumber())
	{
	case TEAM_CIVILIANS:
		czFaction = "civ";
		break;

	case TEAM_GOVERNORS:
		czFaction = "gov";
		break;

	case TEAM_RELIGIOUS:
		czFaction = "rel";
		break;

	default:
		return;
	}

	// Are we using a leader or standard weapon? Determine the kit name we need to fetch.
	if (m_nSelectedWeaponPack <= m_nLeaderPackCount)
		Q_snprintf(czLabel, sizeof(czLabel), "leader_weap%d", m_nSelectedWeaponPack);
	else
		Q_snprintf(czLabel, sizeof(czLabel), "standard_weap%d", m_nSelectedWeaponPack - m_nLeaderPackCount);

	// Perform the weapon kit loadout first.
	Q_snprintf(czLoadoutCommand, sizeof(czLoadoutCommand), "weapon_kit_%s_%s", 
		czFaction, GetKitName(czLabel));

	// Fetch that kit.
	engine->ClientCmd(czLoadoutCommand);

	// Loop through all resources.
	for (int i = 1; ; i++)
	{
		CForsakenButtonImageNumeric *pEntry = NULL;

		// Fetch the buttonimagenumeric first.
		Q_snprintf(czLabel, sizeof(czLabel), "shared_weap%d", i);

		pEntry = dynamic_cast<CForsakenButtonImageNumeric*>(FindChildByName(czLabel));

		// Is the resource being bought?
		if (pEntry)
		{
			Q_snprintf(czLoadoutCommand, sizeof(czLoadoutCommand), "weapon_resource_%s %d", 
				GetKitName(czLabel), pEntry->GetCurrentValue());

			// Reset the resource count.
			pEntry->ResetValue();

			// Fetch that resource.
			engine->ClientCmd(czLoadoutCommand);
		}
		else
			break;
	}
}

void CWeaponLoadout::OnCommand(const char *czCommand)
/*
	
	Pre: 
	Post: 
*/
{
	if (Q_stricmp(czCommand, "vguiok") == 0)
	{
		DoLoadout();

		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);

		// Forsaken Addition: We don't want the panel showing back up again.
		m_pViewport->ShowPanel(this, false);
	}
	else if (Q_stricmp(czCommand, "vguicancel") == 0)
	{
		engine->ClientCmd(const_cast<char*>(czCommand));

		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);

		// Forsaken Addition: We don't want the panel showing back up again.
		m_pViewport->ShowPanel(this, false);
	}
	else if (Q_strnicmp("standard_weap", czCommand, 13) == 0)
	{
		char czWeapon[128] = "";
		int nWeaponID = 0;

		sscanf(czCommand, "standard_weap %d", &nWeaponID);

		if (m_nSelectedWeaponPack == -1)
		{
			Q_snprintf(czWeapon, sizeof(czWeapon), "standard_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedWeaponPack = nWeaponID + m_nLeaderPackCount;
		}
		else
		{
			if (m_nSelectedWeaponPack > m_nLeaderPackCount)
				Q_snprintf(czWeapon, sizeof(czWeapon), "standard_weap%d", 
				m_nSelectedWeaponPack - m_nLeaderPackCount);
			else
				Q_snprintf(czWeapon, sizeof(czWeapon), "leader_weap%d", 
				m_nSelectedWeaponPack);

			SetWeaponSelected(czWeapon, false);

			Q_snprintf(czWeapon, sizeof(czWeapon), "standard_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedWeaponPack = nWeaponID + m_nLeaderPackCount;
		}
	}
	else if (Q_strnicmp("leader_weap", czCommand, 11) == 0)
	{
		char czWeapon[128] = "";
		int nWeaponID = 0;

		sscanf(czCommand, "leader_weap %d", &nWeaponID);

		if (m_nSelectedWeaponPack == -1)
		{
			Q_snprintf(czWeapon, sizeof(czWeapon), "leader_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedWeaponPack = nWeaponID;
		}
		else
		{
			if (m_nSelectedWeaponPack > m_nLeaderPackCount)
				Q_snprintf(czWeapon, sizeof(czWeapon), "standard_weap%d", 
				m_nSelectedWeaponPack - m_nLeaderPackCount);
			else
				Q_snprintf(czWeapon, sizeof(czWeapon), "leader_weap%d", 
				m_nSelectedWeaponPack);

			SetWeaponSelected(czWeapon, false);

			Q_snprintf(czWeapon, sizeof(czWeapon), "leader_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedWeaponPack = nWeaponID;
		}
	}
	else if (Q_strnicmp("shared_weap", czCommand, 11) == 0)
	{
		CForsakenButtonImageNumeric *pEntry = NULL;
		char czWeapon[128] = "";
		int nWeaponID = 0;
		sscanf(czCommand, "shared_weap %d", &nWeaponID);
		Q_snprintf(czWeapon, sizeof(czWeapon), "shared_weap%d", nWeaponID);

		// Fetch the entry.
		pEntry = dynamic_cast<CForsakenButtonImageNumeric*>(FindChildByName(czWeapon));

		if (pEntry)
		{
			// Did we increase or decrease the resource?
			if (pEntry->GetLastMouseEvent() == MOUSE_LEFT)
			{
				// Do we have enough resources to buy it?
				if ((m_nSharedResourcesRemaining - pEntry->GetCost()) < 0)
				{
					// If not, decrease the value and be on our way.
					pEntry->SetCurrentValue(pEntry->GetCurrentValue() - 1);
				}
				else
				{
					// Decrease how many resources we have.
					m_nSharedResourcesRemaining -= pEntry->GetCost();
				}
			}
			else if (pEntry->GetLastMouseEvent() == MOUSE_RIGHT)
			{
				m_nSharedResourcesRemaining += pEntry->GetCost();

				// Cap ourselves at the maximum number of resources.
				if (m_nSharedResourcesRemaining > m_nSharedResourcesAmt)
					m_nSharedResourcesRemaining = m_nSharedResourcesAmt;
			}
		}

		// Update resource totals.
		UpdateResourceLabel();
	}
}

void CWeaponLoadout::SetLabelText(const char *czLabel, const char *czText)
/*
	
	Pre: 
	Post: 
*/
{
	Label *pEntry = dynamic_cast<Label*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetText(czText);
}

void CWeaponLoadout::SetPlayerImage(const char *czImageName)
/*
	
	Pre: 
	Post: 
*/
{
	ImagePanel *pEntry = dynamic_cast<ImagePanel*>(FindChildByName("player_image"));

	if (pEntry)
		pEntry->SetImage(czImageName);
}

void CWeaponLoadout::SetWeaponEnabled(const char *czLabel, bool bEnabled)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenButtonImage *pEntry = dynamic_cast<CForsakenButtonImage*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetEnabled(bEnabled);
}

void CWeaponLoadout::SetWeaponSelected(const char *czLabel, bool bSelected)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenButtonImage *pEntry = dynamic_cast<CForsakenButtonImage*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetActive(bSelected);
}

void CWeaponLoadout::UpdateResourceLabel()
/*
	
	Pre: 
	Post: 
*/
{
	char czResourceAmt[128] = "";

	// Set the player resources.
	Q_snprintf(czResourceAmt, sizeof(czResourceAmt), "%d/%d", m_nSharedResourcesRemaining, 
		m_nSharedResourcesAmt);

	// Update the label.
	SetLabelText("resource_amount", czResourceAmt);
}