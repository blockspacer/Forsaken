#ifndef __FORSAKEN_MAPENTITY_FILTER_H_
#define __FORSAKEN_MAPENTITY_FILTER_H_

class CForsakenMapEntityFilter : public IMapEntityFilter
{
	typedef const char* charPtr;
public:
	// Constructor & Deconstructor
	CForsakenMapEntityFilter();
	virtual ~CForsakenMapEntityFilter();

	// Public Functions
	virtual bool ShouldCreateEntity(const char *czClassname);
	virtual CBaseEntity *CreateNextEntity(const char *czClassname);
	void AddKeepEntity(const char *czClassname);
	void RemoveKeepEntity(const char *czClassname);

	// Public Variables
	CUtlVector<char*> m_vecKeepEntities;
};

#endif