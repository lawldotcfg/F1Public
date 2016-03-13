#pragma once

//#define __DEBUG
//#define __XOR

#include "baseHeaders.h"

#include "Vector.h"
#include "CSignature.h"
#include "WeaponList.h"
#include "CGlobalVars.h"
#include "Log.h"
#include "Netvar.h"
#include "keyval.h"
#include "prediction.h"
#include "bf_buffer.h"
#include "random.h"
#include "IConvar.h"
//#include "CPlayerResource.h"

class CGameTrace;
typedef CGameTrace trace_t;
class CEngineTrace;
class IUniformRandomStream;
class CPlayerResource;

typedef void *(*InstallUniformRandomStreamFn)(IUniformRandomStream *pStream);

#define VMTManager toolkit::VMTManager
#define VMTBaseManager toolkit::VMTBaseManager

using CHandle = int;

// useless here?

#pragma optimize( "gsy", on )
#pragma warning( disable : 4244 )		  //Possible loss of data
#pragma comment( lib, "steamclient.lib" ) // use steamclient as thats where we are getting createinterface from!

typedef float matrix3x4[3][4];
//===================================================================================================
#define me gInts.Engine->GetLocalPlayer()
#define MASK_AIMBOT 0x200400B
#define CONTENTS_HITBOX 0x40000000
#define CH_MASK 0x4600400B
#define PI 3.14159265358979323846f
#define square( x ) ( x * x )
#define RADPI 57.295779513082f
#define SQUARE( a ) a *a
#define BLU_TEAM 3
#define RED_TEAM 2
#define FL_ONGROUND ( 1 << 0 )
#define FL_DUCKING ( 1 << 1 )
#define HANDLE2INDEX( handle ) ( handle & 0xFFF )
//===================================================================================================
typedef struct player_info_s
{
	char name[32];
	int userID;
	char guid[33];
	unsigned long friendsID;
	char friendsName[32];
	bool fakeplayer;
	bool ishltv;
	unsigned long customFiles[4];
	unsigned char filesDownloaded;
} player_info_t;
//===================================================================================================
class CHLClient
{
public:
	ClientClass *GetAllClasses(void)
	{
		typedef ClientClass *(__thiscall * OriginalFn)(PVOID); //Anything inside a VTable is a __thiscall unless it completly disregards the thisptr. You can also call them as __stdcalls, but you won't have access to the __thisptr.
		return getvfunc<OriginalFn>(this, 8)(this);			   //Return the pointer to the head CClientClass.
	}
};
//===================================================================================================
class CGlobals
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxclients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};
//===================================================================================================
class CUserCmd
{
public:
	CUserCmd()
	{
		command_number = 0;
		tick_count = 0;
		viewangles.Init();
		forwardmove = 0.0f;
		sidemove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		random_seed = 0;
		mousedx = 0;
		mousedy = 0;
		hasbeenpredicted = false;
	}
	virtual ~CUserCmd() {}; //Destructor 0
	int command_number;		//4
	int tick_count;			//8
	Vector viewangles;		//C
	float forwardmove;		//18
	float sidemove;			//1C
	float upmove;			//20
	int buttons;			//24
	BYTE impulse;			//28
	int weaponselect;		//2C
	int weaponsubtype;		//30
	int random_seed;		//34
	short mousedx;			//38
	short mousedy;			//3A
	bool hasbeenpredicted;  //3C;
};
//===================================================================================================
class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	CRC32_t m_crc;
};
//===================================================================================================
class CModelInfo
{
public:
	const char *GetModelName(DWORD *model)
	{
		typedef const char *(__thiscall * OriginalFn)(PVOID, DWORD *);
		return getvfunc<OriginalFn>(this, 3)(this, model);
	}
	DWORD *GetStudiomodel(DWORD *model)
	{
		typedef DWORD *(__thiscall * OriginalFn)(PVOID, DWORD *);
		return getvfunc<OriginalFn>(this, 28)(this, model);
	}
};
//===================================================================================================
struct mstudiobbox_t
{
	int bone;
	int group;
	Vector bbmin;
	Vector bbmax;
	int szhitboxnameindex;
	int unused[8];
};
//===================================================================================================
struct mstudiohitboxset_t
{
	int sznameindex;
	inline char *const pszName(void) const { return ((char *)this) + sznameindex; }
	int numhitboxes;
	int hitboxindex;
	mstudiobbox_t *pHitbox(int i) const { return (mstudiobbox_t *)(((BYTE *)this) + hitboxindex) + i; };
};
//===================================================================================================
class CBaseEntity
{
public:
	Vector &GetAbsOrigin()
	{
		typedef Vector &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 9)(this);
	}
	Vector &GetAbsAngles()
	{
		typedef Vector &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 10)(this);
	}
	void GetWorldSpaceCenter(Vector &vWorldSpaceCenter)
	{
		Vector vMin, vMax;
		this->GetRenderBounds(vMin, vMax);
		vWorldSpaceCenter = this->GetAbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2;
	}
	Vector &EyeAngles()
	{
		typedef Vector &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 194)(this);
	}
	DWORD *GetModel()
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef DWORD *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}
	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef bool(__thiscall * OriginalFn)(PVOID, matrix3x4 *, int, int, float);
		return getvfunc<OriginalFn>(pRenderable, 16)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
	ClientClass *GetClientClass()
	{
		PVOID pNetworkable = static_cast<PVOID>(this + 0x8);
		typedef ClientClass *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
	}
	bool IsDormant()
	{
		PVOID pNetworkable = static_cast<PVOID>(this + 0x8);
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 8)(pNetworkable);
	}
	int GetIndex()
	{
		PVOID pNetworkable = static_cast<PVOID>(this + 0x8);
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 9)(pNetworkable);
	}
	void GetRenderBounds(Vector &mins, Vector &maxs)
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef void(__thiscall * OriginalFn)(PVOID, Vector &, Vector &);
		getvfunc<OriginalFn>(pRenderable, 20)(pRenderable, mins, maxs);
	}
};
//===================================================================================================
class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char  *GetName(void) const = 0;	// get channel name
	virtual const char  *GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int *received, int *total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};
