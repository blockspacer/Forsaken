/*
	c_forsaken_player.cpp
	Client-side forsaken player.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "c_forsaken_player.h"
#include "forsaken/forsaken_weapon_base.h"
#include "c_basetempentity.h"

#undef CForsakenPlayer

// Internal Classes
class C_ForsakenPlayerRagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS(C_ForsakenPlayerRagdoll, C_BaseAnimatingOverlay);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor
	C_ForsakenPlayerRagdoll();
	~C_ForsakenPlayerRagdoll();

	// Public Accessor Functions
	IRagdoll *GetIRagdoll() const { return m_pRagdoll; }

	// Public Functions
	virtual void OnDataChanged(DataUpdateType_t dutUpdate);
	void ImpactTrace(trace_t *pTrace, int nDamageType, char *czCustomImpactName);

	// Public Variables

private:
	// Constructor & Deconstructor
	C_ForsakenPlayerRagdoll(const C_ForsakenPlayerRagdoll &);

	// Private Functions
	void CreateRagdoll();
	void Interp_Copy(C_BaseAnimatingOverlay *pSourceEntity);

	// Private Variables
	CNetworkVector(m_vecRagdollVelocity);
	CNetworkVector(m_vecRagdollOrigin);
	EHANDLE m_hPlayer;
};

class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS(C_TEPlayerAnimEvent, C_BaseTempEntity);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor

	// Public Accessor Functions

	// Public Functions
	virtual void PostDataUpdate(DataUpdateType_t dutUpdate)
	{
		C_ForsakenPlayer *pPlayer = dynamic_cast<C_ForsakenPlayer*>(m_hPlayer.Get());

		if (pPlayer && !pPlayer->IsDormant())
			pPlayer->DoAnimationEvent((ForsakenPlayerAnimEvent_t)m_nEvent.Get());
	}

	// Public Variables
	CNetworkHandle(CBasePlayer, m_hPlayer);
	CNetworkVar(int, m_nEvent);
};

// HL2 Class Macros
IMPLEMENT_CLIENTCLASS_EVENT(C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent);

BEGIN_RECV_TABLE_NOBASE(C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent)
	RecvPropEHandle(RECVINFO(m_hPlayer)),
	RecvPropInt(RECVINFO(m_nEvent))
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE(C_ForsakenPlayer, DT_ForsakenLocalPlayer)
	RecvPropEHandle(RECVINFO(m_hObjectiveEntity)),
	RecvPropInt(RECVINFO(m_nShotsFired)),
	RecvPropInt(RECVINFO(m_nSharedResources)),
	RecvPropInt(RECVINFO(m_nThrowCounter)),
	RecvPropInt(RECVINFO(m_nLeaderChoice)),
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT(C_ForsakenPlayer, DT_ForsakenPlayer, CForsakenPlayer)
	RecvPropDataTable("forsakenlocaldata", 0, 0, &REFERENCE_RECV_TABLE(DT_ForsakenLocalPlayer)),
	RecvPropFloat(RECVINFO(m_angEyeAngles[0])),
	RecvPropFloat(RECVINFO(m_angEyeAngles[1])),
	RecvPropFloat(RECVINFO(m_angEyeAngles[2])),
	RecvPropEHandle(RECVINFO(m_hRagdoll)),
	RecvPropBool(RECVINFO(m_bIsKicking)),
	RecvPropBool(RECVINFO(m_bIsLeader)),
	RecvPropBool(RECVINFO(m_bIsLeaning)),
	RecvPropBool(RECVINFO(m_bIsSprinting)),
	RecvPropBool(RECVINFO(m_bIsLowSprint)),
	RecvPropBool(RECVINFO(m_bIsStunned)),
	RecvPropFloat(RECVINFO(m_fForwardSpeed)),
	RecvPropFloat(RECVINFO(m_fKickEndTime)),
	RecvPropFloat(RECVINFO(m_fMaxSprint)),
	RecvPropFloat(RECVINFO(m_fSprint)),
	RecvPropFloat(RECVINFO(m_fStunEndTime)),
	RecvPropInt(RECVINFO(m_nLean)),
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT_NOBASE(C_ForsakenPlayerRagdoll, DT_ForsakenPlayerRagdoll, CForsakenPlayerRagdoll)
	RecvPropVector(RECVINFO(m_vecRagdollOrigin)),
	RecvPropEHandle(RECVINFO(m_hPlayer)),
	RecvPropInt(RECVINFO(m_nModelIndex)),
	RecvPropInt(RECVINFO(m_nForceBone)),
	RecvPropVector(RECVINFO(m_vecForce)),
	RecvPropVector(RECVINFO(m_vecRagdollVelocity))
END_RECV_TABLE()

// Constructor & Deconstructor
C_ForsakenPlayerRagdoll::C_ForsakenPlayerRagdoll()
{
}

C_ForsakenPlayerRagdoll::~C_ForsakenPlayerRagdoll()
{
	PhysCleanupFrictionSounds(this);
}

C_ForsakenPlayer::C_ForsakenPlayer() : m_interangEyeAngles("C_ForsakenPlayer::m_interangEyeAngles")
{
	m_statePlayerAnimation = CreatePlayerAnimState(this, this, LEGANIM_9WAY, true);

	m_bLowHealthSound = false;
	m_fLowHealthEndTime = 0.0f;
	m_fLookedAtTime = 0.0f;
	m_nLookingAtPlayerIndex = 0;

	m_angEyeAngles.Init();
	AddVar(&m_angEyeAngles, &m_interangEyeAngles, LATCH_SIMULATION_VAR);
}

C_ForsakenPlayer::~C_ForsakenPlayer()
{
	m_statePlayerAnimation->Release();
}

// Functions
// C_ForsakenPlayerRagdoll
void C_ForsakenPlayerRagdoll::CreateRagdoll()
/*
	Creates the ragdoll for the forsaken player.
	Pre: None.
	Post: A ragdoll for the player has been created.
*/
{
	C_ForsakenPlayer *pPlayer = dynamic_cast<C_ForsakenPlayer*>(m_hPlayer.Get());

	if (pPlayer && !pPlayer->IsDormant())
	{
		// Migrate the model instance to the ragdoll.
		pPlayer->SnatchModelInstance(this);

		bool bRemotePlayer = (pPlayer != C_BasePlayer::GetLocalPlayer());
		VarMapping_t *pVarMap = GetVarMapping();

		/*if (bRemotePlayer)
		{
			Interp_Copy(pPlayer);

			SetAbsOrigin(m_vecRagdollOrigin);
			SetAbsAngles(pPlayer->GetRenderAngles());
			GetRotationInterpolator().Reset();

			m_flAnimTime = pPlayer->m_flAnimTime;
			SetSequence(pPlayer->GetSequence());
			m_flPlaybackRate = pPlayer->GetPlaybackRate();
		}
		else
		{*/
			int nSeq = LookupSequence("walk_lower");

			SetAbsOrigin(m_vecRagdollOrigin);
			SetAbsAngles(pPlayer->GetRenderAngles());
			SetAbsVelocity(m_vecRagdollVelocity);

			SetSequence(nSeq == -1 ? 0 : nSeq);
			SetCycle(0.0);

			Interp_Reset(pVarMap);
		//}
	}
	else
	{
		SetNetworkOrigin(m_vecRagdollOrigin);
		SetAbsOrigin(m_vecRagdollOrigin);
		SetAbsVelocity(m_vecRagdollVelocity);
		Interp_Reset(GetVarMapping());
	}

	SetModelIndex(m_nModelIndex);

	m_nRenderFX = kRenderFxRagdoll;

	BecomeRagdollOnClient(false);
}

