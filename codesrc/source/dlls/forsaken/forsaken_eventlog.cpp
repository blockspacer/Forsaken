/*
	forsaken_eventlog.cpp
	Forsaken event log code, determines what events should or shouldn't be logged..

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "EventLog.h"
#include "KeyValues.h"

// Function Prototypes
IGameSystem *GameLogSystem();

// Internal Classes
class CForsakenEventLog : public CEventLog
{
	typedef CEventLog BaseClass;

public:
	// Constructor & Deconstructor
	virtual ~CForsakenEventLog();

	// Public Accessor Functions

	// Public Functions
	bool PrintEvent(IGameEvent *pEvent);

	// Public Variables
};

// Global Variable Decleration
static CForsakenEventLog g_ForsakenEventLog;

// Constructor & Deconstructor
CForsakenEventLog::~CForsakenEventLog()
{
}

// Functions
// CForsakenEventLog
bool CForsakenEventLog::PrintEvent(IGameEvent *pEvent)
/*
	Determines whether a event should be logged or not.
	Pre: pEvent is a valid game event.
	Post: The event has been logged if it should be, otherwise it isn't.
*/
{
	if (BaseClass::PrintEvent(pEvent))
		return true;

	return false;
}

// Global
IGameSystem *GameLogSystem()
/*
	
	Pre: 
	Post: 
*/
{
	return &g_ForsakenEventLog;
}