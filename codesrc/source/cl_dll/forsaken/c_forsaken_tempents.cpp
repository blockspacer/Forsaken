/*
	c_forsaken_tempents.cpp
	Forsaken temporary entity code.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "model_types.h"
#include "view_shared.h"
#include "iviewrender.h"
#include "tempentity.h"
#include "dlight.h"
#include "tempent.h"
#include "c_te_legacytempents.h"
#include "forsaken/c_forsaken_tempents.h"
#include "clientsideeffects.h"
#include "cl_animevent.h"
#include "iefx.h"
#include "engine/IEngineSound.h"
#include "env_wind_shared.h"
#include "ClientEffectPrecacheSystem.h"
#include "fx_sparks.h"
#include "fx.h"
#include "movevars_shared.h"
#include "engine/ivmodelinfo.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "view.h"
#include "tier0/vprof.h"
#include "particles_localspace.h"
#include "physpropclientside.h"
#include "forsaken/forsaken_weapon_base.h"
#include "hud_macros.h"
#include "forsaken/c_forsaken_player.h"
#include "tier0/memdbgon.h"
#include "toolframework_client.h"
#include "math.h"

// Function Prototypes
void __MsgFunc_RestartRound(bf_read &rMsg);

// Global Variable Declerations
static CForsakenTempEnts g_TempEnts;
ITempEnts *tempents = ( ITempEnts * )&g_TempEnts;

#define TE_RIFLE_SHELL 1024
#define TE_PISTOL_SHELL 2048
#define TE_SHOTGUN_SHELL 4096

// Functions
void CForsakenTempEnts::CSEjectBrass(const Vector &rvecPosition, const QAngle &rangAngles, 
	int nVelocity, int nShellType, CBasePlayer *pPlayer)
{
	const model_t *pModel = NULL;
	int hitsound = TE_BOUNCE_SHELL;

	switch (nShellType)
	{
	case FSKN_SHELL_1201FP:
		hitsound = TE_SHOTGUN_SHELL;
		pModel = m_pShell_1201FP;
		break;

	case FSKN_SHELL_HK23:
		hitsound = TE_PISTOL_SHELL;
		pModel = m_pShell_HK23;
		break;

	case FSKN_SHELL_MAC10:
		hitsound = TE_RIFLE_SHELL;
		pModel = m_pShell_MAC10;
		break;

	case FSKN_SHELL_P99:
	default:
		hitsound = TE_PISTOL_SHELL;
		pModel = m_pShell_P99;
		break;

	case FSKN_SHELL_REMINGTON:
		hitsound = TE_RIFLE_SHELL;
		pModel = m_pShell_Remington;
		break;

	case FSKN_SHELL_SA80:
		hitsound = TE_RIFLE_SHELL;
		pModel = m_pShell_SA80;
		break;
	}

	if (pModel == NULL)
		return;

	Vector forward, right, up;
	Vector velocity;
	Vector origin;
	QAngle angle;
	
	// Add some randomness to the velocity

	AngleVectors(rangAngles, &forward, &right, &up);
	
	velocity = forward * nVelocity * random->RandomFloat(1.2, 2.8) +
			   up * random->RandomFloat(-10, 10) +
			   right * random->RandomFloat(-20, 20);

	if (pPlayer)
		velocity += pPlayer->GetAbsVelocity();

	C_LocalTempEntity *pTemp = TempEntAlloc(rvecPosition, (model_t *)pModel);
	if (!pTemp)
		return;

	if (pPlayer)
		pTemp->SetAbsAngles(pPlayer->EyeAngles());
	else
		pTemp->SetAbsAngles(vec3_angle);

	pTemp->SetVelocity(velocity);

	pTemp->hitSound = hitsound;

	pTemp->SetGravity(0.4);

	pTemp->m_nBody	= 0;
	pTemp->flags = FTENT_FADEOUT | FTENT_GRAVITY | FTENT_COLLIDEALL | FTENT_HITSOUND | FTENT_ROTATE | FTENT_CHANGERENDERONCOLLIDE;

	pTemp->m_vecTempEntAngVelocity[0] = random->RandomFloat(-256,256);
	pTemp->m_vecTempEntAngVelocity[1] = random->RandomFloat(-256,256);
	pTemp->m_vecTempEntAngVelocity[2] = 0;
	pTemp->SetRenderMode(kRenderNormal);
	pTemp->tempent_renderamt = 255;
	
	pTemp->die = gpGlobals->curtime + 10;

	bool bViewModelBrass = false;

	if (pPlayer && pPlayer->GetObserverMode() == OBS_MODE_IN_EYE)
	{
		// we are spectating the shooter in first person view
		pPlayer = ToBasePlayer( pPlayer->GetObserverTarget() );
		bViewModelBrass = true;
	}

	if (pPlayer)
	{
		pTemp->clientIndex = pPlayer->entindex();
		bViewModelBrass |= pPlayer->IsLocalPlayer();
	}
	else
	{
		pTemp->clientIndex = 0;
	}

	if (bViewModelBrass)
	{
		// for viewmodel brass put it in the viewmodel renderer group
		pTemp->m_RenderGroup = RENDER_GROUP_VIEW_MODEL_OPAQUE;
	}
}

void CForsakenTempEnts::Init()
/*
	
	Pre: 
	Post: 
*/
{
	m_pShell_1201FP = NULL;
	m_pShell_HK23 = NULL;
	m_pShell_MAC10 = NULL;
	m_pShell_P99 = NULL;
	m_pShell_RagingBull = NULL;
	m_pShell_Remington = NULL;
	m_pShell_SA80 = NULL;

	HOOK_MESSAGE(RestartRound);

	BaseClass::Init();
}

