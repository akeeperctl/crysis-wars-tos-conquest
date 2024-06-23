/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Simple Actor implementation
  
 -------------------------------------------------------------------------
  History:
  - 7:10:2004   14:46 : Created by Mбrcio Martins

*************************************************************************/
#ifndef __Actor_H__
#define __Actor_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IActorSystem.h>
#include <IActionMapManager.h>
#include <CoherentValue.h>
#include <IMusicSystem.h>

#include "Game.h" // for stance enum
#include "IAgent.h" // for stance enum
#include "IAnimationGraph.h"
#include "IAnimatedCharacter.h"
#include "IMovementController.h"
#include "ScreenEffects.h"
#include "GrabHandler.h"
#include "WeaponAttachmentManager.h"

struct SActorFrameMovementParams
{
	SActorFrameMovementParams()
		: desiredVelocity(ZERO),
		desiredLean(0.0f),
		deltaAngles(ZERO),
		lookTarget(ZERO),
		aimTarget(ZERO),
		lookIK(false),
		aimIK(false),
		jump(false),
		strengthJump(false),
		sprint(0.0f),
		stance(STANCE_NULL),
		allowStrafe(false) { }

	// desired velocity for this frame (meters/second) (in local space)
	Vec3 desiredVelocity;
	// desired lean
	float desiredLean;
	// desired turn
	Ang3 deltaAngles;
	Vec3 lookTarget;
	Vec3 aimTarget;
	// prediction
	SPredictedCharacterStates prediction;
	// look target
	bool lookIK;
	// aim target
	bool  aimIK;
	bool  jump;
	bool  strengthJump;
	float sprint;
	// stance
	EStance stance;

	bool allowStrafe;
};

struct IActorMovementController : public IMovementController
{
	struct SStats
	{
		bool idle;
	};

	virtual void Reset() = 0;
	// return true if params was filled out, false if not
	virtual bool Update(float frameTime, SActorFrameMovementParams& params) = 0;
	virtual bool GetStats(SStats& stats) = 0;
	virtual void PostUpdate(float frameTime) = 0;
	virtual void Release() = 0;
	virtual void BindInputs(IAnimationGraphState*) {}
	virtual void Serialize(TSerialize& ser) = 0;
};

//FIXME:not sure to put this here
constexpr int ZEROG_AREA_ID = PHYS_FOREIGN_ID_USER + 1;

// Collision ray piercability to ignore leaves and other things
enum
{
	COLLISION_RAY_PIERCABILITY = 10
};

enum EBonesID
{
	BONE_BIP01 = 0,
	BONE_SPINE,
	BONE_SPINE2,
	BONE_SPINE3,
	BONE_HEAD,
	BONE_EYE_R,
	BONE_EYE_L,
	BONE_WEAPON,
	BONE_FOOT_R,
	BONE_FOOT_L,
	BONE_ARM_R,
	BONE_ARM_L,
	BONE_CALF_R,
	BONE_CALF_L,
	BONE_ID_NUM
};

//For first person grabbing
enum EGrabbedCharacterType
{
	eGCT_UNKNOWN = 0,
	eGCT_HUMAN,
	eGCT_TROOPER,
	eGCT_ALIEN
};

//represent the key status of the actor
enum
{
	ACTION_JUMP      = (1 << 0),
	ACTION_CROUCH    = (1 << 1),
	ACTION_PRONE     = (1 << 2),
	ACTION_SPRINT    = (1 << 3),
	ACTION_LEANLEFT  = (1 << 4),
	ACTION_LEANRIGHT = (1 << 5),
	ACTION_GYROSCOPE = (1 << 6),
	ACTION_ZEROGBACK = (1 << 7),
	ACTION_RELAXED   = (1 << 8),
	ACTION_AIMING    = (1 << 9),
	ACTION_STEALTH   = (1 << 10),
	ACTION_MOVE      = (1 << 11)
};

struct IVehicle;
struct IInventory;

struct SActorParams
{
	float maxGrabMass;
	float maxGrabVolume;
	bool  nanoSuitActive;

	Vec3  viewPivot;
	float viewDistance;
	float viewHeightOffset;

	float viewFoVScale;
	float viewSensitivity;

	Vec3  vLimitDir;
	float vLimitRangeH;
	float vLimitRangeV;
	float vLimitRangeVUp;
	float vLimitRangeVDown;

	float weaponBobbingMultiplier;
	float weaponInertiaMultiplier;

	float speedMultiplier;

	float timeImpulseRecover;

	SActorParams()
	{
		viewPivot.Set(0, 0, 0);
		viewDistance     = 0;
		viewHeightOffset = 0;

		vLimitDir.Set(0, 0, 0);
		vLimitRangeH     = 0.0f;
		vLimitRangeV     = 0.0f;
		vLimitRangeVUp   = 0.0f;
		vLimitRangeVDown = 0.0f;

		viewFoVScale    = 1.0f;
		viewSensitivity = 1.0f;

		weaponInertiaMultiplier = 1.0f;
		weaponBobbingMultiplier = 1.0f;

		speedMultiplier = 1.0f;

		timeImpulseRecover = 0.0f;
	}
};

