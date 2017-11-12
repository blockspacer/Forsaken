/*
	forsaken_hud_chat.cpp
	The chat HUD element for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_hud_chat.h"
#include "hud_macros.h"
#include "text_message.h"
#include "vguicenterprint.h"
#include "vgui/ILocalize.h"

// HL2 Class Macros
DECLARE_HUDELEMENT(CForsakenHudChat);
DECLARE_HUD_MESSAGE(CForsakenHudChat, SayText);
DECLARE_HUD_MESSAGE(CForsakenHudChat, TextMsg);

// Global Variable Decleration
float g_ColorGreen[3]	= { 153, 255, 153 };
float g_ColorYellow[3]	= { 255, 178.5, 0.0 };
// Team name colors
float g_ColorReligious[3]	= { 255, 64, 64 };
float g_ColorGovernors[3]	= { 204, 204, 204 };
float g_ColorCivilians[3]	= { 153, 204, 255 };

// Function Prototypes
float *GetClientColor(int nClientIndex);
static char *ConvertCRtoNL(char *czString);
static wchar_t *ConvertCRtoNL(wchar_t *czString);
static void StripEndNewlineFromString(char *czString);
static void StripEndNewlineFromString(wchar_t *czString);

// Constructor & Deconstructor
CForsakenHudChat_InputLine::CForsakenHudChat_InputLine(CBaseHudChat *pParent, 
		const char *czPanelName) : BaseClass(pParent, czPanelName)
{
}

CForsakenHudChat_Line::CForsakenHudChat_Line(vgui::Panel *pParent, const char *czPanelName) 
		: BaseClass(pParent, czPanelName)
{
}

CForsakenHudChat::CForsakenHudChat(const char *czElementName) : BaseClass(czElementName)
{
}

// Functions
// CForsakenHudChat_InputLine
void CForsakenHudChat_InputLine::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);

	vgui::HFont hFont = pScheme->GetFont("ChatFont");

	m_pInput->SetFont(hFont);
	m_pPrompt->SetFont(hFont);

	m_pInput->SetFgColor(pScheme->GetColor("Chat.TypingText", pScheme->GetColor("Panel.FgColor", 
		Color(255, 255, 255))));
}

// CForsakenHudChat_Line
void CForsakenHudChat_Line::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_hFont = pScheme->GetFont("ChatFont");

	SetBorder(NULL);
	SetBgColor(Color(0, 0, 0));
	SetFgColor(Color(0, 0, 0));
	SetFont(m_hFont);
}

void CForsakenHudChat_Line::PerformFadeout(void)
/*
	
	Pre: 
	Post: 
*/
{
	// Flash + Extra bright when new
	float curtime = gpGlobals->curtime;

	int lr = m_clrText[0];
	int lg = m_clrText[1];
	int lb = m_clrText[2];
	
	//CSPort chat only fades out, no blinking.
	if ( curtime <= m_flExpireTime && curtime > m_flExpireTime - CHATLINE_FADE_TIME )
	{
		float frac = ( m_flExpireTime - curtime ) / CHATLINE_FADE_TIME;

		int alpha = frac * 255;
		alpha = clamp( alpha, 0, 255 );

		wchar_t wbuf[4096];
		
		GetText(0, wbuf, sizeof(wbuf));

		SetText( "" );
			
		if ( m_iNameLength > 0 )
		{
			wchar_t wText[4096];
			// draw the first x characters in the player color
			wcsncpy( wText, wbuf, min( m_iNameLength + 1, MAX_PLAYER_NAME_LENGTH+32) );
			wText[ min( m_iNameLength, MAX_PLAYER_NAME_LENGTH+31) ] = 0;

			m_clrNameColor[3] = alpha;

			InsertColorChange( m_clrNameColor );
			InsertString( wText );

			wcsncpy( wText, wbuf + ( m_iNameLength ), wcslen( wbuf + m_iNameLength ) );
			wText[ wcslen( wbuf + m_iNameLength ) ] = '\0';
			InsertColorChange( Color( g_ColorYellow[0], g_ColorYellow[1], g_ColorYellow[2], alpha ) );
			InsertString( wText );
			InvalidateLayout( true );
		}
		else
		{
			InsertColorChange( Color( lr, lg, lb, alpha ) );
			InsertString( wbuf );
		}
	}
	
	OnThink();
}

// CForsakenHudChat
int CForsakenHudChat::GetChatInputOffset(void)
/*
	
	Pre: 
	Post: 
*/
{
	if (m_pChatInput->IsVisible())
		return m_iFontHeight;
	else
		return 0;
}

void CForsakenHudChat::ApplySchemeSettings(vgui::IScheme *pScheme)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetBgColor(Color(0, 0, 0));
	SetFgColor(Color(0, 0, 0));
}

