//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
// 
//=============================================================================//
//---------------------------------------------------------
//---------------------------------------------------------
#include "cbase.h"
#include "decals.h"
#include "forsaken_campfire_entity.h"
#include "entitylist.h"
#include "basecombatcharacter.h"
#include "ndebugoverlay.h"
#include "engine/IEngineSound.h"
#include "ispatialpartition.h"
#include "collisionutils.h"
#include "tier0/vprof.h"
#include "forsaken/forsaken_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	FIRE_HEIGHT				256.0f
#define FIRE_SCALE_FROM_SIZE(firesize)		(firesize * (1/FIRE_HEIGHT))

#define	FIRE_MAX_GROUND_OFFSET	24.0f	//(2 feet)

#define	DEFAULT_ATTACK_TIME	4.0f
#define	DEFAULT_DECAY_TIME	8.0f

// UNDONE: This shouldn't be constant but depend on specific fire
#define	FIRE_WIDTH				128
#define	FIRE_MINS				Vector(-20,-20,0 )   // Sould be FIRE_WIDTH in size
#define FIRE_MAXS				Vector( 20, 20,20)	 // Sould be FIRE_WIDTH in size
#define FIRE_SPREAD_DAMAGE_MULTIPLIER 3.0

#define FIRE_MAX_HEAT_LEVEL		64.0f
#define	FIRE_NORMAL_ATTACK_TIME	20.0f
#define FIRE_THINK_INTERVAL		0.1

ConVar campfire_maxabsorb( "campfire_maxabsorb", "50" );
ConVar campfire_absorbrate( "campfire_absorbrate", "3" );
ConVar campfire_extscale("campfire_extscale", "12");
ConVar campfire_extabsorb("campfire_extabsorb", "5");
ConVar campfire_heatscale( "campfire_heatscale", "1.0" );
ConVar campfire_incomingheatscale( "campfire_incomingheatscale", "0.1" );
ConVar campfire_dmgscale( "campfire_dmgscale", "0.1" );
ConVar campfire_dmgbase( "campfire_dmgbase", "1" );
ConVar campfire_growthrate( "campfire_growthrate", "1.0" );
ConVar campfire_dmginterval( "campfire_dmginterval", "1.0" );

#define VPROF_FIRE(s) VPROF( s )

class CCampFire : public CBaseEntity
{
public:
	DECLARE_CLASS( CCampFire, CBaseEntity );

	CCampFire( void );
	
	virtual void UpdateOnRemove( void );

	void	Precache( void );
	void	Init( const Vector &position, float scale, float attackTime, float fuel, int flags, int fireType );
	bool	GoOut();
	
	void	BurnThink();
	void	GoOutThink();
	void	GoOutInSeconds( float seconds );

	void	SetOwner( CBaseEntity *hOwner ) { m_hOwner = hOwner; }
	
	void	Scale( float end, float time );
	void	AddHeat( float heat, bool selfHeat = false );
	int		OnTakeDamage( const CTakeDamageInfo &info );

	bool	IsBurning( void ) const;

	bool	GetFireDimensions( Vector *pFireMins, Vector *pFireMaxs );
	
	void	Extinguish( float heat );
	void	DestroyEffect();

	virtual	void Update( float simTime );

	void	Spawn( void );
	void	Activate( void );
	void	StartFire( void );
	void	Start();
	void	SetToOutSize()
	{
		UTIL_SetSize( this, Vector(-8,-8,0), Vector(8,8,8) );
	}

	float	GetHeatLevel()	{ return m_flHeatLevel; }

	virtual int UpdateTransmitState();

	void DrawDebugGeometryOverlays(void) 
	{
		if (m_debugOverlays & OVERLAY_BBOX_BIT) 
		{	
			if ( m_lastDamage > gpGlobals->curtime && m_flHeatAbsorb > 0 )
			{
				NDebugOverlay::EntityBounds(this, 88, 255, 128, 0 ,0);
				char tempstr[512];
				Q_snprintf( tempstr, sizeof(tempstr), "Heat: %.1f", m_flHeatAbsorb );
				EntityText(1,tempstr, 0);
			}
			else if ( !IsBurning() )
			{
				NDebugOverlay::EntityBounds(this, 88, 88, 128, 0 ,0);
			}

			if ( IsBurning() )
			{
				Vector mins, maxs;
				if ( GetFireDimensions( &mins, &maxs ) )
				{
					NDebugOverlay::Box(GetAbsOrigin(), mins, maxs, 128, 0, 0, 10, 0);
				}
			}


		}
		BaseClass::DrawDebugGeometryOverlays();
	}

