
#pragma once


class	XHttpBuffer
{
public:

	explicit XHttpBuffer( size_t nMaxSize = 0 );
	~XHttpBuffer();

	void		Init( size_t nSize );

	void		Copy( const void* pData, size_t nSize );
	void		HadCopied( size_t nSize );

	const void*	Buffer()	const;
	void*		Buffer();
	size_t		Size()		const;
	size_t		MaxSize()	const;

	bool		Empty()		const;

	void		Flush( size_t nCount );
	void		TurnBack();

	void		Clear();

private:

	XHttpBuffer( const XHttpBuffer& other );
	XHttpBuffer&	operator=( const XHttpBuffer& other );

private:

	char*	m_pBuffer;
	size_t	m_nMaxSize;

	size_t	m_nDataSize;
	size_t	m_nCurrentPos;

};