void CForsakenHudChat::ChatPrintf(int nPlayerIndex, const char *czFormat, ...)
/*
	
	Pre: 
	Post: 
*/
{
	va_list marker;
	char msg[4096];

	va_start(marker, czFormat);
	Q_vsnprintf(msg, sizeof(msg), czFormat, marker);
	va_end(marker);

	// Strip any trailing '\n'
	if ( strlen( msg ) > 0 && msg[ strlen( msg )-1 ] == '\n' )
	{
		msg[ strlen( msg ) - 1 ] = 0;
	}

	// Strip leading \n characters ( or notify/color signifiers )
	char *pmsg = msg;
	while ( *pmsg && ( *pmsg == '\n' || *pmsg == 1 || *pmsg == 2 ) )
	{
		pmsg++;
	}
	
	if ( !*pmsg )
		return;

	CForsakenHudChat_Line *line = (CForsakenHudChat_Line *)FindUnusedChatLine();
	if ( !line )
	{
		ExpireOldest();
		line = (CForsakenHudChat_Line *)FindUnusedChatLine();
	}

	if ( !line )
	{
		return;
	}

	line->SetText( "" );
	
	int iNameLength = 0;
	
	player_info_t sPlayerInfo;
	if ( nPlayerIndex == 0 )
	{
		Q_memset( &sPlayerInfo, 0, sizeof(player_info_t) );
		Q_strncpy( sPlayerInfo.name, "Console", sizeof(sPlayerInfo.name)  );	
	}
	else
	{
		engine->GetPlayerInfo( nPlayerIndex, &sPlayerInfo );
	}	

	const char *pName = sPlayerInfo.name;

	if ( pName )
	{
		const char *nameInString = strstr( pmsg, pName );

		if ( nameInString )
		{
			iNameLength = strlen( pName ) + (nameInString - pmsg);
		}
	}
	else
		line->InsertColorChange( Color( g_ColorYellow[0], g_ColorYellow[1], g_ColorYellow[2], 255 ) );

	char buf[4096];
	wchar_t wbuf[4096];

	Assert(strlen(pmsg) < sizeof(buf));

	if ( buf )
	{
		float *flColor = GetClientColor( nPlayerIndex );

		line->SetExpireTime();
	
		// draw the first x characters in the player color
		Q_strncpy( buf, pmsg, min( iNameLength + 1, MAX_PLAYER_NAME_LENGTH+32) );
		buf[ min( iNameLength, MAX_PLAYER_NAME_LENGTH+31) ] = 0;
		line->InsertColorChange( Color( flColor[0], flColor[1], flColor[2], 255 ) );
		line->InsertString( buf );
		Q_strncpy( buf, pmsg + iNameLength, sizeof(buf));
		buf[ strlen( pmsg + iNameLength ) ] = '\0';
		line->InsertColorChange( Color( g_ColorYellow[0], g_ColorYellow[1], g_ColorYellow[2], 255 ) );
		vgui::localize()->ConvertANSIToUnicode( buf, wbuf, sizeof(wbuf));
		line->InsertString( wbuf );
		line->SetVisible( true );
		line->SetNameLength( iNameLength );
		line->SetNameColor( Color( flColor[0], flColor[1], flColor[2], 255 ) );
	}

	CLocalPlayerFilter filter;
	C_BaseEntity::EmitSound( filter, -1 /*SOUND_FROM_LOCAL_PLAYER*/, "HudChat.Message" );
}

void CForsakenHudChat::CreateChatInputLine(void)
/*
	
	Pre: 
	Post: 
*/
{
	m_pChatInput = new CForsakenHudChat_InputLine(this, "ChatInputLine");

	m_pChatInput->SetVisible(false);
}

void CForsakenHudChat::CreateChatLines(void)
/*
	
	Pre: 
	Post: 
*/
{
	for (int i = 0; i < CHAT_INTERFACE_LINES; i++)
	{
		char czLineName[32] = "";

		Q_snprintf(czLineName, sizeof(czLineName), "ChatLine%02i", i);

		m_ChatLines[i] = new CForsakenHudChat_Line(this, czLineName);

		m_ChatLines[i]->SetVisible(false);
	}
}

void CForsakenHudChat::Init(void)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Init();

	HOOK_HUD_MESSAGE(CForsakenHudChat, SayText);
	HOOK_HUD_MESSAGE(CForsakenHudChat, TextMsg);
}

void CForsakenHudChat::MsgFunc_SayText(bf_read &rMessage)
/*
	
	Pre: 
	Post: 
*/
{
	bool bWantsToChat = false;
	char czString[256] = "";
	int nClient = -1;

	nClient = rMessage.ReadByte();
	rMessage.ReadString(czString, sizeof(czString));
	bWantsToChat = rMessage.ReadByte();

	if (bWantsToChat)
		ChatPrintf(nClient, "%s", czString);
	else
		Printf("%s", hudtextmessage->LookupString(czString));

	Msg("%s", czString);
}

