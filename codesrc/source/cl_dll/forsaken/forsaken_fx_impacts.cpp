/*
	forsaken_fx_impacts.cpp
	Impact effects for Forsaken.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "fx_impact.h"
#include "engine/IEngineSound.h"
#include "IEffects.h"

// Function Prototypes
void ImpactCallback(const CEffectData &redData);

// HL2 Class Macros
DECLARE_CLIENT_EFFECT("Impact", ImpactCallback);

// Functions
void ImpactCallback(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	// TODO: Perform the impact.
	trace_t tr;
	Vector vecOrigin, vecStart, vecShotDir;
	int iMaterial, iDamageType, iHitbox;
	short nSurfaceProp;

	C_BaseEntity *pEntity = ParseImpactData( redData, &vecOrigin, &vecStart, &vecShotDir, nSurfaceProp, iMaterial, iDamageType, iHitbox );

	if ( !pEntity )
		return;

	// If we hit, perform our custom effects and play the sound
	if ( Impact( vecOrigin, vecStart, iMaterial, iDamageType, iHitbox, pEntity, tr ) )
	{
		// Check for custom effects based on the Decal index
		PerformCustomEffects( vecOrigin, tr, vecShotDir, iMaterial, 1.0 );

		//Play a ricochet sound some of the time
		if( random->RandomInt(1,10) <= 3 && (iDamageType == DMG_BULLET) )
			g_pEffects->Ricochet(vecOrigin, vecShotDir);
	}

	PlayImpactSound( pEntity, tr, vecOrigin, nSurfaceProp );
}