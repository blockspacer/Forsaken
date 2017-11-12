#ifndef __FORSAKEN_CAPTURE_ENTITY_H_
#define __FORSAKEN_CAPTURE_ENTITY_H_

class C_ForsakenCaptureObjectiveEntity : public C_ForsakenObjectiveEntity
{
public:
	DECLARE_CLASS(C_ForsakenCaptureObjectiveEntity, C_ForsakenObjectiveEntity);
	DECLARE_CLIENTCLASS();

	// Constructor & Deconstructor
	C_ForsakenCaptureObjectiveEntity();
	virtual ~C_ForsakenCaptureObjectiveEntity();

	// Public Accessor Functions
	float GetCaptureTime() { return m_fCaptureTime; }
	float GetRemainingTimeCiv() { return m_fRemainingCaptureTimeCiv; }
	float GetRemainingTimeGov() { return m_fRemainingCaptureTimeGov; }
	float GetRemainingTimeRel() { return m_fRemainingCaptureTimeRel; }
	int GetResourceAmount() { return m_nResourceAmount; }
	int GetResourceType() { return m_nResourceType; }
	int GetTeamCapturing() { return m_nTeamCapturing; }
	virtual int GetObjectiveType() { return OBJECTIVE_TYPE_CAPTURE; }
	virtual void Simulate();

	// Public Functions

	// Public Variables

protected:
	// Protected Functions

	// Protected Variables
	float m_fCaptureTime;
	float m_fRemainingCaptureTimeCiv;
	float m_fRemainingCaptureTimeGov;
	float m_fRemainingCaptureTimeRel;
	float m_fStepOutTimeCiv;
	float m_fStepOutTimeGov;
	float m_fStepOutTimeRel;
	int m_nPlayerWeighting;
	int m_nResourceAmount;
	int m_nResourceType;
	int m_nTeamCanCapture;
	int m_nTeamCapturing;
};

#endif