#include <tamtypes.h>
#include <stdlib.h>
#include <sifcmd.h>
#include <kernel.h>
#include <sifrpc.h>
#include <libjpg.h>
#include <gsKit.h>
#include <dmaKit.h>

#include "jpegtest.h"

GSGLOBAL gsGlobal;

void displayjpeg(jpgData *jpg) {
	GSTEXTURE tex;
	u8 *data;
	int i;

	data = malloc(jpg->width * jpg->color_components * jpg->height);
	jpgReadImage(jpg, data);
	tex.Width =  jpg->width;
	tex.Height = jpg->height;
	tex.PSM = GS_PSM_CT24;
	tex.Vram = 0;
	tex.Mem = data;
	gsKit_texture_upload(&gsGlobal, &tex);
	jpgClose(jpg);
	free(data);

	for (i=0; i<120; i++) {
		gsKit_vsync();
	}
}

int main() {
	jpgData *jpg;

	SifInitRpc(0);

	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	/* Generic Values */
	gsGlobal.Mode = GS_MODE_NTSC;
	gsGlobal.Interlace = GS_NONINTERLACED;
	gsGlobal.Field = GS_FRAME;
	gsGlobal.Aspect = GS_ASPECT_4_3;
	gsGlobal.Width = 640;
	gsGlobal.Height = 448;
	gsGlobal.OffsetX = 2048;
	gsGlobal.OffsetY = 2048;
	gsGlobal.StartX = -5;
	gsGlobal.StartY = -5;
	gsGlobal.PSM = GS_PSM_CT32;
	gsGlobal.PSMZ = GS_PSMZ_16;
	gsGlobal.ActiveBuffer = 1;
	gsGlobal.PrimFogEnable = 0;
	gsGlobal.PrimAAEnable = 0;
	gsGlobal.PrimAlphaEnable = 1;
	gsGlobal.PrimAlpha = 1;
	gsGlobal.PrimContext = 0;

	/* BGColor Register Values */
	gsGlobal.BGColor.Red = 0x00;
	gsGlobal.BGColor.Green = 0x00;
	gsGlobal.BGColor.Blue = 0x0;

	/* TEST Register Values */
	gsGlobal.Test.ATE = 0;
	gsGlobal.Test.ATST = 1;
	gsGlobal.Test.AREF = 0x80;
	gsGlobal.Test.AFAIL = 0;
	gsGlobal.Test.DATE = 0;
	gsGlobal.Test.DATM = 0;
	gsGlobal.Test.ZTE = 1;
	gsGlobal.Test.ZTST = 2;

	gsKit_init_screen(&gsGlobal);


	printf("display raw jpeg\n");
	jpg = jpgOpenRAW(jpegtest, sizeof(jpegtest));
	if (jpg == NULL) {
		printf("error opening raw jpeg\n");
	} else {
		displayjpeg(jpg);
	}

	printf("display cdrom0:\\TEST.JPG;1\n");
	jpg = jpgOpen("cdrom0:\\TEST.JPG;1");
	if (jpg == NULL) {
		printf("error opening cdrom0:\\TEST.JPG;1\n");
	} else {
		displayjpeg(jpg);
	}

	printf("display host0:test.jpg\n");
	jpg = jpgOpen("host0:test.jpg");
	if (jpg == NULL) {
		printf("error opening host0:test.jpg\n");
	} else {
		displayjpeg(jpg);
	}

	return 0;
}
