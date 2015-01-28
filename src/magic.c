#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wand/MagickWand.h>

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

uint32_t terminal_columns()
{
#ifdef WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int ret;
	ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ),&csbi);
	return ret?csbi.dwSize.X:0;
#else
	struct winsize win;
	ioctl(0, TIOCGWINSZ, &win);
	return win.ws_col;
#endif
}

int main(int argc,char **argv)
{
#define QuantumScale  ((MagickRealType) 1.0/(MagickRealType) QuantumRange)
#define SigmoidalContrast(x) \
  (QuantumRange*(1.0/(1+exp(10.0*(0.5-QuantumScale*x)))-0.0066928509)*1.0092503)
#define ThrowWandException(wand) \
	{ \
		char \
		*description; \
		\
		ExceptionType \
		severity; \
		\
		description=MagickGetException(wand,&severity); \
		(void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
		description=(char *) MagickRelinquishMemory(description); \
		exit(-1); \
	}

	uint32_t cols = terminal_columns() >> 1; // divided by 2 because of 2 chars

	MagickBooleanType
		status;

	MagickWand
		*magick_wand;

	if (argc != 2)
	{
		(void) fprintf(stdout,"Usage: %s image thumbnail\n",argv[0]);
		exit(0);
	}
	/*
	   Read an image.
	   */
	MagickWandGenesis();
	magick_wand=NewMagickWand();
	status=MagickReadImage(magick_wand,argv[1]);
	if (status == MagickFalse)
		ThrowWandException(magick_wand);
	/*
	   Turn the images into a thumbnail sequence.
	   */
	MagickResetIterator(magick_wand);
	long width = 0, height = 0;
	while (MagickNextImage(magick_wand) != MagickFalse) {
		width = MagickGetImageWidth(magick_wand);
		height = MagickGetImageHeight(magick_wand);
		int hasAlfa = MagickGetImageAlphaChannel(magick_wand);
		if (cols < width) {
			float sc = cols/(float)width;
			MagickResizeImage(magick_wand, width * sc, height * sc, LanczosFilter, 1.0);
		}
		PixelWand* color;
		PixelIterator *iterator;
		PixelWand **pixels;
		MagickPixelPacket pixel;

		iterator = NewPixelIterator(magick_wand);
		if (iterator == (PixelIterator *) NULL)
			ThrowWandException(magick_wand);
		long y;
		for (y=0; y < (long) MagickGetImageHeight(magick_wand); y++) {
			pixels=PixelGetNextIteratorRow(iterator,&width);
			if (pixels == (PixelWand **) NULL)
				break;
			for (long x=0; x < (long) width; x++) {
				uint32_t r, g, b, a;
				PixelGetMagickColor(pixels[x],&pixel);
				r = 255 * pixel.red / 0xffff;
				g = 255 * pixel.green / 0xffff;
				b = 255 * pixel.blue / 0xffff;
				/*printf("%ux%u: (%d,%d,%d)\n", x, y, r, g, b);*/
				/*printf("alpha is %f\n", PixelGetAlpha(pixels[x]));*/
				uint32_t col;
				if (r == g && g == b)
					col = 232 + (r*23)/255;
				else
					col = (16 + ((r*5)/255)*36
							+ ((g*5)/255)*6
							+ (b*5)/255);

				printf("\e[48;5;%um  ", col);
			}
			printf("\n");
		}
		if (y < height)
			ThrowWandException(magick_wand);
		iterator = DestroyPixelIterator(iterator);
	}

	magick_wand=DestroyMagickWand(magick_wand);
	MagickWandTerminus();
	return(0);
}
