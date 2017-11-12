#ifndef __FORSAKEN_WINLOGIC_ENTITY_H_
#define __FORSAKEN_WINLOGIC_ENTITY_H_

#include "utlmap.h"

#define LOGIC_AND		-1
#define LOGIC_OR		-2
#define LOGIC_CHILD		-3

struct tagLogicStatement
{
	tagLogicStatement()
	{
		bPreviousState = false;
		pParent = NULL;
	}

	~tagLogicStatement()
	{
		pParent = NULL;

		vecChildren.PurgeAndDeleteElements();
		vecLogic.Purge();
	}

	tagLogicStatement *pParent;

	bool bPreviousState;
	CUtlVector<int> vecLogic;
	CUtlVector<tagLogicStatement*> vecChildren;
};

class CForsakenWinLogicEntity : public CLogicalEntity
{
public:
	DECLARE_CLASS(CForsakenWinLogicEntity, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor & Deconstructor
	CForsakenWinLogicEntity();
	virtual ~CForsakenWinLogicEntity();

	// Public Accessor Functions
	void SetTeamMapOwner(int nTeamOwner);

	// Public Functions
	virtual void Spawn();
	void ObjectiveChangeState(inputdata_t &rInput);

	// Public Variables

protected:
	// Protected Functions
	bool CheckLogicStatement(tagLogicStatement *pCurrent, bool bCanShortCircuit);
	bool LexicalAnalysis();
	bool StatementTrue(const char *czObjectiveTrue, bool bLatchTrue = true);

	// Protected Variables
	bool m_bParsed;
	CUtlMap<string_t, int> m_mapObjectiveBinds;
	CUtlMap<int, bool> m_mapLatchTrue;
	int m_nTeamOwner;
	string_t m_czLogic;
	tagLogicStatement m_lsParsedLogic;
};

#endif