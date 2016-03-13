#pragma once
#include "Vector.h"
#include "SDK.h"

// stuff for trace

enum TraceType_t : int
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,			   // NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,		   // NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS, // NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

//enum class collisions : int
//{
//	COLLISION_GROUP_NONE = 0,
//	COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
//	COLLISION_GROUP_DEBRIS_TRIGGER,         // Same as debris, but hits triggers
//	COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
//	COLLISION_GROUP_INTERACTIVE,		// Collides with everything except interactive debris or debris
//	COLLISION_GROUP_PLAYER,
//	COLLISION_GROUP_BREAKABLE_GLASS,
//	COLLISION_GROUP_VEHICLE,
//	COLLISION_GROUP_PLAYER_MOVEMENT,  	// For HL2, same as Collision_Group_Player, for
//										// TF2, this filters out other players and CBaseObjects
//	COLLISION_GROUP_NPC,			// Generic NPC group
//	COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
//	COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
//	COLLISION_GROUP_VEHICLE_CLIP,		// vehicle clip brush to restrict vehicle movement
//	COLLISION_GROUP_PROJECTILE,		// Projectiles!
//	COLLISION_GROUP_DOOR_BLOCKER,		// Blocks entities not permitted to get near moving doors
//	COLLISION_GROUP_PASSABLE_DOOR,		// Doors that the player shouldn't collide with
//	COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
//	COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code
//
//	COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
//	COLLISION_GROUP_NPC_SCRIPTED		// Used for NPCs in scripts that should not collide with each other
//};

#define MAX_COORD_INTEGER ( 16384 )
#define COORD_EXTENT ( float )( 2 * MAX_COORD_INTEGER )
#define MAX_TRACE_LENGTH ( float )( 1.732050807569 * COORD_EXTENT )

// tf2 hitboxes
enum class Hitbox : int
{
	head = 0,
	pelvis = 1,
	spine_0 = 2,
	spine_1 = 3,
	spine_2 = 4,
	spine_3 = 5,
	upperArm_L = 6,
	lowerArm_L = 7,
	hand_L = 8,
	upperArm_R = 9,
	lowerArm_R = 10,
	hand_R = 11,
	hip_L = 12,
	knee_L = 13,
	foot_L = 14,
	hip_R = 15,
	knee_R = 16,
	foot_R = 17,
};

enum class hitgroup : int
{
	HITGROUP_GENERIC,
	HITGROUP_HEAD,
	HITGROUP_CHEST,
	HITGROUP_STOMACH,
	HITGROUP_LEFTARM,
	HITGROUP_RIGHTARM,
	HITGROUP_LEFTLEG,
	HITGROUP_RIGHTLEG,
	//HITGROUP_GEAR = 10,
};

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(DWORD *dwRefEHandle) = 0;
	virtual DWORD GetRefEHandle(void) = 0;
	CBaseEntity *GetEntity(void)
	{
		return gInts.EntList->GetClientEntity(((*(PINT)(GetRefEHandle())) & ((1 << (11 + 1)) - 1)));
	}
};

//class ITraceFilter
//{
//public:
//	virtual bool ShouldHitEntity( IHandleEntity *pEntity, int contentsMask ) = 0;
//	virtual TraceType_t GetTraceType( ) const = 0;
//};
//
//class TraceFilter : public ITraceFilter
//{
//public:
//	virtual bool ShouldHitEntity( IHandleEntity *pEntityHandle, int contentsMask ) override
//	{
//		CBaseEntity *pBaseEntity = pEntityHandle->GetEntity( );
//
//		if ( strcmp( pBaseEntity->GetClientClass( )->chName, "CFuncRespawnRoomVisualizer" ) == 0 )
//		{
//			Log::Console( "CFuncRespawnRoomVisualizer id: %i", pBaseEntity->GetClientClass( )->iClassID );
//			return false;
//		}
//		if ( pBaseEntity->GetIndex( ) == gInts.Engine->GetLocalPlayer( ) )
//		{
//			return false;
//		}
//		return true;
//	}
//	virtual TraceType_t GetTraceType( ) const override
//	{
//		return TRACE_EVERYTHING;
//	}
//};
//
//class CTraceFilter : public ITraceFilter
//{
//public:
//	bool ShouldHitEntity( IHandleEntity *pEntityHandle, int contentsMask )
//	{
//		return !( pEntityHandle == pSkip1 );
//	}
//
//	TraceType_t GetTraceType( ) const
//	{
//		return TRACE_EVERYTHING;
//	}
//
//	void *pSkip1;
//};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity *pBaseEntity, int mask) = 0;
	virtual TraceType_t GetTraceType() const = 0;
};

