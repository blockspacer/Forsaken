/*
	forsaken_weapon_p99.cpp
	Forsaken weapon code for the Dual P99s.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_gun.h"
#include "forsaken_fx_shared.h"

#ifdef CLIENT_DLL
#define CForsakenWeaponDualP99 C_ForsakenWeaponDualP99
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#endif

// Internal Classes
class CForsakenWeaponDualP99 : public CForsakenWeaponGunBase
{
public:
	DECLARE_CLASS(CForsakenWeaponDualP99, CForsakenWeaponGunBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenWeaponDualP99();

	// Public Accessor Functions
	virtual bool IsDualie() { return true; }
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_DUALP99; }
	virtual ForsakenMuzzleFlashType GetMuzzleFlashType() const { return FSKN_MUZZLEFLASH_DUALP99; }

	// Public Functions

	// Public Variables

private:
	// Private Functions

	// Private Variables

protected:
	// Protected Functions
	virtual bool SemiAuto() { return true; }
	virtual const Vector &GetBulletSpread(ForsakenMovementID moveType);
	virtual QAngle GetRecoilAmount(ForsakenMovementID moveType);

	// Protected Variables
};

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponDualP99, DT_ForsakenWeaponDualP99)

BEGIN_NETWORK_TABLE(CForsakenWeaponDualP99, DT_ForsakenWeaponDualP99)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeaponDualP99)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_dualp99, CForsakenWeaponDualP99);
PRECACHE_WEAPON_REGISTER(forsaken_weapon_dualp99);

// Constructor & Deconstructor
CForsakenWeaponDualP99::CForsakenWeaponDualP99()
{
}

// Functions
const Vector &CForsakenWeaponDualP99::GetBulletSpread(ForsakenMovementID moveType)
/*
	
	Pre: 
	Post: 
*/
{
	static Vector retVal = VECTOR_CONE_1DEGREES;
	CForsakenPlayer *pPlayer = GetPlayerOwner();
	float fClamp;
	Vector lowBounds, upperBounds;

	// Linear clamp based on shots fired.
	fClamp = RemapValClamped(pPlayer->m_nShotsFired, 0, GetMaxClip1(), 0.0f, 1.0f);

	// Calculate the accuracy.
	switch (moveType)
	{
	case FORSAKEN_MOVEMENT_NONE:
		lowBounds = VECTOR_CONE_3DEGREES;
		upperBounds = VECTOR_CONE_5DEGREES;
		break;

	case FORSAKEN_MOVEMENT_CROUCH:
		lowBounds = VECTOR_CONE_HALFDEGREES;
		upperBounds = VECTOR_CONE_2DEGREES;
		break;

	case FORSAKEN_MOVEMENT_CROUCHWALK:
		lowBounds = VECTOR_CONE_1DEGREES;
		upperBounds = VECTOR_CONE_3DEGREES;
		break;

	case FORSAKEN_MOVEMENT_WALK:
		lowBounds = VECTOR_CONE_4DEGREES;
		upperBounds = VECTOR_CONE_6DEGREES;
		break;

	case FORSAKEN_MOVEMENT_RUN:
		lowBounds = VECTOR_CONE_6DEGREES;
		upperBounds = VECTOR_CONE_8DEGREES;
		break;

	case FORSAKEN_MOVEMENT_JUMP:
		lowBounds = VECTOR_CONE_8DEGREES;
		upperBounds = VECTOR_CONE_10DEGREES;
		break;
	}

	// Link in the number of shots fired.
	VectorLerp(lowBounds, upperBounds, fClamp, retVal);

	// Alter player accuracy dependent on situational data.
	AlterPlayerAccuracy(moveType, retVal);

	return retVal;
}

QAngle CForsakenWeaponDualP99::GetRecoilAmount(ForsakenMovementID moveType)
/*
	
	Pre:
	Post: 
*/
{
	QAngle recoilAngle;

	recoilAngle.Init();

	// What movement type are we?
	switch (moveType)
	{
	case FORSAKEN_MOVEMENT_NONE:
		recoilAngle.x -= RandomFloat(0.75, 1.25);
		break;

	case FORSAKEN_MOVEMENT_CROUCH:
		recoilAngle.x -= RandomFloat(0.25, 1.0);
		break;

	case FORSAKEN_MOVEMENT_CROUCHWALK:
		recoilAngle.x -= RandomFloat(0.5, 1.0);
		break;

	case FORSAKEN_MOVEMENT_JUMP:
		recoilAngle.x -= RandomFloat(2.0, 3.5);
		break;

	case FORSAKEN_MOVEMENT_RUN:
		recoilAngle.x -= RandomFloat(1.0, 1.75);
		break;

	case FORSAKEN_MOVEMENT_WALK:
		recoilAngle.x -= RandomFloat(0.75, 1.75);
		break;

	default:
		break;
	}

	// Alter player recoil dependent on situational data.
	AlterPlayerRecoil(moveType, recoilAngle);

	return recoilAngle;
}