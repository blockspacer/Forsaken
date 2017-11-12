#ifndef __CLIENT_FORSAKEN_PLAYER_CLOAK_H_
#define __CLIENT_FORSAKEN_PLAYER_CLOAK_H_

enum
{
	FORSAKEN_CLOAK_BONEFOLLOWER1 = 0,
	FORSAKEN_CLOAK_BONEFOLLOWER2 = 0,
	FORSAKEN_CLOAK_BONEFOLLOWER3 = 0,
};

class C_ForsakenPlayer_Cloak : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS(C_ForsakenPlayer_Cloak, C_BaseAnimatingOverlay);

	// Constructor & Deconstructor
	C_ForsakenPlayer_Cloak();
	~C_ForsakenPlayer_Cloak();

	// Public Accessor Functions
	void CreateRagdoll(CBasePlayer *pParentPlayer);

	// Public Functions
	bool CreateVPhysics();
	void ImpactTrace(trace_t *pTrace, int nDamageType, char *czCustomImpactName);

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	CNetworkVector(m_vecRagdollVelocity);
	CNetworkVector(m_vecRagdollOrigin);
	EHANDLE m_hPlayer;
};

#endif