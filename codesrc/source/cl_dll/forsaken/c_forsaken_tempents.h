#ifndef __CLIENT_FORSAKEN_TEMPENTS_H_
#define __CLIENT_FORSAKEN_TEMPENTS_H_

class C_BaseEntity;
class C_LocalTempEntity;
struct model_t;

#include "mempool.h"
#include "UtlLinkedList.h"

class CForsakenTempEnts : public CTempEnts
{
public:
	typedef CTempEnts BaseClass;

	// Constructor & Deconstructor

	// Public Functions
	virtual void CSEjectBrass(const Vector &rvecPosition, const QAngle &rangAngles, int nType, 
		int nShellType, CBasePlayer *pPlayer);
	virtual void Init();
	virtual void LevelInit();
	virtual void LevelShutdown();
	virtual void MuzzleFlash(const Vector &rvecPosition, const QAngle &rAngles, int nType, ClientEntityHandle_t hEntity, 
		bool bFirstPerson = false);
	virtual void MuzzleFlash(int nType, ClientEntityHandle_t hEntity, int nAttachmentIndex, bool bFirstPerson);
	virtual void Shutdown();

	// Public Variables

protected:
	// Protected Functions
	void Forsaken_Muzzle(float scale, ClientEntityHandle_t hEntity, int attachmentIndex, bool isCrossShaped);

	// Protected Variables
	struct model_t *m_pShell_1201FP;
	struct model_t *m_pShell_HK23;
	struct model_t *m_pShell_MAC10;
	struct model_t *m_pShell_P99;
	struct model_t *m_pShell_RagingBull;
	struct model_t *m_pShell_Remington;
	struct model_t *m_pShell_SA80;
};

#endif