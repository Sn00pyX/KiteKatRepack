
#include <windows.h>
#include <algorithm>

#include "../../include/sound/XWave.h"

//-----------------------------------------------------------------------------
// Name: RIFFHEADER
// Desc: For parsing WAV files
//-----------------------------------------------------------------------------
struct RIFFHEADER
{
    FOURCC  fccChunkId;
    DWORD   dwDataSize;
};

#define RIFFCHUNK_FLAGS_VALID   0x00000001

//-----------------------------------------------------------------------------
// Name: class CRiffChunk
// Desc: RIFF chunk utility class
//-----------------------------------------------------------------------------
class CRiffChunk
{
    FOURCC            m_fccChunkId;       // Chunk identifier
    const CRiffChunk* m_pParentChunk;     // Parent chunk
    XStreamReader*    m_pStreamReader;
    DWORD             m_dwDataOffset;     // Chunk data offset
    DWORD             m_dwDataSize;       // Chunk data size
    DWORD             m_dwFlags;          // Chunk flags

public:
    CRiffChunk();

    // Initialization
    VOID    Initialize( FOURCC fccChunkId, const CRiffChunk* pParentChunk,
                        XStreamReader *pStreamReader );
    HRESULT Open();
    BOOL    IsValid()     { return !!(m_dwFlags & RIFFCHUNK_FLAGS_VALID); }

    // Data
    HRESULT ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize );

    // Chunk information
    FOURCC  GetChunkId()  { return m_fccChunkId; }
    DWORD   GetDataSize() { return m_dwDataSize; }
};




//-----------------------------------------------------------------------------
// Name: class CWaveFile
// Desc: Wave file utility class
//-----------------------------------------------------------------------------
class CWaveFile
{
    XStreamReader *m_pStreamReader;            // File handle
    CRiffChunk  m_RiffChunk;        // RIFF chunk
    CRiffChunk  m_FormatChunk;      // Format chunk
    CRiffChunk  m_DataChunk;        // Data chunk
    CRiffChunk  m_WaveSampleChunk;  // Wave Sample chunk
    
public:
    CWaveFile();
    ~CWaveFile();

    // Initialization
    HRESULT Open( XStreamReader *pStreamReader );
    VOID    Close();

    // File format
    HRESULT GetFormat( WAVEFORMATEX* pwfxFormat, DWORD dwFormatSize, DWORD *pdwRequiredSize = NULL );

    // File data
    HRESULT ReadSample( DWORD dwPosition, VOID* pBuffer, DWORD dwBufferSize, 
                        DWORD* pdwRead );

    // Loop region
    HRESULT GetLoopRegion( DWORD* pdwStart, DWORD* pdwLength );

	// File properties
    VOID    GetDuration( DWORD* pdwDuration ) { *pdwDuration = m_DataChunk.GetDataSize(); }
};


XWave::XWave( XStreamReader *pStreamReader )
{
	m_pStream = pStreamReader;
	m_bIsMyStream = false;

	init();
}

XWave::XWave( const char* szFileName )
{	
	m_pStream = new XFileStreamReader( szFileName );
	m_bIsMyStream = true;

	init();
}

void XWave::init()
{
	memset( &m_WaveFormat, 0, sizeof(m_WaveFormat) );
	m_unSize = 0;
	m_pWaveFile = new CWaveFile();
}


XWave::~XWave()
{
	if( m_pWaveFile ) delete m_pWaveFile;
	if( m_bIsMyStream ) delete m_pStream;
}

bool XWave::ReadWaveInfo()
{
	if( FAILED( m_pWaveFile->Open( m_pStream ) ) ) return false;

	if( FAILED( m_pWaveFile->GetFormat( &m_WaveFormat, sizeof(m_WaveFormat) ) ) ) return false;

	DWORD duration = 0;
	m_pWaveFile->GetDuration( &duration );
	m_unSize = duration;

	return true;
}

bool XWave::ReadWaveData( void * p, unsigned len )
{
	DWORD read = 0;
	if( FAILED( m_pWaveFile->ReadSample( 0, p, len, &read ) ) ) return false;

	return true;
}

bool XWave::IsStereo()
{
	return ( m_WaveFormat.nChannels > 1 );
}










const DWORD FOURCC_WAVE   = 'EVAW';
const DWORD FOURCC_FORMAT = ' tmf';
const DWORD FOURCC_DATA   = 'atad';
const DWORD FOURCC_WSMP   = 'pmsw';

