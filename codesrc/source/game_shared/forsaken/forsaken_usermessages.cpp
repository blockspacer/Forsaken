#include "cbase.h"
#include "usermessages.h"
#include "shake.h"
#include "voice_gamemgr.h"

void RegisterUserMessages()
/*
	
	Pre: 
	Post: 
*/
{
	usermessages->Register("Geiger", 1);
	usermessages->Register("Train", 1);
	usermessages->Register("HudText", -1);
	usermessages->Register("SayText", -1);
	usermessages->Register("TextMsg", -1);
	usermessages->Register("HudMsg", -1);
	usermessages->Register("ResetHUD", 1);
	usermessages->Register("GameTitle", 0);
	usermessages->Register("ItemPickup", -1);
	usermessages->Register("ShowMenu", -1);
	usermessages->Register("Shake", 13);
	usermessages->Register("Fade", 10);
	usermessages->Register("VGUIMenu", -1);
	usermessages->Register("CloseCaption", 3);

	usermessages->Register("SendAudio", -1);
	usermessages->Register("VoiceMask", VOICE_MAX_PLAYERS_DW*4 * 2 + 1);
	usermessages->Register("RequestState", 0);
	usermessages->Register("HintText", -1);
	usermessages->Register("AmmoDenied", 2);

	// Custom User Message
	// Effects messages
	usermessages->Register("PointsNotice", -1);
	usermessages->Register("Damage", 18);

	// Credits messages
	usermessages->Register("CreditsMsg", 1);
	usermessages->Register("LogoTimeMsg", 4);

	// Round Restart
	usermessages->Register("RestartRound", 0);
}