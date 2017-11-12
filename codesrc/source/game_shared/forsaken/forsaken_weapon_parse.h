#ifndef __FORSAKEN_WEAPON_PARSE_H_
#define __FORSAKEN_WEAPON_PARSE_H_

#include "weapon_parse.h"
#include "networkvar.h"

class CForsakenWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT(CForsakenWeaponInfo, FileWeaponInfo_t);

	// Constructor & Deconstructor
	CForsakenWeaponInfo();

	// Public Accessor Functions

	// Public Functions
	virtual void Parse(::KeyValues *pKeyValuesData, const char *czWeaponName);

	// Public Variables
	char m_czAnimExtension[16];
	float m_fCycleTime;
	int m_nBullets;
	int m_nDamage;
};

#endif