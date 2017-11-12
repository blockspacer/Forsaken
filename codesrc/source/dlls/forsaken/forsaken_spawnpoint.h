#ifndef __FORSAKEN_SPAWNPOINT_H_
#define __FORSAKEN_SPAWNPOINT_H_

#include "baseentity.h"
#include "entityoutput.h"

class CForsakenTeam;

class IForsakenSpawnPoint : public CPointEntity
{
	DECLARE_CLASS(IForsakenSpawnPoint, CPointEntity);

public:
	// Public Functions
	virtual bool IsValid(CBasePlayer *pPlayer)
	{
		CBaseEntity *ent = NULL;

		for (CEntitySphereQuery sphere(GetAbsOrigin(), FORSAKEN_SPAWN_RADIUS); 
			(ent = sphere.GetCurrentEntity()) != NULL; 
			sphere.NextEntity())
		{
			CBaseEntity *plent = ent;

			if ( plent && plent->IsPlayer() && plent != pPlayer )
				return false;
		}

		return true;
	}

	virtual void Activate()
	{
		BaseClass::Activate();
	}

	// Public Variables
	COutputEvent m_OnPlayerSpawn;
};

class CForsakenSpawnPoint_Civ : public IForsakenSpawnPoint
{
	DECLARE_CLASS(CForsakenSpawnPoint_Civ, IForsakenSpawnPoint);

public:
	// Constructor & Deconstructor
	virtual ~CForsakenSpawnPoint_Civ();

	// Public Functions
	virtual void Activate();

	// Public Variables

protected:
	DECLARE_DATADESC();
};

class CForsakenSpawnPoint_Gov : public IForsakenSpawnPoint
{
	DECLARE_CLASS(CForsakenSpawnPoint_Gov, IForsakenSpawnPoint);

public:
	// Constructor & Deconstructor
	virtual ~CForsakenSpawnPoint_Gov();

	// Public Functions
	virtual void Activate();

	// Public Variables

protected:
	DECLARE_DATADESC();
};

class CForsakenSpawnPoint_Rel : public IForsakenSpawnPoint
{
	DECLARE_CLASS(CForsakenSpawnPoint_Rel, IForsakenSpawnPoint);

public:
	// Constructor & Deconstructor
	virtual ~CForsakenSpawnPoint_Rel();

	// Public Functions
	virtual void Activate();

	// Public Variables

protected:
	DECLARE_DATADESC();
};

class CForsakenSpawnPoint_Join : public IForsakenSpawnPoint
{
	DECLARE_CLASS(CForsakenSpawnPoint_Join, IForsakenSpawnPoint);

public:
	// Constructor & Deconstructor
	virtual ~CForsakenSpawnPoint_Join();

	// Public Functions
	virtual void Activate();

	// Public Variables

protected:
	DECLARE_DATADESC();
};

class CForsakenSpawnPoint_Objective : public IForsakenSpawnPoint
{
	DECLARE_CLASS(CForsakenSpawnPoint_Objective, IForsakenSpawnPoint);

public:
	// Constructor & Deconstructor
	CForsakenSpawnPoint_Objective();
	virtual ~CForsakenSpawnPoint_Objective();

	// Public Functions
	virtual void Activate();

	// Public Variables
	string_t m_czObjectiveName;

protected:
	DECLARE_DATADESC();
};

#endif