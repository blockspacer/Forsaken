/*
	forsaken_fx_shared.cpp
	Forsaken effects.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "forsaken_fx_shared.h"
#include "forsaken_weapon_base.h"

#ifndef CLIENT_DLL
#include "ilagcompensationmanager.h"
#endif

#ifdef CLIENT_DLL

#include "fx_impact.h"
#include "forsaken/c_forsaken_player.h"

// Internal Classes
class CGroupedSound
{
public:
	// Public Variables
	string_t m_strSoundName;
	Vector m_vecPosition;
};

// Global Variables
CUtlVector<CGroupedSound> g_GroupedSounds;

// Functions
void EndGroupingSounds()
/*
	
	Pre: 
	Post: 
*/
{
	g_GroupedSounds.Purge();

	SetImpactSoundRoute(NULL);
}

void FX_WeaponSound(int nPlayerIndex, WeaponSound_t wsSound, const Vector &rvecOrigin, 
					CForsakenWeaponInfo *pWeaponInfo)
/*
	
	Pre: 
	Post: 
*/
{
	const char *czSoundName = pWeaponInfo->aShootSounds[wsSound];
	if (!czSoundName || !czSoundName[0])
		return;

	CBroadcastRecipientFilter filter;

	if (!te->CanPredict())
		return;

	CBaseEntity::EmitSound(filter, nPlayerIndex, czSoundName, &rvecOrigin);
}

void ShotgunImpactSoundGroup( const char *czSoundName, const Vector &rvecEndPos )
/*
	
	Pre: 
	Post: 
*/
{
	for (int i = 0; i < g_GroupedSounds.Count(); i++)
	{
		CGroupedSound *pSound = &g_GroupedSounds[i];

		if (rvecEndPos.DistToSqr(pSound->m_vecPosition) < 90000)
		{
			if (Q_stricmp(pSound->m_strSoundName, czSoundName) == 0)
				return;
		}
	}

	CLocalPlayerFilter filter;

	C_BaseEntity::EmitSound(filter, NULL, czSoundName, &rvecEndPos);

	int tmp = g_GroupedSounds.AddToTail();

	g_GroupedSounds[tmp].m_strSoundName = czSoundName;
	g_GroupedSounds[tmp].m_vecPosition = rvecEndPos;
}

void StartGroupingSounds()
/*
	
	Pre: 
	Post: 
*/
{
	SetImpactSoundRoute(ShotgunImpactSoundGroup);
}

#else

#include "forsaken/forsaken_player.h"
#include "forsaken/te_firebullets.h"

// Function Prototypes
void TE_DynamicLight( IRecipientFilter& filter, float delay,
	const Vector* org, int r, int g, int b, int exponent, float radius, float time, float decay );

// Functions
void EndGroupingSounds()
/*
	
	Pre: 
	Post: 
*/
{
}

void FX_WeaponSound(int nPlayerIndex, WeaponSound_t wsSound, const Vector &rvecOrigin, 
					CForsakenWeaponInfo *pWeaponInfo)
/*
	
	Pre: 
	Post: 
*/
{
}

void StartGroupingSounds()
/*
	
	Pre: 
	Post: 
*/
{
}

#endif

// Functions
void FX_FireBullets(int nPlayer, const Vector &rvecOrigin, const QAngle &rangVector, 
					int nWeaponID, int nMode, int nSeed, float fSpread)
/*
	
	Pre: 
	Post: 
*/
{
	bool bDoEffects = true;
	char czWeaponName[128] = "";
	CForsakenWeaponInfo *pWeaponInfo = NULL;
	const char *czWeaponAlias = WeaponIDToAlias(nWeaponID);
	WEAPON_FILE_INFO_HANDLE hWeaponInfo;

	// Get the player.
#ifdef CLIENT_DLL
	C_ForsakenPlayer *pPlayer = ToForsakenPlayer(ClientEntityList().GetBaseEntity(nPlayer));
#else
	CForsakenPlayer *pPlayer = ToForsakenPlayer(UTIL_PlayerByIndex(nPlayer));
#endif

	// Do we have a weapon alias?
	if (!czWeaponAlias)
	{
		DevMsg("FX_FireBullets: Couldn't find the weapon alias for ID %d.\n", nWeaponID);

		return;
	}

	// Get the weapon name.
	Q_snprintf(czWeaponName, sizeof(czWeaponName), "forsaken_weapon_%s", czWeaponAlias);

	// Obtain the weapon info.
	hWeaponInfo = LookupWeaponInfoSlot(czWeaponName);

	// Do we have a valid handle to the weapon info?
	if (hWeaponInfo == GetInvalidWeaponInfoHandle())
	{
		DevMsg("FX_FireBullets: Can't find the weapon info slot for %s.\n", czWeaponName);

		return;
	}

	// Obtain a pointer to the weapon info.
	pWeaponInfo = static_cast<CForsakenWeaponInfo*>(GetFileWeaponInfoFromHandle(hWeaponInfo));

#ifdef CLIENT_DLL
	// Play the proper animation.
	if (pPlayer && !pPlayer->IsDormant())
	{
		if (nMode == FORSAKEN_WEAPON_PRIMARY_MODE)
			pPlayer->m_statePlayerAnimation->DoAnimationEvent(FORSAKEN_PLAYERANIMEVENT_GUN_FIRE_PRIMARY);
		else
			pPlayer->m_statePlayerAnimation->DoAnimationEvent(FORSAKEN_PLAYERANIMEVENT_GUN_FIRE_SECONDARY);
	}
#else
	Vector tmpVector = pPlayer->Weapon_ShootPosition();
	CPASFilter filter(tmpVector);

	// Spawn the bullets and muzzle flash.
	TE_FireBullets(nPlayer, rvecOrigin, rangVector, nWeaponID, nMode, nSeed, fSpread);

	// We're the server, why should we give a damn about effects.
	bDoEffects = false;
#endif

	nSeed++;

	// Play the sound.
	if (bDoEffects)
		FX_WeaponSound(nPlayer, SINGLE, rvecOrigin, pWeaponInfo);

	if (!pPlayer)
		return;

	StartGroupingSounds();

#if !defined (CLIENT_DLL)
	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation(pPlayer, pPlayer->GetCurrentCommand());
#endif

	// Loop through all bullets.
	for (int i = 0; i < pWeaponInfo->m_nBullets; i++)
	{
		float fX, fY;

		RandomSeed(nSeed);

		// Randomize the spread.
		fX = RandomFloat(-0.5, 0.5f) + RandomFloat(-0.5f, 0.5f);
		fY = RandomFloat(-0.5, 0.5f) + RandomFloat(-0.5f, 0.5f);

		// Fire the bullet on the player-side.
		pPlayer->FireBullet(rvecOrigin, rangVector, fSpread, pWeaponInfo->m_nDamage, 
			pWeaponInfo->iAmmoType, pPlayer, bDoEffects, fX, fY);

		nSeed++;
	}

#if !defined (CLIENT_DLL)
	lagcompensation->FinishLagCompensation(pPlayer);
#endif

	EndGroupingSounds();
}