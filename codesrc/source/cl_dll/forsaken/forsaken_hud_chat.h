#ifndef __FORSAKEN_HUD_CHAT_H_
#define __FORSAKEN_HUD_CHAT_H_

#include <hud_basechat.h>

class CForsakenHudChat_InputLine : public CBaseHudChatInputLine
{
	DECLARE_CLASS_SIMPLE(CForsakenHudChat_InputLine, CBaseHudChatInputLine);

public:

	// Constructor & Deconstructor
	CForsakenHudChat_InputLine(CBaseHudChat *pParent, const char *czPanelName);

	// Public Accessor Functions

	// Public Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	// Public Variables
};

class CForsakenHudChat_Line : public CBaseHudChatLine
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenHudChat_Line, CBaseHudChatLine);

	// Constructor & Deconstructor
	CForsakenHudChat_Line(vgui::Panel *pParent, const char *czPanelName);

	// Public Accessor Functions

	// Public Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformFadeout(void);

	// Public Variables
};

class CForsakenHudChat : public CBaseHudChat
{
public:
	DECLARE_CLASS_SIMPLE(CForsakenHudChat, CBaseHudChat);

	// Constructor & Deconstructor
	CForsakenHudChat(const char *czElementName);

	// Public Accessor Functions
	int GetChatInputOffset(void);

	// Public Functions
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void CreateChatInputLine(void);
	virtual void CreateChatLines(void);
	virtual void Init(void);
	virtual void Reset(void);
	void ChatPrintf(int nPlayerIndex, const char *czFormat, ...);
	void MsgFunc_SayText(bf_read &rMessage);
	void MsgFunc_TextMsg(bf_read &rMessage);

	// Public Variables
};

#endif