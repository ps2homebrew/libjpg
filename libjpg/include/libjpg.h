#ifndef __LIBJPG_H__
#define __LIBJPG_H__

#include <tamtypes.h>
#include <jpeglib.h>

typedef struct {
	int width;
	int height;
	int color_components;

	/* libjpg private fields */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPLE *buffer;
	u8 *data;
} jpgData;

jpgData *jpgOpen(char *filename);
jpgData *jpgOpenRAW(u8 *data, int size);
int      jpgReadImage(jpgData *jpg, u8 *dest);
void     jpgClose(jpgData *jpg);

#endif /* __LIBJPG_H__ */
