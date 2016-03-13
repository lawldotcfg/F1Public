#pragma once
#include "baseheaders.h"

template <class T, class I>
struct UtlLinkedListElem_t
{
	T  m_Element;
	I  m_Previous;
	I  m_Next;

private:
	// No copy constructor for these...
	UtlLinkedListElem_t(const UtlLinkedListElem_t&);
};

template <class T, class S = unsigned short, bool ML = false, class I = S, class M = CUtlMemory< UtlLinkedListElem_t<T, S>, I > >
class CUtlLinkedList
{
public:
	typedef T ElemType_t;
	typedef S IndexType_t; // should really be called IndexStorageType_t, but that would be a huge change
	typedef I IndexLocalType_t;
	typedef M MemoryAllocator_t;
	static const bool IsUtlLinkedList = true; // Used to match this at compiletime 

											  // constructor, destructor
	CUtlLinkedList(int growSize = 0, int initSize = 0);
	~CUtlLinkedList();

	// gets particular elements
	T&         Element(I i);
	T const&   Element(I i) const;
	T&         operator[](I i);
	T const&   operator[](I i) const;

	// Make sure we have a particular amount of memory
	void EnsureCapacity(int num);

	void SetGrowSize(int growSize);

	// Memory deallocation
	void Purge();

	// Delete all the elements then call Purge.
	void PurgeAndDeleteElements();

	// Insertion methods....
	I	InsertBefore(I before);
	I	InsertAfter(I after);
	I	AddToHead();
	I	AddToTail();

	I	InsertBefore(I before, T const& src);
	I	InsertAfter(I after, T const& src);
	I	AddToHead(T const& src);
	I	AddToTail(T const& src);

	// Find an element and return its index or InvalidIndex() if it couldn't be found.
	I		Find(const T &src) const;

	// Look for the element. If it exists, remove it and return true. Otherwise, return false.
	bool	FindAndRemove(const T &src);

	// Removal methods
	void	Remove(I elem);
	void	RemoveAll();

	// Allocation/deallocation methods
	// If multilist == true, then list list may contain many
	// non-connected lists, and IsInList and Head + Tail are meaningless...
	I		Alloc(bool multilist = false);
	void	Free(I elem);

	// list modification
	void	LinkBefore(I before, I elem);
	void	LinkAfter(I after, I elem);
	void	Unlink(I elem);
	void	LinkToHead(I elem);
	void	LinkToTail(I elem);

	// invalid index (M will never allocate an element at this index)
	inline static S  InvalidIndex() { return (S)M::InvalidIndex(); }

	// Is a given index valid to use? (representible by S and not the invalid index)
	static bool IndexInRange(I index);

	inline static size_t ElementSize() { return sizeof(ListElem_t); }

	// list statistics
	int	Count() const;
	I	MaxElementIndex() const;
	I	NumAllocated(void) const { return m_NumAlloced; }

	// Traversing the list
	I  Head() const;
	I  Tail() const;
	I  Previous(I i) const;
	I  Next(I i) const;

	// STL compatible const_iterator class
	template < typename List_t >
	class _CUtlLinkedList_constiterator_t
	{
	public:
		typedef typename List_t::ElemType_t ElemType_t;
		typedef typename List_t::IndexType_t IndexType_t;

		// Default constructor -- gives a currently unusable iterator.
		_CUtlLinkedList_constiterator_t()
			: m_list(0)
			, m_index(List_t::InvalidIndex())
		{}
		// Normal constructor.
		_CUtlLinkedList_constiterator_t(const List_t& list, IndexType_t index)
			: m_list(&list)
			, m_index(index)
		{}

		// Pre-increment operator++. This is the most efficient increment
		// operator so it should always be used.
		_CUtlLinkedList_constiterator_t& operator++()
		{
			m_index = m_list->Next(m_index);
			return *this;
		}
		// Post-increment operator++. This is less efficient than pre-increment.
		_CUtlLinkedList_constiterator_t operator++(int)
		{
			// Copy ourselves.
			_CUtlLinkedList_constiterator_t temp = *this;
			// Increment ourselves.
			++*this;
			// Return the copy.
			return temp;
		}

		// Pre-decrement operator--. This is the most efficient decrement
		// operator so it should always be used.
		_CUtlLinkedList_constiterator_t& operator--()
		{
			Assert(m_index != m_list->Head());
			if(m_index == m_list->InvalidIndex())
			{
				m_index = m_list->Tail();
			}
			else
			{
				m_index = m_list->Previous(m_index);
			}
			return *this;
		}
		// Post-decrement operator--. This is less efficient than post-decrement.
		_CUtlLinkedList_constiterator_t operator--(int)
		{
			// Copy ourselves.
			_CUtlLinkedList_constiterator_t temp = *this;
			// Decrement ourselves.
			--*this;
			// Return the copy.
			return temp;
		}

		bool operator==(const _CUtlLinkedList_constiterator_t& other) const
		{
			Assert(m_list == other.m_list);
			return m_index == other.m_index;
		}

		bool operator!=(const _CUtlLinkedList_constiterator_t& other) const
		{
			Assert(m_list == other.m_list);
			return m_index != other.m_index;
		}

		const ElemType_t& operator*() const
		{
			return m_list->Element(m_index);
		}

		const ElemType_t* operator->() const
		{
			return (&**this);
		}

	protected:
		// Use a pointer rather than a reference so that we can support
		// assignment of iterators.
		const List_t* m_list;
		IndexType_t m_index;
	};

