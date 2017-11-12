/*
	forsaken_weaponloadout.cpp
	Forsaken weapon loadout panel.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include "forsaken_weaponloadout.h"
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
CWeaponLoadout::CWeaponLoadout(IViewPort *pViewport) : Frame(NULL, PANEL_WEAPLOADOUT)
{
	m_nSelectedLeaderWeapon = -1;
	m_nSelectedStandardWeapon = -1;
	m_nLastTeam = -1;
	m_pViewport = pViewport;

	// Initial values.
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);
}

CWeaponLoadout::~CWeaponLoadout()
{
}

// Functions
void CWeaponLoadout::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CWeaponLoadout::ShowPanel(bool bState)
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
		char czRankTitle[128] = "";

		if (!pPlayer->IsAlive())
			return;

		if (m_nLastTeam != pPlayer->GetTeamNumber())
		{
			m_nLastTeam = pPlayer->GetTeamNumber();

			switch (m_nLastTeam)
			{
			case TEAM_CIVILIANS:
				if (pPlayer->IsLeader())
					Q_snprintf(czRankTitle, sizeof(czRankTitle), "CIV LEADER");
				else
					Q_snprintf(czRankTitle, sizeof(czRankTitle), "CIV SCOUT");

				LoadControlSettings("Resource/UI/WeaponLoadout_Civ.res");

				break;

			case TEAM_GOVERNORS:
				if (pPlayer->IsLeader())
					Q_snprintf(czRankTitle, sizeof(czRankTitle), "GOV LEADER");
				else
					Q_snprintf(czRankTitle, sizeof(czRankTitle), "GOV SOLDIER");

				LoadControlSettings("Resource/UI/WeaponLoadout_Gov.res");

				break;

			case TEAM_RELIGIOUS:
				if (pPlayer->IsLeader())
					Q_snprintf(czRankTitle, sizeof(czRankTitle), "REL LEADER");
				else
					Q_snprintf(czRankTitle, sizeof(czRankTitle), "REL FANATIC");

				LoadControlSettings("Resource/UI/WeaponLoadout_Rel.res");

				break;

			default:
				return;
			}
		}

		// Setup the initial look of the dialog.
		InvalidateLayout();
		Repaint();
		Activate();
		SetMouseInputEnabled(true);

		// Set the rank title.
		SetLabelText("rank_title", czRankTitle);

		// Enable/disable leader weapons.
		for (int i = 0; i < 15; i++)
		{
			char czLeaderWeapon[128] = "leader_weap";

			Q_snprintf(czLeaderWeapon, sizeof(czLeaderWeapon), "%s%d", czLeaderWeapon, i);

			if (pPlayer->IsLeader())
				SetWeaponEnabled(czLeaderWeapon, true);
			else
				SetWeaponEnabled(czLeaderWeapon, false);
		}
	}
	else
	{
		SetVisible(false);
		SetMouseInputEnabled(false);
	}

	m_pViewport->ShowBackGround(bState);
}

void CWeaponLoadout::OnCommand(const char *czCommand)
/*
	
	Pre: 
	Post: 
*/
{
	if (Q_stricmp(czCommand, "vguiok"))
	{
		char czLoadoutCommand[256] = "";

		Q_snprintf(czLoadoutCommand, sizeof(czLoadoutCommand), "weapon_loadout %d %d", 
			m_nSelectedLeaderWeapon, m_nSelectedStandardWeapon);

		engine->ClientCmd(czLoadoutCommand);

		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);
	}
	else if (Q_stricmp(czCommand, "vguicancel"))
	{
		engine->ClientCmd(const_cast<char*>(czCommand));

		Close();
		gViewPortInterface->ShowBackGround(false);

		BaseClass::OnCommand(czCommand);
	}
	else if (Q_strnicmp("standard_weap", czCommand, 13) == 0)
	{
		char czWeapon[128] = "";
		int nWeaponID = 0;

		sscanf(czCommand, "standard_weap %d", &nWeaponID);

		if (m_nSelectedStandardWeapon == -1)
		{
			Q_snprintf(czWeapon, sizeof(czWeapon), "standard_weap%d", m_nSelectedStandardWeapon);
			SetWeaponSelected(czWeapon, false);

			Q_snprintf(czWeapon, sizeof(czWeapon), "standard_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedStandardWeapon = nWeaponID;
		}
		else
		{
			Q_snprintf(czWeapon, sizeof(czWeapon), "standard_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedStandardWeapon = nWeaponID;
		}
	}
	else if (Q_strnicmp("leader_weap", czCommand, 11) == 0)
	{
		char czWeapon[128] = "";
		int nWeaponID = 0;

		sscanf(czCommand, "leader_weap %d", &nWeaponID);

		if (m_nSelectedLeaderWeapon == -1)
		{
			Q_snprintf(czWeapon, sizeof(czWeapon), "leader_weap%d", m_nSelectedLeaderWeapon);
			SetWeaponSelected(czWeapon, false);

			Q_snprintf(czWeapon, sizeof(czWeapon), "leader_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedLeaderWeapon = nWeaponID;
		}
		else
		{
			Q_snprintf(czWeapon, sizeof(czWeapon), "leader_weap%d", nWeaponID);
			SetWeaponSelected(czWeapon, true);

			m_nSelectedLeaderWeapon = nWeaponID;
		}
	}
	else if (Q_strnicmp("shared_weap", czCommand, 11) == 0)
	{
		ButtonImage *pEntry = NULL;
		char czWeapon[128] = "";
		int nWeaponID = 0;

		sscanf(czCommand, "shared_weap %d", &nWeaponID);
		Q_snprintf(czWeapon, sizeof(czWeapon), "shared_weap%d", nWeaponID);

		pEntry = dynamic_cast<ButtonImage*>(FindChildByName(czWeapon));

		if (pEntry)
			pEntry->SetSelected(!pEntry->IsSelected());
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

void CWeaponLoadout::SetWeaponEnabled(const char *czLabel, bool bEnabled)
/*
	
	Pre: 
	Post: 
*/
{
	ButtonImage *pEntry = dynamic_cast<ButtonImage*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetEnabled(bEnabled);
}

void CWeaponLoadout::SetWeaponSelected(const char *czLabel, bool bSelected)
/*
	
	Pre: 
	Post: 
*/
{
	ButtonImage *pEntry = dynamic_cast<ButtonImage*>(FindChildByName(czLabel));

	if (pEntry)
		pEntry->SetSelected(bSelected);
}

void CWeaponLoadout::Update()
/*
	
	Pre: 
	Post: 
*/
{
	char czTeamRank[64] = "";

	// Team prefix
	switch (m_nLastTeam)
	{
	case TEAM_CIVILIANS:
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "Civilian");
		break;

	case TEAM_GOVERNORS:
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "Governor");
		break;

	case TEAM_RELIGIOUS:
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "Religious");
		break;

	default:
		return;
	}

	// Position postfix
	if (ToForsakenPlayer(C_BasePlayer::GetLocalPlayer())->IsLeader())
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "%s Leader", czTeamRank);
	else
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "%s Soldier", czTeamRank);

	SetLabelText("team_rank", "");
}