//-----------------------------------------------------------------------------
// Name: struct WAVESAMPE
// Desc: RIFF chunk type that contains loop point information
//-----------------------------------------------------------------------------
struct WAVESAMPLE
{
    ULONG   cbSize;
    USHORT  usUnityNote;
    SHORT   sFineTune;
    LONG    lGain;
    ULONG   ulOptions;
    ULONG   cSampleLoops;
};

//-----------------------------------------------------------------------------
// Name: struct WAVESAMPLE_LOOP
// Desc: Loop point (contained in WSMP chunk)
//-----------------------------------------------------------------------------
struct WAVESAMPLE_LOOP
{
    ULONG cbSize;
    ULONG ulLoopType;
    ULONG ulLoopStart;
    ULONG ulLoopLength;
};




//-----------------------------------------------------------------------------
// Name: CRiffChunk()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CRiffChunk::CRiffChunk()
{
    // Initialize defaults
    m_fccChunkId   = 0;
    m_pParentChunk = NULL;
	m_pStreamReader= NULL;
    m_dwDataOffset = 0;
    m_dwDataSize   = 0;
    m_dwFlags      = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the object
//-----------------------------------------------------------------------------
VOID CRiffChunk::Initialize( FOURCC fccChunkId, const CRiffChunk* pParentChunk, 
                             XStreamReader *pStreamReader )
{
    m_fccChunkId   = fccChunkId;
    m_pParentChunk = pParentChunk;
    m_pStreamReader= pStreamReader;
}




//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Opens an existing chunk.
//-----------------------------------------------------------------------------
HRESULT CRiffChunk::Open()
{
	if( !m_pStreamReader ) return S_FALSE;

    RIFFHEADER rhRiffHeader;
    LONG       lOffset = 0;

    // Seek to the first byte of the parent chunk's data section
    if( m_pParentChunk )
    {
        lOffset = m_pParentChunk->m_dwDataOffset;

        // Special case the RIFF chunk
        if( FOURCC_RIFF == m_pParentChunk->m_fccChunkId )
            lOffset += sizeof(FOURCC);
    }
    
    // Read each child chunk header until we find the one we're looking for
    for( ;; )
    {	
        if( !m_pStreamReader->Seek( lOffset, SEEK_SET ) ) return E_FAIL;

        size_t dwRead = m_pStreamReader->Read( &rhRiffHeader, sizeof(rhRiffHeader) );
		if( dwRead != sizeof(rhRiffHeader) )
            return E_FAIL;

        // Hit EOF without finding it
        if( 0 == dwRead )
            return E_FAIL;

        // Check if we found the one we're looking for
        if( m_fccChunkId == rhRiffHeader.fccChunkId )
        {
            // Save the chunk size and data offset
            m_dwDataOffset = lOffset + sizeof(rhRiffHeader);
            m_dwDataSize   = rhRiffHeader.dwDataSize;

            // Success
            m_dwFlags |= RIFFCHUNK_FLAGS_VALID;

            return S_OK;
        }

        lOffset += sizeof(rhRiffHeader) + rhRiffHeader.dwDataSize;
    }
}




//-----------------------------------------------------------------------------
// Name: Read()
// Desc: Reads from the file
//-----------------------------------------------------------------------------
HRESULT CRiffChunk::ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize )
{
	if( !m_pStreamReader ) return S_FALSE;

    // Seek to the offset
	if( !m_pStreamReader->Seek( m_dwDataOffset+lOffset, SEEK_SET ) ) return E_FAIL;

    // Read from the file
	size_t dwRead = m_pStreamReader->Read( pData, dwDataSize );
	if( dwRead != dwDataSize ) return E_FAIL;

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile()
// Desc: Object constructor.
//-----------------------------------------------------------------------------
CWaveFile::CWaveFile()
{
    m_pStreamReader = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CWaveFile()
// Desc: Object destructor.
//-----------------------------------------------------------------------------
CWaveFile::~CWaveFile()
{
    Close();
}




//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Initializes the object.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::Open( XStreamReader *pStreamReader )
{
    // If we're already open, close
    Close();
    
    // Open the file
	m_pStreamReader = pStreamReader;
	if( !m_pStreamReader ) return S_FALSE;

    // Initialize the chunk objects
    m_RiffChunk.Initialize( FOURCC_RIFF, NULL, m_pStreamReader );
    m_FormatChunk.Initialize( FOURCC_FORMAT, &m_RiffChunk, m_pStreamReader );
    m_DataChunk.Initialize( FOURCC_DATA, &m_RiffChunk, m_pStreamReader );
    m_WaveSampleChunk.Initialize( FOURCC_WSMP, &m_RiffChunk, m_pStreamReader );

    HRESULT hr = m_RiffChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_FormatChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_DataChunk.Open();
    if( FAILED(hr) )
        return hr;

    // Wave Sample chunk is not required
    m_WaveSampleChunk.Open();

    // Validate the file type
    FOURCC fccType;
    hr = m_RiffChunk.ReadData( 0, &fccType, sizeof(fccType) );
    if( FAILED(hr) )
        return hr;

    if( FOURCC_WAVE != fccType )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetFormat()
// Desc: Gets the wave file format
//-----------------------------------------------------------------------------
HRESULT CWaveFile::GetFormat( WAVEFORMATEX* pwfxFormat, DWORD dwFormatSize, DWORD * pdwRequiredSize )
{
    HRESULT hr = S_OK;
    DWORD dwValidSize = m_FormatChunk.GetDataSize();

    // We should be reading a wave format and/or
    // telling the caller the size of the format
    if( ( NULL == pwfxFormat ||
          0 == dwFormatSize ) && 
        NULL == pdwRequiredSize )
        return E_INVALIDARG;

    if( pwfxFormat && dwFormatSize )
    {
        // Read the format chunk into the buffer
		hr = m_FormatChunk.ReadData( 0, pwfxFormat, (std::min)(dwFormatSize, dwValidSize) );
        if( FAILED(hr) )
            return hr;

        // Zero out remaining bytes, in case enough bytes were not read
        if( dwFormatSize > dwValidSize )
            ZeroMemory( (BYTE*)pwfxFormat + dwValidSize, dwFormatSize - dwValidSize );
    }
    
    // Tell caller how much space they need for the format
    if( pdwRequiredSize )
    {
		*pdwRequiredSize = (std::max)( dwValidSize, (DWORD)sizeof( *pwfxFormat ) );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadSample()
// Desc: Reads data from the audio file.
//-----------------------------------------------------------------------------
HRESULT CWaveFile::ReadSample( DWORD dwPosition, VOID* pBuffer, 
                               DWORD dwBufferSize, DWORD* pdwRead )
{                                   
    // Don't read past the end of the data chunk
    DWORD dwDuration;
    GetDuration( &dwDuration );

    if( dwPosition + dwBufferSize > dwDuration )
        dwBufferSize = dwDuration - dwPosition;

    HRESULT hr = S_OK;
    if( dwBufferSize )
        hr = m_DataChunk.ReadData( (LONG)dwPosition, pBuffer, dwBufferSize );

    if( pdwRead )
        *pdwRead = dwBufferSize;

    return hr;
}




//-----------------------------------------------------------------------------
// Name: GetLoopRegion
// Desc: Gets the loop region, in terms of samples
//-----------------------------------------------------------------------------
HRESULT CWaveFile::GetLoopRegion( DWORD *pdwStart, DWORD *pdwLength )
{
    HRESULT hr = S_OK;
    WAVESAMPLE ws;
    WAVESAMPLE_LOOP wsl;

    // Check arguments
    if( NULL == pdwStart || NULL == pdwLength )
        return E_INVALIDARG;

    // Check to see if there was a wave sample chunk
    if( !m_WaveSampleChunk.IsValid() )
        return E_FAIL;
    
    // Read the WAVESAMPLE struct from the chunk
    hr = m_WaveSampleChunk.ReadData( 0, &ws, sizeof( ws ) );
    if( FAILED( hr ) )
        return hr;

    // Currently, only 1 loop region is supported
    if( ws.cSampleLoops != 1 )
        return E_FAIL;

    // Read the loop region
    hr = m_WaveSampleChunk.ReadData( ws.cbSize, &wsl, sizeof( wsl ) );
    if( FAILED( hr ) )
        return hr;
    
    // Fill output vars with the loop region
    *pdwStart = wsl.ulLoopStart;
    *pdwLength = wsl.ulLoopLength;

    return S_OK;
}
    



//-----------------------------------------------------------------------------
// Name: Close()
// Desc: Closes the object
//-----------------------------------------------------------------------------
VOID CWaveFile::Close()
{    
}
