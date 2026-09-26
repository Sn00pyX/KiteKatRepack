#pragma once

#include <vector>
#include <cassert>

#include "ILock.h"

template< typename T, typename UNIT, int MAX_COUNT = 100, int MAX_DEPTH = 10 >
class XQuadScanner
{
private:

	struct _NODE
	{
		_NODE( unsigned short _depth, UNIT _left, UNIT _right, UNIT _top, UNIT _bottom ) : depth( _depth ), right( _right ), bottom( _bottom ), left( _left ), top( _top )
		{	
			pNode[0][0] = NULL;
			pNode[1][0] = NULL;
			pNode[0][1] = NULL;
			pNode[1][1] = NULL;
			pvList = NULL;
			bIsLeafNode = true;
		}

		~_NODE()
		{
			if ( pNode[0][0] ) delete pNode[0][0];
			if ( pNode[1][0] ) delete pNode[1][0];
			if ( pNode[0][1] ) delete pNode[0][1];
			if ( pNode[1][1] ) delete pNode[1][1];
			if ( pvList ) delete pvList;
		}

		_NODE *getLeafNode( UNIT x, UNIT y )
		{
			if ( bIsLeafNode ) return this;

			int rx = 0,ry = 0;

			if ( x >= (left + (right - left)/2) ) rx = 1;
			if ( y >= (top + (bottom - top)/2) ) ry = 1;

			return pNode[rx][ry]->getLeafNode( x, y );
		}

		void Scan( UNIT _left, UNIT _top, UNIT _right, UNIT _bottom, std::vector< T > & vList  )
		{
			if ( bIsLeafNode && pvList )
			{		
				for ( std::vector< T >::iterator it = pvList->begin(); it != pvList->end(); ++it )
				{
					if ( (*it).GetQuadX() >= _left && (*it).GetQuadX() <= _right &&
						(*it).GetQuadY() >= _top && (*it).GetQuadY() <= _bottom ) vList.push_back( *it );
				}
				return;
			}

			for ( int rx = 0; rx < 2; ++rx )
			{
				for ( int ry = 0; ry < 2; ++ry )
				{
					if ( !pNode[rx][ry] ) continue;

					if ( min( pNode[rx][ry]->right, _right ) >= max( pNode[rx][ry]->left,  _left ) && 
						min( pNode[rx][ry]->bottom,  _bottom  ) >= max( pNode[rx][ry]->top, _top ) ) 
					{
						pNode[rx][ry]->Scan( _left, _top, _right, _bottom, vList );
					}
				}
			}		
		}

		void Add( const T & t )
		{
			// 유효성 검사
			if ( depth != MAX_DEPTH )
			{
				if ( t.GetQuadY() < top || t.GetQuadY() >= bottom || t.GetQuadX() < left || t.GetQuadX() >= right ) 
				{
					return;
				}
			}
			
			// leaf 노드를 얻자
			_NODE *pNode = getLeafNode( t.GetQuadX(), t.GetQuadY() );

			// 아이템 추가
			if ( !pNode->pvList )
			{
				pNode->pvList = new std::vector< T >;
				pNode->pvList->reserve( MAX_COUNT );
			}
			pNode->pvList->push_back( t );

			// 갯수가 MAX_COUNT보다 많다면 나누자
			if ( depth != MAX_DEPTH && pNode->pvList->size() > MAX_COUNT ) pNode->Divide();
		}

		bool Remove( const T & t )
		{
			// 유효성 검사
			if ( depth != MAX_DEPTH )
			{
				if ( t.GetQuadY() < top || t.GetQuadY() >= bottom || t.GetQuadX() < left || t.GetQuadX() >= right ) 
				{
					return false;
				}
			}

			// leaf 노드를 얻자
			_NODE *pNode = getLeafNode( t.GetQuadX(), t.GetQuadY() );

			if ( !pNode->pvList ) return false;

			for ( std::vector< T >::iterator it = pNode->pvList->begin(); it != pNode->pvList->end(); ++it )
			{
				if ( *it == t )
				{
					pNode->pvList->erase( it );
					return true;
				}
			}

			return false;
		}

		bool IsExist( UNIT x, UNIT y )
		{
			// leaf 노드를 얻자
			_NODE *pNode = getLeafNode( x, y );

			if ( !pNode->pvList ) return false;

			for ( std::vector< T >::iterator it = pNode->pvList->begin(); it != pNode->pvList->end(); ++it )
			{
				if ( (*it).GetQuadX() == x && (*it).GetQuadY() == y ) return true;
			}

			return false;
		}

		bool IsExist( const T & t )
		{
			return IsExist( t.GetQuadX(), t.GetQuadY() );
		}

		void Divide()
		{
			if ( depth != MAX_DEPTH )
			{
				bIsLeafNode = false;

				pNode[0][0] = new _NODE( depth+1, left, left + (right-left)/2, top, top + (bottom-top)/2 );
				pNode[1][0] = new _NODE( depth+1, left + (right-left)/2, right, top, top + (bottom-top)/2 );
				pNode[0][1] = new _NODE( depth+1, left, left + (right-left)/2, top + (bottom-top)/2, bottom );
				pNode[1][1] = new _NODE( depth+1, left + (right-left)/2, right, top + (bottom-top)/2, bottom );

				for ( std::vector< T >::iterator it = pvList->begin(); it != pvList->end(); ++it )
				{
					Add( *it );
				}

				delete pvList;
				pvList = NULL;
			}
		}

		bool			bIsLeafNode;
		unsigned short	depth;
		UNIT			left;
		UNIT			right;
		UNIT			top;
		UNIT			bottom;

		struct _NODE *	pNode[2][2];

		std::vector< T > *	pvList;
	};

public:

	XQuadScanner( UNIT width, UNIT height ) : m_Node( 0, 0, width, 0, height )
	{
		m_nWidth = width;
		m_nHeight = height;
	}

	void Add( const T & t )
	{
		THREAD_SYNCHRONIZE( m_Lock );

		m_Node.Add( t );
	}

	bool IsExist( const T & t )
	{
		THREAD_SYNCHRONIZE( m_Lock );

		return m_Node.IsExist( t );
	}

	bool IsExist( UNIT x, UNIT y )
	{
		THREAD_SYNCHRONIZE( m_Lock );

		return m_Node.IsExist( x, y );
	}

	bool Remove( const T & t )
	{
		THREAD_SYNCHRONIZE( m_Lock );

		return m_Node.Remove( t );
	}

	void Scan( UNIT _left, UNIT _top, UNIT _right, UNIT _bottom, std::vector< T > & vList  )
	{
		THREAD_SYNCHRONIZE( m_Lock );

		return m_Node.Scan( _left, _top, _right, _bottom, vList );
	}

private:

	XCriticalSection		m_Lock;

	UNIT	m_nWidth;
	UNIT	m_nHeight;
	_NODE 	m_Node;
};