/*
	forsaken_winlogic_entity.cpp
	The logic entity in Forsaken that relays to the game rules who won.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "entitylist.h"
#include "forsaken/forsaken_shareddefs.h"
#include "forsaken/forsaken_gamerules_territorywar.h"
#include "forsaken_winlogic_entity.h"
#include "forsaken_objective_entity.h"

// HL2 Class Macros
LINK_ENTITY_TO_CLASS(forsaken_winlogic_entity, CForsakenWinLogicEntity);

BEGIN_DATADESC(CForsakenWinLogicEntity)
	// Fields
	DEFINE_FIELD(m_czLogic, FIELD_STRING),
	DEFINE_FIELD(m_nTeamOwner, FIELD_INTEGER),
	DEFINE_KEYFIELD(m_czLogic, FIELD_STRING, "logic_string"),
	DEFINE_KEYFIELD(m_nTeamOwner, FIELD_INTEGER, "team_owner"),

	// Inputs
	DEFINE_INPUTFUNC(FIELD_INTEGER, "ObjectiveChangeState", ObjectiveChangeState),
END_DATADESC()

// Constructor & Deconstructor
CForsakenWinLogicEntity::CForsakenWinLogicEntity()
{
	m_bParsed = false;
	m_lsParsedLogic.vecChildren.PurgeAndDeleteElements();
	m_lsParsedLogic.vecLogic.Purge();
	m_mapLatchTrue.RemoveAll();
	m_mapObjectiveBinds.RemoveAll();
}

CForsakenWinLogicEntity::~CForsakenWinLogicEntity()
{
	m_bParsed = false;
	m_lsParsedLogic.vecChildren.PurgeAndDeleteElements();
	m_lsParsedLogic.vecLogic.Purge();
	m_mapLatchTrue.RemoveAll();
	m_mapObjectiveBinds.RemoveAll();
}

// Temp
bool LessThan_Int(const int &rLeft, const int &rRight)
{
	return rLeft < rRight;
}

bool LessThan_String(const string_t &rLeft, const string_t &rRight)
{
	return rLeft < rRight;
}

// Functions
bool CForsakenWinLogicEntity::CheckLogicStatement(tagLogicStatement *pCurrent, bool bCanShortCircuit)
/*
	
	Pre: 
	Post: 
*/
{
	// Can we short-circuit?
	if (bCanShortCircuit && pCurrent->bPreviousState)
		return true;

	bool bLeftSide = false;
	int nChildCount = 0;
	int nPreviousLogic = 0;

	for (int i = 0; i < pCurrent->vecLogic.Size(); i++)
	{
		int nElement = pCurrent->vecLogic.Element(i);

		// Is this a logic statement?
		if (nElement != LOGIC_AND && nElement != LOGIC_OR)
		{
			// Do we have previous logic?
			if (nPreviousLogic == 0)
			{
				// Child?
				if (nElement == LOGIC_CHILD)
				{
					tagLogicStatement *pChild = pCurrent->vecChildren.Element(nChildCount++);

					// Evaluate the child's logic and store it.
					bLeftSide = CheckLogicStatement(pChild, bCanShortCircuit);
					pChild->bPreviousState = bLeftSide;
				}
				// Are we true?
				else
					bLeftSide = m_mapLatchTrue[nElement];
			}
			else
			{
				// AND?
				if (nPreviousLogic == LOGIC_AND)
				{
					bool bRightSide = false;

					// Child?
					if (nElement == LOGIC_CHILD)
					{
						tagLogicStatement *pChild = pCurrent->vecChildren.Element(nChildCount++);

						// Evaluate the child's logic.
						bRightSide = CheckLogicStatement(pChild, bCanShortCircuit);
						pChild->bPreviousState = bRightSide;
					}
					// Are we true?
					else
						bRightSide = m_mapLatchTrue[nElement];

					// Is it true?
					bLeftSide = bLeftSide && bRightSide;
				}
				// OR?
				else
				{
					// We've hit an OR, so if our left side is true short circuit.
					if (bLeftSide)
						return true;

					// Child?
					if (nElement == LOGIC_CHILD)
					{
						tagLogicStatement *pChild = pCurrent->vecChildren.Element(nChildCount++);

						// Evaluate the child's logic.
						bLeftSide = CheckLogicStatement(pChild, bCanShortCircuit);
					}
					// Are we true?
					else
						bLeftSide = m_mapLatchTrue[nElement];

					nPreviousLogic = 0;
				}
			}
		}
		// Store the logic operand.
		else
			nPreviousLogic = nElement;
	}

	return bLeftSide;
}

