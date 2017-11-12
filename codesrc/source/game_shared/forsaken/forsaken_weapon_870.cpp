/*
	forsaken_weapon_870.cpp
	Forsaken weapon code for the Remington 870 Shotgun.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_gun.h"
#include "forsaken_weapon_base_shotgun.h"
#include "forsaken_fx_shared.h"

#ifdef CLIENT_DLL
#define CForsakenWeapon870Remington C_ForsakenWeapon870Remington
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#endif

// Internal Classes
class CForsakenWeapon870Remington : public CForsakenWeaponShotgunBase
{
public:
	DECLARE_CLASS(CForsakenWeapon870Remington, CForsakenWeaponShotgunBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenWeapon870Remington();

	// Public Accessor Functions
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_870REMINGTON; }
	virtual ForsakenMuzzleFlashType GetMuzzleFlashType() const { return FSKN_MUZZLEFLASH_870REMINGTON; }

	// Public Functions

	// Public Variables

private:
	// Private Functions

	// Private Variables

protected:
	// Protected Functions
	virtual const Vector &GetBulletSpread(ForsakenMovementID moveType);
	virtual QAngle GetRecoilAmount(ForsakenMovementID moveType);

	// Protected Variables
};

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeapon870Remington, DT_ForsakenWeapon870Remington)

BEGIN_NETWORK_TABLE(CForsakenWeapon870Remington, DT_ForsakenWeapon870Remington)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeapon870Remington)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_870remington, CForsakenWeapon870Remington);
PRECACHE_WEAPON_REGISTER(forsaken_weapon_870remington);

// Constructor & Deconstructor
CForsakenWeapon870Remington::CForsakenWeapon870Remington()
{
}

// Functions
const Vector &CForsakenWeapon870Remington::GetBulletSpread(ForsakenMovementID moveType)
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
		lowBounds = VECTOR_CONE_8DEGREES;
		upperBounds = VECTOR_CONE_10DEGREES;
		break;

	case FORSAKEN_MOVEMENT_CROUCH:
		lowBounds = VECTOR_CONE_6DEGREES;
		upperBounds = VECTOR_CONE_8DEGREES;
		break;

	case FORSAKEN_MOVEMENT_CROUCHWALK:
		lowBounds = VECTOR_CONE_7DEGREES;
		upperBounds = VECTOR_CONE_9DEGREES;
		break;

	case FORSAKEN_MOVEMENT_WALK:
		lowBounds = VECTOR_CONE_9DEGREES;
		upperBounds = VECTOR_CONE_11DEGREES;
		break;

	case FORSAKEN_MOVEMENT_RUN:
		lowBounds = VECTOR_CONE_11DEGREES;
		upperBounds = VECTOR_CONE_13DEGREES;
		break;

	case FORSAKEN_MOVEMENT_JUMP:
		lowBounds = VECTOR_CONE_13DEGREES;
		upperBounds = VECTOR_CONE_15DEGREES;
		break;
	}

	// Link in the number of shots fired.
	VectorLerp(lowBounds, upperBounds, fClamp, retVal);

	// Alter player accuracy dependent on situational data.
	AlterPlayerAccuracy(moveType, retVal);

	return retVal;
}

QAngle CForsakenWeapon870Remington::GetRecoilAmount(ForsakenMovementID moveType)
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
		recoilAngle.x -= RandomInt(8, 11);
		break;

	case FORSAKEN_MOVEMENT_CROUCH:
		recoilAngle.x -= RandomInt(4, 6);
		break;

	case FORSAKEN_MOVEMENT_CROUCHWALK:
		recoilAngle.x -= RandomInt(3, 5);
		break;

	case FORSAKEN_MOVEMENT_JUMP:
		recoilAngle.x -= RandomInt(10, 13);
		break;

	case FORSAKEN_MOVEMENT_RUN:
		recoilAngle.x -= RandomInt(10, 11);
		break;

	case FORSAKEN_MOVEMENT_WALK:
		recoilAngle.x -= RandomInt(9, 11);
		break;

	default:
		break;
	}

	// Alter player recoil dependent on situational data.
	AlterPlayerRecoil(moveType, recoilAngle);

	return recoilAngle;
}