class CBaseFilter : public ITraceFilter
{
public:
	CBaseFilter() : pSelf(nullptr), pIgnore(nullptr), ccIgnore(0), bDebugMe(false) {}

	virtual bool ShouldHitEntity(IHandleEntity *pEntity, int contentsMask);
	virtual TraceType_t GetTraceType() const;

	// Setup the filter
	inline void SetIgnoreSelf(CBaseEntity *pSelf) { this->pSelf = pSelf; }
	inline void SetIgnoreEntity(CBaseEntity *pIgnore) { this->pIgnore = pIgnore; }
	inline void SetIgnoreClass(unsigned int cls) { ccIgnore = cls; }
	inline void SetDebug(int position = 0)
	{
		bDebugMe = true;
		iDebugCount = position;
	}

protected:
	//void Debug( CBaseEntity* pEnt );

protected:
	CBaseEntity *pSelf;
	CBaseEntity *pIgnore;
	unsigned int ccIgnore;

	bool bDebugMe;
	int iDebugCount;
};

inline bool CBaseFilter::ShouldHitEntity(IHandleEntity *pHandle, int contentsMask)
{
	CBaseEntity *pEnt = pHandle->GetEntity();

	// Debug traces...
	//Debug( pEnt );

	// Skip certain entity classes
	// TF2: Always filter respawn room visualizer entity! (should not interfere)
	ClientClass *pEntCC = pEnt->GetClientClass();

	if(!strcmp(pEntCC->chName, "CFuncRespawnRoomVisualizer") || !strcmp(pEntCC->chName, "CTFMedigunShield") || !strcmp(pEntCC->chName, "CFuncAreaPortalWindow") /*|| ( ccIgnore && pEntCC->iClassID == ccIgnore )*/)
	{
		return false;
	}

	// Skip 'fake' entities used to throw us off
	// Left as an exercise to the reader ;)

	// Ignore specific entities
	if(pEnt == pSelf || pEnt == pIgnore)
	{
		return false;
	}

	return true;
}
inline TraceType_t CBaseFilter::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

struct Ray_t
{
	Vector m_Start;
	float w1;
	Vector m_Delta;
	float w2;
	Vector m_StartOffset;
	float w3;
	Vector m_Extents;
	float w4;

	bool m_IsRay;
	bool m_IsSwept;

	void Init(Vector const &start, Vector const &end)
	{
		//VectorSubtract( end, start, m_Delta );

		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		VectorClear(m_Extents);
		m_IsRay = true;

		VectorClear(m_StartOffset);
		VectorCopy(start, m_Start);
	}

	void Init(Vector const &start, Vector const &end, Vector const &mins, Vector const &maxs)
	{
		//VectorSubtract( end, start, m_Delta );

		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		//VectorSubtract( maxs, mins, m_Extents );

		m_Extents = maxs - mins;

		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		//VectorAdd( mins, maxs, m_StartOffset );

		m_StartOffset = mins + maxs;

		m_StartOffset *= 0.5f;
		//VectorAdd( start, m_StartOffset, m_Start );
		m_Start = start + m_StartOffset;

		m_StartOffset *= -1.f;
	}
};

struct csurface_t
{
	const char *name;
	short surfaceProps;
	unsigned short flags;
};

struct cplane_t
{
	Vector normal;
	float dist;
	BYTE type;
	BYTE signbits;
	BYTE pad[2];
};

class CBaseTrace
{
public:
	Vector start;
	Vector end;
	cplane_t plane;
	float fraction;
	int contents;
	unsigned short dispFlags;
	bool allsolid;
	bool startsolid;
};

class CGameTrace : public CBaseTrace
{
public:
	float fractionleftsolid;
	csurface_t surface;
	int hitGroup;
	short physicsBone;
	CBaseEntity *m_pEnt;
	int hitbox;
};

typedef CGameTrace trace_t;

class CTraceFilter : public ITraceFilter
{
public:
	CTraceFilter(CBaseEntity *pPassEntity) { m_pPassEnt = pPassEntity; }

	bool ShouldHitEntity(IHandleEntity *pHandleEntity, int contentsMask) { return pHandleEntity->GetEntity() != m_pPassEnt; }
	TraceType_t GetTraceType() const { return TRACE_EVERYTHING; }

