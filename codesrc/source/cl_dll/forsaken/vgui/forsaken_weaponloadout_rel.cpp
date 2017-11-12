/*
	forsaken_weaponloadout_rel.cpp
	Forsaken weapon loadout panel for the religious.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"

#include <cdll_client_int.h>
#include "forsaken_weaponloadout_base.h"
#include "forsaken_weaponloadout_rel.h"
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
CWeaponLoadout_Rel::CWeaponLoadout_Rel(IViewPort *pViewport) : CWeaponLoadout(pViewport, NULL, PANEL_WEAPLOADOUT_REL)
{
	// Initial values.
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);
	SetTitleBarVisible(false);
	SetProportional(true);
	LoadControlSettings("Resource/UI/WeaponLoadout_Rel.res");
}

CWeaponLoadout_Rel::~CWeaponLoadout_Rel()
{
}

// Functions
void CWeaponLoadout_Rel::ApplySchemeSettings(IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CWeaponLoadout_Rel::ShowPanel(bool bState)
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
			SetPlayerImage("loadouts/rel_leader");
			Q_snprintf(czRankTitle, sizeof(czRankTitle), "REL LEADER");
		}
		else
		{
			SetPlayerImage("loadouts/rel_fanatic");
			Q_snprintf(czRankTitle, sizeof(czRankTitle), "REL FANATIC");
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
			char czLeaderWeapon[128] = "leader_weap";
			char czResource[128] = "shared_weap";

			// Append the current weapon/resource number.
			Q_snprintf(czLeaderWeapon, sizeof(czLeaderWeapon), "%s%d", czLeaderWeapon, i);
			Q_snprintf(czResource, sizeof(czResource), "%s%d", czResource, i);

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

void CWeaponLoadout_Rel::Update()
/*
	
	Pre: 
	Post: 
*/
{
	char czTeamRank[64] = "";

	// Position postfix
	if (ToForsakenPlayer(C_BasePlayer::GetLocalPlayer())->IsLeader())
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "Religious Leader");
	else
		Q_snprintf(czTeamRank, sizeof(czTeamRank), "Religious Fanatic");

	SetLabelText("team_rank", "");
}