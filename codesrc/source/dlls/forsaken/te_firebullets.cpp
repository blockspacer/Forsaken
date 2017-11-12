/*
	te_firebullets.cpp
	Server-side bullet firing code.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "basetempentity.h"

// Internal Classes
class CTEFireBullets : public CBaseTempEntity
{
public:
	DECLARE_CLASS(CTEFireBullets, CBaseTempEntity);
	DECLARE_SERVERCLASS();

	// Constructor & Deconstructor
	CTEFireBullets(const char *czName);
	virtual ~CTEFireBullets();

	// Public Accessor Functions

	// Public Functions

	// Public Variables
	CNetworkVar(float, m_fSpread);
	CNetworkVar(int, m_nMode);
	CNetworkVar(int, m_nPlayer);
	CNetworkVar(int, m_nSeed);
	CNetworkVar(int, m_nWeaponID);
	CNetworkVector(m_vecOrigin);
	CNetworkQAngle(m_vecAngles);
};

// Global Variable Decleration
static CTEFireBullets g_TEFireBullets("Shotgun Shot?");

// HL2 Class Macros
IMPLEMENT_SERVERCLASS_ST_NOBASE(CTEFireBullets, DT_TEFireBullets)
	SendPropVector(SENDINFO(m_vecOrigin), -1, SPROP_COORD),
	SendPropAngle(SENDINFO_VECTORELEM(m_vecAngles, 0), 13, 0),
	SendPropAngle(SENDINFO_VECTORELEM(m_vecAngles, 1), 13, 0),
	SendPropInt(SENDINFO(m_nWeaponID), 5, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_nMode), 1, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_nSeed), 8, SPROP_UNSIGNED),
	SendPropInt(SENDINFO(m_nPlayer), 6, SPROP_UNSIGNED),
	SendPropFloat(SENDINFO(m_fSpread), 10, 0, 0, 1),
END_SEND_TABLE()

// Constructor & Deconstructor
CTEFireBullets::CTEFireBullets(const char *czName) : CBaseTempEntity(czName)
{
}

CTEFireBullets::~CTEFireBullets()
{
}

// Functions
// Global
void TE_FireBullets(int nPlayerIndex, const Vector &vecOrigin, const QAngle &angles, int nWeaponID, 
					int nMode, int nSeed, float fSpread)
{
	CPASFilter filter(vecOrigin);

	filter.UsePredictionRules();

	g_TEFireBullets.m_nPlayer = nPlayerIndex - 1;
	g_TEFireBullets.m_vecOrigin = vecOrigin;
	g_TEFireBullets.m_fSpread = fSpread;
	g_TEFireBullets.m_nMode = nMode;
	g_TEFireBullets.m_nSeed = nSeed;
	g_TEFireBullets.m_nWeaponID = nWeaponID;
	g_TEFireBullets.m_vecAngles = angles;

	g_TEFireBullets.Create(filter, 0);
}