void CForsakenHudChat::MsgFunc_TextMsg(bf_read &rMessage)
/*
	
	Pre: 
	Post: 
*/
{
	char szString[2048];
	int msg_dest = rMessage.ReadByte();
	
	wchar_t szBuf[5][128];
	wchar_t outputBuf[256];

	for ( int i=0; i<5; ++i )
	{
		rMessage.ReadString( szString, sizeof(szString) );
		char *tmpStr = hudtextmessage->LookupString( szString, &msg_dest );
		const wchar_t *pBuf = vgui::localize()->Find( tmpStr );
		if ( pBuf )
		{
			// Copy pBuf into szBuf[i].
			int nMaxChars = sizeof( szBuf[i] ) / sizeof( wchar_t );
			wcsncpy( szBuf[i], pBuf, nMaxChars );
			szBuf[i][nMaxChars-1] = 0;
		}
		else
		{
			if ( i )
			{
				StripEndNewlineFromString( tmpStr );  // these strings are meant for subsitution into the main strings, so cull the automatic end newlines
			}
			vgui::localize()->ConvertANSIToUnicode( tmpStr, szBuf[i], sizeof(szBuf[i]) );
		}
	}

	int len;
	switch ( msg_dest )
	{
	case HUD_PRINTCENTER:
		vgui::localize()->ConstructString( outputBuf, sizeof(outputBuf), szBuf[0], 4, szBuf[1], szBuf[2], szBuf[3], szBuf[4] );
		internalCenterPrint->Print( ConvertCRtoNL( outputBuf ) );
		break;

	case HUD_PRINTNOTIFY:
		szString[0] = 1;  // mark this message to go into the notify buffer
		vgui::localize()->ConstructString( outputBuf, sizeof(outputBuf), szBuf[0], 4, szBuf[1], szBuf[2], szBuf[3], szBuf[4] );
		vgui::localize()->ConvertUnicodeToANSI( outputBuf, szString+1, sizeof(szString)-1 );
		len = strlen( szString );
		if ( len && szString[len-1] != '\n' && szString[len-1] != '\r' )
		{
			Q_strncat( szString, "\n", sizeof(szString), 1 );
		}
		Msg( "%s", ConvertCRtoNL( szString ) );
		break;

	case HUD_PRINTTALK:
		vgui::localize()->ConstructString( outputBuf, sizeof(outputBuf), szBuf[0], 4, szBuf[1], szBuf[2], szBuf[3], szBuf[4] );
		vgui::localize()->ConvertUnicodeToANSI( outputBuf, szString, sizeof(szString) );
		len = strlen( szString );
		if ( len && szString[len-1] != '\n' && szString[len-1] != '\r' )
		{
			Q_strncat( szString, "\n", sizeof(szString), 1 );
		}
		Printf( "%s", ConvertCRtoNL( szString ) );
		break;

	case HUD_PRINTCONSOLE:
		vgui::localize()->ConstructString( outputBuf, sizeof(outputBuf), szBuf[0], 4, szBuf[1], szBuf[2], szBuf[3], szBuf[4] );
		vgui::localize()->ConvertUnicodeToANSI( outputBuf, szString, sizeof(szString) );
		len = strlen( szString );
		if ( len && szString[len-1] != '\n' && szString[len-1] != '\r' )
		{
			Q_strncat( szString, "\n", sizeof(szString), 1 );
		}
		Msg( "%s", ConvertCRtoNL( szString ) );
		break;
	}
}

void CForsakenHudChat::Reset(void)
/*
	
	Pre: 
	Post: 
*/
{
}

// Global
float *GetClientColor(int nClientIndex)
/*
	
	Pre: 
	Post: 
*/
{
	if (nClientIndex == 0)
		return g_ColorGreen;
	else
	{
		// Return color based on client's team
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(nClientIndex);

		if (pPlayer)
		{
			int nTeamNumber = pPlayer->GetTeamNumber();

			switch (nTeamNumber)
			{
				case TEAM_CIVILIANS:
					return g_ColorCivilians;
				case TEAM_GOVERNORS:
					return g_ColorGovernors;
				case TEAM_RELIGIOUS:
					return g_ColorReligious;
				default:
					return g_ColorYellow;
			}
		}
		else
			return g_ColorYellow;
	}
}

static char *ConvertCRtoNL(char *czString)
/*
	
	Pre: 
	Post: 
*/
{
	for (char *c = czString; *c != 0; c++)
	{
		if (*c == '\r')
			*c = '\n';
	}

	return czString;
}

static wchar_t *ConvertCRtoNL(wchar_t *czString)
/*
	
	Pre: 
	Post: 
*/
{
	for (wchar_t *c = czString; *c != 0; c++)
	{
		if (*c == L'\r')
			*c = L'\n';
	}

	return czString;
}

static void StripEndNewlineFromString(char *czString)
/*
	
	Pre: 
	Post: 
*/
{
	int nLen = strlen(czString) - 1;

	if (nLen >= 0)
	{
		if (czString[nLen] == '\n' || czString[nLen] == '\r')
			czString[nLen] = 0;
	}
}

static void StripEndNewlineFromString(wchar_t *czString)
/*
	
	Pre: 
	Post: 
*/
{
	int nLen = wcslen(czString) - 1;

	if (nLen >= 0)
	{
		if (czString[nLen] == L'\n' || czString[nLen] == L'\r')
			czString[nLen] = 0;
	}
}