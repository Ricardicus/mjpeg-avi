#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "avi.h"

#define DEFAULT_DESTINATION "Video.avi"

void usage(const char *argv0)
{
  printf("usage: %s [flag [value]]*\n", argv0);
  printf("  flags:\n");
  printf("    -h\n");
  printf("      display this help text\n");
  printf("    -r\n");
  printf("      resolution (e.g. 800x600)\n");
  printf("    -l\n");
  printf("      directory of the jpg files,\n");
  printf("      under this folder, all files\n");
  printf("      must be named accordingly:\n");
  printf("      1.jpeg, 2.jpeg, ..., N.jpeg\n");
  printf("    -n\n");
  printf("      number of jpges to include in\n");
  printf("      the video file\n");
  printf("    -s\n");
  printf("      frames per second\n");
  printf("    -o\n");
  printf("      output file destination\n");
  printf("\n");
  printf("%s compiled %s\n", argv0, __TIME__);
}

const char *file_size_to_text(unsigned long len)
{
  static char buffer[64];

  char *sizes[] = {"B", "kB", "MB", "GB"};

  unsigned long factor = 100; // the number of zeros here determine precision
  unsigned long limit = 1000;
  int index = 0;
  unsigned long tot = len * factor;

  while (tot > limit && index < (sizeof(sizes) / sizeof(*sizes))) {
    tot = tot / limit;
    index++;
  }

  double result = tot * 1.0 / factor;

  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%.2f %s", result, sizes[index]);
  return buffer;
}

int main(int argc, char *argv[])
{
  char *resolution = "800x600";
  char *location = "jpgs";
  char *fps = "10";
  char *nbrjpgs = "5";
  char *out_file = DEFAULT_DESTINATION;
  unsigned long len = 0;

  int i = 0;
  while (i < argc) {
    if (argv[i][0] == '-') {
      if (argv[i][1] != 'h' && i + 1 >= argc) goto error;
      switch (argv[i][1]) {
      case 'h':
        usage(argv[0]);
        return 1;
        break;
      case 'r':
        resolution = argv[i + 1];
        break;
      case 'l':
        location = argv[i + 1];
        break;
      case 's':
        fps = argv[i + 1];
        break;
      case 'o':
        out_file = argv[i + 1];
        break;
      case 'n':
        nbrjpgs = argv[i + 1];
        break;
      default:
        break;
      }
    }
    i++;
  }

  FILE *fp = fopen(out_file, "wb");
  assert(fp != NULL);
  output_AVI_file(fp, NULL, resolution, location, (unsigned long)atol(fps),
                  (unsigned long)atol(nbrjpgs));
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  printf("File created: %s (%s)\n", out_file, file_size_to_text(len));
  fclose(fp);

  return EXIT_SUCCESS;

error:
  printf("Could not interpret message");
  return EXIT_FAILURE;
}