struct SActorStats
{
	float inAir; //double purpose, tells if the actor is in air and for how long
	float onGround; //double purpose, tells if the actor is on ground and for how long

	float inWaterTimer;
	float relativeWaterLevel;
	float relativeBottomDepth;
	float worldWaterLevelDelta;
	bool  swimJumping;

	float headUnderWaterTimer;
	float worldWaterLevel;

	float speed;
	float speedFlat;
	Vec3  angVelocity;

	Vec3 velocity;
	Vec3 velocityUnconstrained;

	Vec3 gravity;

	bool  inZeroG;
	Vec3  zeroGUp;
	float nextZeroGCheck;

	int   movementDir;
	float inFiring;
	float mass;
	Vec3  forceUpVector;
	Vec3  forceLookVector;
	float bobCycle;

	float grabbedTimer;
	bool  isGrabbed;
	bool  isRagDoll;

	int8                 inFreefallLast;
	CCoherentValue<int8> inFreefall; //1 == freefalling, 2 == parachute open
	CCoherentValue<bool> isHidden;
	CCoherentValue<bool> isShattered;
	CCoherentValue<bool> isFrozen;
	CCoherentValue<bool> isOnLadder;

	float timeFallen;

	EntityId mountedWeaponID;

	int groundMaterialIdx;

	SActorStats()
	{
		memset(this, 0, sizeof(SActorStats));
	}
};

struct SStanceInfo
{
	//dimensions
	float heightCollider;
	float heightPivot;

	bool useCapsule;

	Vec3 size;

	//view
	Vec3 viewOffset;
	Vec3 leanLeftViewOffset;
	Vec3 leanRightViewOffset;

	//weapon
	Vec3 weaponOffset;
	Vec3 leanLeftWeaponOffset;
	Vec3 leanRightWeaponOffset;

	//movement
	float normalSpeed;
	float maxSpeed;

	//misc
	char name[32];

	Vec3 modelOffset;

	Vec3 GetViewOffsetWithLean(const float lean) const
	{
		if (lean < -0.01f)
		{
			const float a = CLAMP(-lean, 0.0f, 1.0f);
			return viewOffset + a * (leanLeftViewOffset - viewOffset);
		}
		if (lean > 0.01f)
		{
			const float a = CLAMP(lean, 0.0f, 1.0f);
			return viewOffset + a * (leanRightViewOffset - viewOffset);
		}
		return viewOffset;
	}

	Vec3 GetWeaponOffsetWithLean(const float lean) const
	{
		if (lean < -0.01f)
		{
			const float a = CLAMP(-lean, 0.0f, 1.0f);
			return weaponOffset + a * (leanLeftWeaponOffset - weaponOffset);
		}
		if (lean > 0.01f)
		{
			const float a = CLAMP(lean, 0.0f, 1.0f);
			return weaponOffset + a * (leanRightWeaponOffset - weaponOffset);
		}
		return weaponOffset;
	}

	static Vec3 GetOffsetWithLean(const float lean, const Vec3& sOffset, const Vec3& sLeftLean, const Vec3& sRightLean)
	{
		if (lean < -0.01f)
		{
			const float a = CLAMP(-lean, 0.0f, 1.0f);
			return sOffset + a * (sLeftLean - sOffset);
		}
		if (lean > 0.01f)
		{
			const float a = CLAMP(lean, 0.0f, 1.0f);
			return sOffset + a * (sRightLean - sOffset);
		}
		return sOffset;
	}

	// Returns the size of the stance including the collider and the ground location.
	AABB GetStanceBounds() const
	{
		AABB aabb(-size, size);
		// Compensate for capsules.
		if (useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}
		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;
		// Include ground position
		aabb.Add(Vec3(0, 0, 0));
		// Make relative to the entity
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;
		return aabb;
	}

	// Returns the size of the collider of the stance.
	AABB GetColliderBounds() const
	{
		AABB aabb(-size, size);
		// Compensate for capsules.
		if (useCapsule)
		{
			aabb.min.z -= max(size.x, size.y);
			aabb.max.z += max(size.x, size.y);
		}
		// Lift from ground.
		aabb.min.z += heightCollider;
		aabb.max.z += heightCollider;
		// Make relative to the entity
		aabb.min.z -= heightPivot;
		aabb.max.z -= heightPivot;
		return aabb;
	}

	SStanceInfo()
	{
		heightCollider = 0;
		heightPivot    = 0;

		useCapsule = false;

		size.Set(0.5f, 0.5f, 0.5f);
		modelOffset.Set(0, 0, 0);
		viewOffset.Set(0, 0, 0);
		leanLeftViewOffset.Set(0, 0, 0);
		leanRightViewOffset.Set(0, 0, 0);
		weaponOffset.Set(0, 0, 0);
		leanLeftWeaponOffset.Set(0, 0, 0);
		leanRightWeaponOffset.Set(0, 0, 0);

		maxSpeed = 0.001f;

		strcpy(name, "null");
	}
};

enum
{
	IKLIMB_RIGHTHAND = (1 << 0),
	IKLIMB_LEFTHAND  = (1 << 1)
};

struct SIKLimb
{
	int flags;

	int rootBoneID;
	int endBoneID;
	int middleBoneID; //optional for custom IK