//===================================================================================================
class EngineClient
{
public:
	void GetScreenSize(int &width, int &height)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int &, int &);
		return getvfunc<OriginalFn>(this, 5)(this, width, height);
	}
	bool GetPlayerInfo(int ent_num, player_info_t *pinfo)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID, int, player_info_t *);
		return getvfunc<OriginalFn>(this, 8)(this, ent_num, pinfo);
	}
	bool Con_IsVisible(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 11)(this);
	}
	int GetLocalPlayer(void)
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 12)(this);
	}
	float Time(void)
	{
		typedef float(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 14)(this);
	}
	void GetViewAngles(Vector &va)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, Vector &);
		return getvfunc<OriginalFn>(this, 19)(this, va);
	}
	void SetViewAngles(Vector &va)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, Vector &);
		return getvfunc<OriginalFn>(this, 20)(this, va);
	}
	int GetMaxClients(void)
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 21)(this);
	}
	bool IsInGame(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 26)(this);
	}
	bool IsConnected(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 27)(this);
	}
	bool IsDrawingLoadingImage(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 28)(this);
	}
	const matrix3x4 &WorldToScreenMatrix(void)
	{
		typedef const matrix3x4 &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 36)(this);
	}
	bool IsTakingScreenshot(void)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 85)(this);
	}
	INetChannelInfo *GetNetChannelInfo(void)
	{
		typedef INetChannelInfo *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 72)(this);
	}
	void ExecuteClientCmd(const char *chCommandString)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const char *);
		return getvfunc<OriginalFn>(this, 102)(this, chCommandString);
	}
	void ClientCmd_Unrestricted(const char *chCommandString)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const char *);
		return getvfunc<OriginalFn>(this, 106)(this, chCommandString);
	}
	void ServerCmdKeyValues(PVOID keyval)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, PVOID);
		return getvfunc<OriginalFn>(this, 127)(this, keyval);
	}
};
//===================================================================================================
class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel)
	{
		typedef const char *(__thiscall * OriginalFn)(PVOID, unsigned int);
		return getvfunc<OriginalFn>(this, 36)(this, vguiPanel);
	}
};
//===================================================================================================
class ISurface
{
public:
	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 11)(this, r, g, b, a);
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 12)(this, x0, y0, x1, y1);
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 14)(this, x0, y0, x1, y1);
	}
	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 15)(this, x0, y0, x1, y1);
	}
	void DrawPolyLine(int *x, int *y, int n)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int *, int *, int);
		getvfunc<OriginalFn>(this, 16)(this, x, y, n);
	}
	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, unsigned long);
		getvfunc<OriginalFn>(this, 17)(this, font);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int, int, int);
		getvfunc<OriginalFn>(this, 19)(this, r, g, b, a);
	}
	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, int, int);
		getvfunc<OriginalFn>(this, 20)(this, x, y);
	}
	void DrawPrintText(const wchar_t *text, int textLen)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, const wchar_t *, int, int);
		return getvfunc<OriginalFn>(this, 22)(this, text, textLen, 0);
	}
	unsigned long CreateFont()
	{
		typedef unsigned int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 66)(this);
	}
	void SetFontGlyphSet(unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, unsigned long, const char *, int, int, int, int, int, int, int);
		getvfunc<OriginalFn>(this, 67)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
	void GetTextSize(unsigned long font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, unsigned long, const wchar_t *, int &, int &);
		getvfunc<OriginalFn>(this, 75)(this, font, text, wide, tall);
	}
};
//===================================================================================================
class CEntList
{
public:
	CBaseEntity *GetClientEntity(int entnum)
	{
		typedef CBaseEntity *(__thiscall * OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(this, 3)(this, entnum);
	}
	CBaseEntity *GetClientEntityFromHandle(CHandle hEnt)
	{
		typedef CBaseEntity *(__thiscall * OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(this, 4)(this, hEnt);
	}
	int GetHighestEntityIndex(void)
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 6)(this);
	}
};
//===================================================================================================
class CBaseCombatWeapon
{
public:
	int GetMaxClip1()
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 318)(this);
	}

	int GetMaxClip2()
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 319)(this);
	}

	char *GetName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 329)(this);
	}

	char *GetPrintName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 330)(this);
	}

	Vector &GetBulletSpread()
	{
		typedef Vector &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 286)(this);
	}
};
//===================================================================================================
class CTFBaseWeaponGun
{
public:
	float WeaponGetSpread()
	{
		typedef double(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 453)(this);
	}
};
//===================================================================================================
class CBaseCombatCharacter
{
public:
	// tested
	void UpdateGlowEffect()
	{
		typedef void(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 226)(this);
	}
	void DestroyGlowEffect()
	{
		typedef void(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 227)(this);
	}
};
//===================================================================================================
class CTFBaseWeaponMelee
{
public:
	bool DoSwingTrace(trace_t &trace)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID, trace_t &);
		return getvfunc<OriginalFn>(this, 443)(this, trace);
	}

	// unused
