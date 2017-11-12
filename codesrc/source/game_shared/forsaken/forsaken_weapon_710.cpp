/*
	forsaken_weapon_710.cpp
	Forsaken weapon code for the Remington 710 Sniper Rifle.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_gun.h"
#include "forsaken_weapon_base_rifle.h"
#include "forsaken_fx_shared.h"

#ifdef CLIENT_DLL
#define CForsakenWeapon710 C_ForsakenWeapon710
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#include "forsaken/te_firebullets.h"
#endif

// Internal Classes
class CForsakenWeapon710 : public CForsakenWeaponRifleBase
{
public:
	DECLARE_CLASS(CForsakenWeapon710, CForsakenWeaponRifleBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenWeapon710();

	// Public Accessor Functions
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_710; }
	virtual ForsakenMuzzleFlashType GetMuzzleFlashType() const { return FSKN_MUZZLEFLASH_710; }

	// Public Functions

	// Public Variables

protected:
	// Protected Functions
	virtual bool SemiAuto() { return true; }
	virtual const Vector &GetBulletSpread(ForsakenMovementID moveType);
	virtual float GetZoomRate() { return 0.25f; }
	virtual float GetZoomWait() { return 0.1f; }
	virtual int GetFOVForZoom(int nZoomLevel)
	{
		switch (nZoomLevel)
		{
		case 1:
			return 60;
		case 2:
			return 25;
		default:
			return 0;
		}
	}
	virtual int GetMaxZoomLevel() { return 2; }
	virtual QAngle GetRecoilAmount(ForsakenMovementID moveType);

	// Protected Variables
};

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeapon710, DT_ForsakenWeapon710)

BEGIN_NETWORK_TABLE(CForsakenWeapon710, DT_ForsakenWeapon710)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeapon710)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_710, CForsakenWeapon710);
PRECACHE_WEAPON_REGISTER(forsaken_weapon_710);

// Constructor & Deconstructor
CForsakenWeapon710::CForsakenWeapon710()
{
}

// Functions
const Vector &CForsakenWeapon710::GetBulletSpread(ForsakenMovementID moveType)
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
	if (GetZoomLevel() == 1)
	{
		switch (moveType)
		{
		case FORSAKEN_MOVEMENT_NONE:
			lowBounds = VECTOR_CONE_3DEGREES;
			upperBounds = VECTOR_CONE_4DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCH:
			lowBounds = VECTOR_CONE_1DEGREES;
			upperBounds = VECTOR_CONE_2DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCHWALK:
			lowBounds = VECTOR_CONE_2DEGREES;
			upperBounds = VECTOR_CONE_3DEGREES;
			break;

		case FORSAKEN_MOVEMENT_WALK:
			lowBounds = VECTOR_CONE_4DEGREES;
			upperBounds = VECTOR_CONE_5DEGREES;
			break;

		case FORSAKEN_MOVEMENT_RUN:
			lowBounds = VECTOR_CONE_6DEGREES;
			upperBounds = VECTOR_CONE_8DEGREES;
			break;

		case FORSAKEN_MOVEMENT_JUMP:
			lowBounds = VECTOR_CONE_10DEGREES;
			upperBounds = VECTOR_CONE_12DEGREES;
			break;
		}
	}
	else if (GetZoomLevel() == 2)
	{
		switch (moveType)
		{
		case FORSAKEN_MOVEMENT_NONE:
			lowBounds = VECTOR_CONE_1DEGREES;
			upperBounds = VECTOR_CONE_2DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCH:
			lowBounds = VECTOR_CONE_PRECALCULATED;
			upperBounds = VECTOR_CONE_HALFDEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCHWALK:
			lowBounds = VECTOR_CONE_HALFDEGREES;
			upperBounds = VECTOR_CONE_1DEGREES;
			break;

		case FORSAKEN_MOVEMENT_WALK:
			lowBounds = VECTOR_CONE_2DEGREES;
			upperBounds = VECTOR_CONE_3DEGREES;
			break;

		case FORSAKEN_MOVEMENT_RUN:
			lowBounds = VECTOR_CONE_3DEGREES;
			upperBounds = VECTOR_CONE_6DEGREES;
			break;

		case FORSAKEN_MOVEMENT_JUMP:
			lowBounds = VECTOR_CONE_10DEGREES;
			upperBounds = VECTOR_CONE_11DEGREES;
			break;
		}
	}
	else
	{
		switch (moveType)
		{
		case FORSAKEN_MOVEMENT_NONE:
			lowBounds = VECTOR_CONE_4DEGREES;
			upperBounds = VECTOR_CONE_6DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCH:
			lowBounds = VECTOR_CONE_2DEGREES;
			upperBounds = VECTOR_CONE_3DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCHWALK:
			lowBounds = VECTOR_CONE_3DEGREES;
			upperBounds = VECTOR_CONE_4DEGREES;
			break;

		case FORSAKEN_MOVEMENT_WALK:
			lowBounds = VECTOR_CONE_6DEGREES;
			upperBounds = VECTOR_CONE_8DEGREES;
			break;

		case FORSAKEN_MOVEMENT_RUN:
			lowBounds = VECTOR_CONE_8DEGREES;
			upperBounds = VECTOR_CONE_10DEGREES;
			break;

		case FORSAKEN_MOVEMENT_JUMP:
			lowBounds = VECTOR_CONE_11DEGREES;
			upperBounds = VECTOR_CONE_14DEGREES;
			break;
		}
	}

	// Link in the number of shots fired.
	VectorLerp(lowBounds, upperBounds, fClamp, retVal);

	// Alter player accuracy dependent on situational data.
	AlterPlayerAccuracy(moveType, retVal);

	return retVal;
}

QAngle CForsakenWeapon710::GetRecoilAmount(ForsakenMovementID moveType)
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
		recoilAngle.x -= RandomInt(3, 4);
		break;

	case FORSAKEN_MOVEMENT_CROUCH:
		recoilAngle.x -= RandomInt(1, 2);
		break;

	case FORSAKEN_MOVEMENT_CROUCHWALK:
		recoilAngle.x -= RandomInt(1, 3);
		break;

	case FORSAKEN_MOVEMENT_JUMP:
		recoilAngle.x -= RandomInt(3, 6);
		break;

	case FORSAKEN_MOVEMENT_RUN:
		recoilAngle.x -= RandomInt(2, 5);
		break;

	case FORSAKEN_MOVEMENT_WALK:
		recoilAngle.x -= RandomInt(1, 5);
		break;

	default:
		break;
	}

	// Alter player recoil dependent on situational data.
	AlterPlayerRecoil(moveType, recoilAngle);

	return recoilAngle;
}