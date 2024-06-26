#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "avi.h"
// AVI atoms

#ifdef __cplusplus
extern "C" {
#endif

#define AVIIF_KEYFRAME 0x00000010
// Tried to output directly through fputc ms ,file_ptr Will instead store header in file and then
// read and output it

FILE *_ffopen(const char *location, int image)
{
  char path[256];
  memset(path, '\0', sizeof(path));
  snprintf(path, sizeof(path), "%s/%d.jpg", location, image);
  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    // Attempt to open as jpeg
    memset(path, '\0', sizeof(path));
    snprintf(path, sizeof(path), "%s/%d.jpeg", location, image);
    fp = fopen(path, "r");
  }
  return fp;
}

void fwrite_DWORD(FILE *file_ptr, DWORD word)
{
  unsigned char *p;

  p = (unsigned char *)&word;
  int i;
  for (i = 0; i < 4; i++) {
    fputc(p[i], file_ptr);
  }
}

void fwrite_WORD(FILE *file_ptr, WORD word)
{
  unsigned char *p;

  p = (unsigned char *)&word;
  int i;
  for (i = 0; i < 2; i++) {
    fputc(p[i], file_ptr);
  }
}

unsigned long get_all_sizes(const char *location, unsigned long nbr_of_jpgs)
{
  FILE *fp;
  unsigned long count = 1;
  unsigned long sizes = 0;
  for (; count <= nbr_of_jpgs; count++) {
    unsigned long len;
    fp = _ffopen(location, count);
    if (!fp) {
      continue;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (len % 2) {
      len += 1;
    }

    sizes += len;

    fclose(fp);
  }

  return sizes;
}

void output_every_jpg_correctly(FILE *file_ptr, const char *location, unsigned long nbr_of_jpgs)
{
  FILE *fp;
  unsigned long count = 1;
  for (; count <= nbr_of_jpgs; count++) {
    unsigned long len;
    fp = _ffopen(location, count);
    if (!fp) {
      continue;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    CHUNK data;
    data.dwFourCC = '00db';
    fputc('0', file_ptr);
    fputc('0', file_ptr);
    fputc('d', file_ptr);
    fputc('c', file_ptr);

    data.dwSize = len + (len % 2 ? 1 : 0);
    fwrite_DWORD(file_ptr, data.dwSize);

    int c;
    while ((c = fgetc(fp)) != EOF) {
      fputc(c, file_ptr);
    }

    if (len % 2) {
      fputc('\0', file_ptr);
    }

    fclose(fp);
  }

  count = 1;

  fputc('i', file_ptr);
  fputc('d', file_ptr);
  fputc('x', file_ptr);
  fputc('1', file_ptr);
  unsigned long index_length = 4 * 4 * nbr_of_jpgs;
  fwrite_DWORD(file_ptr, index_length);

  unsigned long AVI_KEYFRAME = 16;

  unsigned long offset_count = 4;

  for (; count <= nbr_of_jpgs; count++) {
    unsigned long len;
    fp = _ffopen(location, count);
    if (!fp) {
      continue;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fclose(fp);
    if (len % 2) len += 1;

    fputc('0', file_ptr);
    fputc('0', file_ptr);
    fputc('d', file_ptr);
    fputc('c', file_ptr);
    fwrite_DWORD(file_ptr, AVI_KEYFRAME);
    fwrite_DWORD(file_ptr, offset_count);
    fwrite_DWORD(file_ptr, len);
    offset_count += len + 8;
  }
}

void _write_AVI_RIFF_header(FILE *file_ptr, DWORD len, unsigned long jpgs_width,
                            unsigned long jpgs_height, int fps, unsigned long nbr_of_jpgs,
                            int update_len_fields)
{
  RIFF RIFF_LIST;
  LIST hdrl;
  MainAVIHeader avih;
  LIST strl;
  AVIStreamHeader strh;
  EXBMINFOHEADER strf;
  LIST movi;

  if (update_len_fields) {
    if (fseek(file_ptr, 4, SEEK_SET) != 0) {
      return;
    }
    RIFF_LIST.dwSize = 256 + len + 8 * nbr_of_jpgs + 16 * nbr_of_jpgs;
    fwrite_DWORD(file_ptr, RIFF_LIST.dwSize);
    if (fseek(file_ptr, 48, SEEK_SET) != 0) {
      return;
    }
    avih.dwTotalFrames = nbr_of_jpgs;
    fwrite_DWORD(file_ptr, avih.dwTotalFrames);
    if (fseek(file_ptr, 140, SEEK_SET) != 0) {
      return;
    }
    strh.dwLength = nbr_of_jpgs; // +4 = 36
    fwrite_DWORD(file_ptr, strh.dwLength);
    if (fseek(file_ptr, 240, SEEK_SET) != 0) {
      return;
    }
    DWORD totalframes = nbr_of_jpgs;
    fwrite_DWORD(file_ptr, totalframes);
    if (fseek(file_ptr, 248, SEEK_SET) != 0) {
      return;
    }
    movi.dwSize = len + 4 + 8 * nbr_of_jpgs;
    fwrite_DWORD(file_ptr, movi.dwSize);
    if (fseek(file_ptr, 0, SEEK_END) != 0) {
      return;
    }
    return;
  }

  RIFF_LIST.dwRIFF = 'RIFF';
  fputc('R', file_ptr);
  fputc('I', file_ptr);
  fputc('F', file_ptr);
  fputc('F', file_ptr);

  // dwSize = magic number (don't ask me) + dataload + dataheaders (8 per jpeg) + idx frames ( 16
  // per jpeg )
  RIFF_LIST.dwSize = 256 + len + 8 * nbr_of_jpgs + 16 * nbr_of_jpgs;
  fwrite_DWORD(file_ptr, RIFF_LIST.dwSize);

  RIFF_LIST.dwFourCC = 'AVI ';
  fputc('A', file_ptr);
  fputc('V', file_ptr);
  fputc('I', file_ptr);
  fputc(' ', file_ptr);
  // 	RIFF_LIST.data = WAIT WITH THIS

  hdrl.dwList = 'LIST';
  fputc('L', file_ptr);
  fputc('I', file_ptr);
  fputc('S', file_ptr);
  fputc('T', file_ptr);
  hdrl.dwSize = 224;
  fwrite_DWORD(file_ptr, hdrl.dwSize);
  hdrl.dwFourCC = 'hdrl';
  fputc('h', file_ptr);
  fputc('d', file_ptr);
  fputc('r', file_ptr);
  fputc('l', file_ptr);

  avih.dwFourCC = 'avih';
  fputc('a', file_ptr);
  fputc('v', file_ptr);
  fputc('i', file_ptr);
  fputc('h', file_ptr);
  avih.dwSize = 56;
  fwrite_DWORD(file_ptr, avih.dwSize);

  avih.dwMicroSecPerFrame = 1000000 / fps;
  fwrite_DWORD(file_ptr, avih.dwMicroSecPerFrame);

  avih.dwMaxBytesPerSec = 7000;
  fwrite_DWORD(file_ptr, avih.dwMaxBytesPerSec);

  avih.dwPaddingGranularity = 0;
  fwrite_DWORD(file_ptr, avih.dwPaddingGranularity);

  // dwFlags set to 16, do not know why!
  avih.dwFlags = 16;
  fwrite_DWORD(file_ptr, avih.dwFlags);

  avih.dwTotalFrames = nbr_of_jpgs;
  fwrite_DWORD(file_ptr, avih.dwTotalFrames);

  avih.dwInitialFrames = 0;
  fwrite_DWORD(file_ptr, avih.dwInitialFrames);

  avih.dwStreams = 1;
  fwrite_DWORD(file_ptr, avih.dwStreams);

  avih.dwSuggestedBufferSize = 0;
  fwrite_DWORD(file_ptr, avih.dwSuggestedBufferSize);

  avih.dwWidth = jpgs_width;
  fwrite_DWORD(file_ptr, avih.dwWidth);

  avih.dwHeight = jpgs_height;
  fwrite_DWORD(file_ptr, avih.dwHeight);

  avih.dwReserved[0] = 0;
  fwrite_DWORD(file_ptr, avih.dwReserved[0]);
  avih.dwReserved[1] = 0;
  fwrite_DWORD(file_ptr, avih.dwReserved[1]);
  avih.dwReserved[2] = 0;
  fwrite_DWORD(file_ptr, avih.dwReserved[2]);
  avih.dwReserved[3] = 0;
  fwrite_DWORD(file_ptr, avih.dwReserved[3]);

  strl.dwList = 'LIST';
  fputc('L', file_ptr);
  fputc('I', file_ptr);
  fputc('S', file_ptr);
  fputc('T', file_ptr);
  strl.dwSize = 148;
  fwrite_DWORD(file_ptr, strl.dwSize);

  strl.dwFourCC = 'strl';
  fputc('s', file_ptr);
  fputc('t', file_ptr);
  fputc('r', file_ptr);
  fputc('l', file_ptr);

  strh.dwFourCC = 'strh';
  fputc('s', file_ptr);
  fputc('t', file_ptr);
  fputc('r', file_ptr);
  fputc('h', file_ptr);

  strh.dwSize = 64;
  fwrite_DWORD(file_ptr, strh.dwSize);
  strh.fccType = 'vids';
  fputc('v', file_ptr);
  fputc('i', file_ptr);
  fputc('d', file_ptr);
  fputc('s', file_ptr);
  strh.fccHandler = 'MJPG';
  fputc('M', file_ptr);
  fputc('J', file_ptr);
  fputc('P', file_ptr);
  fputc('G', file_ptr);
  strh.dwFlags = 0;
  fwrite_DWORD(file_ptr, strh.dwFlags);
  strh.wPriority = 0; // +2 = 14
  fwrite_WORD(file_ptr, strh.wPriority);
  strh.wLanguage = 0; // +2 = 16
  fwrite_WORD(file_ptr, strh.wLanguage);
  strh.dwInitialFrames = 0; // +4 = 20
  fwrite_DWORD(file_ptr, strh.dwInitialFrames);
  strh.dwScale = 1; // +4 = 24
  fwrite_DWORD(file_ptr, strh.dwScale);
  // insert FPS
  strh.dwRate = fps; // +4 = 28
  fwrite_DWORD(file_ptr, strh.dwRate);
  strh.dwStart = 0; // +4 = 32
  fwrite_DWORD(file_ptr, strh.dwStart);
  // insert nbr of jpegs
  strh.dwLength = nbr_of_jpgs; // +4 = 36
  fwrite_DWORD(file_ptr, strh.dwLength);

  strh.dwSuggestedBufferSize = 0; // +4 = 40
  fwrite_DWORD(file_ptr, strh.dwSuggestedBufferSize);
  strh.dwQuality = 0; // +4 = 44
  fwrite_DWORD(file_ptr, strh.dwQuality);
  // Specifies the size of a single sample of data.
  // This is set to zero if the samples can vary in size.
  // If this number is nonzero, then multiple samples of data
  // can be grouped into a single chunk within the file.
  // If it is zero, each sample of data (such as a video frame) must be in a separate chunk.
  // For video streams, this number is typically zero, although
  // it can be nonzero if all video frames are the same size.
  //
  strh.dwSampleSize = 0; // +4 = 48
  fwrite_DWORD(file_ptr, strh.dwSampleSize);
  strh.rcFrame.left = 0;
  fwrite_DWORD(file_ptr, strh.rcFrame.left);
  strh.rcFrame.top = 0;
  fwrite_DWORD(file_ptr, strh.rcFrame.top);
  strh.rcFrame.right = jpgs_width;
  fwrite_DWORD(file_ptr, strh.rcFrame.right);
  strh.rcFrame.bottom = jpgs_height;
  fwrite_DWORD(file_ptr, strh.rcFrame.bottom);

  strf.dwFourCC = 'strf';
  fputc('s', file_ptr);
  fputc('t', file_ptr);
  fputc('r', file_ptr);
  fputc('f', file_ptr);
  strf.dwSize = 40;
  fwrite_DWORD(file_ptr, strf.dwSize);

  strf.biSize = 40;
  fwrite_DWORD(file_ptr, strf.biSize);

  strf.biWidth = jpgs_width;
  fwrite_DWORD(file_ptr, strf.biWidth);
  strf.biHeight = jpgs_height;
  fwrite_DWORD(file_ptr, strf.biHeight);
  strf.biPlanes = 1;
  fwrite_WORD(file_ptr, strf.biPlanes);
  strf.biBitCount = 24;
  fwrite_WORD(file_ptr, strf.biBitCount);
  strf.biCompression = 'MJPG';
  fputc('M', file_ptr);
  fputc('J', file_ptr);
  fputc('P', file_ptr);
  fputc('G', file_ptr);

  strf.biSizeImage = ((strf.biWidth * strf.biBitCount / 8 + 3) & 0xFFFFFFFC) * strf.biHeight;
  fwrite_DWORD(file_ptr, strf.biSizeImage);
  strf.biXPelsPerMeter = 0;
  fwrite_DWORD(file_ptr, strf.biXPelsPerMeter);
  strf.biYPelsPerMeter = 0;
  fwrite_DWORD(file_ptr, strf.biYPelsPerMeter);
  strf.biClrUsed = 0;
  fwrite_DWORD(file_ptr, strf.biClrUsed);
  strf.biClrImportant = 0;
  fwrite_DWORD(file_ptr, strf.biClrImportant);

  fputc('L', file_ptr);
  fputc('I', file_ptr);
  fputc('S', file_ptr);
  fputc('T', file_ptr);

  DWORD ddww = 16;
  fwrite_DWORD(file_ptr, ddww);
  fputc('o', file_ptr);
  fputc('d', file_ptr);
  fputc('m', file_ptr);
  fputc('l', file_ptr);

  fputc('d', file_ptr);
  fputc('m', file_ptr);
  fputc('l', file_ptr);
  fputc('h', file_ptr);

  DWORD szs = 4;
  fwrite_DWORD(file_ptr, szs);

  // nbr of jpgs
  DWORD totalframes = nbr_of_jpgs;
  fwrite_DWORD(file_ptr, totalframes);

  movi.dwList = 'LIST';
  fputc('L', file_ptr);
  fputc('I', file_ptr);
  fputc('S', file_ptr);
  fputc('T', file_ptr);

  movi.dwSize = len + 4 + 8 * nbr_of_jpgs;
  fwrite_DWORD(file_ptr, movi.dwSize);
  movi.dwFourCC = 'movi';
  fputc('m', file_ptr);
  fputc('o', file_ptr);
  fputc('v', file_ptr);
  fputc('i', file_ptr);
}

void output_AVI_file(FILE *file_ptr, const char *resolution, const char *location,
                     unsigned long fps, unsigned long nbr_of_jpgs)
{
  // I need to know the nbr of jpg files, 'nbr_of_jpgs', and their respective
  // size in bytes to be able to construct this AVI file
  FILE *fp;

  fp = _ffopen(location, 1);
  assert(fp != NULL);
  fseek(fp, 0, SEEK_END);
  fclose(fp);

  // Size found!

  // I also need the width and the height of the jpgs.
  // These will be stored in the variables 'jpgs_width' and 'jpgs_height'

  DWORD jpgs_width;
  DWORD jpgs_height;

  char jpg_resolution[20];
  memset(jpg_resolution, '\0', 20);
  strcpy(jpg_resolution, resolution);

  char *cptr;
  cptr = jpg_resolution;

  // .. resolution is given on the form [width]x[height]
  while (*cptr != 'x') {
    cptr++;
  }
  *cptr = '\0';
  cptr++;

  jpgs_width = (unsigned long)atol(jpg_resolution);
  jpgs_height = (unsigned long)atol(cptr);

  DWORD len = get_all_sizes(location, nbr_of_jpgs);

  _write_AVI_RIFF_header(file_ptr, len, jpgs_width, jpgs_height, fps, nbr_of_jpgs, 0);

  output_every_jpg_correctly(file_ptr, location, nbr_of_jpgs);
}


avi_file_stream_t *avi_file_stream_new(const char *filename, const char *resolution,
                                       unsigned long fps)
{
  char resolutionBuf[20];
  char filenameBufTmp[256];
  DWORD len;
  char *cptr;
  avi_file_stream_t *stream = (avi_file_stream_t *)malloc(sizeof(avi_file_stream_t));
  if (stream == NULL) {
    return NULL;
  }
  stream->nbr_jpgs = 0;
  stream->filename = calloc(strlen(filename) + 1, sizeof(char));
  if (stream->filename == NULL) {
    free(stream);
    return NULL;
  }
  snprintf(stream->filename, strlen(filename) + 1, "%s", filename);
  snprintf(filenameBufTmp, sizeof(filenameBufTmp), "%s.tmp", filename);
  stream->raw_data_len = 0;
  stream->fps = fps;
  FILE *file_ptr = fopen(filenameBufTmp, "wb");
  if (file_ptr == NULL) {
    free(stream);
    return NULL;
  }
  stream->file_ptr = file_ptr;
  stream->indexer = NULL;
  stream->last = NULL;
  len = 0;

  snprintf(resolutionBuf, sizeof(resolutionBuf), "%s", resolution);

  cptr = resolutionBuf;
  // .. resolution is given on the form [width]x[height]
  while (*cptr != 'x') {
    cptr++;
  }
  *cptr = '\0';
  cptr++;

  stream->jpgs_width = (unsigned long)atol(resolutionBuf);
  stream->jpgs_height = (unsigned long)atol(cptr);

  _write_AVI_RIFF_header(file_ptr, 0, stream->jpgs_width, stream->jpgs_height, fps, 0, 0);
  return stream;
}

void avi_file_stream_free(avi_file_stream_t *stream)
{
  avi_file_idx_node_t *node = stream->indexer;
  while (node != NULL) {
    avi_file_idx_node_t *next = node->next;
    free(node);
    node = next;
  }
  free(stream->filename);
  free(stream);
}

void avi_file_stream_write_jpg_data(avi_file_stream_t *stream, const char *data_buf,
                                    size_t data_len)
{
  avi_file_idx_node_t *node = (avi_file_idx_node_t *)malloc(sizeof(avi_file_idx_node_t));
  if (node == NULL) {
    return;
  }
  unsigned long len = data_len;
  FILE *file_ptr = stream->file_ptr;

  CHUNK data;
  data.dwFourCC = '00db';
  fputc('0', file_ptr);
  fputc('0', file_ptr);
  fputc('d', file_ptr);
  fputc('c', file_ptr);

  data.dwSize = len + (len % 2 ? 1 : 0);
  fwrite_DWORD(file_ptr, data.dwSize);

  unsigned long i = 0;
  while (i < len) {
    int c = data_buf[i];
    fputc(c, file_ptr);
    i++;
  }

  if (len % 2) {
    fputc('\0', file_ptr);
  }

  node->next = NULL;
  node->len = data.dwSize;
  if (stream->last == NULL) {
    stream->indexer = node;
    stream->last = node;
  }
  else {
    stream->last->next = node;
    stream->last = node;
  }
  stream->nbr_jpgs++;
  stream->raw_data_len += node->len;
  _write_AVI_RIFF_header(file_ptr, stream->raw_data_len, stream->jpgs_width, stream->jpgs_height,
                         stream->fps, stream->nbr_jpgs, 1);
}

void avi_file_stream_finalize(avi_file_stream_t *stream)
{
  unsigned int count = 1;
  char filenameBuf[256];
  FILE *file_ptr = stream->file_ptr;
  unsigned long nbr_of_jpgs = stream->nbr_jpgs;

  snprintf(filenameBuf, sizeof(filenameBuf), "%s.tmp", stream->filename);

  fputc('i', file_ptr);
  fputc('d', file_ptr);
  fputc('x', file_ptr);
  fputc('1', file_ptr);
  unsigned long index_length = 4 * 4 * nbr_of_jpgs;
  fwrite_DWORD(file_ptr, index_length);

  unsigned long AVI_KEYFRAME = 16;

  unsigned long offset_count = 4;

  avi_file_idx_node_t *node = stream->indexer;
  while (node != NULL) {
    unsigned long len = node->len;
    node = node->next;
    fputc('0', file_ptr);
    fputc('0', file_ptr);
    fputc('d', file_ptr);
    fputc('c', file_ptr);
    fwrite_DWORD(file_ptr, AVI_KEYFRAME);
    fwrite_DWORD(file_ptr, offset_count);
    fwrite_DWORD(file_ptr, len);
    offset_count += len + 8;
    count++;
  }
  fclose(file_ptr);
  // move the file filenameBuf to stream->filename
  rename(filenameBuf, stream->filename);
}

#ifdef __cplusplus
}
#endif
