/*!
 *  \file   dfx.h
 *  \brief  DFX: Digital image processing Functions and eXamples.
 *
 *  This is a collection of basic image processing primitives implemented entirely in C, 
 *  without reliance on any external libraries. 
 * 
 *   *  Functions: read_bitmap() / write_bitmap() load and store 24-bit-packed sRGB images. 
 *      In memory, such images are presented as unsigned character arrays. Parameters "width" 
 *      and "height" define image dimensions. 
 *  
 *   *  Functions: srgb_to_linear() and linear_to_srgb() perform conversions between sRGB and Linear RGB format. 
 *      In linear RGB form, images are stored in memory as 3 arrays of floats, called "R" (red), "G" (green), 
 *      and "B" (blue) planes. Linear representation is fundamental. Only in linear space we can mix colors. 
 *      All color and image processing operations are implemented in the linear RGB space.
 *      
 *   *  When images are converted from sRGB format to linear, they can be padded. 
 *      Parameter "p" defines the number of pixels to be reserved around the boundary of the original image. 
 *      Such padding becomes handy in implementing filtering and scaling operations. 
 * 
 *   *  Functions linear_to_luminance() and luminance_to_grayscale_image() allow the extraction of luminance 
 *      (Y channel in CIE 1931 XYZ space), and translation of luminance to a grayscale RGB image. 
 * 
 *   *  Functions filter_plane() and filter_image() implement various image filtering operations. 
 * 
 *   *  Functions dft_plane() and dft_magnitude() and dft_phase() compute Discrete Fourier Transform over an image plane.
 * 
 *  Examples showing how to use these operations can be found in the directory dfx/demos. 
 *  
 *  Copyright (c) 2026 Yuriy A. Reznik
 *  Licensed under the MIT License: https://opensource.org/licenses/MIT
 * 
 *  \author  Yuriy A. Reznik
 *  \version 1.0
 *  \date    February 20, 2026
 */

#ifndef _DFX_H_
#define _DFX_H_  1

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* filter kenels: */
enum {
	FILT_SINC = 0,		/* truncated sinc filter */
	FILT_LANCZOS = 1,	/* Lanczos filter */
	FILT_GAUSS = 2,		/* Gaussian filter */
	/* ... */
	N_FILTERS			/* number of supported filters */
};

/* image padding types: */
enum {
	PAD_ZERO = 0,		/* padding by zeros */
	PAD_REPLICATE = 1,	/* padding by replicating boundary pixels */
	PAD_REFLECT = 2		/* padding by reflecting image along boundary */
};

/* common error codes returned by all library functions: */
enum {
	DFX_SUCCESS = 0,	/* success */
	DFX_INVARG = 1,	/* invalid arguments */
	DFX_CANTOPEN = 2,	/* file cannot be opened/created */
	DFX_IOERR = 3,	/* file reading/writing error */
	DFX_NOTSUP = 4,	/* not supported input format */
	DFX_NOMEM = 5	/* out of memory */
};

/* reading/writing bitmap files (dfx_bmp.c): */
extern int read_bitmap(char *filename, unsigned char **p_srgb, int* width, int* height, int* ppm_x, int* ppm_y);
extern int write_bitmap(char* filename, unsigned char* srgb, int width, int height, int ppm_x, int ppm_y);

/* alloc/dealloc of sRGB images (dfx_bmp.c): */
extern int alloc_srgb_image(unsigned char** p_srgb, int width, int height);
extern int free_srgb_image(unsigned char* srgb);

/* alloc/dealloc of linear images (dfx_image.c): */
extern int alloc_plane(float** pX, int wip_xdth, int height, int p);
extern int free_plane(float* X);
extern int alloc_image(float** pR, float** pG, float** pB, int width, int height, int p);
extern int free_image(float* R, float* G, float* B);

/* zero & copy functions: */
extern int zero_plane(float* X, int width, int height, int p);
extern int zero_image(float* R, float* G, float* B, int width, int height, int p);
extern int copy_plane(float* X_in, float* X_out, int width, int height, int p);
extern int copy_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p);

/* padding functions: */
extern int pad_plane(float* X, int width, int height, int p, int padding_type);
extern int pad_image(float* R, float* G, float* B, int width, int height, int p, int padding_type);

/* conversions between sRGB and linear RGB: */
extern int srgb_to_linear(unsigned char* sRGB, float* R, float* G, float* B, int width, int height, int p);
extern int linear_to_srgb(unsigned char* sRGB, float* R, float* G, float* B, int width, int height, int p);
extern int linear_to_srgb_dithered(unsigned char* sRGB, float* R, float* G, float* B, int width, int height, int p);

/* luminance extraction and visualization: */
extern int linear_to_luminance(float* Y, float* R, float* G, float* B, int width, int height, int p);
extern int luminance_to_grayscale_image(float* Y, float* R, float* G, float* B, int width, int height, int p);

/* Filtering operations (dfx_filter.c): */
extern int filter_plane(float* X, float* X_out, int width, int height, int p, int n, int t, float fc);
extern int filter_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p, int n, int t, float fc);

/* DFT-related funcitons (dfx_dft.c): */
extern int dft_plane(float* x, float* reX, float* imX, int width, int height, int p, int center_dc);
extern int dft_magnitude(float* reX, float* imX, float* magX, int width, int height, int p);
extern int dft_phase(float* reX, float* imX, float* phaseX, int width, int height, int p);

#ifdef __cplusplus
}
#endif

#endif // _DFX_H
