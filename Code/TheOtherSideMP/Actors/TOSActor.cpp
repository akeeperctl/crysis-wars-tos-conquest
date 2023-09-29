#include "StdAfx.h"
#include "TOSActor.h"
#include "Actor.h"

CTOSActor::CTOSActor() : 
	m_masterEntityId(0),
	m_slaveEntityId(0)
{
}

CTOSActor::~CTOSActor()
{

}

void CTOSActor::PostInit(IGameObject* pGameObject)
{
	CryLogAlways("[C++][%s][%s][CTOSActor::PostInit] Actor: %s|%i",
		TOS_Debug::GetEnv(), TOS_Debug::GetAct(1), GetEntity()->GetName(), GetEntity()->GetId());

	CActor::PostInit(pGameObject);
}

bool CTOSActor::NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags)
{
	CActor::NetSerialize(ser,aspect,profile,flags);
	return false;
}

void CTOSActor::Update(SEntityUpdateContext& ctx, int updateSlot)
{
	CActor::Update(ctx, updateSlot);
}

void CTOSActor::Release()
{
	CryLogAlways("[C++][%s][%s][CTOSActor::Release] Actor: %s|%i",
		TOS_Debug::GetEnv(), TOS_Debug::GetAct(1), GetEntity()->GetName(), GetEntity()->GetId());

	CActor::Release();
}

void CTOSActor::SetMasterEntityId(EntityId id)
{
	//gEnv->pRenderer->GetFrameID();
	m_masterEntityId = id;
}

void CTOSActor::SetSlaveEntityId(EntityId id)
{
	m_slaveEntityId = id;
}
