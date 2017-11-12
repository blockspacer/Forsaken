#ifndef __FORSAKEN_OBJECTIVE_ENTITY_H_
#define __FORSAKEN_OBJECTIVE_ENTITY_H_

class CForsakenPlayer;

class CForsakenObjectiveEntity : public CBaseAnimating
{
public:
	DECLARE_CLASS(CForsakenObjectiveEntity, CBaseAnimating);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	// Constructor & Deconstructor
	CForsakenObjectiveEntity();
	virtual ~CForsakenObjectiveEntity();

	// Public Accessor Functions
	const char *GetObjectiveName() { return m_czObjectiveName.Get(); }
	int GetTeamOwners() { return m_nTeamOwner; }
	virtual bool IsPlayerAffectingObjective(CForsakenPlayer *pPlayer) { return false; }
	virtual int GetObjectiveType() { return OBJECTIVE_TYPE_BASE; }
	virtual void SetTeamMapOwner(int nTeamOwner) { Assert(0); }

	// Public Functions
	bool KeyValue(const char *czKeyName, const char *czValue);
	virtual int UpdateTransmitState() { return FL_EDICT_ALWAYS; }

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	CNetworkString(m_czObjectiveName, OBJECTIVE_NAME_LENGTH);
	CNetworkVar(int, m_nTeamOwner);
	COutputInt m_ObjectiveCompleted;
};

#endif