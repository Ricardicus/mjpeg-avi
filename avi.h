

typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned char BYTE;

// AVI atoms
typedef struct {
	DWORD dwRIFF;
	DWORD dwSize;
	DWORD dwFourCC;
} RIFF;

typedef struct {
	DWORD dwFourCC;
	DWORD dwSize;
	//	BYTE* data; // dwSize in length
} CHUNK;

typedef struct {
	DWORD dwList;
	DWORD dwSize;
	DWORD dwFourCC;
	//	BYTE* data; // dwSize - 4 in length
} LIST;

typedef struct {
	DWORD dwFourCC;
	DWORD dwSize;

	DWORD dwMicroSecPerFrame;
	DWORD dwMaxBytesPerSec;
	DWORD dwPaddingGranularity;

	DWORD dwFlags;
	DWORD dwTotalFrames;
	DWORD dwInitialFrames;
	DWORD dwStreams;
	DWORD dwSuggestedBufferSize;

	DWORD dwWidth;
	DWORD dwHeight;

	DWORD dwReserved[4];
} MainAVIHeader;

typedef struct _RECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT;

typedef struct {
	DWORD dwFourCC;
	DWORD dwSize;

	DWORD fccType;
	DWORD fccHandler;
	DWORD dwFlags;
	WORD  wPriority;
	WORD  wLanguage;
	DWORD dwInitialFrames;
	DWORD dwScale;
	DWORD dwRate;
	DWORD dwStart;
	DWORD dwLength;
	DWORD dwSuggestedBufferSize;
	DWORD dwQuality;
	DWORD dwSampleSize;

} AVIStreamHeader;

typedef struct {
	DWORD dwFourCC;
	DWORD dwSize;

	DWORD biSize;
	DWORD  biWidth;
	DWORD  biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD  biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagEXBMINFOHEADER {
	DWORD dwFourCC;
	DWORD dwSize;

	DWORD biSize;
	LONG  biWidth;
	LONG  biHeight;
	WORD  biPlanes;
	WORD  biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG  biXPelsPerMeter;
	LONG  biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;

} EXBMINFOHEADER;

typedef struct {
	DWORD ckid;
	DWORD dwFlags;
	DWORD dwChunkOffset;
	DWORD dwChunkLength;
} AVIINDEXENTRY;

typedef struct {
	DWORD fcc;
	DWORD cd;
	WORD wLongsPerEntry;
	char bIndexSubType;
	char bIndexType;
	DWORD nEntriesInUse;
	DWORD dwChunkId;
	DWORD dwReserved[3];
	AVIINDEXENTRY axiindex_entry;
} AVIINDEXCHUNK;

typedef struct {
	DWORD name;
	DWORD dwSize;
	DWORD dwTotalFrames;
} ODMLExtendedAVIheader;

typedef struct {
	RIFF riff_AVI;
	LIST hdrl;
	MainAVIHeader avih;
	LIST strl;
	AVIStreamHeader strh;
	EXBMINFOHEADER strf;
	LIST odml;
	ODMLExtendedAVIheader dmlh;
	LIST movi;
	CHUNK movi_data;
} avi_file;

/* IDEA:
*
* make:		RIFF 'AVI '
*			LIST hdrl
- avih
- LIST strl
- strh
- strf
LIST movi
*/


void output_AVI_file(FILE *, char*, char *, char*, unsigned long, unsigned long);

void output_every_jpg_in(FILE *, char*, char*, unsigned long);

void fwrite_DWORD(FILE *, DWORD);

void fwrite_WORD(FILE *, WORD);