	Vec3 goalWPos;
	Vec3 currentWPos;

	Vec3 goalNormal;

	//limb local position in the animation
	Vec3 lAnimPos;
	Vec3 lAnimPosLast;

	char name[64];

	float recoverTime; //the time to go back to the animation pose
	float recoverTimeMax;

	float blendTime;
	float blendTimeMax;

	int blendID;

	int characterSlot;

	bool keepGoalPos;

	SIKLimb()
	{
		memset(this, 0, sizeof(SIKLimb));
	}

	void SetLimb(const int slot, const char* limbName, const int rootID, const int midID, const int endID, const int iFlags)
	{
		rootBoneID   = rootID;
		endBoneID    = endID;
		middleBoneID = midID;

		strncpy(name, limbName, 64);
		name[63] = 0;

		blendID = -1;

		flags = iFlags;

		characterSlot = slot;
	}

	void SetWPos(IEntity* pOwner, const Vec3& pos, const Vec3& normal, float blend, float recover, int requestID);
	void Update(IEntity* pOwner, float frameTime);
};

struct SLinkStats
{
#define LINKED_FREELOOK (1<<0)
#define LINKED_VEHICLE (1<<1)

	//which entity are we linked to?
	EntityId linkID;
	//
	uint32 flags;
	//when freelook is on, these are used to update the player rotation
	Quat baseQuatLinked;
	Quat viewQuatLinked;

	//
	SLinkStats()
		: linkID(0),
		flags(0),
		baseQuatLinked(IDENTITY),
		viewQuatLinked(IDENTITY) { }

	SLinkStats(const EntityId eID, const uint32 flgs = 0)
		: linkID(eID),
		flags(flgs),
		baseQuatLinked(IDENTITY),
		viewQuatLinked(IDENTITY) { }

	ILINE void UnLink()
	{
		linkID = 0;
		flags  = 0;
	}

	ILINE bool CanRotate() const
	{
		return (!linkID || flags & LINKED_FREELOOK);
	}

	ILINE bool CanMove() const
	{
		return (!linkID);
	}

	ILINE bool CanDoIK() const
	{
		return (!linkID);
	}

	ILINE bool CanMoveCharacter() const
	{
		return (!linkID);
	}

	ILINE IEntity* GetLinked()
	{
		if (!linkID)
			return nullptr;
		IEntity* pEnt = gEnv->pEntitySystem->GetEntity(linkID);
		//if the entity doesnt exist anymore forget about it.
		if (!pEnt)
			UnLink();

		return pEnt;
	}

	IVehicle* GetLinkedVehicle();

	void Serialize(TSerialize ser);
};

class CItem;
class CWeapon;

