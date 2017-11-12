//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include <stdio.h>

#include <cdll_client_int.h>
#include <cdll_util.h>
#include <globalvars_base.h>
#include <igameresources.h>

#include "clientscoreboarddialog.h"

#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vstdlib/IKeyValuesSystem.h>

#include <KeyValues.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/SectionedListPanel.h>

#include <cl_dll/iviewport.h>
#include <igameresources.h>

//#include "voice_status.h"
//#include "Friends/IFriendsUser.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// extern vars
//extern IFriendsUser *g_pFriendsUser;

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CClientScoreBoardDialog::CClientScoreBoardDialog(IViewPort *pViewPort) : Frame( NULL, PANEL_SCOREBOARD )
{
	m_iPlayerIndexSymbol = KeyValuesSystem()->GetSymbolForString("playerIndex");

	//memset(s_VoiceImage, 0x0, sizeof( s_VoiceImage ));
	TrackerImage = 0;
	m_pViewPort = pViewPort;

	// initialize dialog
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetSizeable(false);

	// hide the system buttons
	SetTitleBarVisible( false );

	// set the scheme before any child control is created
	SetScheme("ClientScheme");

	m_pPlayerList = new SectionedListPanel(this, "PlayerList");
	m_pPlayerList->SetVerticalScrollbar(false);

	LoadControlSettings("Resource/UI/ScoreBoard.res");
	m_iDesiredHeight = GetTall();
	m_pPlayerList->SetVisible( false ); // hide this until we load the images in applyschemesettings

	m_HLTVSpectators = 0;
	
	// update scoreboard instantly if on of these events occure
	gameeventmanager->AddListener(this, "hltv_status", false );
	gameeventmanager->AddListener(this, "server_spawn", false );

	for(int a = 0; a <= 3; a++)
	{
		numPlayersOnTeam[a] = 0;	// init # players to 0 so they aren't random huge numbers
	}
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CClientScoreBoardDialog::~CClientScoreBoardDialog()
{
	gameeventmanager->RemoveListener(this);
}

//-----------------------------------------------------------------------------
// Purpose: clears everything in the scoreboard and all it's state
//-----------------------------------------------------------------------------
void CClientScoreBoardDialog::Reset()
{
	// clear
	m_pPlayerList->DeleteAllItems();
	m_pPlayerList->RemoveAllSections();

	m_iSectionId = 0;
	m_fNextUpdateTime = 0;
	// add all the sections
	InitScoreboardSections();
}

//-----------------------------------------------------------------------------
// Purpose: adds all the team sections to the scoreboard
//-----------------------------------------------------------------------------
void CClientScoreBoardDialog::InitScoreboardSections()
{
	AddHeader();
	// *note* might rework later to create all sections in here instead of AddHeader()
}

//-----------------------------------------------------------------------------
// Purpose: sets up screen
//-----------------------------------------------------------------------------
void CClientScoreBoardDialog::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	ImageList *imageList = new ImageList(false);
//	s_VoiceImage[0] = 0;	// index 0 is always blank
//	s_VoiceImage[CVoiceStatus::VOICE_NEVERSPOKEN] = imageList->AddImage(scheme()->GetImage("gfx/vgui/640_speaker1", true));
//	s_VoiceImage[CVoiceStatus::VOICE_NOTTALKING] = imageList->AddImage(scheme()->GetImage("gfx/vgui/640_speaker2", true));
//	s_VoiceImage[CVoiceStatus::VOICE_TALKING] = imageList->AddImage(scheme()->GetImage( "gfx/vgui/640_speaker3", true));
//	s_VoiceImage[CVoiceStatus::VOICE_BANNED] = imageList->AddImage(scheme()->GetImage("gfx/vgui/640_voiceblocked", true));

//	TrackerImage = imageList->AddImage(scheme()->GetImage("gfx/vgui/640_scoreboardtracker", true));

	// Forsaken Addition: Leader Icons
	m_nBlankIcon = imageList->AddImage(NULL);
	m_nLeaderIcon = imageList->AddImage(scheme()->GetImage("leader_icon", true));
	
	// resize the images to our resolution
	for (int i = 0; i < imageList->GetImageCount(); i++ )
	{
		int wide, tall;

		if (imageList->GetImage(i) == NULL)
			continue;

		imageList->GetImage(i)->GetSize(wide, tall);
		imageList->GetImage(i)->SetSize(scheme()->GetProportionalScaledValueEx( GetScheme(),wide), scheme()->GetProportionalScaledValueEx( GetScheme(),tall));
	}

	m_pPlayerList->SetImageList(imageList, false);
	m_pPlayerList->SetVisible( true );

	// light up scoreboard a bit
	SetBgColor( Color( 0,0,0,0) );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CClientScoreBoardDialog::ShowPanel(bool bShow)
{
	if ( BaseClass::IsVisible() == bShow )
		return;

	if ( bShow )
	{
		Reset();
		Update();

		Activate();
	}
	else
	{
		BaseClass::SetVisible( false );
		SetMouseInputEnabled( false );
		SetKeyBoardInputEnabled( false );
	}
}

void CClientScoreBoardDialog::FireGameEvent( IGameEvent *event )
/*
     Handles specific game spawn events
     Pre: event is a game event
     Post: scoreboard is setup if game is spawned
*/
{
	const char * type = event->GetName();

	if ( Q_strcmp(type, "hltv_status") == 0 )
	{
		// spectators = clients - proxies
		m_HLTVSpectators = event->GetInt( "clients" );
		m_HLTVSpectators -= event->GetInt( "proxies" );
	}

	else if ( Q_strcmp(type, "server_spawn") == 0 )
	{
		// sets top line to read "server name - map name"
		const char * host_name = event->GetString("hostname");
		const char * map_name = event->GetString("mapname");
		char top_label[256];
		sprintf(top_label, "%s - %s", host_name,  map_name);
		Panel *control = FindChildByName( "ServerName" );
		if ( control )
		{
			PostMessage( control, new KeyValues( "SetText", "text", top_label ) );
		}
		control->MoveToFront();
	}

	if( IsVisible() )
		Update();
}

bool CClientScoreBoardDialog::NeedsUpdate( void )
{
	return (m_fNextUpdateTime < gpGlobals->curtime);
}

void CClientScoreBoardDialog::Update( void )
/*
     Update scoreboard data
     Pre: scoreboard needs to be updated
     Post: scoreboard is flushed and then updated
*/
{
	m_pPlayerList->DeleteAllItems();
	
	FillScoreBoard();

	// enlarge scoreboard to fit # of players
	int wide, tall;
	m_pPlayerList->GetContentSize(wide, tall);
	wide = GetWide();
	if (m_iDesiredHeight < tall)
	{
		SetSize(wide, tall);
		m_pPlayerList->SetSize(wide, tall);
	}
	else
	{
		SetSize(wide, m_iDesiredHeight);
		m_pPlayerList->SetSize(wide, m_iDesiredHeight);
		m_pPlayerList->SetVerticalScrollbar(true);
	}

	MoveToCenterOfScreen();

	// update every second
	m_fNextUpdateTime = gpGlobals->curtime + 1.0f; 
}

void CClientScoreBoardDialog::UpdateTeamInfo()
/*
     Updates the data in the team sections on the scoreboard
     Pre: called whenever scoreboard is updated
     Post: team data is reset
*/
{
    int sectionId;							
    
    char tempHeader[256];					// hold header as string before ANSI to Unicode conversion
    char pluralPlayers[2];					// hold 's' if more than one player on team (for team header)
	char pluralWins[2];						// hold 's' if more team has more than 1 win (for team header)
    wchar_t teamHeader[256];				// string to hold team header ( # players - # wins )

    IGameResources *gr = GameResources();	
    if ( !gr )		// return if gameresources aren't found
        return;

	int i;
    for (i=TEAM_GOVERNORS;i<=TEAM_SPECTATOR;i++)
    {
        sectionId = iTeamSections[i];		// reference scoreboard section id for team

        if (numPlayersOnTeam[i] == 1)		// only add 's' if more than 1 player on a team
            sprintf(pluralPlayers,"");
        else
            sprintf(pluralPlayers,"s");

		if(gr->GetTeamScore(i) > 1)			// only add 's' if team has more than 1 win
			sprintf(pluralWins,"s");
		else
			sprintf(pluralWins,"");

		if(i == TEAM_SPECTATOR)				// spectators don't have wins, so just show # of players
			sprintf(tempHeader, "%s - (%i player%s)", gr->GetTeamName(i),  numPlayersOnTeam[i], pluralPlayers);
		else
			sprintf(tempHeader, "%s - (%i player%s - %i win%s)", gr->GetTeamName(i),  numPlayersOnTeam[i], pluralPlayers, gr->GetTeamScore(i), pluralWins);
		
        localize()->ConvertANSIToUnicode(tempHeader, teamHeader, sizeof(teamHeader));
        m_pPlayerList->ModifyColumn(sectionId, "name", teamHeader);

        if ( numPlayersOnTeam[i] > 0 )		// update team ping, or set to 0 if no players
            teamPing[i] /= numPlayersOnTeam[i];
        else
            teamPing[i] = 0;
    }
}

void CClientScoreBoardDialog::UpdatePlayerInfo()
/*
     Updates the data of each player for the scoreboard
     Pre: called whenever scoreboard is updated
     Post: player data is reset
*/
{
	m_iSectionId = 0;						// 0 row is the header
	int selectedRow = -1;					

	for(int a = 0; a <= 4; a++)			
	{
		numPlayersOnTeam[a] = 0;			// reset # of players so they don't grow each time
	}

	for ( int i = 1; i < gpGlobals->maxClients; i++ )		// go through all the players
	{
		IGameResources *gr = GameResources();

		if ( gr && gr->IsConnected( i ) )
		{
			// Fill out the player's data
			KeyValues *playerData = new KeyValues("data");
			GetPlayerData( i, playerData );

			const char *oldName = playerData->GetString("name","");		
			int bufsize = strlen(oldName) * 2;
			char *newName = (char *)_alloca( bufsize );
			UTIL_MakeSafeName( oldName, newName, bufsize );
			playerData->SetString("name", newName);			// sets player's name

			int itemID = FindItemIDForPlayerIndex( i );		// player itemID for section
            int playerTeam = gr->GetTeam(i);				
            int sectionID = iTeamSections[playerTeam];		// get sectionID based on team

            numPlayersOnTeam[playerTeam]++;					// add player to global variable
            teamPing[playerTeam]+=playerData->GetInt("ping");
           
            if ( gr->IsLocalPlayer( i ) )
            {
                selectedRow = itemID;
            }
            if (itemID == -1)								 // add a new row if no row for that team has been created yet
            {
                itemID = m_pPlayerList->AddItem( sectionID, playerData );
            }
            else											// else, modify the current row
            {
                m_pPlayerList->ModifyItem( itemID, sectionID, playerData );
            }

            m_pPlayerList->SetItemFgColor( itemID, gr->GetTeamColor( playerTeam ) );
            playerData->deleteThis();
		}
		else
		{
			int itemID = FindItemIDForPlayerIndex( i );		// remove player if disconnected
			if (itemID != -1)
			{
				m_pPlayerList->RemoveItem(itemID);
			}
		}
	}

	if ( selectedRow != -1 )
	{
		m_pPlayerList->SetSelectedItem(selectedRow);
	}
}

//-----------------------------------------------------------------------------
// Purpose: adds the top header of the scoreboard and additional sections and columns
//-----------------------------------------------------------------------------
void CClientScoreBoardDialog::AddHeader()
{
	m_iSectionId = 0;		// main blank top section to leave space for server name and map name
    m_pPlayerList->AddSection(m_iSectionId, "");
    m_pPlayerList->SetSectionAlwaysVisible(m_iSectionId);
	m_pPlayerList->SetSectionFgColor(m_iSectionId, Color(203, 176, 154, 255));
    m_pPlayerList->AddColumnToSection(m_iSectionId, "name", "", 0, scheme()->GetProportionalScaledValue(NAME_WIDTH) );

    m_iSectionId = 1;
    m_pPlayerList->AddSection(m_iSectionId, "");
    m_pPlayerList->SetSectionAlwaysVisible(m_iSectionId);
	m_pPlayerList->SetSectionFgColor(m_iSectionId, Color(203, 176, 154, 255));

	m_pPlayerList->AddColumnToSection(m_iSectionId, "leadericon", "",SectionedListPanel::COLUMN_IMAGE, scheme()->GetProportionalScaledValue(11) );
    m_pPlayerList->AddColumnToSection(m_iSectionId, "name", "", 0, scheme()->GetProportionalScaledValue(NAME_WIDTH) );
    m_pPlayerList->AddColumnToSection(m_iSectionId, "score", "#PlayerScore", 0, scheme()->GetProportionalScaledValue(SCORE_WIDTH) );
	m_pPlayerList->AddColumnToSection(m_iSectionId, "kills", "Kills", 0, scheme()->GetProportionalScaledValue(KILL_WIDTH) );
	m_pPlayerList->AddColumnToSection(m_iSectionId, "deaths", "Deaths", 0, scheme()->GetProportionalScaledValue(DEATH_WIDTH) );
    m_pPlayerList->AddColumnToSection(m_iSectionId, "ping", "Ping", 0, scheme()->GetProportionalScaledValue(PING_WIDTH) );

    m_iSectionId = 2;		// add team sections starting in new section 
	iTeamSections[TEAM_GOVERNORS]  = AddSection(TYPE_TEAM, TEAM_GOVERNORS);
	iTeamSections[TEAM_RELIGIOUS]   = AddSection(TYPE_TEAM, TEAM_RELIGIOUS);
	iTeamSections[TEAM_CIVILIANS]   = AddSection(TYPE_TEAM, TEAM_CIVILIANS);
	iTeamSections[TEAM_SPECTATOR]   = AddSection(TYPE_SPECTATORS, TEAM_SPECTATOR);
}

//-----------------------------------------------------------------------------
// Purpose: Adds a new section to the scoreboard
//-----------------------------------------------------------------------------
int CClientScoreBoardDialog::AddSection(int teamType, int teamNumber)
{
	IGameResources *gr = GameResources();

	if ( !gr )
		return -1;

	if ( teamType == TYPE_TEAM )
    {
        // setup the team name
        wchar_t *teamName = localize()->Find( gr->GetTeamName(teamNumber) );	// team name might need to be converted
        wchar_t name[64];
       
        if (!teamName)
        {
            localize()->ConvertANSIToUnicode(gr->GetTeamName(teamNumber), name, sizeof(name));
            teamName = name;
        }

        m_pPlayerList->AddSection(m_iSectionId, "", StaticPlayerSortFunc);
	    m_pPlayerList->SetSectionAlwaysVisible(m_iSectionId);
		m_pPlayerList->SetSectionFgColor(m_iSectionId, gr->GetTeamColor(teamNumber));

		m_pPlayerList->AddColumnToSection(m_iSectionId, "leadericon", "", SectionedListPanel::COLUMN_IMAGE, scheme()->GetProportionalScaledValue(11) );
        m_pPlayerList->AddColumnToSection(m_iSectionId, "name", teamName, 0, scheme()->GetProportionalScaledValue(NAME_WIDTH) );
        m_pPlayerList->AddColumnToSection(m_iSectionId, "score", "", 0, scheme()->GetProportionalScaledValue(SCORE_WIDTH) );
		m_pPlayerList->AddColumnToSection(m_iSectionId, "kills", "", 0, scheme()->GetProportionalScaledValue(KILL_WIDTH) );
		m_pPlayerList->AddColumnToSection(m_iSectionId, "deaths", "", 0, scheme()->GetProportionalScaledValue(DEATH_WIDTH) );
        m_pPlayerList->AddColumnToSection(m_iSectionId, "ping", "", 0, scheme()->GetProportionalScaledValue(PING_WIDTH) );
    }
    else if ( teamType == TYPE_SPECTATORS )
    {
        m_pPlayerList->AddSection(m_iSectionId, "");
		m_pPlayerList->SetSectionAlwaysVisible(m_iSectionId);
		m_pPlayerList->AddColumnToSection(m_iSectionId, "leadericon", "", SectionedListPanel::COLUMN_IMAGE, scheme()->GetProportionalScaledValue(11) );
		m_pPlayerList->AddColumnToSection(m_iSectionId, "name", "Spectators", 0, scheme()->GetProportionalScaledValue(NAME_WIDTH));
    }

	m_pPlayerList->SetSectionFgColor(m_iSectionId, Color(203, 176, 154, 255));
    m_iSectionId++; // switch to new section for next added
    return m_iSectionId-1;
}

//-----------------------------------------------------------------------------
// Purpose: Used for sorting players
//-----------------------------------------------------------------------------
bool CClientScoreBoardDialog::StaticPlayerSortFunc(vgui::SectionedListPanel *list, int itemID1, int itemID2)
{
	KeyValues *it1 = list->GetItemData(itemID1);
	KeyValues *it2 = list->GetItemData(itemID2);
	Assert(it1 && it2);

	// first compare frags
	int v1 = it1->GetInt("score");
	int v2 = it2->GetInt("score");
	if (v1 > v2)
		return true;
	else if (v1 < v2)
		return false;

	// next compare deaths
	v1 = it1->GetInt("deaths");
	v2 = it2->GetInt("deaths");
	if (v1 > v2)
		return false;
	else if (v1 < v2)
		return true;

	// the same, so compare itemID's (as a sentinel value to get deterministic sorts)
	return itemID1 < itemID2;
}

//-----------------------------------------------------------------------------
// Purpose: Adds a new row to the scoreboard, from the playerinfo structure
//-----------------------------------------------------------------------------
bool CClientScoreBoardDialog::GetPlayerData(int playerIndex, KeyValues *kv)
{
	IGameResources *gr = GameResources();

	if (!gr )
		return false;

	kv->SetInt("score", gr->GetPlayerScore( playerIndex ) );
	kv->SetInt("kills", gr->GetFrags( playerIndex ) );
	kv->SetInt("deaths", gr->GetDeaths( playerIndex) );
    kv->SetInt("ping", gr->GetPing( playerIndex ) ) ;

	if (gr->IsLeader(playerIndex))
		kv->SetInt("leadericon", m_nLeaderIcon);
	else
		kv->SetInt("leadericon", m_nBlankIcon);

	// Forsaken: If dead, prepend *DEAD* to the name.
	if (!gr->IsAlive(playerIndex) && 
		(gr->GetTeam(playerIndex) == C_BasePlayer::GetLocalPlayer()->GetTeamNumber() || 
		!C_BasePlayer::GetLocalPlayer()->IsAlive()))
	{
		char czName[MAX_PLAYER_NAME_LENGTH+8] = "";
		const char *deathTag = "*DEAD* ";
		const char *playerName = gr->GetPlayerName( playerIndex );

		Q_snprintf(czName, sizeof(czName), "%s%s", deathTag, playerName);

		kv->SetString("name", czName);
	}
	else
	    kv->SetString("name", gr->GetPlayerName(playerIndex) );

    kv->SetInt("playerIndex", playerIndex);

//	kv->SetInt("voice",	s_VoiceImage[GetClientVoice()->GetSpeakerStatus( playerIndex - 1) ]);	

/*	// setup the tracker column
	if (g_pFriendsUser)
	{
		unsigned int trackerID = gEngfuncs.GetTrackerIDForPlayer(row);

		if (g_pFriendsUser->IsBuddy(trackerID) && trackerID != g_pFriendsUser->GetFriendsID())
		{
			kv->SetInt("tracker",TrackerImage);
		}
	}
*/

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: reload the player list on the scoreboard
//-----------------------------------------------------------------------------
void CClientScoreBoardDialog::FillScoreBoard()
{
	// reset player lists so they don't grow continuously
	for (int i = TEAM_UNASSIGNED; i < 2; i++)
    {
        numPlayersOnTeam[i] = 0; 
    }
	
	// update player info, then team info
    UpdatePlayerInfo();		
    UpdateTeamInfo(); 
} 

//-----------------------------------------------------------------------------
// Purpose: searches for the player in the scoreboard
//-----------------------------------------------------------------------------
int CClientScoreBoardDialog::FindItemIDForPlayerIndex(int playerIndex)
{
	for (int i = 0; i <= m_pPlayerList->GetHighestItemID(); i++)
	{
		if (m_pPlayerList->IsItemIDValid(i))
		{
			KeyValues *kv = m_pPlayerList->GetItemData(i);
			kv = kv->FindKey(m_iPlayerIndexSymbol);
			if (kv && kv->GetInt() == playerIndex)
				return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: Sets the text of a control by name
//-----------------------------------------------------------------------------
void CClientScoreBoardDialog::MoveLabelToFront(const char *textEntryName)
{
	Label *entry = dynamic_cast<Label *>(FindChildByName(textEntryName));
	if (entry)
	{
		entry->MoveToFront();
	}
}