private:
	bool CalcIsAttackCriticalHelper()
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 389)(this);
	}
};
//===================================================================================================
class ICvar
{
public:
	void ConsoleColorPrintf(const Color &clr, const char *fmt, ...)
	{
		// we cant simply pass on varargs to other functions, so we have to set up a va_list and pass that
		char buf[950]; // max is 989: https://developer.valvesoftware.com/wiki/Developer_Console_Control
		va_list vlist;
		va_start(vlist, fmt);
		_vsnprintf(buf, sizeof(buf), fmt, vlist);
		va_end(vlist);
		typedef void(__cdecl * OriginalFn)(PVOID, const Color &, const char *, va_list);
		getvfunc<OriginalFn>(this, 23)(this, clr, "%s", buf);
	}

	void ConsolePrintf(const char *fmt, ...)
	{
		// we cant simply pass on varargs to other functions, so we have to set up a va_list and pass that
		char buf[950]; // max is 989: https://developer.valvesoftware.com/wiki/Developer_Console_Control
		va_list vlist;
		va_start(vlist, fmt);
		_vsnprintf(buf, sizeof(buf), fmt, vlist);
		va_end(vlist);
		typedef void(__cdecl * OriginalFn)(PVOID, const char *, va_list);
		getvfunc<OriginalFn>(this, 24)(this, "%s", buf);
	}

	// find a var by name
	ConVar *FindVar(const char *pszName)
	{
		typedef ConVar *(__thiscall * OriginalFn)(PVOID, const char *);
		return getvfunc<OriginalFn>(this, 12)(this, pszName);
	}