class CActor :
	//TheOtherSide crash fix
	public CGameObjectExtensionHelper<CActor, IActor, 64>,
	//public CGameObjectExtensionHelper<CActor, IActor>,
	//~TheOtherSide
	public IGameObjectView, public IGameObjectProfileManager
{
public:

	friend class CTOSActor;

	struct ItemIdParam
	{
		ItemIdParam()
			: itemId(0) {};

		explicit ItemIdParam(const EntityId item)
			: itemId(item) {}

		void SerializeWith(TSerialize ser)
		{
			ser.Value("itemId", itemId, 'eid');
		}

		EntityId itemId;
	};

	struct DropItemParams
	{
		DropItemParams()
			: impulseScale(1.0f),
			itemId(0),
			selectNext(true),
			byDeath(false) {};

		DropItemParams(const EntityId item, const float scale, const bool next = true, const bool death = false)
			: impulseScale(scale),
			itemId(item),
			selectNext(next),
			byDeath(death) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("itemId", itemId, 'eid');
			ser.Value("impulseScale", impulseScale, 'iScl');
			ser.Value("selectNext", selectNext, 'bool');
			ser.Value("byDeath", byDeath, 'bool');
		}

		float    impulseScale;
		EntityId itemId;
		bool     selectNext;
		bool     byDeath;
	};

	struct ReviveParams
	{
		ReviveParams()
			: teamId(0),
			pos(0, 0, 0),
			rot(0, 0, 0, 1.0f) {};

		ReviveParams(const Vec3& p, const Ang3& a, const int tId)
			: teamId(tId),
			pos(p),
			rot(Quat::CreateRotationXYZ(a)) {};

		ReviveParams(const Vec3& p, const Quat& q, const int tId)
			: teamId(tId),
			pos(p),
			rot(q) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("teamId", teamId, 'team');
			ser.Value("pos", pos, 'wrld');
			ser.Value("rot", rot, 'ori1');
		};

		int  teamId;
		Vec3 pos;
		Quat rot;
	};

	struct ReviveInVehicleParams
	{
		ReviveInVehicleParams()
			: teamId(0),
			vehicleId(0),
			seatId(0) {};

		ReviveInVehicleParams(const EntityId vId, const int sId, const int tId)
			: teamId(tId),
			vehicleId(vId),
			seatId(sId) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("teamId", teamId, 'team');
			ser.Value("vehicleId", vehicleId, 'eid');
			ser.Value("seatId", seatId, 'seat');
		};

		int      teamId;
		EntityId vehicleId;
		int      seatId;
	};

	struct KillParams
	{
		KillParams() {};

		KillParams(const EntityId _shooterId, const int _weaponClassId, const float _damage, const int _material, const int _hit_type, const Vec3& _impulse, const int _healthOnKill)
			: shooterId(_shooterId),
			weaponClassId(_weaponClassId),
			damage(_damage),
			material(_material),
			hit_type(_hit_type),
			impulse(_impulse),
			healthOnKill(_healthOnKill) {};

		EntityId shooterId;
		int      weaponClassId;
		float    damage;
		int      material;
		int      hit_type;
		Vec3     impulse;
		int      healthOnKill;

		void SerializeWith(TSerialize ser)
		{
			ser.Value("shooterId", shooterId, 'eid');
			ser.Value("weaponClassId", weaponClassId, 'ui16');
			ser.Value("damage", damage, 'dmg');
			ser.Value("material", material, 'mat');
			ser.Value("hit_type", hit_type, 'hTyp');
			ser.Value("impulse", impulse, 'kImp');
			ser.Value("health_on_kill", healthOnKill, 'i8');
		};
	};

	struct MoveParams
	{
		MoveParams() {};

		MoveParams(const Vec3& p, const Quat& q)
			: pos(p),
			rot(q) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("pos", pos, 'wrld');
			ser.Value("rot", rot, 'ori1');
		}

		Vec3 pos;
		Quat rot;
	};

	struct AmmoParams
	{
		AmmoParams() {};

		AmmoParams(const char* name, const int amount)
			: ammo(name),
			count(amount) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("ammo", ammo);
			ser.Value("amount", count);
		}

		string ammo;
		int    count;
	};

	struct PickItemParams
	{
		PickItemParams()
			: itemId(0),
			select(false),
			sound(false) {};

		PickItemParams(const EntityId item, const bool slct, const bool snd)
			: itemId(item),
			select(slct),
			sound(snd) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("itemId", itemId, 'eid');
			ser.Value("select", select, 'bool');
			ser.Value("sound", sound, 'bool');
		}

		EntityId itemId;
		bool     select;
		bool     sound;
	};

	struct SelectItemParams
	{
		SelectItemParams() {};

		SelectItemParams(const string& _itemName)
			: itemName(_itemName) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("itemName", itemName);
		}

		string itemName;
	};

	struct SetSpectatorModeParams
	{
		SetSpectatorModeParams()
			: mode(0),
			targetId(0) {};

		SetSpectatorModeParams(const uint8 _mode, const EntityId _target)
			: mode(_mode),
			targetId(_target) {};
		uint8    mode;
		EntityId targetId;

		void SerializeWith(TSerialize ser)
		{
			ser.Value("mode", mode, 'spec');
			ser.Value("target", targetId, 'eid');
		}
	};

	struct SetSpectatorHealthParams
	{
		SetSpectatorHealthParams()
			: health(0) {};

		SetSpectatorHealthParams(const int _health)
			: health(_health) {};
		int health;

		void SerializeWith(TSerialize ser)
		{
			ser.Value("health", health, 'i8');
		}
	};

	struct NoParams
	{
		void SerializeWith(TSerialize ser) {};
	};

	enum EActorSpectatorMode
	{
		eASM_None = 0,
		// normal, non-spectating

		eASM_FirstMPMode,
		eASM_Fixed = eASM_FirstMPMode,
		// fixed position camera
		eASM_Free,
		// free roaming, no collisions
		eASM_Follow,
		// follows an entity in 3rd person
		eASM_LastMPMode = eASM_Follow,
		eASM_Cutscene,
		// HUDInterfaceEffects.cpp sets this
	};

	DECLARE_SERVER_RMI_NOATTACH_FAST(SvRequestDropItem, DropItemParams, eNRT_ReliableOrdered);
	DECLARE_SERVER_RMI_NOATTACH_FAST(SvRequestPickUpItem, ItemIdParam, eNRT_ReliableOrdered);
	DECLARE_SERVER_RMI_NOATTACH_FAST(SvRequestUseItem, ItemIdParam, eNRT_ReliableOrdered);
	// cannot be _FAST - see comment on InvokeRMIWithDependentObject
	DECLARE_CLIENT_RMI_NOATTACH(ClPickUp, PickItemParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClClearInventory, NoParams, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_NOATTACH(ClSelectItemByName, SelectItemParams, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_NOATTACH_FAST(ClDrop, DropItemParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH_FAST(ClSelect, NoParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH_FAST(ClStartUse, ItemIdParam, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH_FAST(ClStopUse, ItemIdParam, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_PREATTACH(ClSetSpectatorMode, SetSpectatorModeParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_PREATTACH(ClSetSpectatorHealth, SetSpectatorHealthParams, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_PREATTACH(ClRevive, ReviveParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_PREATTACH(ClReviveInVehicle, ReviveInVehicleParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_POSTATTACH(ClSimpleKill, NoParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH_FAST(ClKill, KillParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH_FAST(ClMoveTo, MoveParams, eNRT_ReliableOrdered);

	DECLARE_CLIENT_RMI_NOATTACH(ClSetAmmo, AmmoParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClAddAmmo, AmmoParams, eNRT_ReliableOrdered);

	CItem*   GetItem(EntityId itemId) const;
	CItem*   GetItemByClass(IEntityClass* pClass) const;
	CWeapon* GetWeapon(EntityId itemId) const;
	CWeapon* GetWeaponByClass(IEntityClass* pClass) const;

	virtual void SelectNextItem(int direction, bool keepHistory, const char* category = nullptr);
	void         HolsterItem(bool holster) override;
	virtual void SelectLastItem(bool keepHistory, bool forceNext = false);
	virtual void SelectItemByName(const char* name, bool keepHistory);
	virtual void SelectItem(EntityId itemId, bool keepHistory);

	virtual bool UseItem(EntityId itemId);
	virtual bool PickUpItem(EntityId itemId, bool sound, bool ignoreOffhand = false);
	virtual bool DropItem(EntityId itemId, float impulseScale = 1.0f, bool selectNext = true, bool byDeath = false);
	virtual void DropAttachedItems();

	virtual void NetReviveAt(const Vec3& pos, const Quat& rot, int teamId);
	virtual void NetReviveInVehicle(EntityId vehicleId, int seatId, int teamId);
	virtual void NetSimpleKill();
	virtual void NetKill(EntityId shooterId, uint16 weaponClassId, int damage, int material, int hit_type, int killerHealthOnKill);

	virtual void SetSleepTimer(float timer);
	Vec3         GetWeaponOffsetWithLean(EStance stance, float lean, const Vec3& eyeOffset);

	virtual bool CanRagDollize() const;

public:
	CActor();
	virtual ~CActor();

	// IActor
	void ProcessEvent(SEntityEvent& event) override;

	void Release() override
	{
		delete this;
	};
	void                 ResetAnimGraph() override;
	void                 FullSerialize(TSerialize ser) override;
	bool                 NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int flags) override;
	void                 PostSerialize() override;
	void                 SerializeSpawnInfo(TSerialize ser) override;
	ISerializableInfoPtr GetSpawnInfo() override;
	void                 SetChannelId(uint16 id) override;
	void                 SerializeLevelToLevel(TSerialize& ser) override;
	IInventory*          GetInventory() const override;

	bool IsClient() const override;

	IMaterial* GetReplacementMaterial() override
	{
		return m_pReplacementMaterial;
	};

	bool Init(IGameObject* pGameObject) override;
	void InitClient(int channelId) override;
	void PostInit(IGameObject* pGameObject) override;
	void PostInitClient(int channelId) override {};
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void UpdateView(SViewParams& viewParams) override {};
	void PostUpdateView(SViewParams& viewParams) override {};

	void InitLocalPlayer() override {};

	virtual void InitiateCombat();
	virtual void ExtendCombat();

	void SetIKPos(const char* pLimbName, const Vec3& goalPos, int priority) override;

	void HandleEvent(const SGameObjectEvent& event) override;
	void PostUpdate(float frameTime) override;
	void PostRemoteSpawn() override {};

	bool IsThirdPerson() const override
	{
		return true;
	};
	void ToggleThirdPerson() override {}

	void RequestFacialExpression(const char* pExpressionName /* = NULL */) override;
	void PrecacheFacialExpression(const char* pExpressionName) override;

	void NotifyInventoryAmmoChange(IEntityClass* pAmmoClass, int amount) override;

	EntityId GetGrabbedEntityId() const override
	{
		return 0;
	}

	void HideAllAttachments(bool isHiding) override;

	//[Kirill] need this to make sure AG inputs are correct for MP exiting vehicle gunner seat
	virtual void UpdateTPItem();

	// ~IActor

	// IGameObjectProfileManager
	bool SetAspectProfile(EEntityAspects aspect, uint8 profile) override;

	uint8 GetDefaultProfile(const EEntityAspects aspect) override
	{
		return aspect == eEA_Physics ? eAP_NotPhysicalized : 0;
	}

	// ~IGameObjectProfileManager

	// IActionListener
	virtual void OnAction(const ActionId& actionId, int activationMode, float value);
	// ~IActionListener

	virtual void ProfileChanged(uint8 newProfile);

	//------------------------------------------------------------------------
	float GetAirControl() const
	{
		return m_airControl;
	};

	float GetAirResistance() const
	{
		return m_airResistance;
	};

	float GetTimeImpulseRecover() const
	{
		return m_timeImpulseRecover;
	};

	void SetViewRotation(const Quat& rotation) override {};

	Quat GetViewRotation() const override
	{
		return GetEntity()->GetRotation();
	};
	void EnableTimeDemo(bool bTimeDemo) override {};

	// offset to add to the computed camera angles every frame
	virtual void SetViewAngleOffset(const Vec3& offset) {};

	Vec3 GetViewAngleOffset() override
	{
		return Vec3(0, 0, 0);
	};

	//------------------------------------------------------------------------
	virtual void Revive(bool fromInit = false);
	virtual void Reset(bool toGame) {};
	//physicalization
	virtual void Physicalize(EStance stance = STANCE_NULL);
	virtual void PostPhysicalize();
	virtual void RagDollize(bool fallAndPlay);
	//
	int IsGod() override
	{
		return 0;
	}

	virtual void SetSpectatorMode(uint8 mode, EntityId targetId) {};

	virtual uint8 GetSpectatorMode() const
	{
		return 0;
	};
	virtual void SetSpectatorTarget(EntityId targetId) {};

	virtual EntityId GetSpectatorTarget() const
	{
		return 0;
	};
	virtual void SetSpectatorHealth(int health) {};

	virtual int GetSpectatorHealth() const
	{
		return 0;
	};

	//get actor status
	virtual SActorStats* GetActorStats()
	{
		return nullptr;
	};

	virtual const SActorStats* GetActorStats() const
	{
		return nullptr;
	};

	virtual SActorParams* GetActorParams()
	{
		return nullptr;
	};

	virtual void SetStats(SmartScriptTable& rTable);
	virtual void UpdateScriptStats(SmartScriptTable& rTable);

	virtual ICharacterInstance* GetFPArms(const int i) const
	{
		return GetEntity()->GetCharacter(3 + i);
	};
	//set/get actor params
	virtual void SetParams(SmartScriptTable& rTable, bool resetFirst = false);

	virtual bool GetParams(SmartScriptTable& rTable)
	{
		return false;
	};
	//
	virtual void Freeze(bool freeze) {};
	virtual void Fall(Vec3 hitPos = Vec3(0, 0, 0), bool forceFall = false, float time = 0.0f);
	//throw away the actors helmet (if available) [Jan Mьller]
	virtual bool LooseHelmet(Vec3 hitDir = Vec3(0, 0, 0), Vec3 hitPos = Vec3(0, 0, 0), bool simulate = false);
	virtual void GoLimp();
	virtual void StandUp();
	void         NotifyLeaveFallAndPlay() override;
	bool         IsFallen() const override;
	virtual int  GetFallenTime() const;
	//
	IEntity*         LinkToVehicle(EntityId vehicleId) override;
	virtual IEntity* LinkToVehicleRemotely(EntityId vehicleId);
	virtual void     LinkToMountedWeapon(EntityId weaponId) {};
	virtual IEntity* LinkToEntity(EntityId entityId, bool bKeepTransformOnDetach = true);

	ILINE IEntity* GetLinkedEntity() const override
	{
		return m_linkStats.GetLinked();
	}

	ILINE IVehicle* GetLinkedVehicle() const override
	{
		return m_linkStats.GetLinkedVehicle();
	}

	void SetViewInVehicle(Quat viewRotation) override {};

	virtual void SupressViewBlending() {};

	ILINE Vec3 GetLBodyCenter() const
	{
		const SStanceInfo* pStance(GetStanceInfo(GetStance()));
		return Vec3(0, 0, (pStance->viewOffset.z - pStance->heightPivot) * 0.5f);
	}

	ILINE Vec3 GetWBodyCenter() const
	{
		return GetEntity()->GetWorldTM() * GetLBodyCenter();
	}

	//for animations
	void PlayAction(const char* action, const char* extension, bool looping = false) override {};
	//
	virtual void SetMovementTarget(const Vec3& position, const Vec3& looktarget, const Vec3& up, float speed) {};
	//
	virtual void CreateScriptEvent(const char* event, float value, const char* str = nullptr);
	virtual bool CreateCodeEvent(SmartScriptTable& rTable);
	void         AnimationEvent(ICharacterInstance* pCharacter, const AnimEventInstance& event) override;
	//
	virtual void CameraShake(float angle, float shift, float duration, float frequency, Vec3 pos, int ID, const char* source = "") {};
	//
	virtual void VectorToLocal(Vec3& v) {};
	//
	virtual void SetAngles(const Ang3& angles) {};

	virtual Ang3 GetAngles()
	{
		return Ang3(0, 0, 0);
	};
	virtual void AddAngularImpulse(const Ang3& angular, float deceleration = 0.0f, float duration = 0.0f) {}
	//
	virtual void  SetViewLimits(Vec3 dir, float rangeH, float rangeV) {};
	virtual void  SetZoomSpeedMultiplier(float m);
	virtual float GetZoomSpeedMultiplier() const;
	void          SetHealth(int health) override;
	virtual void  DamageInfo(EntityId shooterID, EntityId weaponID, float damage, const char* damageType);

	IAnimatedCharacter* GetAnimatedCharacter() override
	{
		return m_pAnimatedCharacter;
	}

	void SetMaxHealth(int maxHealth);

	int32 GetHealth() const override
	{
		return static_cast<int32>(m_health);
	}

	int32 GetMaxHealth() const override
	{
		return m_maxHealth;
	}

	int32 GetArmor() const override
	{
		return 0;
	}

	int32 GetMaxArmor() const override
	{
		return 0;
	}

	virtual void Kill();

	void ResetHelmetAttachment();

	virtual float GetFrozenAmount(bool stages = false) const
	{
		return m_frozenAmount;
	}

	void SetFrozenAmount(float amount);

	bool         IsFrozen();
	virtual void AddFrost(float frost);

	virtual void BindInputs(IAnimationGraphState* pAGState);
	//marcok: GC workaround
	virtual bool IsSwimming()
	{
		return false;
	}

	virtual bool ShouldSwim()
	{
		return false;
	};

	virtual bool IsSprinting()
	{
		return false;
	}

	virtual bool CanFire()
	{
		return true;
	}

	//stances
	ILINE virtual EStance GetStance() const
	{
		/*
				if(!m_pAnimatedCharacter)
					return STANCE_NULL;
				int stance = m_pAnimatedCharacter?m_pAnimatedCharacter->GetCurrentStance():STANCE_NULL;
		
				if (stance < 0 || stance > STANCE_LAST)
					return STANCE_NULL;
				return (EStance)stance;
		*/
		return m_stance;
	}

	ILINE const SStanceInfo* GetStanceInfo(const EStance stance) const
	{
		if (stance < 0 || stance > STANCE_LAST)
			return &m_defaultStance;
		return &m_stances[stance];
	}

	ILINE const EStance GetDesiredStance() const
	{
		return m_desiredStance;
	}

	ILINE const EStance GetPreviousStance() const
	{
		return m_previousStance;
	}

	ILINE const float GetStanceChangeTime() const
	{
		return m_stanceChangeTime;
	}

	virtual void SetupStance(EStance stance, SStanceInfo* info);
	//

	// forces the animation graph to select a state
	void QueueAnimationState(const char* state);
	void ChangeAnimGraph(const char* graph, int layer);

	virtual bool SetAnimationInput(const char* inputID, const char* value)
	{
		// Handle action and signal inputs via AIproxy, since the AI system and
		// the AI agent behavior depend on those inputs.
		if (IEntity*               pEntity = GetEntity())
			if (const IAIObject*   pAI     = pEntity->GetAI())
				if (IUnknownProxy* pProxy  = pAI->GetProxy())
					if (pProxy->IsEnabled())
					{
						const bool bSignal = strcmp(inputID, "Signal") == 0;
						const bool bAction = strcmp(inputID, "Action") == 0;
						if (bSignal)
						{
							return pProxy->SetAGInput(AIAG_SIGNAL, value);
						}
						if (bAction)
						{
							// Dejan: actions should not go through the ai proxy anymore!
							/*
							if(_stricmp(value, "idle") == 0)
								return pProxy->ResetAGInput( AIAG_ACTION );
							else
							{
								return pProxy->SetAGInput( AIAG_ACTION, value );
							}
							*/
						}
					}

		if (IAnimationGraphState* pState = GetAnimationGraphState())
		{
			pState->SetInput(pState->GetInputId(inputID), value);
			return true;
		}

		return false;
	}

	//
	virtual int  GetBoneID(int ID, int slot = 0) const;
	Vec3         GetLocalEyePos(int slot = 0) const override;
	virtual Vec3 GetLocalEyePos2(int slot = 0) const;

	ILINE /* virtual */ bool InZeroG() const
	{
		if (const SActorStats* pActorStats = GetActorStats())
			return pActorStats->inZeroG;
		return false;
	}

	void UpdateZeroG(float frameTime);

	//
	virtual void ProcessBonesRotation(ICharacterInstance* pCharacter, float frameTime);

	virtual void OnPhysicsPreStep(float frameTime) {};

	//grabbing
	virtual IGrabHandler* CreateGrabHanlder();
	virtual void          UpdateGrab(float frameTime);

	virtual bool CheckInventoryRestrictions(const char* itemClassName);
	virtual bool CheckVirtualInventoryRestrictions(const std::vector<string>& inventory, const char* itemClassName);

	virtual bool  CanPickUpObject(IEntity* obj, float& heavyness, float& volume);
	virtual bool  CanPickUpObject(float mass, float volume);
	virtual float GetActorStrength() const;

	//
	virtual void ProcessIKLimbs(ICharacterInstance* pCharacter, float frameTime);

	//IK limbs
	int            GetIKLimbIndex(const char* limbName);
	ILINE SIKLimb* GetIKLimb(const int limbIndex)
	{
		return &m_IKLimbs[limbIndex];
	}

	void CreateIKLimb(int characterSlot, const char* limbName, const char* rootBone, const char* midBone, const char* endBone, int flags = 0);

	void ClearIKPosBlending(const char* pLimbName);

	//
	IMovementController* GetMovementController() override
	{
		return m_pMovementController;
	}

	//stances
	virtual void SetStance(EStance stance);
	virtual void StanceChanged(EStance last) {};
	virtual bool TrySetStance(EStance stance); // Shared between humans and aliens.
	//

	IAnimationGraphState* GetAnimationGraphState() override;

	IAnimatedCharacter* GetAnimatedCharacter() const
	{
		return m_pAnimatedCharacter;
	};
	void SetFacialAlertnessLevel(int alertness) override;

	//weapons
	IItem*   GetCurrentItem(bool includeVehicle = false) const override;
	EntityId GetCurrentItemId(bool includeVehicle = false) const;
	IItem*   GetHolsteredItem() const override;

	//Net
	EntityId NetGetCurrentItem() const;
	void     NetSetCurrentItem(EntityId id);

	//AI
	Vec3 GetAIAttentionPos();

	virtual void UpdateFootSteps(float frameTime);

	CScreenEffects* GetScreenEffects() const
	{
		return m_screenEffects;
	};

	CWeaponAttachmentManager* GetWeaponAttachmentManager() const
	{
		return m_pWeaponAM;
	}

	void InitActorAttachments();

	void SwitchDemoModeSpectator(bool activate) override {}; //this is a player only function

	//misc
	virtual void ReplaceMaterial(const char* strMaterial);
	virtual void SendMusicLogicEvent(EMusicLogicEvents event) {};

	// ugly: replace by real class system
	static const char* GetActorClassType()
	{
		return "CActor";
	}

	virtual const char* GetActorClass() const
	{
		return GetActorClassType();
	}

	//For sleeping bullets
	virtual bool CanSleep()
	{
		return false;
	}

	//Grabbing
	virtual int GetActorSpecies()
	{
		return eGCT_UNKNOWN;
	}

	void SetAnimTentacleParams(pe_params_rope& rope, float animBlend) override {};

	virtual bool IsCloaked() const
	{
		return false;
	}

	virtual void DumpActorInfo();

	virtual bool IsAlien()
	{
		return false;
	}

	void Suicide(const float delay)
	{
		if (m_suicideDelay < 0.f)
			m_suicideDelay = delay;
	}

	void UpdateSuicide(float frameTime);

protected:
	virtual void SetMaterialRecursive(ICharacterInstance* charInst, bool undo, IMaterial* newMat = nullptr);

	void DisplayDebugInfo();

	virtual void UpdateAnimGraph(IAnimationGraphState* pState);

	//movement
	virtual IActorMovementController* CreateMovementController() = 0;
	//

	virtual Vec3 GetModelOffset() const
	{
		return GetStanceInfo(GetStance())->modelOffset;
	}

private:
	mutable IInventory* m_pInventory;
	void                ClearExtensionCache();
	void                CrapDollize(bool enable = true);

protected:
	void RequestServerResync() const
	{
		if (!IsClient())
			GetGameObject()->RequestRemoteUpdate(eEA_Physics | eEA_GameClientDynamic | eEA_GameServerDynamic | eEA_GameClientStatic | eEA_GameServerStatic);
	}

	void GetActorMemoryStatistics(ICrySizer* pSizer);

	//
	typedef std::vector<SIKLimb> TIKLimbs;

	//
	virtual void SetActorModel();
	virtual bool UpdateStance();
	virtual void OnCloaked(bool cloaked) {};

	mutable int16 m_boneIDs[BONE_ID_NUM];

	bool  m_isClient;
	float m_health;
	int32 m_maxHealth;

	EStance m_stance;
	EStance m_desiredStance;
	EStance m_previousStance;
	float   m_stanceChangeTime;

	static SStanceInfo m_defaultStance;
	SStanceInfo        m_stances[STANCE_LAST];

	float m_zoomSpeedMultiplier;

	SmartScriptTable m_actorStats;

	IAnimatedCharacter*       m_pAnimatedCharacter;
	IActorMovementController* m_pMovementController;

	static IItemSystem*       m_pItemSystem;
	static IGameFramework*    m_pGameFramework;
	static IGameplayRecorder* m_pGameplayRecorder;

	//
	IAnimationGraph::InputID m_inputHealth;
	IAnimationGraph::InputID m_inputStance;
	IAnimationGraph::InputID m_inputFiring;
	IAnimationGraph::InputID m_inputWaterLevel;

	IGrabHandler* m_pGrabHandler;

	mutable SLinkStats m_linkStats;

	TIKLimbs m_IKLimbs;

	float m_frozenAmount; // internal amount. used to leave authority over frozen state at game

	// ScreenEffects-related variables
	CScreenEffects* m_screenEffects;

	// Weapon Attachment manager
	CWeaponAttachmentManager* m_pWeaponAM;

	uint8 m_currentPhysProfile;

	EBonesID m_currentFootID;

	float m_fallAndPlayTimer;

	float m_combatTimer;
	bool  m_enterCombat;
	bool  m_inCombat;
	bool  m_rightFoot;
	bool  m_bHasHUD;

	float m_airControl;
	float m_airResistance;
	float m_timeImpulseRecover;

	EntityId m_netLastSelectablePickedUp;

	//helmet serialization
	EntityId m_lostHelmet,    m_serializeLostHelmet;
	string   m_lostHelmetObj, m_serializelostHelmetObj;
	string   m_lostHelmetMaterial;
	string   m_lostHelmetPos;

	_smart_ptr<IMaterial> m_pReplacementMaterial;

	//
	std::map<ICharacterInstance*, _smart_ptr<IMaterial>> m_testOldMats;
	std::map<IAttachmentObject*, _smart_ptr<IMaterial>>  m_attchObjMats;
	//std::map< EntityId, IMaterial* > m_wepAttchMats;

	float m_sleepTimer, m_sleepTimerOrg;

	int      m_teamId;
	EntityId m_lastItemId;

	float m_suicideDelay;

	EntityId m_dropWpnPendingId;
	float    m_dropWpnWaitTime;

	float m_reviveNoReactionTime;

public:
	void SetDropWeaponTimer(EntityId weaponId, float delay);
	void ForceAutoDrop();

	// Can occur only when we're not zooming out
	int m_autoZoomInID;
	// Can occur at any time
	int m_autoZoomOutID;
	// Can occur at any time
	int m_saturationID;

	int m_hitReactionID;
};

#endif //__Actor_H__