void CForsakenTempEnts::LevelInit()
/*
	
	Pre: 
	Post: 
*/
{
	m_pShell_1201FP = (model_t*)engine->LoadModel("models/shells/shell_1201fp.mdl");
	m_pShell_HK23 = (model_t*)engine->LoadModel("models/shells/shell_hk23.mdl");
	m_pShell_MAC10 = (model_t*)engine->LoadModel("models/shells/shell_mac10.mdl");
	m_pShell_P99 = (model_t*)engine->LoadModel("models/shells/shell_p99.mdl");
	m_pShell_RagingBull = (model_t*)engine->LoadModel("models/shells/shell_ragingbull.mdl");
	m_pShell_Remington = (model_t*)engine->LoadModel("models/shells/shell_remington.mdl");
	m_pShell_SA80 = (model_t*)engine->LoadModel("models/shells/shell_sa80.mdl");

	BaseClass::LevelInit();
}

void CForsakenTempEnts::LevelShutdown()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::LevelShutdown();
}

void CForsakenTempEnts::MuzzleFlash(int nType, ClientEntityHandle_t hEntity, int nAttachmentIndex, 
	bool bFirstPerson)
/*
	
	Pre: 
	Post: 
*/
{
	// What weapon are we using?
	switch (nType)
	{
		//shotgun
	case FSKN_MUZZLEFLASH_1201FP:
		Forsaken_Muzzle(2.0f, hEntity, nAttachmentIndex, false);
		break;

		// pistols and rifle
	case FSKN_MUZZLEFLASH_710:
	case FSKN_MUZZLEFLASH_RAGINGBULL:
	case FSKN_MUZZLEFLASH_REVOLVER:
	case FSKN_MUZZLEFLASH_HK23:
	case FSKN_MUZZLEFLASH_DUALM10:
	case FSKN_MUZZLEFLASH_DUALP99:
		Forsaken_Muzzle(0.8f, hEntity, nAttachmentIndex, false);
		break;

		// RPG
	case FSKN_MUZZLEFLASH_RPG:
		{
			Vector origin;
			QAngle angles;

			FX_GetAttachmentTransform( hEntity, nAttachmentIndex, &origin, &angles );

			unsigned char color[3];

			color[0] = color[1] = color[2] = 255;

			FX_Smoke( origin, angles, 1.0f, 4, &color[0], 255);
			break;
		}

		// machine gun
	case FSKN_MUZZLEFLASH_SA80:
			Forsaken_Muzzle(0.8f, hEntity, nAttachmentIndex, true);
		break;

	default:
		{
		char czErrMsg[256];
		Q_snprintf(czErrMsg, sizeof(czErrMsg), "MuzzleFlash event called on invalid type. Type = %d, EntIndex = %d, AttachIndex = %d", nType, hEntity, nAttachmentIndex);
		DevMsg(czErrMsg);

		break;
		}
	}
}

void CForsakenTempEnts::MuzzleFlash(const Vector &rvecPosition, const QAngle &rAngles, int nType, 
	ClientEntityHandle_t hEntity, bool bFirstPerson)
/*
	
	Pre: 
	Post: 
*/
{
}

void CForsakenTempEnts::Shutdown()
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::Shutdown();
}

