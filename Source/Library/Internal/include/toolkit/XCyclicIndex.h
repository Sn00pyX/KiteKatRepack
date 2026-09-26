#ifndef __X_CYCLIC_INDEX_H__
#define __X_CYCLIC_INDEX_H__
#pragma once

namespace xmod {

	template< typename T >
	struct cyclic_index
	{
		cyclic_index( const T& _size ) : mMin( 0 ), mMax( _size - 1 ), mIndex( 0 ) {}
		cyclic_index( const T& _base, const T& _range ) : mMin( _base ), mMax( _base + _range - 1 ), mIndex( _base ) {}
		cyclic_index( const cyclic_index& rhs ) : mMin( rhs.mMin ), mMax( rhs.mMax ), mIndex( rhs.mIndex ) {}
		cyclic_index& operator =( const cyclic_index& rhs )
		{
			mMin = rhs.mMin;
			mMax = rhs.mMax;
			mIndex = rhs.mIndex;
			return *this;
		}
		cyclic_index& operator =( const T& index )
		{
			set( index );
			return *this;
		}
		void set( const T& _index )
		{
			if( _index > mMax ) mIndex = ( _index - mMin ) % size() + mMin;
			else if( _index < mMin ) mIndex = mMax - ( ( mMax - _index ) % size() );
			else mIndex = _index;
		}
		operator T()
		{
			return mIndex;
		}
		operator const T() const
		{
			return mIndex;
		}
		cyclic_index& operator ++() 
		{
			mIndex = ( mIndex + 1 > mMax ) ? mMin : mIndex + 1;
			return *this;
		}
		cyclic_index operator ++( int )
		{
			cyclic_index other = *this;
			mIndex = ( mIndex + 1 > mMax ) ? mMin : mIndex + 1;
			return other;
		}
		cyclic_index& operator --()
		{
			if( mIndex == 0 )
				mIndex = mMax;
			else
				mIndex = ( mIndex - 1 < mMin ) ? mMax : mIndex - 1;
			return *this;
		}
		cyclic_index operator --( int )
		{
			cyclic_index other = *this;
			if( mIndex == 0 )
				mIndex = mMax;
			else
				mIndex = ( mIndex - 1 < mMin ) ? mMax : mIndex - 1;
			return other;
		}
		T size() const
		{
			return mMax - mMin + 1;
		}
		const T getMin() const 
		{
			return mMin;
		}
		const T getMax() const
		{
			return mMax;
		}
		const T getIndex() const
		{
			return mIndex;
		}
	private:
		T	mMin;
		T	mMax;
		T	mIndex;
	};

} // namespace xmod

#endif