	// get the starting command in the chain
	ConCommandBase *GetCommands()
	{
		typedef ConCommandBase *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 16)(this);
	}

	// register a new conCommand
	void RegisterConCommand(ConCommandBase *base)
	{
		typedef void(__thiscall * OriginalFn)(PVOID, ConCommandBase *);
		return getvfunc<OriginalFn>(this, 6)(this, base);
	}
};
//===================================================================================================
enum playercontrols
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_ATTACK2 = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_ALT1 = (1 << 14),
	IN_ALT2 = (1 << 15),
	IN_SCORE = (1 << 16),   // Used by client.dll for when scoreboard is held down
	IN_SPEED = (1 << 17),   // Player is holding the speed key
	IN_WALK = (1 << 18),	// Player holding walk key
	IN_ZOOM = (1 << 19),	// Zoom key for HUD zoom
	IN_WEAPON1 = (1 << 20), // weapon defines these bits
	IN_WEAPON2 = (1 << 21), // weapon defines these bits
	IN_BULLRUSH = (1 << 22),
};
//===================================================================================================
enum tf_cond : int
{
	TFCond_Slowed = (1 << 0),				  //Toggled when a player is slowed down.
	TFCond_Zoomed = (1 << 1),				  //Toggled when a player is zoomed.
	TFCond_Disguising = (1 << 2),			  //Toggled when a Spy is disguising.
	TFCond_Disguised = (1 << 3),			  //Toggled when a Spy is disguised.
	TFCond_Cloaked = (1 << 4),			  //Toggled when a Spy is invisible.
	TFCond_Ubercharged = (1 << 5),		  //Toggled when a player is uberCharged.
	TFCond_TeleportedGlow = (1 << 6),		  //Toggled when someone leaves a teleporter and has glow beneath their feet.
	TFCond_Taunting = (1 << 7),			  //Toggled when a player is taunting.
	TFCond_UberchargeFading = (1 << 8),	 //Toggled when the uberCharge is fading.
	TFCond_CloakFlicker = (1 << 9),		  //Toggled when a Spy is visible during cloak.
	TFCond_Teleporting = (1 << 10),		  //Only activates for a brief second when the player is being teleported; not very useful.
	TFCond_Kritzkrieged = (1 << 11),		  //Toggled when a player is being crit buffed by the KritzKrieg.
	TFCond_TmpDamageBonus = (1 << 12),	  //Unknown what this is for. Name taken from the AlliedModders SDK.
	TFCond_DeadRingered = (1 << 13),		  //Toggled when a player is taking reduced damage from the Deadringer.
	TFCond_Bonked = (1 << 14),			  //Toggled when a player is under the effects of The Bonk! Atomic Punch.
	TFCond_Stunned = (1 << 15),			  //Toggled when a player's speed is reduced from airblast or a Sandman ball.
	TFCond_Buffed = (1 << 16),			  //Toggled when a player is within range of an activated Buff Banner.
	TFCond_Charging = (1 << 17),			  //Toggled when a Demoman charges with the shield.
	TFCond_DemoBuff = (1 << 18),			  //Toggled when a Demoman has heads from the Eyelander.
	TFCond_CritCola = (1 << 19),			  //Toggled when the player is under the effect of The Crit-a-Cola.
	TFCond_InHealRadius = (1 << 20),		  //Unused condition, name taken from AlliedModders SDK.
	TFCond_Healing = (1 << 21),			  //Toggled when someone is being healed by a medic or a dispenser.
	TFCond_OnFire = (1 << 22),			  //Toggled when a player is on fire.
	TFCond_Overhealed = (1 << 23),		  //Toggled when a player has >100% health.
	TFCond_Jarated = (1 << 24),			  //Toggled when a player is hit with a Sniper's Jarate.
	TFCond_Bleeding = (1 << 25),			  //Toggled when a player is taking bleeding damage.
	TFCond_DefenseBuffed = (1 << 26),		  //Toggled when a player is within range of an activated Battalion's Backup.
	TFCond_Milked = (1 << 27),			  //Player was hit with a jar of Mad Milk.
	TFCond_MegaHeal = (1 << 28),			  //Player is under the effect of Quick-Fix charge.
	TFCond_RegenBuffed = (1 << 29),		  //Toggled when a player is within a Concheror's range.
	TFCond_MarkedForDeath = (1 << 30),	  //Player is marked for death by a Fan O'War hit. Effects are similar to TFCond_Jarated.
	TFCond_NoHealingDamageBuff = (1 << 31), //Unknown what this is used for.

