#pragma once

#include "baseHeaders.h"

// moved from macros to class to not pollute the global namespace
enum class ConvarFlags
{
	// The default, no flags at all
	FCVAR_NONE = 0,

	// Command to ConVars and ConCommands
	// ConVar Systems
	FCVAR_UNREGISTERED = (1 << 0), // If this is set, don't add to linked list, etc.
	FCVAR_DEVELOPMENTONLY = (1 << 1), // Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
	FCVAR_GAMEDLL = (1 << 2), // defined by the game DLL
	FCVAR_CLIENTDLL = (1 << 3), // defined by the client DLL
	FCVAR_HIDDEN = (1 << 4), // Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

							   // ConVar only
	FCVAR_PROTECTED = (1 << 5),  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
	FCVAR_SPONLY = (1 << 6),  // This cvar cannot be changed by clients connected to a multiplayer server.
	FCVAR_ARCHIVE = (1 << 7),  // set to cause it to be saved to vars.rc
	FCVAR_NOTIFY = (1 << 8),  // notifies players when changed
	FCVAR_USERINFO = (1 << 9),  // changes the client's info string
	FCVAR_CHEAT = (1 << 14), // Only useable in singleplayer / debug / multiplayer & sv_cheats

	FCVAR_PRINTABLEONLY = (1 << 10), // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
	FCVAR_UNLOGGED = (1 << 11), // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
	FCVAR_NEVER_AS_STRING = (1 << 12), // never try to print that cvar

										 // It's a ConVar that's shared between the client and the server.
										 // At signon, the values of all such ConVars are sent from the server to the client (skipped for local
										 //  client, of course )
										 // If a change is requested it must come from the console (i.e., no remote client changes)
										 // If a value is changed while a server is active, it's replicated to all connected clients
	FCVAR_REPLICATED = (1 << 13), // server setting enforced on clients, TODO rename to FCAR_SERVER at some time
	FCVAR_DEMO = (1 << 16), // record this cvar when starting a demo file
	FCVAR_DONTRECORD = (1 << 17), // don't record these command in demofiles
	FCVAR_RELOAD_MATERIALS = (1 << 20), // If this cvar changes, it forces a material reload
	FCVAR_RELOAD_TEXTURES = (1 << 21), // If this cvar changes, if forces a texture reload

	FCVAR_NOT_CONNECTED = (1 << 22), // cvar cannot be changed by a client that is connected to a server
	FCVAR_MATERIAL_SYSTEM_THREAD = (1 << 23), // Indicates this cvar is read from the material system thread
	FCVAR_ARCHIVE_XBOX = (1 << 24), // cvar written to config.cfg on the Xbox

	FCVAR_ACCESSIBLE_FROM_THREADS = (1 << 25), // used as a debugging tool necessary to check material system thread convars

	FCVAR_SERVER_CAN_EXECUTE = (1 << 28), // the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
	FCVAR_SERVER_CANNOT_QUERY = (1 << 29), // If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
	FCVAR_CLIENTCMD_CAN_EXECUTE = (1 << 30), // IVEngineClient::ClientCmd is allowed to execute this command.

	FCVAR_MATERIAL_THREAD_MASK = (FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD)
};

class IConVar
{
public:
	// Value set
	virtual void SetValue(const char *pValue) = 0;
	virtual void SetValue(float flValue) = 0;
	virtual void SetValue(int nValue) = 0;

	// Return name of command
	virtual const char *GetName(void)const = 0;

	// Accessors.. not as efficient as using GetState()/GetInfo()
	// if you call these methods multiple times on the same IConVar
	virtual bool IsFlagSet(int nFlag) const = 0;
};

// Called when a ConVar changes value
// NOTE: For FCVAR_NEVER_AS_STRING ConVars, pOldValue == NULL
typedef void(*FnChangeCallback_t)(IConVar *var, const char *pOldValue, float flOldValue);

//-----------------------------------------------------------------------------
// Purpose: The base console invoked command/cvar interface
//-----------------------------------------------------------------------------
class ConCommandBase
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
	friend void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor);
	friend void ConVar_PublishToVXConsole();

	// FIXME: Remove when ConVar changes are done
	friend class CDefaultCvar;

