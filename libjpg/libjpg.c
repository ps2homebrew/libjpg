#include <tamtypes.h>
#include <kernel.h>
#include <string.h>
#include <malloc.h>
#include <fileio.h>

#include <libjpg.h>
#include "jpeglib.h"

jpgData *jpgOpenRAW(u8 *data, int size) {
  jpgData *jpg;

  jpg = malloc(sizeof(jpgData));
  if (jpg == NULL) return NULL;

  memset(jpg, 0, sizeof(jpgData));
  /* Initialize the JPEG decompression object with default error handling. */
  jpg->cinfo.err = jpeg_std_error(&jpg->jerr);
  jpeg_create_decompress(&jpg->cinfo);

  /* Specify data source for decompression */
  jpeg_data_src(&jpg->cinfo, data, size);

  /* Read file header, set default decompression parameters */
  (void) jpeg_read_header(&jpg->cinfo, TRUE);

  /* Calculate output image dimensions so we can allocate space */
  jpeg_calc_output_dimensions(&jpg->cinfo);

  jpg->buffer = (*jpg->cinfo.mem->alloc_small)
      ((j_common_ptr) &jpg->cinfo, JPOOL_IMAGE,
       jpg->cinfo.output_width * jpg->cinfo.out_color_components);

  /* Start decompressor */
  (void) jpeg_start_decompress(&jpg->cinfo);

  jpg->width = jpg->cinfo.output_width;
  jpg->height = jpg->cinfo.output_height;
  jpg->color_components = jpg->cinfo.out_color_components;

  /* All done. */
  return(jpg->jerr.num_warnings ? NULL : jpg);
}

jpgData *jpgOpen(char *filename) {
  jpgData *jpg;
  u8 *data;
  int size;
  int fd;

  fd = fioOpen(filename, O_RDONLY);
  if (fd == -1) {
    printf("jpgOpen: error opening '%s'\n", filename);
    return NULL;
  }
  size = fioLseek(fd, 0, SEEK_END);
  fioLseek(fd, 0, SEEK_SET);
  data = (u8*)malloc(size);
  if (data == NULL) return NULL;
  fioRead(fd, data, size);
  fioClose(fd);

  jpg = jpgOpenRAW(data, size);
  if (jpg == NULL) return NULL;
  jpg->data = data;

  return jpg;
}

int  jpgReadImage(jpgData *jpg, u8 *dest) {
  JDIMENSION num_scanlines;
  int width = jpg->cinfo.output_width * jpg->cinfo.out_color_components;

  /* Process data */
  while (jpg->cinfo.output_scanline < jpg->cinfo.output_height) {
    num_scanlines = jpeg_read_scanlines(&jpg->cinfo, &jpg->buffer, 1);
	if (num_scanlines != 1) break;
	memcpy(dest, jpg->buffer, jpg->cinfo.output_width * jpg->cinfo.out_color_components);
	dest+= width;
  }

  return(jpg->jerr.num_warnings ? -1 : 0);
}

void jpgClose(jpgData *jpg) {
  /* Finish decompression and release memory.
   * I must do it in this order because output module has allocated memory
   * of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
   */
  (void) jpeg_finish_decompress(&jpg->cinfo);
  jpeg_destroy_decompress(&jpg->cinfo);
  if (jpg->data) free(jpg->data);
  free(jpg);
}