	TFCond_MiniCrits = (TFCond_Buffed | TFCond_CritCola),
	TFCond_IgnoreStates = (TFCond_Ubercharged | TFCond_Bonked),
};
enum tf_cond_ex : int
{
	TFCondEx_SpeedBuffAlly = (1 << 0),		   //Toggled when a player gets hit with the disciplinary action.
	TFCondEx_HalloweenCritCandy = (1 << 1),	  //Only for Scream Fortress event maps that drop crit candy.
	TFCondEx_CritCanteen = (1 << 2),			   //Player is getting a crit boost from a MVM canteen.
	TFCondEx_CritDemoCharge = (1 << 3),		   //From demo's shield
	TFCondEx_CritHype = (1 << 4),				   //Soda Popper crits.
	TFCondEx_CritOnFirstBlood = (1 << 5),		   //Arena first blood crit buff.
	TFCondEx_CritOnWin = (1 << 6),			   //End of round crits.
	TFCondEx_CritOnFlagCapture = (1 << 7),	   //CTF intelligence capture crits.
	TFCondEx_CritOnKill = (1 << 8),			   //Unknown what this is for.
	TFCondEx_RestrictToMelee = (1 << 9),		   //Unknown what this is for.
	TFCondEx_DefenseBuffNoCritBlock = (1 << 10), //MvM Buff.
	TFCondEx_Reprogrammed = (1 << 11),		   //MvM Bot has been reprogrammed.
	TFCondEx_PyroCrits = (1 << 12),			   //Player is getting crits from the Mmmph charge.
	TFCondEx_PyroHeal = (1 << 13),			   //Player is being healed from the Mmmph charge.
	TFCondEx_FocusBuff = (1 << 14),			   //Player is getting a focus buff.
	TFCondEx_DisguisedRemoved = (1 << 15),	   //Disguised remove from a bot.
	TFCondEx_MarkedForDeathSilent = (1 << 16),   //Player is under the effects of the Escape Plan/Equalizer or GRU.
	TFCondEx_DisguisedAsDispenser = (1 << 17),   //Bot is disguised as dispenser.
	TFCondEx_Sapped = (1 << 18),				   //MvM bot is being sapped.
	TFCondEx_UberchargedHidden = (1 << 19),	  //MvM Related
	TFCondEx_UberchargedCanteen = (1 << 20),	 //Player is receiving uberCharge from a canteen.
	TFCondEx_HalloweenBombHead = (1 << 21),	  //Player has a bomb on their head from Merasmus.
	TFCondEx_HalloweenThriller = (1 << 22),	  //Players are forced to dance from Merasmus.
	TFCondEx_BulletCharge = (1 << 26),		   //Player is receiving 75% reduced damage from bullets.
	TFCondEx_ExplosiveCharge = (1 << 27),		   //Player is receiving 75% reduced damage from explosives.
	TFCondEx_FireCharge = (1 << 28),			   //Player is receiving 75% reduced damage from fire.
	TFCondEx_BulletResistance = (1 << 29),	   //Player is receiving 10% reduced damage from bullets.
	TFCondEx_ExplosiveResistance = (1 << 30),	//Player is receiving 10% reduced damage from explosives.
	TFCondEx_FireResistance = (1 << 31),		   //Player is receiving 10% reduced damage from fire.