void CForsakenTempEnts::Forsaken_Muzzle(float scale, ClientEntityHandle_t hEntity, int attachmentIndex, bool isCrossShaped)
{
	VPROF_BUDGET( "Forsaken_Muzzle", VPROF_BUDGETGROUP_PARTICLE_RENDERING );
	
	CSmartPtr<CLocalSpaceEmitter> pSimple = CLocalSpaceEmitter::Create( "Forsaken_Muzzle", hEntity, attachmentIndex );
	
	SimpleParticle *pParticle;

	float angle = cos(M_PI / 4.0f);
	Vector forward(1,0,0), offset;

	CUtlVector <Vector> directions;
	directions.AddToTail(Vector(0,angle,angle)); // Right
	directions.AddToTail(Vector(0,-angle,angle)); // Up
	directions.AddToTail(Vector(0,angle,-angle)); // Left
	directions.AddToTail(Vector(0,-angle,-angle)); // Down

	//
	// Scale Checking
	//

	float flScale = random->RandomFloat( scale-0.25f, scale+0.25f );

	if ( flScale < 0.5f )
	{
		flScale = 0.5f;
	}
	else if ( flScale > 8.0f )
	{
		flScale = 8.0f;
	}

	//
	// Flash
	//

	for ( int i = 1; i < 9; i++ )
	{
		offset = (forward * (i*2.0f*scale));

		pParticle = (SimpleParticle *) pSimple->AddParticle( sizeof( SimpleParticle ), pSimple->GetPMaterial( VarArgs( "effects/muzzleflash%d", random->RandomInt(1,4) ) ), offset );
			
		if ( pParticle == NULL )
			return;

		pParticle->m_flLifetime		= 0.0f;
		pParticle->m_flDieTime		= 0.0001f;

		pParticle->m_vecVelocity.Init();

		pParticle->m_uchColor[0]	= 255;
		pParticle->m_uchColor[1]	= 255;
		pParticle->m_uchColor[2]	= 255;

		pParticle->m_uchStartAlpha	= 255;
		pParticle->m_uchEndAlpha	= 128;

		pParticle->m_uchStartSize	= (random->RandomFloat( 3.0f, 6.0f ) * (12-i)/9) * flScale;
		pParticle->m_uchEndSize		= pParticle->m_uchStartSize;
		pParticle->m_flRoll			= random->RandomInt( 0, 360 );
		pParticle->m_flRollDelta	= 0.0f;
	}

	// For SA80
	if (isCrossShaped)
	{
		for ( int j = 0; j < directions.Count(); j++)
		{
			for ( int i = 3; i < 6; i++ )
			{
				offset = (directions[j] * (i*2.0f*scale));

				pParticle = (SimpleParticle *) pSimple->AddParticle( sizeof( SimpleParticle ), pSimple->GetPMaterial( VarArgs( "effects/muzzleflash%d", random->RandomInt(1,4) ) ), offset );
					
				if ( pParticle == NULL )
					return;

				pParticle->m_flLifetime		= 0.0f;
				pParticle->m_flDieTime		= 0.0001f;

				pParticle->m_vecVelocity.Init();

				pParticle->m_uchColor[0]	= 255;
				pParticle->m_uchColor[1]	= 255;
				pParticle->m_uchColor[2]	= 255;

				pParticle->m_uchStartAlpha	= 255;
				pParticle->m_uchEndAlpha	= 128;

				pParticle->m_uchStartSize	= (random->RandomFloat( 3.0f, 6.0f ) * (12-i)/9) * flScale;
				pParticle->m_uchEndSize		= pParticle->m_uchStartSize;
				pParticle->m_flRoll			= random->RandomInt( 0, 360 );
				pParticle->m_flRollDelta	= 0.0f;
			}
		}
	}

	// Smoke
	Vector origin;
	QAngle angles;

	FX_GetAttachmentTransform( hEntity, attachmentIndex, &origin, &angles );
	unsigned char color[3];
	color[0] = color[1] = color[2] = 255;
	FX_Smoke( origin, angles, flScale, 4, &color[0], 255);

	// For shotgun
	/*if (hasSparks)
	{
		Vector origin;
		QAngle angles;
		Vector vecAngles;
		FX_GetAttachmentTransform( hEntity, attachmentIndex, &origin, &angles );
		AngleVectors(angles, &vecAngles);
		Forsaken_Sparks(origin, vecAngles, Vector(1,0,0), flScale);
	}*/
}

// Global
void __MsgFunc_RestartRound(bf_read &rMsg)
{
	// We want to remove all decals at round restart.
	engine->ClientCmd("r_cleardecals\n");
}