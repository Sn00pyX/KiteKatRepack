#pragma once

#include <mmo/ArOption.h>
#include <mmo/ArObject.h>

struct GameObject : ArObject
{
	struct iterator
	{
	public:
		iterator()
			: m_pObject( NULL )
		{
		}
		iterator( GameObject * p )
			: m_pObject( p )
		{
			inc_ref();
		}
		iterator( const iterator & p )
			: m_pObject( p.m_pObject )
		{
			inc_ref();
		}
		virtual ~iterator()
		{
			dec_ref();
		}

		iterator & operator =( GameObject * p )
		{
			set( p );
			return *this;
		}

		iterator & operator =( const iterator & p )
		{
			set( p.m_pObject );
			return *this;
		}

		struct GameObject* operator*()	{ return m_pObject; }
		struct GameObject* get()		{ return m_pObject; }
		void set( GameObject* p )
		{
			dec_ref();

			m_pObject = p;

			inc_ref();
		}

	private:

		void	inc_ref()
		{
			if( m_pObject != NULL )
			{
				InterlockedIncrement( &m_pObject->m_nRefCount );
				assert( m_pObject->m_nRefCount > 0 );
			}
		}
		void	dec_ref()
		{
			if( m_pObject != NULL )
			{
				InterlockedDecrement( &m_pObject->m_nRefCount );
				assert( m_pObject->m_nRefCount >= 0 );
			}
		}

	protected:

		friend struct	GameObject;
		GameObject*		m_pObject;
	};

	GameObject() : m_nRefCount( 0 )
	{
	}

	static iterator			get( AR_HANDLE handle );
	static GameObject*		raw_get( AR_HANDLE handle );

	virtual bool	IsCreature() const			{ return false;		}
	virtual bool	IsPlayer() const			{ return false;		}
	virtual bool	IsMonster() const			{ return false;		}
	virtual bool	IsNPC() const				{ return false;		}
	virtual bool	IsItem() const				{ return false;		}
	virtual bool	IsSummon() const			{ return false;		}
	virtual bool	IsPet() const				{ return false;		}
	virtual bool	IsSkillProp() const			{ return false;		}
	virtual bool	IsFieldProp() const			{ return false;		}
	virtual bool	IsRoamer() const			{ return false;		}
	
protected:

	// IsDeleteable() 을 상속받을때는 GameObject::IsDeleteable() 도 꼭 불러줄것.
	virtual bool			IsDeleteable()
	{	
		if( !ArSchedulerObject::IsDeleteable() ) return false;

		if( m_nRefCount > 0 ) return false;

		return true;
	}

	virtual ~GameObject()
	{
	}

	volatile		LONG	m_nRefCount;
};

// MFC 스타일 'ㅠ ';?
#define ITERATOR_DEFINITION( GAME_OBJECT_TYPE ) \
struct iterator : GameObject::iterator\
{\
	iterator() : GameObject::iterator()									{}\
	iterator( struct GAME_OBJECT_TYPE * p ) : GameObject::iterator( p )	{}\
	iterator( const iterator & p ) : GameObject::iterator( p )			{}\
	virtual ~iterator()													{}\
\
	iterator & operator =( struct GAME_OBJECT_TYPE * p )				{ return static_cast< iterator & >( static_cast< GameObject::iterator & >(*this) = static_cast< GameObject * >( p ) ); }\
	iterator & operator =( const iterator & p )							{ return static_cast< iterator & >( static_cast< GameObject::iterator & >(*this) = static_cast< const GameObject::iterator & >( p ) ); }\
\
	struct GAME_OBJECT_TYPE * get()										{ return static_cast< struct GAME_OBJECT_TYPE * >( GameObject::iterator::get() ); }\
	struct GAME_OBJECT_TYPE * operator*()								{ return static_cast< struct GAME_OBJECT_TYPE * >( static_cast< GameObject * >( *static_cast< GameObject::iterator & >(*this) ) ); }\
\
protected:\
	friend struct GAME_OBJECT_TYPE;\
}