void C_ForsakenPlayerRagdoll::ImpactTrace(trace_t *pTrace, int nDamageType, 
	char *czCustomImpactName)
/*
	Traces the impact of a force to the ragdoll.
	Pre: None.
	Post: A impact has been traced.
*/
{
	IPhysicsObject *pPhysObject = VPhysicsGetObject();
	Vector vecDirection = pTrace->endpos - pTrace->startpos;

	if (!pPhysObject)
		return;

	if (nDamageType == DMG_BLAST)
	{
		// Exaggerate the direction.
		vecDirection *= 4000;

		// Apply the force.
		pPhysObject->ApplyForceCenter(vecDirection);
	}
	else
	{
		Vector vecHit;

		// Determine where we got hit.
		VectorMA(pTrace->startpos, pTrace->fraction, vecDirection, vecHit);
		VectorNormalize(vecDirection);

		// Exaggerate the direction.
		vecDirection *= 4000;

		// Apply the force.
		pPhysObject->ApplyForceOffset(vecDirection, vecHit);
	}
}

void C_ForsakenPlayerRagdoll::Interp_Copy(C_BaseAnimatingOverlay *pSourceEntity)
/*
	
	Pre: 
	Post: 
*/
{
	if (!pSourceEntity)
		return;

	VarMapping_t *pDestination = GetVarMapping();
	VarMapping_t *pSource = pSourceEntity->GetVarMapping();

	for (int i = 0; i < pDestination->m_Entries.Count(); i++)
	{
		VarMapEntry_t *pDestinationEntry = &pDestination->m_Entries[i];

		for (int j = 0; j < pSource->m_Entries.Count(); j++)
		{
			VarMapEntry_t *pSourceEntry = &pSource->m_Entries[i];

			if (!Q_strcmp(pSourceEntry->watcher->GetDebugName(), 
				pDestinationEntry->watcher->GetDebugName()))
				pDestinationEntry->watcher->Copy(pSourceEntry->watcher);
		}
	}
}

