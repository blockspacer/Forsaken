/*
	forsaken_player_voicecmds.cpp
	Forsaken player voice commands.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_player.h"

// Function Prototypes
char *ParseVoiceSelection_Direcitonal(int nSelection);
char *ParseVoiceSelection_General(int nSelection);
char *ParseVoiceSelection_Team(int nSelection);
void CC_VoiceCmd_Directional(void);
void CC_VoiceCmd_General(void);
void CC_VoiceCmd_Team(void);

// Global Variable Decleration
static ConCommand voicecmd_directional("voicecmd_directional", CC_VoiceCmd_Directional, 
	"Opens up the directional voice command menu.");
static ConCommand voicecmd_general("voicecmd_general", CC_VoiceCmd_General, 
	"Opens up the general voice command menu.");
static ConCommand voicecmd_team("voicecmd_team", CC_VoiceCmd_Team, 
	"Opens up the team voice command menu.");

// Functions
// Global
char *ParseVoiceSelection_Direcitonal(int nSelection)
/*
	
	Pre: 
	Post: 
*/
{
	switch (nSelection)
	{
	case 1:
		return "Voice.MoveOut";
	case 2:
		return "Voice.YouTakeLeft";
	case 3:
		return "Voice.YouTakeRight";
	case 4:
		return "Voice.ITakeLeft";
	case 5:
		return "Voice.ITakeRight";
	case 6:
		return "Voice.DownThere";
	case 7:
		return "Voice.UpThere";
	case 8:
		return "Voice.StraightAcross";
	case 9:
		return "Voice.Incoming";
	}

	return "Voice.None";
}

char *ParseVoiceSelection_General(int nSelection)
/*
	
	Pre: 
	Post: 
*/
{
	switch (nSelection)
	{
	case 1:
		return "Voice.Affirmative";
	case 2:
		return "Voice.Negative";
	case 3:
		return "Voice.ReportingIn";
	case 4:
		return "Voice.IHit";
	case 5:
		return "Voice.GetOut";
	case 6:
		return "Voice.Sorry";
	case 7:
		return "Voice.EnemyDown";
	case 8:
		return "Voice.Taunt1";
	case 9:
		return "Voice.Taunt2";
	}

	return "Voice.None";
}

char *ParseVoiceSelection_Team(int nSelection)
/*
	
	Pre: 
	Post: 
*/
{
	switch (nSelection)
	{
	case 1:
		return "Voice.CoverMe";
	case 2:
		return "Voice.FollowMe";
	case 3:
		return "Voice.FallBack";
	case 4:
		return "Voice.NeedBackup";
	case 5:
		return "Voice.HoldThisPosition";
	case 6:
		return "Voice.StickTogether";
	case 7:
		return "Voice.LeadWay";
	case 8:
		return "Voice.Protect";
	case 9:
		return "Voice.ReportIn";
	}

	return "Voice.None";
}

void CC_VoiceCmd_Directional(void)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CSingleUserRecipientFilter filter(pPlayer);

	UserMessageBegin(filter, "ShowMenu");
	WRITE_WORD(16383);
	WRITE_CHAR(-1);
	WRITE_BYTE(0);
	WRITE_STRING("->1. Move Out\n->2. You go left\n->3. You go right\n->4. I'll go left\n->5. I'll go right\n->6. Down there\n->7. Up there\n->8. Straight across!\n->9. Incoming!\n \n->0. Cancel");
	MessageEnd();

	pPlayer->SetVoiceCommandState(VOICECMD_DIRECTIONAL);
	pPlayer->SetSpawnSelection(true);
}

void CC_VoiceCmd_General(void)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CSingleUserRecipientFilter filter(pPlayer);

	UserMessageBegin(filter, "ShowMenu");
	WRITE_WORD(16383);
	WRITE_CHAR(-1);
	WRITE_BYTE(0);
	WRITE_STRING("->1. Roger that.\n->2. Negative.\n->3. Reporting in.\n->4. I've been hit!\n->5. Get out of here!\n->6. Sorry...\n->7. Enemy down!\n->8. Taunt 1\n->9. Taunt 2\n \n->0. Cancel");
	MessageEnd();

	pPlayer->SetVoiceCommandState(VOICECMD_GENERAL);
	pPlayer->SetSpawnSelection(true);
}

void CC_VoiceCmd_Team(void)
/*
	
	Pre: 
	Post: 
*/
{
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_GetCommandClient());
	CSingleUserRecipientFilter filter(pPlayer);

	UserMessageBegin(filter, "ShowMenu");
	WRITE_WORD(1023);
	WRITE_CHAR(-1);
	WRITE_BYTE(0);
	WRITE_STRING("->1. Cover me\n->2. Follow me\n->3. Fall back!\n->4. Need backup!\n->5. Hold this position\n->6. Stick together\n->7. Lead the way\n->8. Protect _MY_ object, baby\n->9. Report in, everyone.\n \n->0. Cancel");
	MessageEnd();

	pPlayer->SetVoiceCommandState(VOICECMD_TEAM);
	pPlayer->SetSpawnSelection(true);
}