	TFCondEx_IgnoreStates = (TFCondEx_PyroHeal)
};
enum tf_cond_ex2 : int
{
	TFCondEx2_Stealthed = (1 << 0),
	TFCondEx2_MedigunDebuff = (1 << 1),
	TFCondEx2_StealthedUserBuffFade = (1 << 2),
	TFCondEx2_BulletImmune = (1 << 3),
	TFCondEx2_BlastImmune = (1 << 4),
	TFCondEx2_FireImmune = (1 << 5),
	TFCondEx2_PreventDeath = (1 << 6),
	TFCondEx2_MVMBotRadiowave = (1 << 7),
	TFCondEx2_HalloweenSpeedBoost = (1 << 8), //Wheel has granted player speed boost.
	TFCondEx2_HalloweenQuickHeal = (1 << 9),  //Wheel has granted player quick heal.
	TFCondEx2_HalloweenGiant = (1 << 10),		//Wheel has granted player giant mode.
	TFCondEx2_HalloweenTiny = (1 << 11),		//Wheel has granted player tiny mode.
	TFCondEx2_HalloweenInHell = (1 << 12),	//Wheel has granted player in hell mode.
	TFCondEx2_HalloweenGhostMode = (1 << 13), //Wheel has granted player ghost mode.
	TFCondEx2_Parachute = (1 << 16),			//Player has deployed the BASE Jumper.
	TFCondEx2_BlastJumping = (1 << 17),		//Player has sticky or rocket jumped.
};
//===================================================================================================
enum tf_classes
{
	TF2_Scout = 1,
	TF2_Sniper = 2,
	TF2_Soldier = 3,
	TF2_Demoman = 4,
	TF2_Medic = 5,
	TF2_Heavy = 6,
	TF2_Pyro = 7,
	TF2_Spy = 8,
	TF2_Engineer = 9,
};
//===================================================================================================
enum source_lifestates
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};
//===================================================================================================
enum class ClientFrameStage_t
{
	FRAME_UNDEFINED = -1, // (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};
//===================================================================================================
enum class ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,   // A fake button which is 'pressed' and 'released' when the wheel is moved up
	MOUSE_WHEEL_DOWN, // A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1
};
//===================================================================================================
class ClientModeShared
{
public:
	bool IsChatPanelOutOfFocus(void)
	{
		typedef PVOID(__thiscall * OriginalFn)(PVOID);
		PVOID CHudChat = getvfunc<OriginalFn>(this, 19)(this);
		if(CHudChat)
		{
			return *(PFLOAT)((DWORD)CHudChat + 0xFC) == 0;
		}
		return false;
	}
};
//===================================================================================================
class CInput
{
public:
	// get the current cmd
	CUserCmd *GetUserCmd(int seq)
	{
		typedef CUserCmd *(__thiscall * OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(this, 8)(this, seq);
	}
};
//===================================================================================================
class CSteamInterfaces
{
public:
	//ISteamClient017 	*client;
	nn_interface<ISteamClient017 *> client;

	//ISteamFriends002 	*friends;
	nn_interface<ISteamFriends002 *> friends;

	//ISteamUser017 	*user;
	nn_interface<ISteamUser017 *> user;

	//ISteamUserStats011 *userStats;
	nn_interface<ISteamUserStats011 *> userStats;
};
//===================================================================================================
class CInterfaces
{
public:
	//CEntList 				*EntList;
	nn_interface<CEntList *> EntList;

	//EngineClient 			*Engine;
	nn_interface<EngineClient *> Engine;

	//IPanel 			*Panels;
	nn_interface<IPanel *> Panels;

	//ISurface 				*Surface;
	nn_interface<ISurface *> Surface;

	//ClientModeShared 		*ClientMode;
	nn_interface<ClientModeShared *> ClientMode;

	//CHLClient 				*Client;
	nn_interface<CHLClient *> Client;

	//ICvar 					*Cvar;
	nn_interface<ICvar *> Cvar;

	//CEngineTrace 			*EngineTrace;
	nn_interface<CEngineTrace *> EngineTrace;

	//CModelInfo 				*ModelInfo;
	nn_interface<CModelInfo *> ModelInfo;

	//CInput 					*Input;
	nn_interface<CInput *> Input;

	CSteamInterfaces 		steam;
	
	//CGlobals 				*Globals;
	nn_interface<CGlobals *> Globals;

	//CUniformRandomStream 	*RandomStream;
	nn_interface<CUniformRandomStream *> RandomStream;

	//CPrediction 			*Prediction;
	nn_interface<CPrediction *> Prediction;

	//IMoveHelper 			*MoveHelper;
	nn_interface<IMoveHelper *> MoveHelper;

	//CPlayerResource			*GameResource;
	nn_interface<CPlayerResource *> GameResource;

	HMODULE thisDll;
};
//===================================================================================================
typedef struct predictedVars_s
{
	Vector origin;
	Vector oldOrigin;
} predictedVars_t;
//===================================================================================================
typedef struct playerVars_s
{
	tf_classes Class;
	int cond;
	int condEx;
	int health;
	int team;
	classId activeWeapon;
	int cmdNum;
	float flNextAttack;
	predictedVars_t pred;
} playerVars_t;
//===================================================================================================
extern CInterfaces gInts;
extern CPlayerVariables gEntVars;
extern COffsets gOffsets;
extern playerVars_t gLocalPlayerVars;

class CHack;
extern CHack gHack;

class CFileManager;
extern CFileManager gFileManager;
//===================================================================================================
#include "CHack.h"