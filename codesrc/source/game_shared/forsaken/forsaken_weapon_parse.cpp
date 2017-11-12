/*
	forsaken_weapon_parse.cpp
	Weapon parser for all Forsaken weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "KeyValues.h"
#include "forsaken_weapon_parse.h"

// Function Prototypes
FileWeaponInfo_t *CreateWeaponInfo();

// Constructor & Deconstructor
CForsakenWeaponInfo::CForsakenWeaponInfo()
{
}

// Functions
// CForsakenWeaponInfo
void CForsakenWeaponInfo::Parse(::KeyValues *pKeyValuesData, const char *czWeaponName)
/*
	
	Pre: 
	Post: 
*/
{
	const char *czAnimExtension = NULL;
	BaseClass::Parse(pKeyValuesData, czWeaponName);

	m_nDamage = pKeyValuesData->GetInt("Damage", 42);
	m_nBullets = pKeyValuesData->GetInt("Bullets", 1);
	m_fCycleTime = pKeyValuesData->GetFloat("CycleTime", 0.15f);
	czAnimExtension = pKeyValuesData->GetString("PlayerAnimationExtension", "1201fp");

	Q_strncpy(m_czAnimExtension, czAnimExtension, sizeof(m_czAnimExtension));
}

// Global
FileWeaponInfo_t *CreateWeaponInfo()
/*
	
	Pre: 
	Post: 
*/
{
	return new CForsakenWeaponInfo;
}