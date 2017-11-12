/*
	forsaken_weapon_sa80.cpp
	Forsaken weapon code for the SA80.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_weapon_base.h"
#include "forsaken_weapon_base_gun.h"
#include "forsaken_weapon_base_rifle.h"
#include "forsaken_fx_shared.h"

#ifdef CLIENT_DLL
#define CForsakenWeaponSA80 C_ForsakenWeaponSA80
#include "forsaken/c_forsaken_player.h"
#else
#include "forsaken/forsaken_player.h"
#include "forsaken/te_firebullets.h"
#endif

// Internal Classes
class CForsakenWeaponSA80 : public CForsakenWeaponRifleBase
{
public:
	DECLARE_CLASS(CForsakenWeaponSA80, CForsakenWeaponRifleBase);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Constructor & Deconstructor
	CForsakenWeaponSA80();

	// Public Accessor Functions
	virtual ForsakenWeaponID GetWeaponID() const { return FORSAKEN_WEAPON_SA80; }
	virtual ForsakenMuzzleFlashType GetMuzzleFlashType() const { return FSKN_MUZZLEFLASH_SA80; }

	// Public Functions

	// Public Variables

protected:
	// Protected Functions
	virtual bool SemiAuto() { return (m_nZoomLevel != 0); }
	virtual const Vector &GetBulletSpread(ForsakenMovementID moveType);
	virtual float GetZoomRate() { return 0.2f; }
	virtual float GetZoomWait() { return 0.1f; }
	virtual int GetFOVForZoom(int nZoomLevel) { return (!nZoomLevel) ? 0 : 60; }
	virtual int GetMaxZoomLevel() { return 1; }
	virtual QAngle GetRecoilAmount(ForsakenMovementID moveType);

	// Protected Variables
};

// HL2 Class Macros
IMPLEMENT_NETWORKCLASS_ALIASED(ForsakenWeaponSA80, DT_ForsakenWeaponSA80)

BEGIN_NETWORK_TABLE(CForsakenWeaponSA80, DT_ForsakenWeaponSA80)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CForsakenWeaponSA80)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(forsaken_weapon_sa80, CForsakenWeaponSA80);
PRECACHE_WEAPON_REGISTER(forsaken_weapon_sa80);

// Constructor & Deconstructor
CForsakenWeaponSA80::CForsakenWeaponSA80()
{
}

// Functions
const Vector &CForsakenWeaponSA80::GetBulletSpread(ForsakenMovementID moveType)
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
		case FORSAKEN_MOVEMENT_WALK:
			lowBounds = VECTOR_CONE_2DEGREES;
			upperBounds = VECTOR_CONE_4DEGREES;
			break;

		case FORSAKEN_MOVEMENT_RUN:
			lowBounds = VECTOR_CONE_4DEGREES;
			upperBounds = VECTOR_CONE_7DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCH:
			lowBounds = VECTOR_CONE_1DEGREES / 2;
			upperBounds = VECTOR_CONE_1DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCHWALK:
			lowBounds = VECTOR_CONE_1DEGREES;
			upperBounds = VECTOR_CONE_2DEGREES;
			break;

		case FORSAKEN_MOVEMENT_JUMP:
			lowBounds = VECTOR_CONE_5DEGREES;
			upperBounds = VECTOR_CONE_9DEGREES;
			break;

		case FORSAKEN_MOVEMENT_NONE:
			lowBounds = VECTOR_CONE_1DEGREES;
			upperBounds = VECTOR_CONE_3DEGREES;
			break;

		default:
			break;
		}
	}
	else
	{
		switch (moveType)
		{
		case FORSAKEN_MOVEMENT_WALK:
			lowBounds = VECTOR_CONE_3DEGREES;
			upperBounds = VECTOR_CONE_5DEGREES;
			break;

		case FORSAKEN_MOVEMENT_RUN:
			lowBounds = VECTOR_CONE_4DEGREES;
			upperBounds = VECTOR_CONE_7DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCH:
			lowBounds = VECTOR_CONE_1DEGREES / 2;
			upperBounds = VECTOR_CONE_1DEGREES;
			break;

		case FORSAKEN_MOVEMENT_CROUCHWALK:
			lowBounds = VECTOR_CONE_1DEGREES;
			upperBounds = VECTOR_CONE_2DEGREES;
			break;

		case FORSAKEN_MOVEMENT_JUMP:
			lowBounds = VECTOR_CONE_7DEGREES;
			upperBounds = VECTOR_CONE_9DEGREES;
			break;

		case FORSAKEN_MOVEMENT_NONE:
			lowBounds = VECTOR_CONE_2DEGREES;
			upperBounds = VECTOR_CONE_3DEGREES;
			break;

		default:
			break;
		}
	}

	// Link in the number of shots fired.
	VectorLerp(lowBounds, upperBounds, fClamp, retVal);

	// Alter player accuracy dependent on situational data.
	AlterPlayerAccuracy(moveType, retVal);

	return retVal;
}

QAngle CForsakenWeaponSA80::GetRecoilAmount(ForsakenMovementID moveType)
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
		recoilAngle.x -= RandomFloat(0.75, 1.0);
		break;

	case FORSAKEN_MOVEMENT_CROUCH:
		recoilAngle.x -= RandomFloat(0.25, 0.75);
		break;

	case FORSAKEN_MOVEMENT_CROUCHWALK:
		recoilAngle.x -= RandomFloat(0.5, 0.75);
		break;

	case FORSAKEN_MOVEMENT_JUMP:
		recoilAngle.x -= RandomFloat(2.0, 3.5);
		break;

	case FORSAKEN_MOVEMENT_RUN:
		recoilAngle.x -= RandomFloat(1.0, 1.25);
		break;

	case FORSAKEN_MOVEMENT_WALK:
		recoilAngle.x -= RandomFloat(0.75, 1.25);
		break;

	default:
		break;
	}

	// Alter player recoil dependent on situational data.
	AlterPlayerRecoil(moveType, recoilAngle);

	return recoilAngle;
}