	CBaseEntity *m_pPassEnt;
};

//struct Ray_t
//{
//	__declspec( align( 16 ) ) VectorAligned m_Start;	   // starting point, centered within the extents
//	__declspec( align( 16 ) ) VectorAligned m_Delta;	   // direction + length of the ray
//	__declspec( align( 16 ) ) VectorAligned m_StartOffset; // Add this to m_Start to get the actual ray start
//	__declspec( align( 16 ) ) VectorAligned m_Extents;	 // Describes an axis aligned box extruded along a ray
//	bool m_IsRay;											   // are the extents zero?
//	bool m_IsSwept;											   // is delta != 0?
//
//	// compute inverse delta
//	Vector InvDelta( ) const
//	{
//		Vector vecInvDelta;
//		for ( int iAxis = 0; iAxis < 3; ++iAxis )
//		{
//			if ( m_Delta[ iAxis ] != 0.0f )
//			{
//				vecInvDelta[ iAxis ] = 1.0f / m_Delta[ iAxis ];
//			}
//			else
//			{
//				vecInvDelta[ iAxis ] = FLT_MAX;
//			}
//		}
//		return vecInvDelta;
//	}
//
//	//void Init( Vector const& start, Vector const& end )
//	//{
//	//	m_Delta = end - start;
//
//	//	m_IsSwept = ( m_Delta.LengthSqr() != 0 );
//
//	//	m_Extents.x = m_Extents.y = m_Extents.z = 0.0f;
//
//	//	m_IsRay = true;
//
//	//	m_StartOffset.x = m_StartOffset.y = m_StartOffset.z = 0.0f;
//
//	//	m_Start = start;
//	//}
//	void Init( Vector const &start, Vector const &end )
//	{
//		//VectorSubtract( end, start, m_Delta );
//		m_Delta = end - start;
//
//		m_IsSwept = ( m_Delta.LengthSqr( ) != 0 );
//
//		VectorClear( m_Extents );
//		m_IsRay = true;
//
//		// Offset m_Start to be in the center of the box...
//		VectorClear( m_StartOffset );
//		VectorCopy( start, m_Start );
//	}
//};

//struct cplane_t
//{
//	Vector normal;
//	float dist;
//	byte type;	 // for fast side tests
//	byte signbits; // signx + (signy<<1) + (signz<<1)
//	byte pad[ 2 ];
//};
//
//struct csurface_t
//{
//	const char *name;
//	short surfaceProps;
//	unsigned short flags; // BUGBUG: These are declared per surface, not per material, but this database is per-material now
//};
//
//class CBaseTrace
//{
//public:
//	// these members are aligned!!
//	Vector start;   // start position
//	Vector end;		// final position
//	cplane_t plane; // surface normal at impact
//
//	float fraction; // time completed, 1.0 = didn't hit anything
//
//	int contents;			  // contents on other side of surface hit
//	unsigned short dispFlags; // displacement flags for marking surfaces with data
//
//	bool allsolid;   // if true, plane is not valid
//	bool startsolid; // if true, the initial point was in a solid area
//};

//class trace_t : public CBaseTrace
//{
//public:
//	float fractionleftsolid; // time we left a solid, only valid if we started in solid
//	csurface_t surface;		 // surface hit (impact surface)
//
//	int hitGroup;	  // 0 == generic, non-zero is specific body part
//	short physicsBone; // physics bone hit by trace in studio
//
//	CBaseEntity *m_pEnt;
//
//	// NOTE: this member is overloaded.
//	// If hEnt points at the world entity, then this is the static prop index.
//	// Otherwise, this is the hitbox index.
//	int hitbox; // box hit by trace in studio
//};

//class trace_t
//{
//public:
//	Vector start;
//	Vector end;
//	BYTE plane[ 20 ];
//	float fraction;
//	int contents;
//	WORD dispFlags;
//	bool allSolid;
//	bool startSolid;
//	float fractionLeftSolid;
//	BYTE surface[ 8 ];
//	int hitGroup;
//	short physicsBone;
//	CBaseEntity *m_pEnt;
//	int hitbox;
//};

class CEngineTrace
{
public:
	void TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const Ray_t &, unsigned int, ITraceFilter *, trace_t *);
		return getvfunc<OriginalFn>(this, 4)(this, ray, fMask, pTraceFilter, pTrace);
	}
};