	void Disable();

	//Inputs
	void	InputStartFire( inputdata_t &inputdata );
	void	InputExtinguish( inputdata_t &inputdata );
	void	InputExtinguishTemporary( inputdata_t &inputdata );
	void	InputEnable( inputdata_t &inputdata );
	void	InputDisable( inputdata_t &inputdata );

protected:
	
	void	Spread( void );
	void	SpawnEffect( fireType_e type, float scale );

	CHandle<CBaseFire>	m_hEffect;
	EHANDLE		m_hOwner;
	
	int		m_nFireType;

	float	m_flFuel;
	float	m_flDamageTime;
	float	m_lastDamage;
	float	m_flFireSize;	// size of the fire in world units

	float	m_flHeatLevel;	// Used as a "health" for the fire.  > 0 means the fire is burning
	float	m_flHeatAbsorb;	// This much heat must be "absorbed" before it gets transferred to the flame size
	float	m_flDamageScale;

	float	m_flMaxHeat;
	float	m_flLastHeatLevel;

	//NOTENOTE: Lifetime is an expression of the sum total of these amounts plus the global time when started
	float	m_flAttackTime;	//Amount of time to scale up

	bool	m_bEnabled;
	bool	m_bStartDisabled;
	bool	m_bDidActivate;


	COutputEvent	m_OnIgnited;
	COutputEvent	m_OnExtinguished;

	DECLARE_DATADESC();
};

//-----------------------------------------------------------------------------
// Purpose: Starts a fire on a specified model.
// Input  : pEntity - The model entity to catch on fire.
//			fireHeight - 
//			attack - 
//			fuel - 
//			flags - 
//			owner - 
//			type - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CampFireSystem_StartFire( CBaseAnimating *pEntity, float fireHeight, float attack, float fuel, int flags, CBaseEntity *owner, fireType_e type )
{
	VPROF_FIRE( "FireSystem_StartFire2" );

	Vector position = pEntity->GetAbsOrigin();
	Vector testPos = position;

	// Create a new fire entity
	CCampFire *fire = (CCampFire *) CreateEntityByName( "forsaken_env_campfire" );
	if ( fire == NULL )
	{
		return false;
	}

	// Spawn the fire.
	// Fires not placed by a designer should be cleaned up automatically (not catch fire again).
	fire->AddSpawnFlags( SF_FIRE_DIE_PERMANENT );
	fire->Spawn();
	fire->Init( testPos, fireHeight, attack, fuel, flags, type );
	fire->Start();
	fire->SetOwner( owner );

	return true;
}


bool CampFireSystem_GetFireDamageDimensions( CBaseEntity *pEntity, Vector *pFireMins, Vector *pFireMaxs )
{
	CCampFire *pFire = dynamic_cast<CCampFire *>(pEntity);

	if ( pFire && pFire->GetFireDimensions( pFireMins, pFireMaxs ) )
	{
		*pFireMins /= FIRE_SPREAD_DAMAGE_MULTIPLIER;
		*pFireMaxs /= FIRE_SPREAD_DAMAGE_MULTIPLIER;
		return true;
	}
	pFireMins->Init();
	pFireMaxs->Init();
	return false;
}