void C_ForsakenPlayerRagdoll::OnDataChanged(DataUpdateType_t dutUpdate)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::OnDataChanged(dutUpdate);

	if (dutUpdate == DATA_UPDATE_CREATED)
	{
		CreateRagdoll();

		IPhysicsObject *pPhysObject = VPhysicsGetObject();

		if (pPhysObject)
		{
			AngularImpulse aVelocity(0.0f, 0.0f, 0.0f);
			Vector vecNewVelocity = 3 * m_vecRagdollVelocity;

			pPhysObject->AddVelocity(&vecNewVelocity, &aVelocity);
		}
	}
}

// C_ForsakenPlayer
bool C_ForsakenPlayer::ShouldDraw()
/*
	
	Pre: 
	Post: 
*/
{
	if (!IsAlive())
		return false;

	if (GetTeamNumber() == TEAM_SPECTATOR || GetTeamNumber() == TEAM_UNASSIGNED)
		return false;

	if (IsLocalPlayer() && IsRagdoll())
		return true;

	return BaseClass::ShouldDraw();
}

C_BaseAnimating *C_ForsakenPlayer::BecomeRagdollOnClient(bool bCopyEntity)
/*
	
	Pre: 
	Post: 
*/
{
	return NULL;
}

C_ForsakenPlayer *C_ForsakenPlayer::GetLocalForsakenPlayer()
/*
	
	Pre: 
	Post: 
*/
{
	return ToForsakenPlayer(C_BasePlayer::GetLocalPlayer());
}

CForsakenWeaponBase *C_ForsakenPlayer::GetActiveForsakenWeapon() const
/*
	
	Pre: 
	Post: 
*/
{
	return dynamic_cast<CForsakenWeaponBase*>(GetActiveWeapon());
}

const QAngle &C_ForsakenPlayer::GetRenderAngles()
/*
	
	Pre: 
	Post: 
*/
{
	if (IsRagdoll())
		return vec3_angle;
	else
		return m_statePlayerAnimation->GetRenderAngles();
}

float C_ForsakenPlayer::GetMinFOV() const
/*
	
	Pre: 
	Post: 
*/
{
	return 25;
}

IRagdoll *C_ForsakenPlayer::GetRepresentativeRagdoll() const
/*
	
	Pre: 
	Post: 
*/
{
	if (m_hRagdoll.Get())
	{
		C_ForsakenPlayerRagdoll *pRagdoll = (C_ForsakenPlayerRagdoll*)m_hRagdoll.Get();

		return pRagdoll->GetIRagdoll();
	}
	else
		return NULL;
}

ShadowType_t C_ForsakenPlayer::ShadowCastType()
/*
	Returns the type of shadow that should be cast.
	Pre: None.
	Post: The type of shadow that should be cast.
*/
{
	if (!IsVisible())
		return SHADOWS_NONE;

	return SHADOWS_RENDER_TO_TEXTURE_DYNAMIC;
}

void C_ForsakenPlayer::DoAnimationEvent(ForsakenPlayerAnimEvent_t eAnimEvent)
/*
	
	Pre: 
	Post: 
*/
{
	if (eAnimEvent != FORSAKEN_PLAYERANIMEVENT_THROW)
		m_statePlayerAnimation->DoAnimationEvent(eAnimEvent);
}

