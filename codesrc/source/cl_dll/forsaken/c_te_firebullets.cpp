/*
	c_te_firebullets.cpp
	Bullet temporary entities.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken/forsaken_fx_shared.h"
#include "forsaken/c_forsaken_player.h"
#include "c_basetempentity.h"
#include <cliententitylist.h>

// Internal Classes
class C_TEFireBullets : public C_BaseTempEntity
{
public:
	DECLARE_CLASS(C_TEFireBullets, C_BaseTempEntity);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor

	// Public Accessor Functions

	// Public Functions
	virtual void PostDataUpdate(DataUpdateType_t dutUpdate);

	// Public Variables
	float m_fSpread;
	int m_nMode;
	int m_nPlayer;
	int m_nSeed;
	int m_nWeaponID;
	QAngle m_vecAngles;
	Vector m_vecOrigin;
};

// HL2 Class Macros
IMPLEMENT_CLIENTCLASS_EVENT(C_TEFireBullets, DT_TEFireBullets, CTEFireBullets)

BEGIN_RECV_TABLE_NOBASE(C_TEFireBullets, DT_TEFireBullets)
	RecvPropVector(RECVINFO(m_vecOrigin)),
	RecvPropFloat(RECVINFO(m_vecAngles[0])),
	RecvPropFloat(RECVINFO(m_vecAngles[1])),
	RecvPropInt(RECVINFO(m_nWeaponID)),
	RecvPropInt(RECVINFO(m_nMode)),
	RecvPropInt(RECVINFO(m_nSeed)),
	RecvPropInt(RECVINFO(m_nPlayer)),
	RecvPropFloat(RECVINFO(m_fSpread)),
END_RECV_TABLE()

// Constructor & Deconstructor

// Functions
// C_TEFireBullets
void C_TEFireBullets::PostDataUpdate(DataUpdateType_t dutUpdate)
/*
	
	Pre: 
	Post: 
*/
{
	m_vecAngles.z = 0;

	FX_FireBullets(m_nPlayer + 1, m_vecOrigin, m_vecAngles, m_nWeaponID, m_nMode, m_nSeed, 
		m_fSpread);
}