//==================================================
// CFire
//==================================================
BEGIN_DATADESC( CCampFire )

	DEFINE_FIELD( m_hEffect, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hOwner, FIELD_EHANDLE ),
	DEFINE_KEYFIELD( m_nFireType,	FIELD_INTEGER, "firetype" ),

	DEFINE_FIELD( m_flFuel, FIELD_FLOAT ),
	DEFINE_FIELD( m_flDamageTime, FIELD_TIME ),
	DEFINE_FIELD( m_lastDamage, FIELD_TIME ),
	DEFINE_KEYFIELD( m_flFireSize,	FIELD_FLOAT, "firesize" ),

	DEFINE_KEYFIELD( m_flHeatLevel,	FIELD_FLOAT,	"ignitionpoint" ),
 	DEFINE_FIELD( m_flHeatAbsorb, FIELD_FLOAT ),
 	DEFINE_KEYFIELD( m_flDamageScale,FIELD_FLOAT,	"damagescale" ),

	DEFINE_FIELD( m_flMaxHeat, FIELD_FLOAT ),
	//DEFINE_FIELD( m_flLastHeatLevel,	FIELD_FLOAT  ),

	DEFINE_KEYFIELD( m_flAttackTime, FIELD_FLOAT, "fireattack" ),
	DEFINE_FIELD( m_bEnabled, FIELD_BOOLEAN ),
	DEFINE_KEYFIELD( m_bStartDisabled, FIELD_BOOLEAN, "StartDisabled" ),
	DEFINE_FIELD( m_bDidActivate, FIELD_BOOLEAN ),

	DEFINE_FUNCTION( BurnThink ),
	DEFINE_FUNCTION( GoOutThink ),



	DEFINE_INPUTFUNC( FIELD_VOID, "StartFire", InputStartFire ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "Extinguish", InputExtinguish ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "ExtinguishTemporary", InputExtinguishTemporary ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	
	DEFINE_OUTPUT( m_OnIgnited, "OnIgnited" ),
	DEFINE_OUTPUT( m_OnExtinguished, "OnExtinguished" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( forsaken_env_campfire, CCampFire );

//==================================================
// CCampFire
//==================================================

CCampFire::CCampFire( void )
{
	m_flFuel = 0.0f;
	m_flAttackTime = 0.0f;
	m_flDamageTime = 0.0f;
	m_lastDamage = 0;
	m_flDamageScale = 5.0f;
	m_nFireType = FIRE_NATURAL;

	//Spreading
	m_flHeatAbsorb = 8.0f;
	m_flHeatLevel = 0;

	// Must be in the constructor!
	AddEFlags( EFL_USE_PARTITION_WHEN_NOT_SOLID );
}

//-----------------------------------------------------------------------------
// UpdateOnRemove
//-----------------------------------------------------------------------------
void CCampFire::UpdateOnRemove( void )
{
	//Stop any looping sounds that might be playing
	StopSound( "Fire.Plasma" );

	DestroyEffect();

	// Chain at end to mimic destructor unwind order
	BaseClass::UpdateOnRemove();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCampFire::Precache( void )
{
	if ( m_nFireType == FIRE_NATURAL )
	{
		UTIL_PrecacheOther("_firesmoke");
	}

	if ( m_nFireType == FIRE_PLASMA )
	{
		UTIL_PrecacheOther("_plasma");
	}

	PrecacheScriptSound( "Fire.Plasma" );
}

//------------------------------------------------------------------------------
// Purpose : Input handler for starting the fire.
//------------------------------------------------------------------------------
void CCampFire::InputStartFire( inputdata_t &inputdata )
{
	if ( !m_bEnabled )
		return;

	StartFire();
}

void CCampFire::InputEnable( inputdata_t &inputdata )
{
	m_bEnabled = true;
}

void CCampFire::InputDisable( inputdata_t &inputdata )
{
	Disable();
}

void CCampFire::Disable() 
{
	m_bEnabled = false;
	if ( IsBurning() )
	{
		GoOut();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CCampFire::InputExtinguish( inputdata_t &inputdata )
{
	m_spawnflags &= ~SF_FIRE_INFINITE;
	GoOutInSeconds( inputdata.value.Float() );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CCampFire::InputExtinguishTemporary( inputdata_t &inputdata )
{
	GoOutInSeconds( inputdata.value.Float() );
}

//-----------------------------------------------------------------------------
// Purpose: Starts burning.
//-----------------------------------------------------------------------------
void CCampFire::StartFire( void )
{
	if ( m_hEffect != NULL )
		return;

	// Trace down and start a fire there. Nothing fancy yet.
	Vector vFirePos;
	trace_t tr;
	if ( m_spawnflags & SF_FIRE_DONT_DROP )
	{
		vFirePos = GetAbsOrigin();
	}
	else
	{
		UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() - Vector( 0, 0, 1024 ), MASK_FIRE_SOLID, this, COLLISION_GROUP_NONE, &tr );
		vFirePos = tr.endpos;
	}

	int spawnflags = m_spawnflags;
	m_spawnflags |= SF_FIRE_START_ON;
	Init( vFirePos, m_flFireSize, m_flAttackTime, GetHealth(), m_spawnflags, (fireType_e) m_nFireType );
	Start();
	m_spawnflags = spawnflags;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCampFire::Spawn( void )
{
	BaseClass::Spawn();

	Precache();

	m_takedamage = DAMAGE_NO;

	SetSolid( SOLID_NONE );
	AddEffects( EF_NODRAW );
	SetToOutSize();

	// set up the ignition point
	m_flHeatAbsorb = m_flHeatLevel * 0.05;
	m_flHeatLevel = 0;
	Init( GetAbsOrigin(), m_flFireSize, m_flAttackTime, m_flFuel, m_spawnflags, m_nFireType );
	
	if( m_bStartDisabled )
	{
		Disable();
	}
	else
	{
		m_bEnabled = true;
	}
}

int CCampFire::UpdateTransmitState()
{
	// Don't want to be FL_EDICT_DONTSEND because our fire entity may make us transmit.
	return SetTransmitState( FL_EDICT_PVSCHECK );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCampFire::Activate( void )
{
	BaseClass::Activate();
	
	//See if we should start active
	if ( !m_bDidActivate && ( m_spawnflags & SF_FIRE_START_ON ) )
	{
		m_flHeatLevel = m_flMaxHeat;

		StartFire();
	}

	m_bDidActivate = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCampFire::SpawnEffect( fireType_e type, float scale )
{
	CBaseFire *pEffect = NULL;
	switch ( type )
	{
	default:
	case FIRE_NATURAL:
		{
			CFireSmoke	*fireSmoke = (CFireSmoke *) CreateEntityByName( "_firesmoke" );
			fireSmoke->EnableSmoke( ( m_spawnflags & SF_FIRE_SMOKELESS )==false );
			fireSmoke->EnableGlow( ( m_spawnflags & SF_FIRE_NO_GLOW )==false );
			fireSmoke->EnableVisibleFromAbove( ( m_spawnflags & SF_FIRE_VISIBLE_FROM_ABOVE )!=false );
			//fireSmoke->AddEffects(EF_BRIGHTLIGHT); //CHAMBY: this is where dynamic lighting can be toggled later on

			pEffect			= fireSmoke;
			m_nFireType		= FIRE_NATURAL;
			m_takedamage	= DAMAGE_YES;

			// Start burn sound
			EmitSound( "Weapon_Forsaken_Molotov.Burning" );
		}
		break;

	case FIRE_PLASMA:
		{
			CPlasma	*plasma = (CPlasma *) CreateEntityByName( "_plasma" );
			plasma->EnableSmoke( true );
		
			pEffect			= plasma;
			m_nFireType		= FIRE_PLASMA;
			m_takedamage	= DAMAGE_YES;

			// Start burn sound
			EmitSound( "Fire.Plasma" );
		}
		break;
	}

	UTIL_SetOrigin( pEffect, GetAbsOrigin() );
	pEffect->Spawn();
	pEffect->SetParent( this );
	
	//Start it going
	pEffect->Enable( ( m_spawnflags & SF_FIRE_START_ON ) );
	m_hEffect = pEffect;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn and initialize the fire
// Input  : &position - where the fire resides
//			lifetime - 
//-----------------------------------------------------------------------------
void CCampFire::Init( const Vector &position, float scale, float attackTime, float fuel, int flags, int fireType )
{
	m_flAttackTime = attackTime;
	
	m_spawnflags = flags;
	m_nFireType = fireType;

	if ( flags & SF_FIRE_INFINITE )
	{
		fuel = 0;
	}
	m_flFuel = fuel;
	if ( m_flFuel )
	{
		m_spawnflags |= SF_FIRE_DIE_PERMANENT;
	}

	Vector localOrigin = position;
	if ( GetMoveParent() )
	{
		EntityMatrix parentMatrix;
		parentMatrix.InitFromEntity( GetMoveParent() );
		localOrigin = parentMatrix.WorldToLocal( position );
	}
	UTIL_SetOrigin( this, localOrigin );

	SetSolid( SOLID_NONE );
	m_flFireSize = scale;
	m_flMaxHeat = FIRE_MAX_HEAT_LEVEL * FIRE_SCALE_FROM_SIZE(scale);
	//See if we should start on
	if ( m_spawnflags & SF_FIRE_START_FULL )
	{
		m_flHeatLevel = m_flMaxHeat;
	}
	m_flLastHeatLevel = 0;

}

void CCampFire::Start()
{
	float boxWidth = (m_flFireSize * (FIRE_WIDTH/FIRE_HEIGHT))*0.5f;
	UTIL_SetSize(this, Vector(-boxWidth,-boxWidth,0),Vector(boxWidth,boxWidth,m_flFireSize));

	//Spawn the client-side effect
	SpawnEffect( (fireType_e)m_nFireType, FIRE_SCALE_FROM_SIZE(m_flFireSize) );
	m_OnIgnited.FireOutput( this, this );
	SetThink( &CCampFire::BurnThink );
	m_flDamageTime = 0;
	// think right now
	BurnThink();
}

//-----------------------------------------------------------------------------
// Purpose: Determines whether or not the fire is still active
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CCampFire::IsBurning( void ) const
{
	if ( m_flHeatLevel > 0 )
		return true;

	return false;
}


//-----------------------------------------------------------------------------
// Purpose: Get the damage box of the fire
//-----------------------------------------------------------------------------
bool CCampFire::GetFireDimensions( Vector *pFireMins, Vector *pFireMaxs )
{
	if ( m_flHeatLevel <= 0 )
	{
		pFireMins->Init();
		pFireMaxs->Init();
		return false;
	}

	float scale = m_flHeatLevel / m_flMaxHeat;
	float damageRadius = scale * m_flFireSize * FIRE_WIDTH / FIRE_HEIGHT * 0.5;	

	damageRadius *= FIRE_SPREAD_DAMAGE_MULTIPLIER; //FIXME: Trying slightly larger radius for burning

	if ( damageRadius < 16 )
	{
		damageRadius = 16;
	}

	pFireMins->Init(-damageRadius,-damageRadius,0);
	pFireMaxs->Init(damageRadius,damageRadius,m_flFireSize*scale);

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Update the fire and its children
//-----------------------------------------------------------------------------
void CCampFire::Update( float simTime )
{
	VPROF_FIRE( "CFire::Update" );

	if ( m_flFuel != 0 )
	{
		m_flFuel -= simTime;
		if ( m_flFuel <= 0 )
		{
			GoOutInSeconds( 1 );
			return;
		}
	}

	float strength = m_flHeatLevel / FIRE_MAX_HEAT_LEVEL;
	if ( m_flHeatLevel != m_flLastHeatLevel )
	{
		m_flLastHeatLevel = m_flHeatLevel;
		// Make the effect the appropriate size given the heat level
		m_hEffect->Scale( strength, 0.5f );		
	}
	// add heat to myself (grow)
	float addedHeat = (m_flAttackTime > 0) ? m_flMaxHeat / m_flAttackTime : m_flMaxHeat;
	addedHeat *= simTime * campfire_growthrate.GetFloat();
	AddHeat( addedHeat, true );

	// add heat to nearby fires
	float outputHeat = strength * m_flHeatLevel;

	Vector fireMins;
	Vector fireMaxs;
	Vector fireEntityDamageMins;
	Vector fireEntityDamageMaxs;

	GetFireDimensions( &fireMins, &fireMaxs );

	if ( FIRE_SPREAD_DAMAGE_MULTIPLIER != 1.0 ) // if set to 1.0, optimizer will remove this code
	{
		fireEntityDamageMins = fireMins / FIRE_SPREAD_DAMAGE_MULTIPLIER;
		fireEntityDamageMaxs = fireMaxs / FIRE_SPREAD_DAMAGE_MULTIPLIER;
	}

	//NDebugOverlay::Box( GetAbsOrigin(), fireMins, fireMaxs, 255, 255, 255, 0, fire_dmginterval.GetFloat() );
	fireMins += GetAbsOrigin();
	fireMaxs += GetAbsOrigin();

	if ( FIRE_SPREAD_DAMAGE_MULTIPLIER != 1.0 )
	{
		fireEntityDamageMins += GetAbsOrigin();
		fireEntityDamageMaxs += GetAbsOrigin();
	}

	CBaseEntity *pNearby[256];
	CCampFire *pFires[16];
	int nearbyCount = UTIL_EntitiesInBox( pNearby, ARRAYSIZE(pNearby), fireMins, fireMaxs, 0 );
	int fireCount = 0;
	int i;

	// is it time to do damage?
	bool damage = false;
	int outputDamage = 0;
	if ( m_flDamageTime <= gpGlobals->curtime )
	{
		m_flDamageTime = gpGlobals->curtime + campfire_dmginterval.GetFloat();
		outputDamage = (campfire_dmgbase.GetFloat() + outputHeat * campfire_dmgscale.GetFloat() * m_flDamageScale) * campfire_dmginterval.GetFloat();
		if ( outputDamage )
		{
			damage = true;
		}
	}
	int damageFlags = (m_nFireType == FIRE_NATURAL) ? DMG_BURN : DMG_PLASMA;
	for ( i = 0; i < nearbyCount; i++ )
	{
		CBaseEntity *pOther = pNearby[i];

		if ( pOther == this )
		{
			continue;
		}
		else if ( FClassnameIs( pOther, "forsaken_env_campfire" ) )
		{
			if ( fireCount < ARRAYSIZE(pFires) )
			{
				pFires[fireCount] = (CCampFire *)pOther;
				fireCount++;
			}
			continue;
		}
		else if ( pOther->m_takedamage == DAMAGE_NO )
		{
			pNearby[i] = NULL;
		}
		else if ( damage )
		{
			bool bDoDamage;

			if ( FIRE_SPREAD_DAMAGE_MULTIPLIER != 1.0 && !pOther->IsPlayer() ) // if set to 1.0, optimizer will remove this code
			{
				Vector otherMins, otherMaxs;
				pOther->CollisionProp()->WorldSpaceAABB( &otherMins, &otherMaxs );
				bDoDamage = IsBoxIntersectingBox( otherMins, otherMaxs, 
												  fireEntityDamageMins, fireEntityDamageMaxs );

			}
			else
				bDoDamage = true;

			if ( bDoDamage )
			{
				// Make sure can actually see entity (don't damage through walls)
				trace_t tr;
				UTIL_TraceLine( this->WorldSpaceCenter(), pOther->WorldSpaceCenter(), MASK_FIRE_SOLID, pOther, COLLISION_GROUP_NONE, &tr );

				if (tr.fraction == 1.0 && !tr.startsolid)
				{
					pOther->TakeDamage( CTakeDamageInfo( this, m_hOwner, outputDamage, damageFlags ) );

					CForsakenPlayer *pPlayer = ToForsakenPlayer(pOther);
					if (pPlayer == NULL)
					{
						continue;
					}

					if (!pPlayer->IsAlive())
					{
						continue;
					}

					//pPlayer->Ignite( 7.0f, false, 7.0f, false, m_hOwner );
				}
			}
		}
	}

	outputHeat *= campfire_heatscale.GetFloat() * simTime;

	if ( fireCount > 0 )
	{
		outputHeat /= fireCount;
		for ( i = 0; i < fireCount; i++ )
		{
			pFires[i]->AddHeat( outputHeat, false );
		}
	}
}

// Destroy any effect I have
void CCampFire::DestroyEffect()
{
	CBaseFire *pEffect = m_hEffect;
	if ( pEffect != NULL )
	{
		//disable the graphics and remove the entity
		pEffect->Enable( false );
		UTIL_Remove( pEffect );
	}
}
//-----------------------------------------------------------------------------
// Purpose: Think
//-----------------------------------------------------------------------------
void CCampFire::BurnThink( void )
{
	SetNextThink( gpGlobals->curtime + FIRE_THINK_INTERVAL );

	Update( FIRE_THINK_INTERVAL );
}

void CCampFire::GoOutThink()
{
	GoOut();
}

void CCampFire::GoOutInSeconds( float seconds )
{
	Scale( 0.0f, seconds );
	
	SetThink( &CCampFire::GoOutThink );
	SetNextThink( gpGlobals->curtime + seconds );
}

//------------------------------------------------------------------------------
// Purpose : Blasts of significant size blow out fires that take damage
// Input   :
// Output  :
//------------------------------------------------------------------------------
int CCampFire::OnTakeDamage( const CTakeDamageInfo &info )
{
	return 0;
}

void CCampFire::AddHeat( float heat, bool selfHeat )
{
	if ( m_bEnabled )
	{
		if ( !selfHeat )
		{
			if ( IsBurning() )
			{
				// scale back the incoming heat from surrounding fires
				// if I've already ignited
				heat *= campfire_incomingheatscale.GetFloat();
			}
		}
		m_lastDamage = gpGlobals->curtime + 0.5;
		bool start = m_flHeatLevel <= 0 ? true : false;
		if ( m_flHeatAbsorb > 0 )
		{
			float absorbDamage = heat * campfire_absorbrate.GetFloat();
			if ( absorbDamage > m_flHeatAbsorb )
			{
				heat -= m_flHeatAbsorb / campfire_absorbrate.GetFloat();
				m_flHeatAbsorb = 0;
			}
			else
			{
				m_flHeatAbsorb -= absorbDamage;
				heat = 0;
			}
		}

		m_flHeatLevel += heat;
		if ( start && m_flHeatLevel > 0 && m_hEffect == NULL )
		{
			StartFire();
		}
		if ( m_flHeatLevel > m_flMaxHeat )
			m_flHeatLevel = m_flMaxHeat;
	}
}

	
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : end - 
//			time - 
//-----------------------------------------------------------------------------
void CCampFire::Scale( float end, float time )
{
	CBaseFire *pEffect = m_hEffect;
	if ( pEffect )
	{
		pEffect->Scale( end, time );
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : time - 
//-----------------------------------------------------------------------------
void CCampFire::Extinguish( float heat )
{
	if ( !m_bEnabled )
		return;

	m_lastDamage = gpGlobals->curtime + 0.5;
	bool out = m_flHeatLevel > 0 ? true : false;

	m_flHeatLevel -= heat;
	m_flHeatAbsorb += campfire_extabsorb.GetFloat() * heat;
	if ( m_flHeatAbsorb > campfire_maxabsorb.GetFloat() )
	{
		m_flHeatAbsorb = campfire_maxabsorb.GetFloat();
	}

	// drift toward the average attack time after being sprayed
	// some fires are heavily scripted so their attack looks weird 
	// once interacted with.  Basically, this blends out the scripting 
	// as the fire is sprayed with the extinguisher.
	float averageAttackTime = m_flMaxHeat * (FIRE_NORMAL_ATTACK_TIME/FIRE_MAX_HEAT_LEVEL);
	m_flAttackTime = Approach( averageAttackTime, m_flAttackTime, 2 * gpGlobals->frametime );

	if ( m_flHeatLevel <= 0 )
	{
		m_flHeatLevel = 0;
		if ( out )
		{
			GoOut();
		}
	}
}

bool CCampFire::GoOut()
{
	//Signal death
	m_OnExtinguished.FireOutput( this, this );

	DestroyEffect();
	m_flHeatLevel -= 20;
	if ( m_flHeatLevel > 0 )
		m_flHeatLevel = 0;

	m_flLastHeatLevel = m_flHeatLevel; 
	SetThink(NULL);
	SetNextThink( TICK_NEVER_THINK );
	if ( m_spawnflags & SF_FIRE_DIE_PERMANENT )
	{
		UTIL_Remove( this );
		return true;
	}
	SetToOutSize();
	
	return false;
}