public:
	ConCommandBase(void);
	ConCommandBase(const char *pName, const char *pHelpString = 0,
		int flags = 0);

	virtual						~ConCommandBase(void);

	virtual	bool				IsCommand(void) const;

	// Check flag
	virtual bool				IsFlagSet(int flag) const;
	// Set flag
	virtual void				AddFlags(int flags);

	// Return name of cvar
	virtual const char			*GetName(void) const;

	// Return help text for cvar
	virtual const char			*GetHelpText(void) const;

	// Deal with next pointer
	const ConCommandBase		*GetNext(void) const;
	ConCommandBase				*GetNext(void);

	virtual bool				IsRegistered(void) const;

	// Returns the DLL identifier
	virtual int					GetDLLIdentifier() const;

//protected:
	virtual void				Create(const char *pName, const char *pHelpString = 0,
		int flags = 0);

	// Used internally by OneTimeInit to initialize/shutdown
	virtual void				Init();
	void						Shutdown();

	// Internal copy routine ( uses new operator from correct module )
	char						*CopyString(const char *from);

//private:
	// Next ConVar in chain
	// Prior to register, it points to the next convar in the DLL.
	// Once registered, though, m_pNext is reset to point to the next
	// convar in the global list
	ConCommandBase				*m_pNext;

	// Has the cvar been added to the global list?
	bool						m_bRegistered;

	// Static data
	const char 					*m_pszName;
	const char 					*m_pszHelpString;

	// ConVar flags
	int							m_nFlags;

//protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase		*s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor	*s_pAccessor;
};

//__declspec(noinline) inline void registerNewCvar(ConVar *pVar, const char *pName, const char *pDefaultValue, int flags = 0)
//{
//	typedef void(__thiscall *cvarConfn)(ConVar *, const char *, const char *, int);
//	static DWORD dwAddr = gSignatures.GetEngineSignature("55 8B EC D9 EE 56 6A 00 51 D9 14 24 6A 00 51 D9 14 24 6A 00 51 D9 14 24 6A 00 51 D9 1C 24 6A 00 8B F1");
//	static cvarConfn constr = (cvarConfn)dwAddr;
//
//	constr(pVar, pName, pDefaultValue, flags);
//
//	return;
//}

class ConVar : public ConCommandBase, public IConVar
{
	friend class CCvar;
	friend class ConVarRef;

public:
	typedef ConCommandBase BaseClass;

	//ConVar(const char *pName, const char *pDefaultValue, int flags = 0)
	//{
	//	registerNewCvar(this, pName, pDefaultValue, flags);
	//}

	ConVar(const char *pName, const char *pDefaultValue, int flags,
		const char *pHelpString);
	ConVar(const char *pName, const char *pDefaultValue, int flags,
		const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax);
	ConVar(const char *pName, const char *pDefaultValue, int flags,
		const char *pHelpString, FnChangeCallback_t callback);
	ConVar(const char *pName, const char *pDefaultValue, int flags,
		const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax,
		FnChangeCallback_t callback);

	virtual						~ConVar(void);

	virtual bool				IsFlagSet(int flag) const = 0;
	virtual const char*			GetHelpText(void) const = 0;
	virtual bool				IsRegistered(void) const = 0;
	virtual const char			*GetName(void) const = 0;
	virtual void				AddFlags(int flags) = 0;
	virtual	bool				IsCommand(void) const = 0;

	// Install a change callback (there shouldn't already be one....)
	void InstallChangeCallback(FnChangeCallback_t callback);

	// Retrieve value
	__forceinline float			GetFloat(void) const;
	__forceinline int			GetInt(void) const;
	__forceinline bool			GetBool() const { return !!GetInt(); }
	__forceinline char const	   *GetString(void) const;

	// Any function that allocates/frees memory needs to be virtual or else you'll have crashes
	//  from alloc/free across dll/exe boundaries.

	// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
	virtual void				SetValue(const char *value) = 0;
	virtual void				SetValue(float value) = 0;
	virtual void				SetValue(int value) = 0;

	// Reset to default value
	void						Revert(void);

	// True if it has a min/max setting
	bool						GetMin(float& minVal) const;
	bool						GetMax(float& maxVal) const;
	const char					*GetDefault(void) const;
	void						SetDefault(const char *pszDefault);

	//private:
		// Called by CCvar when the value of a var is changing.
	virtual void				InternalSetValue(const char *value) = 0;
	// For CVARs marked FCVAR_NEVER_AS_STRING
	virtual void				InternalSetFloatValue(float fNewValue) = 0;
	virtual void				InternalSetIntValue(int nValue) = 0;