bool CForsakenWinLogicEntity::LexicalAnalysis()
/*
	Parses the logic string into a byte stream for easy solving.
	Pre: m_czLogic is a valid logic string.
	Post: The logic string (if valid) is parsed and ready to go; otherwise we error out and 
		let the user know (just in case they are a mapper).
*/
{
	bool bExpectLogic = false, bExpectObjective = true;
	char czNewLogic[5096];
	char *czSeperators = " ";
	char *czToken = NULL;
	int nObjectiveCount = 0;
	tagLogicStatement *pCurrent = &m_lsParsedLogic;

	// Copy the string then tokenize it.
	Q_strncpy(czNewLogic, STRING(m_czLogic), sizeof(czNewLogic));
	czToken = strtok(czNewLogic, czSeperators);

	// Set our less than functions.
	m_mapObjectiveBinds.SetLessFunc(LessThan_String);
	m_mapLatchTrue.SetLessFunc(LessThan_Int);

	DevMsg("Performing Lexical Analysis on logic string '%s' for team '%d'.\n", 
		STRING(m_czLogic), m_nTeamOwner);

	// Loop through all tokens.
	while (czToken != NULL)
	{
		if (stricmp(czToken, "&&") == 0)
		{
			// We expecting a logic statmeent?
			if (bExpectLogic)
			{
				// Add it to the end of the logic.
				pCurrent->vecLogic.AddToTail(LOGIC_AND);

				// We want an objective now.
				bExpectLogic = false;
				bExpectObjective = true;
			}
			// Oops, we weren't. Error out.
			else
			{
				DevMsg("LOGIC ERROR: Expected objective, got a logic operand instead.\n");
				DevMsg("Error occurred when parsing '%s' for team '%d'.\n", czToken, m_nTeamOwner);

				m_lsParsedLogic.vecChildren.PurgeAndDeleteElements();
				m_lsParsedLogic.vecLogic.Purge();

				return false;
			}
		}
		else if (stricmp(czToken, "||") == 0)
		{
			// We expecting a logic statmeent?
			if (bExpectLogic)
			{
				// Add it to the end of the logic.
				pCurrent->vecLogic.AddToTail(LOGIC_OR);

				// We want an objective now.
				bExpectLogic = false;
				bExpectObjective = true;
			}
			// Oops, we weren't. Error out.
			else
			{
				DevMsg("LOGIC ERROR: Expected objective, got a logic operand instead.\n");
				DevMsg("Error occurred when parsing '%s' for team '%d'.\n", czToken, m_nTeamOwner);

				m_lsParsedLogic.vecChildren.PurgeAndDeleteElements();
				m_lsParsedLogic.vecLogic.Purge();

				return false;
			}
		}
		else
		{
			bool bRemovedClose = false;

			// We opening up a child logic statement?
			if (strncmp(czToken, "(", 1) == 0)
			{
				tagLogicStatement *pChild = new tagLogicStatement;

				// Let ourself know we now have a child and add it to the child list.
				pCurrent->vecChildren.AddToTail(pChild);
				pCurrent->vecLogic.AddToTail(LOGIC_CHILD);

				// We need to set the previous state to false.
				pChild->bPreviousState = false;

				// Set our parent and set the child as the current logic.
				pChild->pParent = pCurrent;
				pCurrent = pChild;

				// Don't need that pesky '('.
				czToken++;
			}
			// We closing a child logic statement?
			else if (strncmp(czToken + strlen(czToken) - 1, ")", 1) == 0)
			{
				// Don't need that pesky ')'.
				*(czToken + strlen(czToken) - 1) = 0;

				// But we do need to keep track of if we had it.
				bRemovedClose = true;
			}

			CBaseEntity *pEntity = gEntList.FindEntityByName(NULL, czToken, NULL);

			// We expecting a objective?
			if (pEntity && bExpectObjective)
			{
				// Add the objective number to the logic statement (it's what we go by).
				pCurrent->vecLogic.AddToTail(nObjectiveCount);

				// Latch the objective to false (initial state) and store it's name in the bind.
				m_mapLatchTrue.Insert(nObjectiveCount, false);
				m_mapObjectiveBinds.Insert(pEntity->GetEntityName(), nObjectiveCount++);

				// We want a logic statement now.
				bExpectLogic = true;
				bExpectObjective = false;
			}
			// Oops, we weren't. Error out.
			else if (pEntity && !bExpectObjective)
			{
				DevMsg("LOGIC ERROR: Expected a logic operand, got a objective instead.\n");
				DevMsg("Error occurred when parsing '%s' for team '%d'.\n", czToken, m_nTeamOwner);

				m_lsParsedLogic.vecChildren.PurgeAndDeleteElements();
				m_lsParsedLogic.vecLogic.Purge();

				return false;
			}
			else if (!pEntity)
				DevMsg("LOGIC WARNING: The objective '%s' could not be found.\n", czToken);

			// Did we close out a child?
			if (bRemovedClose)
			{
				// If so, we need to make sure we got a parent.
				if (pCurrent->pParent == NULL)
				{
					DevMsg("LOGIC ERROR: Unexpected ')' when parsing %s.\n", czToken);
					DevMsg("Error occurred when parsing '%s) for team '%d''.\n", czToken, m_nTeamOwner);

					m_lsParsedLogic.vecChildren.PurgeAndDeleteElements();
					m_lsParsedLogic.vecLogic.Purge();

					return false;
				}

				// Go up to the parent now.
				pCurrent = pCurrent->pParent;
			}
		}

		// Next token, please.
		czToken = strtok(NULL, czSeperators);
	}

	// Lexical analysis completed without a problem, pimp.
	DevMsg("Lexical Analysis Complete\n");

	// So we are now parsed.
	m_bParsed = true;

	return true;
}

