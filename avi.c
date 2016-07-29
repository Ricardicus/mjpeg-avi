#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "avi.h"
// AVI atoms

#define AVIIF_KEYFRAME 0x00000010
// Tried to output directly through fputc ms ,file_ptr Will instead store header in file and then read and output it

void 
fwrite_DWORD(FILE * file_ptr, DWORD word){
	unsigned char * p;

	p = (unsigned char *)&word;
	int i;
	for(i = 0; i<4;i++){
		fputc(p[i],file_ptr);
	}

}

void 
fwrite_WORD(FILE * file_ptr, WORD word){
	unsigned char * p;

	p = (unsigned char*)&word;
	int i;
	for(i = 0; i<2;i++){
		fputc(p[i],file_ptr);
	}

}

unsigned long
get_all_sizes(char * location, unsigned long nbr_of_jpgs){
	chdir(location);

	FILE * fp; char filename[128]; unsigned long count = 1;
	unsigned long sizes = 0;
	for(;count<=nbr_of_jpgs;count++){
		
		sprintf(filename, "echo \"get all sizes: nbrjpgs: %lu, count: %lu \" >> info.info", nbr_of_jpgs, count);
		system(filename);

		unsigned long len;
		memset(filename, '\0',128);
		sprintf(filename, "%lu.jpeg", count);
		fp = fopen(filename, "r");
		if(!fp){
			system("echo \"could not open .jpeg\" > avigarage_error.info");			
			continue;
		}
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET); 

		if(len%2) len+=1;

		sizes+=len;

  		fclose(fp);

	}

	sprintf(filename, "echo \"returning size: %lu\" >> info.info", sizes);
	system(filename);

	return sizes;
}

void
output_every_jpg_correctly(FILE * file_ptr, char * location, char * id, unsigned long nbr_of_jpgs){

	chdir("jpgs");

	FILE * fp; char filename[128]; unsigned long count = 1;
	for(;count<=nbr_of_jpgs;count++){
		
		unsigned long len;
		memset(filename, '\0',128);
		sprintf(filename, "%lu.jpeg", count);
		fp = fopen(filename, "r");
		if(!fp){
			system("echo \"could not open .jpeg\" > avigarage_error.info");			
			continue;
		}
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET); 


		CHUNK data;
		data.dwFourCC = '00db';
		fputc('0',file_ptr); 
		fputc('0',file_ptr); 
		fputc('d',file_ptr); 
		fputc('b',file_ptr);

		data.dwSize = len;
		fwrite_DWORD(file_ptr, data.dwSize);

		int c;
		while((c = fgetc(fp)) != EOF){
			fputc(c,file_ptr);
		}

		if(len%2) {
			fputc('\0',file_ptr);
		}

  		fclose(fp);

	}

	count = 1;

	fputc('i', file_ptr); fputc('d', file_ptr); fputc('x', file_ptr); fputc('1', file_ptr);
	unsigned long index_length = 4*4*nbr_of_jpgs;
	fwrite_DWORD(file_ptr, index_length);

	unsigned long AVI_KEYFRAME = 16;

	unsigned long offset_count = 4;

	for(;count<=nbr_of_jpgs;count++){

		unsigned long len;
		memset(filename, '\0',128);
		sprintf(filename, "%lu.jpeg", count);
		fp = fopen(filename, "r");
		if(!fp){
			system("echo \"could not open .jpeg\" > avigarage_error.info");			
			continue;
		}
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET); 	
		fclose(fp);
		if(len%2) len+=1;

		fputc('0', file_ptr); fputc('0', file_ptr); fputc('d', file_ptr); fputc('b', file_ptr);
		fwrite_DWORD(file_ptr, AVI_KEYFRAME);
		fwrite_DWORD(file_ptr, offset_count);
		fwrite_DWORD(file_ptr, len);
		offset_count+=len+8;
	}
}