	// STL compatible iterator class, using derivation so that a non-const
	// list can return a const_iterator.
	template < typename List_t >
	class _CUtlLinkedList_iterator_t : public _CUtlLinkedList_constiterator_t< List_t >
	{
	public:
		typedef typename List_t::ElemType_t ElemType_t;
		typedef typename List_t::IndexType_t IndexType_t;
		typedef _CUtlLinkedList_constiterator_t< List_t > Base;

		// Default constructor -- gives a currently unusable iterator.
		_CUtlLinkedList_iterator_t()
		{}
		// Normal constructor.
		_CUtlLinkedList_iterator_t(const List_t& list, IndexType_t index)
			: _CUtlLinkedList_constiterator_t< List_t >(list, index)
		{}

		// Pre-increment operator++. This is the most efficient increment
		// operator so it should always be used.
		_CUtlLinkedList_iterator_t& operator++()
		{
			Base::m_index = Base::m_list->Next(Base::m_index);
			return *this;
		}
		// Post-increment operator++. This is less efficient than pre-increment.
		_CUtlLinkedList_iterator_t operator++(int)
		{
			// Copy ourselves.
			_CUtlLinkedList_iterator_t temp = *this;
			// Increment ourselves.
			++*this;
			// Return the copy.
			return temp;
		}

		// Pre-decrement operator--. This is the most efficient decrement
		// operator so it should always be used.
		_CUtlLinkedList_iterator_t& operator--()
		{
			Assert(Base::m_index != Base::m_list->Head());
			if(Base::m_index == Base::m_list->InvalidIndex())
			{
				Base::m_index = Base::m_list->Tail();
			}
			else
			{
				Base::m_index = Base::m_list->Previous(Base::m_index);
			}
			return *this;
		}
		// Post-decrement operator--. This is less efficient than post-decrement.
		_CUtlLinkedList_iterator_t operator--(int)
		{
			// Copy ourselves.
			_CUtlLinkedList_iterator_t temp = *this;
			// Decrement ourselves.
			--*this;
			// Return the copy.
			return temp;
		}

		ElemType_t& operator*() const
		{
			// Const_cast to allow sharing the implementation with the
			// base class.
			List_t* pMutableList = const_cast<List_t*>(Base::m_list);
			return pMutableList->Element(Base::m_index);
		}

		ElemType_t* operator->() const
		{
			return (&**this);
		}
	};

	typedef _CUtlLinkedList_constiterator_t<CUtlLinkedList<T, S, ML, I, M> > const_iterator;
	typedef _CUtlLinkedList_iterator_t<CUtlLinkedList<T, S, ML, I, M> > iterator;
	const_iterator begin() const
	{
		return const_iterator(*this, Head());
	}
	iterator begin()
	{
		return iterator(*this, Head());
	}

	const_iterator end() const
	{
		return const_iterator(*this, InvalidIndex());
	}
	iterator end()
	{
		return iterator(*this, InvalidIndex());
	}

	// Are nodes in the list or valid?
	bool  IsValidIndex(I i) const;
	bool  IsInList(I i) const;

protected:

	// What the linked list element looks like
	typedef UtlLinkedListElem_t<T, S>  ListElem_t;

	// constructs the class
	I		AllocInternal(bool multilist = false);
	void ConstructList();

	// Gets at the list element....
	ListElem_t& InternalElement(I i) { return m_Memory[i]; }
	ListElem_t const& InternalElement(I i) const { return m_Memory[i]; }

	// copy constructors not allowed
	CUtlLinkedList(CUtlLinkedList<T, S, ML, I, M> const& list) { Assert(0); }

	M	m_Memory;
	I	m_Head;
	I	m_Tail;
	I	m_FirstFree;
	I	m_ElementCount;		// The number actually in the list
	I	m_NumAlloced;		// The number of allocated elements
	typename M::Iterator_t	m_LastAlloc; // the last index allocated

										 // For debugging purposes; 
										 // it's in release builds so this can be used in libraries correctly
	ListElem_t  *m_pElements;

	__forceinline M const &Memory(void) const
	{
		return m_Memory;
	}

	void ResetDbgInfo()
	{
		m_pElements = m_Memory.Base();
	}

private:
	// Faster version of Next that can only be used from tested code internal
	// to this class, such as Find(). It avoids the cost of checking the index
	// validity, which is a big win on debug builds.
	I  PrivateNext(I i) const;
};

// this is kind of ugly, but until C++ gets templatized typedefs in C++0x, it's our only choice
template < class T >
class CUtlFixedLinkedList : public CUtlLinkedList< T, int, true, int, CUtlFixedMemory< UtlLinkedListElem_t< T, int > > >
{
public:
	CUtlFixedLinkedList(int growSize = 0, int initSize = 0)
		: CUtlLinkedList< T, int, true, int, CUtlFixedMemory< UtlLinkedListElem_t< T, int > > >(growSize, initSize) {}

	typedef CUtlLinkedList< T, int, true, int, CUtlFixedMemory< UtlLinkedListElem_t< T, int > > > BaseClass;
	bool IsValidIndex(int i) const
	{
		if(!BaseClass::Memory().IsIdxValid(i))
			return false;

#ifdef _DEBUG // it's safe to skip this here, since the only way to get indices after m_LastAlloc is to use MaxElementIndex
		if(BaseClass::Memory().IsIdxAfter(i, this->m_LastAlloc))
		{
			Assert(0);
			return false; // don't read values that have been allocated, but not constructed
		}
#endif

		return (BaseClass::Memory()[i].m_Previous != i) || (BaseClass::Memory()[i].m_Next == i);
	}

private:
	int	MaxElementIndex() const { Assert(0); return BaseClass::InvalidIndex(); } // fixedmemory containers don't support iteration from 0..maxelements-1
	void ResetDbgInfo() {}
};