/*
	forsaken_fx_weapons.cpp
	Forsaken effects for weapons.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "fx_impact.h"
#include "tempent.h"
#include "c_te_effect_dispatch.h"
#include "c_te_legacytempents.h"
#include "forsaken/c_forsaken_tempents.h"
#include "forsaken/forsaken_weapon_base.h"

// Function Prototypes
void Forsaken_EjectWeaponBrass_1201FP(const CEffectData &redData);
void Forsaken_EjectWeaponBrass_HK23(const CEffectData &redData);
void Forsaken_EjectWeaponBrass_MAC10(const CEffectData &redData);
void Forsaken_EjectWeaponBrass_P99(const CEffectData &redData);
void Forsaken_EjectWeaponBrass_RagingBull(const CEffectData &redData);
void Forsaken_EjectWeaponBrass_Remington(const CEffectData &redData);
void Forsaken_EjectWeaponBrass_SA80(const CEffectData &redData);

// HL2 Class Macros
DECLARE_CLIENT_EFFECT("EjectBrass_1201FP", Forsaken_EjectWeaponBrass_1201FP);
DECLARE_CLIENT_EFFECT("EjectBrass_HK23", Forsaken_EjectWeaponBrass_HK23);
DECLARE_CLIENT_EFFECT("EjectBrass_MAC10", Forsaken_EjectWeaponBrass_MAC10);
DECLARE_CLIENT_EFFECT("EjectBrass_P99", Forsaken_EjectWeaponBrass_P99);
DECLARE_CLIENT_EFFECT("EjectBrass_RagingBull", Forsaken_EjectWeaponBrass_RagingBull);
DECLARE_CLIENT_EFFECT("EjectBrass_Remington", Forsaken_EjectWeaponBrass_Remington);
DECLARE_CLIENT_EFFECT("EjectBrass_SA80", Forsaken_EjectWeaponBrass_SA80);

// Functions
void Forsaken_EjectWeaponBrass(int nShellType, const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if (!pPlayer)
		return;

	// Eject the brass.
	tempents->CSEjectBrass(redData.m_vOrigin, redData.m_vAngles, redData.m_fFlags, 
		nShellType, pPlayer);
}

void Forsaken_EjectWeaponBrass_1201FP(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	Forsaken_EjectWeaponBrass(FSKN_SHELL_1201FP, redData);
}

void Forsaken_EjectWeaponBrass_HK23(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	Forsaken_EjectWeaponBrass(FSKN_SHELL_HK23, redData);
}

void Forsaken_EjectWeaponBrass_MAC10(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	Forsaken_EjectWeaponBrass(FSKN_SHELL_MAC10, redData);
}

void Forsaken_EjectWeaponBrass_P99(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	Forsaken_EjectWeaponBrass(FSKN_SHELL_P99, redData);
}

void Forsaken_EjectWeaponBrass_RagingBull(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	Forsaken_EjectWeaponBrass(FSKN_SHELL_RAGINGBULL, redData);
}

void Forsaken_EjectWeaponBrass_Remington(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	Forsaken_EjectWeaponBrass(FSKN_SHELL_REMINGTON, redData);
}

void Forsaken_EjectWeaponBrass_SA80(const CEffectData &redData)
/*
	
	Pre: 
	Post: 
*/
{
	Forsaken_EjectWeaponBrass(FSKN_SHELL_SA80, redData);
}