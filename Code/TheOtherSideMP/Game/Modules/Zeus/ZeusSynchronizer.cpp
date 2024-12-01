#include "StdAfx.h"
#include "GameActions.h"
#include "ZeusSynchronizer.h"
#include "ZeusModule.h"
#include <TheOtherSideMP/Helpers/TOS_AI.h>
#include <TheOtherSideMP/Helpers/TOS_Entity.h>
#include <TheOtherSideMP/Helpers/TOS_Inventory.h>
#include <TheOtherSideMP/Helpers/TOS_NET.h>

//------------------------------------------------------------------------
IMPLEMENT_RMI(CTOSZeusSynchronizer, SvRequestMakeZeus)
{
	// Здесь пишем всё, что должно выполниться на сервере

	//TODO:
	// 1) Не спавнятся/удаляются/перемещаются (синхронно) сущности в мультиплеере
	// 2) Через меню паузы можно кликнуть по объектам...
	// 3) Странный вылет при выходе зевса из матча
	// 4) Не скрыта модель зевса
	// 5) Дергается модель зевса при передвижении (рассинхрон) (так ли оно важно, мж так и оставить?)

	if (gEnv->bServer)
	{
		CryLog("[C++][%s][%s][SvRequestMakeZeus]",
			TOS_Debug::GetEnv(), TOS_Debug::GetAct(3));

		auto pZeusModule = g_pTOSGame->GetZeusModule();
		auto pTOSPlayer = static_cast<CTOSPlayer*>(g_pGame->GetIGameFramework()->GetIActorSystem()->GetActorByChannelId(params.playerChannelId));
		assert(pZeusModule != nullptr);
		assert(pTOSPlayer != nullptr);

		auto pInventory = pTOSPlayer->GetInventory();
		assert(pInventory != nullptr);

		// Отбираем оружие
		pInventory->HolsterItem(true);
		pInventory->RemoveAllItems();
		TOS_Inventory::GiveItem(pTOSPlayer, "NightVision", false, false, false);

		// Сбрасываем статы
		pTOSPlayer->GetActorStats()->inAir = 0.0f;
		pTOSPlayer->GetActorStats()->onGround = 0.0f;

		// Становимся неуязвимым к урону
		pTOSPlayer->SetMeZeus(true);
		pTOSPlayer->GetGameObject()->ChangedNetworkState(TOS_NET::SERVER_ASPECT_STATIC);

		// Откл. ИИ для перса зевса
		auto pAI = pTOSPlayer->GetEntity()->GetAI();
		if (pAI)
			TOS_AI::SendEvent(pAI, AIEVENT_DISABLE);

		pTOSPlayer->GetGameObject()->SetAspectProfile(eEA_Physics, eAP_Spectator);

		// Режим полета со столкновениями
		pTOSPlayer->SetFlyMode(1);

		// убираем нанокостюм
		CNanoSuit* pSuit = pTOSPlayer->GetNanoSuit();
		if (pSuit)
		{
			pSuit->SetMode(NANOMODE_DEFENSE);
			pSuit->SetModeDefect(NANOMODE_CLOAK, true);
			pSuit->SetModeDefect(NANOMODE_SPEED, true);
			pSuit->SetModeDefect(NANOMODE_STRENGTH, true);
		}

		if (pTOSPlayer->GetAnimatedCharacter())
		{
			pTOSPlayer->GetAnimatedCharacter()->ForceRefreshPhysicalColliderMode();
			pTOSPlayer->GetAnimatedCharacter()->RequestPhysicalColliderMode(eColliderMode_Spectator, eColliderModeLayer_Game, "CTOSZeusModule::MakeZeus");
		}

		pTOSPlayer->GetGameObject()->InvokeRMI(CTOSActor::ClMarkHideMe(), NetHideMeParams(true), eRMI_ToAllClients);

		auto pSync = static_cast<CTOSZeusSynchronizer*>(pZeusModule->GetSynchronizer());
		assert(pSync != nullptr);

		pSync->GetGameObject()->InvokeRMI(CTOSZeusSynchronizer::ClMakeZeus(), params, eRMI_ToClientChannel, params.playerChannelId);
	}

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CTOSZeusSynchronizer, ClMakeZeus)
{
	// Здесь пишем всё, что должно выполниться на клиенте

	if (gEnv->bClient)
	{
		CryLog("[C++][%s][%s][ClMakeZeus]",
			TOS_Debug::GetEnv(), TOS_Debug::GetAct(3));

		auto pZeusModule = g_pTOSGame->GetZeusModule();
		auto pTOSPlayer = pZeusModule->GetPlayer();
		//auto pTOSPlayer = static_cast<CTOSPlayer*>(g_pGame->GetIGameFramework()->GetClientActor());

		assert(pZeusModule != nullptr);
		assert(pTOSPlayer != nullptr);

		auto pInventory = pTOSPlayer->GetInventory();
		assert(pInventory != nullptr);

		// Отбираем оружие
		pInventory->HolsterItem(true);
		pInventory->RemoveAllItems();

		// Убираем лишние действия
		g_pGameActions->FilterZeus()->Enable(true);

		// Скрываем HUD игрока
		pZeusModule->GetHUD().ShowPlayerHUD(false);
		pZeusModule->GetHUD().ShowZeusMenu(true);

		//Включаем мышь
		pZeusModule->GetLocal().ShowMouse(true);
		pZeusModule->GetLocal().SetFlag(CTOSZeusModule::EFlag::CanUseMouse, true);

		//Включаем режим зевса
		pZeusModule->GetLocal().SetFlag(CTOSZeusModule::EFlag::Zeusing, true);
		// pZeusModule->SetPlayer(pTOSPlayer);

		pTOSPlayer->GetGameObject()->SetAspectProfile(eEA_Physics, eAP_Spectator);

		// Режим полета со столкновениями
		pTOSPlayer->SetFlyMode(1);
		pTOSPlayer->SetMeZeus(true);

		// убираем нанокостюм
		CNanoSuit* pSuit = pTOSPlayer->GetNanoSuit();
		if (pSuit)
		{
			pSuit->SetMode(NANOMODE_DEFENSE);
			pSuit->SetModeDefect(NANOMODE_CLOAK, true);
			pSuit->SetModeDefect(NANOMODE_SPEED, true);
			pSuit->SetModeDefect(NANOMODE_STRENGTH, true);
		}

		if (pTOSPlayer->GetAnimatedCharacter())
		{
			pTOSPlayer->GetAnimatedCharacter()->ForceRefreshPhysicalColliderMode();
			pTOSPlayer->GetAnimatedCharacter()->RequestPhysicalColliderMode(eColliderMode_Spectator, eColliderModeLayer_Game, "CTOSZeusModule::MakeZeus");
		}

		pTOSPlayer->GetGameObject()->InvokeRMI(CTOSActor::SvRequestHideMe(), NetHideMeParams(true), eRMI_ToServer);
	}

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CTOSZeusSynchronizer, SvRequestSpawnEntity)
{
	// Здесь пишем всё, что должно выполниться на сервере

	//TODO:
	// 1) Не спавнятся/удаляются/перемещаются (синхронно) сущности в мультиплеере
	// 2) Через меню паузы можно кликнуть по объектам...
	// 3) Странный вылет при выходе зевса из матча
	// 4) Не скрыта модель зевса
	// 5) Дергается модель зевса при передвижении (рассинхрон) (так ли оно важно, мж так и оставить?)

	if (gEnv->bServer)
	{
		CryLog("[C++][%s][%s][SvRequestSpawnEntity]",
			TOS_Debug::GetEnv(), TOS_Debug::GetAct(3));

		auto pZeusModule = g_pTOSGame->GetZeusModule();
		assert(pZeusModule != nullptr);

		STOSEntitySpawnParams spawnParams;
		spawnParams.vanilla.bStaticEntityId = false; // true - вылетает в редакторе и медленно работает O(n), false O(1)
		spawnParams.vanilla.bIgnoreLock = false; // spawn lock игнор

		auto pPlayer = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActorByChannelId(params.playerChannelId);
		if (pPlayer)
			spawnParams.authorityPlayerName = pPlayer->GetEntity()->GetName();

		const string* const psClassName = &params.className;
		IEntityClass* pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(psClassName->c_str());

		const string name = string("zeus_") + psClassName->c_str();
		spawnParams.vanilla.sName = name;
		spawnParams.vanilla.pClass = pClass;
		spawnParams.vanilla.vPosition = params.pos;

		const auto pArchetype = gEnv->pEntitySystem->LoadEntityArchetype(psClassName->c_str());
		if (pArchetype)
			spawnParams.vanilla.pArchetype = pArchetype;

		if (!pClass && !pArchetype)
		{
			CryLogError("[Zeus] not defined entity class '%s'", psClassName->c_str());
			return true;
		}

		IEntity* pSpawned = TOS_Entity::Spawn(spawnParams, false);
		if (!pSpawned)
		{
			CryLogError("[Zeus] entity with class '%s' spawn failed!", psClassName->c_str());
			return true;
		}

		//pSpawned->Hide(true);
		// TODO: 01/12/2024 malformed packet при спавне техники какого хуя 


		//const EntityId spawnedId = pSpawned->GetId();
		//char buffer[64];
		//sprintf(buffer, "%d", spawnedId);
		//pSpawned->SetName(name + "_" + buffer);

		// Извещаем клиента, о том, что он может перемещать заспавненную сущность
		//auto pSync = static_cast<CTOSZeusSynchronizer*>(pZeusModule->GetSynchronizer());
		//assert(pSync != nullptr);

		//NetSpawnedInfo info;
		//info.spawnedId = spawnedId;
		//info.spawnedPos = params.pos;
		//pSync->GetGameObject()->InvokeRMI(CTOSZeusSynchronizer::ClSpawnEntity(), info, eRMI_ToClientChannel, params.playerChannelId);
	}

	return true;
}

//------------------------------------------------------------------------
IMPLEMENT_RMI(CTOSZeusSynchronizer, ClSpawnEntity)
{
	// Здесь пишем всё, что должно выполниться на клиенте

	if (gEnv->bClient)
	{
		CryLog("[C++][%s][%s][ClSpawnEntity]",
			TOS_Debug::GetEnv(), TOS_Debug::GetAct(3));

		auto pZeusModule = g_pTOSGame->GetZeusModule();
		assert(pZeusModule != nullptr);

		pZeusModule->GetLocal().m_dragging = true;
		pZeusModule->GetHUD().m_menuSpawnHandling = true;

		pZeusModule->GetLocal().SelectEntity(params.spawnedId);
		pZeusModule->GetLocal().m_curClickedEntityId = params.spawnedId;
		pZeusModule->GetLocal().m_mouseOveredEntityId = params.spawnedId;

		pZeusModule->GetLocal().m_selectStartEntitiesPositions[params.spawnedId] = params.spawnedPos;
		pZeusModule->GetLocal().m_storedEntitiesPositions[params.spawnedId] = params.spawnedPos;
		pZeusModule->GetLocal().m_clickedSelectStartPos = params.spawnedPos;
	}

	return true;
}
