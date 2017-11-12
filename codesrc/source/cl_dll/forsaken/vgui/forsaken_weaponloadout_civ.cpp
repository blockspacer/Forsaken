/*
	forsaken_weaponloadout_civ.cpp
	Forsaken weapon loadout panel for the civilians.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include "forsaken_weaponloadout_base.h"
#include "forsaken_weaponloadout_civ.h"
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
CWeaponLoadout_Civ::CWeaponLoadout_Civ(IViewPort *pViewport) : CWeaponLoadout(pViewport, NULL, PANEL_WEAPLOADOUT_CIV)
{
	// Initial values.
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);
	LoadControlSettings("Resource/UI/WeaponLoadout_Civ.res");
}

CWeaponLoadout_Civ::~CWeaponLoadout_Civ()
{
}

// Functions
void CWeaponLoadout_Civ::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CWeaponLoadout_Civ::ShowPanel(bool bState)
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

		// Determine the rank title.
		if (pPlayer->IsLeader())
		{
			SetPlayerImage("loadouts/civ_leader");
			Q_snprintf(czRankTitle, sizeof(czRankTitle), "CIV LEADER");
		}
		else
		{
			SetPlayerImage("loadouts/civ_scout");
			Q_snprintf(czRankTitle, sizeof(czRankTitle), "CIV SCOUT");
		}

		// Set the resource amount we have.
		m_nSharedResourcesAmt = m_nSharedResourcesRemaining = pPlayer->GetSharedResources();

		// Setup the initial look of the dialog.
		InvalidateLayout();
		Activate();

		// Set the rank title and resource amount..
		SetLabelText("rank_title", czRankTitle);
		UpdateResourceLabel();

		// Reset the weapon leader pack count.
		m_nLeaderPackCount = 0;

		// Enable/disable leader weapons and unpurchaseable resources.
		for (int i = 1; ; i++)
		{
			bool bFound = false;
			CForsakenButtonImageNumeric *pEntry = NULL;
			char czLeaderWeapon[128] = "";
			char czResource[128] = "";

			// Append the current weapon/resource number.
			Q_snprintf(czLeaderWeapon, sizeof(czLeaderWeapon), "leader_weap%d", i);
			Q_snprintf(czResource, sizeof(czResource), "shared_weap%d", i);

			// Does the leader weapon exist?
			if (FindChildByName(czLeaderWeapon))
			{
				bFound = true;

				m_nLeaderPackCount++;

				if (pPlayer->IsLeader())
					SetWeaponEnabled(czLeaderWeapon, true);
				else
					SetWeaponEnabled(czLeaderWeapon, false);
			}

			// Or the resource?
			if ((pEntry = dynamic_cast<CForsakenButtonImageNumeric*>(FindChildByName(czResource))) != NULL)
			{
				bFound = true;

				// If the cost is greater than how many resources we have, disable it... otherwise enable.
				if (pEntry->GetCost() > m_nSharedResourcesAmt)
					pEntry->SetEnabled(false);
				else
					pEntry->SetEnabled(true);
			}

			// If no leader weapon or shared resource exists, stop.
			if (!bFound)
				break;
		}

		// Make sure we already have a valid weapon kit selected.
		CheckPack();
	}

	SetVisible(bState);
	SetMouseInputEnabled(bState);

	m_pViewport->ShowBackGround(bState);
}

void CWeaponLoadout_Civ::Update()
/*
	
	Pre: 
	Post: 
*/
{
	char czTeamRank[64] = "";

	// Position postfix
	if (ToForsakenPlayer(C_BasePlayer::GetLocalPlayer())->IsLeader())
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "Civilian Leader");
	else
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "Civilian Scout");

	SetLabelText("team_rank", "");
}