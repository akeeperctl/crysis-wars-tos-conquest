// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
// ReSharper disable CppInconsistentNaming
#pragma once

#include "../GenericModule.h"
#include "IEntity.h"


enum ETOSEntityFlags
{
	TOS_ENTITY_FLAG_MUST_RECREATED = (1 << 0),
	TOS_ENTITY_FLAG_SCHEDULED_RECREATION = (1 << 1),
	//TOS_ENTITY_FLAG_ALREADY_RECREATED = (1 << 2),
};

struct STOSEntitySpawnParams
{
	STOSEntitySpawnParams()
		: pSavedScript(nullptr),
		tosFlags(0),
		m_refs(0)
	{
		vanilla = SEntitySpawnParams();
	}

	explicit STOSEntitySpawnParams(const SEntitySpawnParams& _vanillaParams)
		: pSavedScript(nullptr),
		tosFlags(0),
		m_refs(0)
	{
		vanilla = _vanillaParams;
	}

	explicit STOSEntitySpawnParams(const STOSEntitySpawnParams& params)
		: m_refs(0)
	{
		this->tosFlags = params.tosFlags;
		this->vanilla = params.vanilla;
		this->pSavedScript = params.pSavedScript;
	}

	~STOSEntitySpawnParams() { }

	void AddRef() const { ++m_refs; };
	uint GetRefCount() const { return m_refs; };
	void Release() const
	{
		if (--m_refs <= 0)
		{
			delete this;
		}
	}

	SEntitySpawnParams vanilla;

	string savedName; //ИСПРАВИЛО БАГ https://github.com/akeeperctl/crysis-wars-the-other-side/issues/6

	IScriptTable* pSavedScript;

	uint32 tosFlags;

private:
	mutable uint	m_refs;
};


typedef std::vector<EntityId> TVecEntities;
typedef std::map<EntityId, SEntitySpawnParams*> TMapParams;
typedef std::map<EntityId, _smart_ptr<STOSEntitySpawnParams>> TMapTOSParams;

/**
 * \brief Модуль создания сущностей, используемых в моде The Other Side
 * \note Также модуль предназначен для воскрешения сущностей, удаленных во время работы консольной команды sv_restart.
 */
class CTOSEntitySpawnModule final :
	public CTOSGenericModule
{
public:
	CTOSEntitySpawnModule();
	~CTOSEntitySpawnModule() override;

	//ITOSGameModule
	void OnExtraGameplayEvent(IEntity* pEntity, const STOSGameEvent& event) override;
	const char* GetName() override { return "CTOSEntitySpawnModule"; };
	void Init() override;
	void Update(float frametime) override;
	void Serialize(TSerialize ser) override;
	//~ITOSGameModule

	/**
	 * \brief Создаёт сущность по определенным параметрам
	 * \param params - параметры создания сущности
	 * \param sendTosEvent - если True, то при спавне произойдет отправка события eEGE_TOSEntityOnSpawn
	 * \return Если успешно, то указатель IEntity* на созданную сущность, иначе nullptr
	 */
	static IEntity* SpawnEntity(STOSEntitySpawnParams& params, bool sendTosEvent = true);

	/**
	 * \brief Проверяет, должна ли быть воссоздана сущность после sv_restart
	 * \param pEntity - указатель на проверяемую сущность
	 * \return Если сущность должна быть воссоздана после sv_restart, то True, иначе False
	 */
	bool MustBeRecreated(const IEntity* pEntity) const;

private:

	/**
	 * \brief Запланировать пересоздание сущности после sv_restart 
	 * \param pEntity - указатель на сущность, которую нужно пересоздать
	 */
	void ScheduleRecreation(const IEntity* pEntity);
	bool HaveSavedParams(const IEntity* pEntity) const;

	void DebugDraw(const Vec2& screenPos, float fontSize, float interval, int maxElemNum, bool draw);

	static TVecEntities s_markedForRecreation; // сущности которые должны быть пересозданы после sv_restart
	TMapTOSParams m_scheduledRecreations; 
	TMapParams m_savedParams; // сюда добавляются entityId и SEntitySpawnParams от сущности, заспавненной в этом модуле
};