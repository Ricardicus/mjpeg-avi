
#ifdef __cplusplus
extern "C" {
#endif

typedef struct avi_file_idx_node {
  size_t len;
  struct avi_file_idx_node *next;
} avi_file_idx_node_t;

typedef struct avi_file_stream {
  size_t nbr_jpgs;
  size_t raw_data_len;
  avi_file_idx_node_t *indexer;
  avi_file_idx_node_t *last;
  FILE *file_ptr;
  unsigned long jpgs_width;
  unsigned long jpgs_height;
  unsigned long fps;
  char *filename;
} avi_file_stream_t;

// ====================================== API FUNCTIONS ======================================
// If you want to continously dump raw jpg data into an AVI file, use the stream functions.
// Workflow:
// avi_file_stream_t *stream = avi_file_stream_new("output.avi", "640x480", 30);
// // in a loop or something
//     avi_file_stream_write_jpg_data(stream, raw_jpg_data, raw_jpg_data_len);
//     // repeat
// // when finished
// avi_file_stream_finalize(stream);
// // free the stream
// avi_file_stream_free(stream);
avi_file_stream_t *avi_file_stream_new(const char *filename, const char *resolution,
                                       unsigned long fps);
void avi_file_stream_free(avi_file_stream_t *stream);
void avi_file_stream_write_jpg_data(avi_file_stream_t *stream, const char *data, size_t len);
void avi_file_stream_finalize(avi_file_stream_t *stream);

// If all files are found numberes in the folder with the path 'location' you can call
// this function directly, that compounds them into an AVI file.
void output_AVI_file(FILE *file_ptr, const char *resolution, const char *location,
                     unsigned long fps, unsigned long nbr_of_jpgs);
// ===========================================================================================

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
  WORD wPriority;
  WORD wLanguage;
  DWORD dwInitialFrames;
  DWORD dwScale;
  DWORD dwRate;
  DWORD dwStart;
  DWORD dwLength;
  DWORD dwSuggestedBufferSize;
  DWORD dwQuality;
  DWORD dwSampleSize;
  RECT rcFrame;
} AVIStreamHeader;

typedef struct {
  DWORD dwFourCC;
  DWORD dwSize;

  DWORD biSize;
  DWORD biWidth;
  DWORD biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  DWORD biXPelsPerMeter;
  DWORD biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagEXBMINFOHEADER {
  DWORD dwFourCC;
  DWORD dwSize;

  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
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



void fwrite_DWORD(FILE *, DWORD);
void fwrite_WORD(FILE *, WORD);

#ifdef __cplusplus
}
#endif