void
output_AVI_file(FILE * file_ptr, char * id, char * resolution, char * location, unsigned long fps, unsigned long nbr_of_jpgs){

	// I need to know the nbr of jpg files, 'nbr_of_jpgs', and their respective
	// size in bytes to be able to construct this AVI file
	char path[128];
	memset(path,'\0',128);
	sprintf(path, "%s",location);
	chdir(path);

	FILE * fp;

	fp = fopen("1.jpeg", "r");
	fseek(fp, 0, SEEK_END);
	DWORD jpg_size = (unsigned long) ftell(fp);

	fclose(fp);

	// Size found! 

	// I also need the width and the height of the jpgs.
	// These will be stored in the variables 'jpgs_width' and 'jpgs_height'

	DWORD jpgs_width;
	DWORD jpgs_height;

	char jpg_resolution[20];
	memset(jpg_resolution, '\0', 20);
	strcpy(jpg_resolution, resolution);

	char * cptr;
	cptr = jpg_resolution;

	// .. resolution is given on the form [width]x[height] 
	while(*cptr != 'x'){
		cptr++;
	}
	*cptr = '\0';
	cptr++;

	jpgs_width = (unsigned long) atol(jpg_resolution);
	jpgs_height = (unsigned long) atol(cptr);

	DWORD len = get_all_sizes(location, nbr_of_jpgs);

	RIFF RIFF_LIST;

	RIFF_LIST.dwRIFF = 'RIFF';
	int cc = 'R';
	fputc('R',file_ptr); 
	fputc('I',file_ptr); 
	fputc('F',file_ptr); 
	fputc('F',file_ptr); 

	RIFF_LIST.dwSize =  150 + 12 + len + 8*nbr_of_jpgs + 8 + 4*4*nbr_of_jpgs;
	fwrite_DWORD(file_ptr, RIFF_LIST.dwSize);

	RIFF_LIST.dwFourCC = 'AVI ';
	fputc('A',file_ptr); 
	fputc('V',file_ptr); 
	fputc('I',file_ptr); 
	fputc(' ',file_ptr); 
	// 	RIFF_LIST.data = WAIT WITH THIS

	LIST hdrl;
	hdrl.dwList = 'LIST';
	fputc('L',file_ptr); 
	fputc('I',file_ptr); 
	fputc('S',file_ptr); 
	fputc('T',file_ptr); 
	hdrl.dwSize = 208;
	fwrite_DWORD(file_ptr, hdrl.dwSize);
	hdrl.dwFourCC = 'hdrl';
	fputc('h',file_ptr); 
	fputc('d',file_ptr); 
	fputc('r',file_ptr); 
	fputc('l',file_ptr); 

	MainAVIHeader avih;

	avih.dwFourCC = 'avih';
	fputc('a',file_ptr); 
	fputc('v',file_ptr); 
	fputc('i',file_ptr); 
	fputc('h',file_ptr); 
	avih.dwSize = 56;
	fwrite_DWORD(file_ptr, avih.dwSize);

	avih.dwMicroSecPerFrame = 1000000/fps;
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

	LIST strl;
	strl.dwList = 'LIST';
	fputc('L',file_ptr); 
	fputc('I',file_ptr); 
	fputc('S',file_ptr); 
	fputc('T',file_ptr); 
	strl.dwSize = 132;
	fwrite_DWORD(file_ptr, strl.dwSize);

	strl.dwFourCC = 'strl';
	fputc('s',file_ptr); 
	fputc('t',file_ptr); 
	fputc('r',file_ptr); 
	fputc('l',file_ptr); 

	AVIStreamHeader strh;
	strh.dwFourCC = 'strh';
	fputc('s',file_ptr); 
	fputc('t',file_ptr); 
	fputc('r',file_ptr); 
	fputc('h',file_ptr); 

	strh.dwSize = 48;
	fwrite_DWORD(file_ptr, strh.dwSize);
	strh.fccType = 'vids';
	fputc('v',file_ptr); 
	fputc('i',file_ptr); 
	fputc('d',file_ptr); 
	fputc('s',file_ptr); 
	strh.fccHandler = 'MJPG';
	fputc('M',file_ptr); 
	fputc('J',file_ptr); 
	fputc('P',file_ptr); 
	fputc('G',file_ptr); 
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

	EXBMINFOHEADER strf;

	strf.dwFourCC = 'strf';
	fputc('s',file_ptr); 
	fputc('t',file_ptr); 
	fputc('r',file_ptr); 
	fputc('f',file_ptr); 
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
	fputc('M',file_ptr); 
	fputc('J',file_ptr); 
	fputc('P',file_ptr); 
	fputc('G',file_ptr); 

	strf.biSizeImage = ((strf.biWidth*strf.biBitCount/8 + 3)&0xFFFFFFFC)*strf.biHeight;
	fwrite_DWORD(file_ptr, strf.biSizeImage);
	strf.biXPelsPerMeter = 0;
	fwrite_DWORD(file_ptr, strf.biXPelsPerMeter);
	strf.biYPelsPerMeter = 0;
	fwrite_DWORD(file_ptr, strf.biYPelsPerMeter);
	strf.biClrUsed = 0;
	fwrite_DWORD(file_ptr, strf.biClrUsed);
	strf.biClrImportant = 0;
	fwrite_DWORD(file_ptr, strf.biClrImportant);

	fputc('L',file_ptr); 
	fputc('I',file_ptr); 
	fputc('S',file_ptr); 
	fputc('T',file_ptr); 

	DWORD ddww = 16;
	fwrite_DWORD(file_ptr, ddww);
	fputc('o',file_ptr); 
	fputc('d',file_ptr); 
	fputc('m',file_ptr); 
	fputc('l',file_ptr); 

	fputc('d',file_ptr); 
	fputc('m',file_ptr); 
	fputc('l',file_ptr); 
	fputc('h',file_ptr); 

	DWORD szs = 4;
	fwrite_DWORD(file_ptr, szs);

	// nbr of jpgs
	DWORD totalframes = nbr_of_jpgs;
	fwrite_DWORD(file_ptr, totalframes);

	LIST movi;
	movi.dwList = 'LIST';
	fputc('L',file_ptr); 
	fputc('I',file_ptr); 
	fputc('S',file_ptr); 
	fputc('T',file_ptr); 

	movi.dwSize = len + 4 + 8*nbr_of_jpgs;
	fwrite_DWORD(file_ptr, movi.dwSize);
	movi.dwFourCC = 'movi';
	fputc('m',file_ptr); 
	fputc('o',file_ptr); 
	fputc('v',file_ptr); 
	fputc('i',file_ptr); 

	output_every_jpg_correctly(file_ptr, location, id, nbr_of_jpgs);

}

int 
main(int argc, char * argv[])
{
	char * resolution = "800x600";
	char * location = "jpgs";
	char * fps = "10";
	char * nbrjpgs = "5";

	int i = 0;
	while(i<argc){
		if(argv[i][0] == '-'){
			if(i+1>=argc) goto error;
			switch(argv[i][1]){
				case 'r':
					resolution = argv[i+1];
					break;
				case 'l':
					location = argv[i+1];
					break;
				case 's':
					fps = argv[i+1];
					break;
				case 'n':
					nbrjpgs = argv[i+1];
					break;
				default:
					break;
			}
		}
		i++;
	}

	FILE * fp = fopen("Video.avi", "w");
	output_AVI_file(fp, NULL, resolution, location, (unsigned long) atol(fps), (unsigned long) atol(nbrjpgs));

	fclose(fp);

	return EXIT_SUCCESS;

	error:
		printf("Could not interpret message");
		return EXIT_FAILURE;

}
