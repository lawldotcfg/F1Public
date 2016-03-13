#pragma once

#include "baseHeaders.h"

class CEngineRecipientFilter : public IRecipientFilter
{
public:	// IRecipientFilter interface:

	CEngineRecipientFilter();
	virtual int		GetRecipientCount(void) const;
	virtual int		GetRecipientIndex(int slot) const;
	virtual bool	IsReliable(void) const { return m_bReliable; };
	virtual bool	IsInitMessage(void)  const { return m_bInit; };

public:

	void			Reset(void);

	void			MakeInitMessage(void);
	void			MakeReliable(void);

	void			AddAllPlayers(void);
	void			AddRecipientsByPVS(const Vector& origin);
	void			AddRecipientsByPAS(const Vector& origin);
	// ABSALOUTE_PLAYER_LIMIT
	void			AddPlayersFromBitMask(CBitVec< 32 >& playerbits);
	void			AddPlayersFromFilter(const IRecipientFilter *filter);
	void			AddRecipient(int playerindex);
	void			RemoveRecipient(int playerindex);
	bool			IncludesPlayer(int playerindex);

private:

	bool				m_bInit;
	bool				m_bReliable;
	CUtlVector< int >	m_Recipients;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CEventInfo
{
public:
	enum
	{
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	// 0 implies not in use
	short classID;

	// If non-zero, the delay time when the event should be fired ( fixed up on the client )
	float fire_delay;

	// send table pointer or NULL if send as full update
	const /*SendTable*/ int *pSendTable;
	const ClientClass *pClientClass;

	// Length of data bits
	int		bits;
	// Raw event data
	BYTE	*pData;
	// CLIENT ONLY Reliable or not, etc.
	int		flags;

	// clients that see that event
	CEngineRecipientFilter filter;
};

