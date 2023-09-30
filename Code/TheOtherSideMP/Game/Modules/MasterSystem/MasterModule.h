#pragma once

#include "TheOtherSideMP/Game/Modules/ITOSGameModule.h"
#include "TheOtherSideMP/Game/TOSGame.h"

class CTOSMasterSynchronizer;
class CTOSGenericSynchronizer;

/**
 * TOS Master Info
 * �� �������: ������ ���������� � ������ ������ �������
 * �� �������: ����������.
 * �������� ������������.
 */
//struct STOSMasterInfo
//{
//public:
//	STOSMasterInfo() :
//		id(0),
//		slaveId(0)
//	{}
//
//	STOSMasterInfo(EntityId _id, EntityId _slaveId) :
//		id(_id),
//		slaveId(_slaveId)
//	{}
//
//	~STOSMasterInfo()
//	{
//		delete this;
//	}
//
//	EntityId id;
//	EntityId slaveId;
//};

/**
 * TOS Master System
 * �� �������: ������ ���������� � ������ ���� ��������.
 * �� �������: ������ �� ������.
 * ������ - ��� �����, ����������� ������� ������.
 * ������������: �����������.
 */
class CTOSMasterModule : ITOSGameModule
{
public:
	CTOSMasterModule();
	~CTOSMasterModule();

	//ITOSGameModule
	void OnExtraGameplayEvent(IEntity* pEntity, const STOSGameEvent& event) override;
	void GetMemoryStatistics(ICrySizer* s) override;
	const char* GetName() override {return "CTOSMasterSystem";};
	void Init() override;
	void Update(float frametime) override;
	void Serialize(TSerialize ser) override;
	//ITOSGameModule

	void MasterAdd(const IEntity* pMasterEntity);
	void MasterRemove(const IEntity* pMasterEntity);
	bool IsMaster(const IEntity* pMasterEntity);
	void GetMasters(std::map<EntityId, EntityId>& masters);
	IEntity* GetSlave(const IEntity* pMasterEntity);
	void DebugDrawMasters(const Vec2& screenPos = {20,300}, float fontSize = 1.2f, float interval = 20.0f, int maxElemNum = 5);
	CTOSGenericSynchronizer* GetSynchronizer() const override;

private:
	std::map<EntityId, EntityId> m_masters;
	CTOSMasterSynchronizer* m_pSynchronizer;

};