	virtual bool				ClampValue(float& value) = 0;
	virtual void				ChangeStringValue(const char *tempVal, float flOldValue) = 0;

	virtual void				Create(const char *pName, const char *pDefaultValue, int flags = 0,
		const char *pHelpString = 0, bool bMin = false, float fMin = 0.0,
		bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0) = 0;

	// Used internally by OneTimeInit to initialize.
	virtual void				Init() = 0;
	int GetFlags() { return m_pParent->m_nFlags; }
	//private:

		// This either points to "this" or it points to the original declaration of a ConVar.
		// This allows ConVars to exist in separate modules, and they all use the first one to be declared.
		// m_pParent->m_pParent must equal m_pParent (ie: m_pParent must be the root, or original, ConVar).
	ConVar						*m_pParent;

	// Static data
	const char					*m_pszDefaultValue;

	// Value
	// Dynamically allocated
	char						*m_pszString;
	int							m_StringLength;

	// Values
	float						m_fValue;
	int							m_nValue;

	// Min/Max values
	bool						m_bHasMin;
	float						m_fMinVal;
	bool						m_bHasMax;
	float						m_fMaxVal;

	// Call this function when ConVar changes
	FnChangeCallback_t			m_fnChangeCallback;
};

//-----------------------------------------------------------------------------
// Purpose: Return ConVar value as a float
// Output : float
//-----------------------------------------------------------------------------
__forceinline float ConVar::GetFloat(void) const
{
	return m_pParent->m_fValue;
}

//-----------------------------------------------------------------------------
// Purpose: Return ConVar value as an int
// Output : int
//-----------------------------------------------------------------------------
__forceinline int ConVar::GetInt(void) const
{
	return m_pParent->m_nValue;
}

//-----------------------------------------------------------------------------
// Purpose: Return ConVar value as a string, return "" for bogus string pointer, etc.
// Output : const char *
//-----------------------------------------------------------------------------
__forceinline const char *ConVar::GetString(void) const
{
	if(m_nFlags & static_cast<int>(ConvarFlags::FCVAR_NEVER_AS_STRING))
		return "FCVAR_NEVER_AS_STRING";

	return (m_pParent->m_pszString) ? m_pParent->m_pszString : "";
}


// hrmmph
struct characterset_t
{
	char set[256];
};

//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------
class CCommand
{
public:
	CCommand();
	CCommand(int nArgC, const char **ppArgV);
	bool Tokenize(const char *pCommand, characterset_t *pBreakSet = NULL);
	void Reset();

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[](int nIndex) const;	// Gets at arguments
	const char *Arg(int nIndex) const;		// Gets at arguments

											// Helper functions to parse arguments to commands.
	const char* FindArg(const char *pName) const;
	int FindArgInt(const char *pName, int nDefaultVal) const;

	static int MaxCommandLength();
	static characterset_t* DefaultBreakSet();

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		m_nArgc;
	int		m_nArgv0Size;
	char	m_pArgSBuffer[COMMAND_MAX_LENGTH];
	char	m_pArgvBuffer[COMMAND_MAX_LENGTH];
	const char*	m_ppArgv[COMMAND_MAX_ARGC];
};

inline int CCommand::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

inline int CCommand::ArgC() const
{
	return m_nArgc;
}

inline const char **CCommand::ArgV() const
{
	return m_nArgc ? (const char**)m_ppArgv : NULL;
}

inline const char *CCommand::ArgS() const
{
	return m_nArgv0Size ? &m_pArgSBuffer[m_nArgv0Size] : "";
}

inline const char *CCommand::GetCommandString() const
{
	return m_nArgc ? m_pArgSBuffer : "";
}

inline const char *CCommand::Arg(int nIndex) const
{
	// FIXME: Many command handlers appear to not be particularly careful
	// about checking for valid argc range. For now, we're going to
	// do the extra check and return an empty string if it's out of range
	if(nIndex < 0 || nIndex >= m_nArgc)
		return "";
	return m_ppArgv[nIndex];
}

inline const char *CCommand::operator[](int nIndex) const
{
	return Arg(nIndex);
}


// CONVAR::CREATE = 55 8B EC 8B 45 0C 85 C0 56