void C_ForsakenPlayer::MouseID()
/*

	Pre: 
	Post: 
*/
{
	trace_t tr;
	Vector vecForward, vecTraceStart, vecTraceEnd;

	// Find out where we are looking.
	EyeVectors(&vecForward);

	// Get the start and end trace positions.
	vecTraceStart = EyePosition();
	vecTraceEnd = vecTraceStart + vecForward * MAX_TRACE_LENGTH;

	// Do the trace.
	UTIL_TraceLine(vecTraceStart, vecTraceEnd, MASK_SOLID | CONTENTS_HITBOX, this, COLLISION_GROUP_NONE, &tr);

	// Did we hit something?
	if (tr.fraction < 1.0f)
	{
		if (tr.m_pEnt && tr.m_pEnt->IsPlayer() && tr.m_pEnt->IsAlive())
		{
			C_ForsakenPlayer *pLookingAtPlayer = ToForsakenPlayer(tr.m_pEnt);

			// Are we looking at something new?
			if (m_nLookingAtPlayerIndex != pLookingAtPlayer->entindex())
			{
				m_fLookedAtTime = gpGlobals->curtime;
				m_nLookingAtPlayerIndex = pLookingAtPlayer->entindex();
			}
		}
		// Not looking at a alive player.
		else
		{
			m_fLookedAtTime = 0.0f;
			m_nLookingAtPlayerIndex = -1;
		}
	}
	// Not looking at anything.
	else
	{
		m_fLookedAtTime = 0.0f;
		m_nLookingAtPlayerIndex = -1;
	}
}

void C_ForsakenPlayer::OnDataChanged(DataUpdateType_t dutUpdate)
/*
	
	Pre: 
	Post: 
*/
{
	BaseClass::OnDataChanged(dutUpdate);

	if (dutUpdate == DATA_UPDATE_CREATED)
		SetNextClientThink(CLIENT_THINK_ALWAYS);

	UpdateVisibility();
}

void C_ForsakenPlayer::PostDataUpdate(DataUpdateType_t dutUpdate)
/*
	
	Pre: 
	Post: 
*/
{
	SetNetworkAngles(GetLocalAngles());

	BaseClass::PostDataUpdate(dutUpdate);
}

void C_ForsakenPlayer::PostThink()
/*
	
	Pre: 
	Post: 
*/
{
	// Are we the local health player?
	if (this == GetLocalForsakenPlayer())
	{
		// Are we dead? If so, stop the low health sound.
		if (!IsAlive())
		{
			StopSound("Player.LowHealth");

			m_bLowHealthSound = false;
			m_fLowHealthEndTime = 0.0f;
		}
		// Are we below 20% health?
		else if (GetHealth() <= (int)(GetMaxHealth() * 0.2f))
		{
			// If so and we aren't playing the low health sound... play it.
			if (!m_bLowHealthSound && m_fLowHealthEndTime == 0.0f)
			{
				// Play the low health sound and find the sound duration.
				EmitSound("Player.LowHealth");

				// We are now playing the low health sound, so update end times and loop times.
				m_bLowHealthSound = true;
				m_fLowHealthEndTime = gpGlobals->curtime + 10.0f;
			}
			// Is it time for our low health sound to end?
			else if (m_bLowHealthSound && m_fLowHealthEndTime <= gpGlobals->curtime)
			{
				// Stop the sound.
				StopSound("Player.LowHealth");

				m_bLowHealthSound = false;
			}
		}
		else
		{
			// We don't have low health anymore... stop the low health sound.
			if (m_bLowHealthSound)
				StopSound("Player.LowHealth");

			m_bLowHealthSound = false;
		}
	}

	MouseID();

	BaseClass::PostThink();
}

void C_ForsakenPlayer::PreThink()
/*
	
	Pre: 
	Post: 
*/
{
	if (m_lifeState == LIFE_DEAD)
		m_statePlayerAnimation->ClearAnimationState();

	HandleCustomMovement();

	BaseClass::PreThink();
}

void C_ForsakenPlayer::Spawn()
/*
	
	Pre: 
	Post: 
*/
{
	m_bLowHealthSound = false;
	m_fLowHealthEndTime = 0.0f;
	m_fLookedAtTime = 0.0f;
	m_nLookingAtPlayerIndex = -1;

	C_BaseEntity::StopSound("Player.LowHealth");

	BaseClass::Spawn();
}

void C_ForsakenPlayer::UpdateClientSideAnimation()
/*
	
	Pre: 
	Post: 
*/
{
	if (this == C_ForsakenPlayer::GetLocalForsakenPlayer())
		m_statePlayerAnimation->Update(EyeAngles()[YAW], EyeAngles()[PITCH]);
	else
		m_statePlayerAnimation->Update(m_angEyeAngles[YAW], m_angEyeAngles[PITCH]);

	BaseClass::UpdateClientSideAnimation();
}