bool CForsakenWinLogicEntity::StatementTrue(const char *czObjectiveTrue, bool bLatchTrue)
/*
	Checks to see if a logic statement has latched true.
	Pre: czObjectiveTrue is the name of the objective that latched true.
	Post: If a logic statement latched true, return true; otherwise, return false.
*/
{
	// If we haven't parsed, do lexical analysis now.
	if (!m_bParsed)
	{
		if (!LexicalAnalysis())
			return false;
	}

	int nIndex = m_mapObjectiveBinds.FirstInorder();
	int nObjective = -1;

	// Debug msg.
	DevMsg("%s - %s has latched %s\n", STRING(GetEntityName()), czObjectiveTrue, 
		(bLatchTrue ? "true" : "false"));

	// Find out which objective latched to true.
	while (m_mapObjectiveBinds.IsValidIndex(nIndex))
	{
		string_t mystr = m_mapObjectiveBinds.Key(nIndex);

		// Is this the objective we are looking for?
		if (Q_stricmp(STRING(mystr), czObjectiveTrue) == 0)
		{
			DevMsg("Found %s, latching it\n\n", czObjectiveTrue);

			// It is, so get the objective number and switch its current state.
			nObjective = m_mapObjectiveBinds.Element(nIndex);
			m_mapLatchTrue[nObjective] = bLatchTrue;

			break;
		}

		// It isn't, so next.
		nIndex = m_mapObjectiveBinds.NextInorder(nIndex);
	}

	// Re-parse the logic.
	if (CheckLogicStatement(&m_lsParsedLogic, bLatchTrue))
		return true;

	return false;
}

void CForsakenWinLogicEntity::ObjectiveChangeState(inputdata_t &rInput)
/*
	An input that lets us know a objective latched true.
	Pre: An objective actually did latch true.
	Post: If a logic statement is now true, fire our output.
*/
{
	const char *czEntityName = STRING(rInput.pCaller->GetEntityName());
	int nCompleted = rInput.value.Int();

	// Is it true?
	if (StatementTrue(czEntityName, m_nTeamOwner == nCompleted))
	{
		CReliableBroadcastRecipientFilter filter;

		DevMsg("Win logic has latched true for %d\n\n", nCompleted);

		// End the current round, somebody won.
		ForsakenGameRules()->EndCurrentRound(nCompleted);
	}
}

void CForsakenWinLogicEntity::SetTeamMapOwner(int nTeamOwner)
/*
	
	Pre: 
	Post: 
*/
{
	// Well, the team owns this map... so latch everything true.
	if (m_nTeamOwner == nTeamOwner)
	{
		int nIndex = m_mapObjectiveBinds.FirstInorder();
		int nObjective = -1;

		// Latch all objectives true.
		while (m_mapObjectiveBinds.IsValidIndex(nIndex))
		{
			string_t mystr = m_mapObjectiveBinds.Key(nIndex);

			// Latch that bad boy!
			nObjective = m_mapObjectiveBinds.Element(nIndex);
			m_mapLatchTrue[nObjective] = true;

			// It isn't, so next.
			nIndex = m_mapObjectiveBinds.NextInorder(nIndex);
		}
	}
}

void CForsakenWinLogicEntity::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	// Reset all objectives.
	for (int i = 0; i < m_mapLatchTrue.Count(); i++)
		m_mapLatchTrue[i] = false;
}