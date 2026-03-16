/*!
 *  \file   dfx.h
 *  \brief  DFX: Digital image processing Functions and eXamples.
 *
 *  This library provides a collection of basic image‑processing primitives
 *  implemented entirely in C, without relying on external libraries. 
 *
 *  •  read_bitmap() and write_bitmap() load and store 24‑bit packed sRGB images.
 *     In memory, these images are stored as arrays of unsigned chars.
 *     The parameters “width” and “height” specify the image dimensions.
 *
 *  •  srgb_to_linear() and linear_to_srgb() convert between sRGB and linear RGB.
 *     In linear RGB form, images are stored as three float arrays: R (red),
 *     G (green), and B (blue). Linear representation is fundamental. 
 *     Most color- and image‑processing operations in this library are implemented 
 *     in linear space.
 *
 *  •  When converting from sRGB to linear RGB, images may be padded.
 *     The parameter “p” defines the number of pixels reserved around the
 *     boundary of the original image. Padding is required for filtering and
 *     resampling operations.
 *
 *  •  linear_to_luminance() and luminance_to_grayscale_image() extract image
 *     luminance (the Y channel in CIE 1931 XYZ space) and convert luminance
 *     channel into a grayscale RGB image.
 *
 *  •  filter_plane() and filter_image() implement a variety of image‑filtering
 *     operations.
 *
 *  •  resample_plane() and resample_image() implement image‑resampling
 *     operations.
 *
 *  •  dft_plane(), dft_magnitude(), and dft_phase() compute the Discrete
 *     Fourier Transform of an image plane and extract magnitude and phase.
 *
 *  Examples demonstrating how to use these operations can be found in
 *  the dfx/examples directory.
 *  
 *  Copyright (c) 2026 Yuriy A. Reznik
 *  Licensed under the MIT License: https://opensource.org/licenses/MIT
 * 
 *  \author  Yuriy A. Reznik
 *  \version 1.02
 *  \date    March 15, 2026
 */

#ifndef _DFX_H_
#define _DFX_H_  1

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Filter kernels: */
enum {
	FILT_SINC = 0,		/* truncated sinc filter */
	FILT_LANCZOS = 1,	/* Lanczos filter */
	FILT_GAUSS = 2,		/* Gaussian filter */
	/* ... */
	N_FILTERS			/* the number of supported filters */
};

/*! Image padding operations: */
enum {
	PAD_ZERO = 0,		/* padding by zeros */
	PAD_REPLICATE = 1,	/* padding by replicating boundary pixels */
	PAD_REFLECT = 2		/* padding by reflecting image along boundary */
};

/*! RGB primary colors corresponding to various color system used in practice */
enum {
	COLORS_BT709 = 0,   /* ITU-R Rec. BT.709 primaries */
	COLORS_SRGB = 0,    /* sRGB primaries = the same as BT.709 */
	COLORS_NTSC = 1,    /* NTSC 1953 primaries - legacy */
	COLORS_SMPTEC = 2,  /* SMPTE RP 145 (aka SMPTE - C), used by NTSC since 1987 */
	COLORS_EBU3213 = 3, /* EBU Tech 3213, used by PAL and SECAM systems since 1975 */
	COLORS_PAL = 3,     /* PAL/SECAM primaries = the same as EBU Tech 3213 */
	COLORS_BT2020 = 4,  /* ITU-R Rec. BT.2020 primaries */
	COLORS_P3D65 = 5,	/* DCI-P3 primaries + D65 white */
	COLORS_P3 = 6,		/* DCI-P3 space */
	N_COLORS			/* the number of supported color systems */
};

/* Error codes returned by DFX library functions: */
enum {
	DFX_SUCCESS = 0,	/* success */
	DFX_INVARG = 1,		/* invalid arguments */
	DFX_CANTOPEN = 2,	/* file cannot be opened/created */
	DFX_IOERR = 3,		/* file reading/writing error */
	DFX_NOTSUP = 4,		/* not supported input format */
	DFX_NOMEM = 5,		/* out of memory */
	DFX_INVRANGE = 6	/* invalid pixel range */
};

/* Allocation/deallocation of sRGB images (dfx_bmp.c): */
extern int alloc_srgb_image(unsigned char** p_srgb, int width, int height);
extern int free_srgb_image(unsigned char* srgb);

/* Reading/writing bitmap files (dfx_bmp.c): */
extern int read_bitmap(char *filename, unsigned char **p_srgb, int* width, int* height, int* ppm_x, int* ppm_y);
extern int write_bitmap(char* filename, unsigned char* srgb, int width, int height, int ppm_x, int ppm_y);

/* Conversions between sRGB and linear RGB (dfx_srgb.c): */
extern int srgb_to_linear(unsigned char* sRGB, float* R, float* G, float* B, int width, int height, int p);
extern int linear_to_srgb(float* R, float* G, float* B, unsigned char* sRGB, int width, int height, int p);
extern int linear_to_srgb_dithered(float* R, float* G, float* B, unsigned char* sRGB, int width, int height, int p);

/* Allocation/deallocation of linear images (dfx_image.c): */
extern int alloc_plane(float** pX, int width, int height, int p);
extern int free_plane(float* X);
extern int alloc_image(float** pR, float** pG, float** pB, int width, int height, int p);
extern int free_image(float* R, float* G, float* B);

/* Initialization functions: */
extern int zero_plane(float* X, int width, int height, int p);
extern int zero_image(float* R, float* G, float* B, int width, int height, int p);
extern int unit_plane(float* X, int width, int height, int p);
extern int unit_image(float* R, float* G, float* B, int width, int height, int p);

/* Padding functions: */
extern int pad_plane(float* X, int width, int height, int p, int padding_type);
extern int pad_image(float* R, float* G, float* B, int width, int height, int p, int padding_type);

/* Copy, scale, add, subtract, and blend operations: */
extern int copy_plane(float* X_in, float* X_out, int width, int height, int p);
extern int copy_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p);
extern int scale_plane(float* X_in, float* X_out, int width, int height, int p, float scale);
extern int scale_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p, float scale);
extern int add_planes(float* X_1, float* X_2, float* X_out, int width, int height, int p);
extern int add_images(float* R_1, float* G_1, float* B_1, float* R_2, float* G_2, float* B_2, float* R_out, float* G_out, float* B_out, int width, int height, int p);
extern int subtract_planes(float* X_1, float* X_2, float* X_out, int width, int height, int p);
extern int subtract_images(float* R_1, float* G_1, float* B_1, float* R_2, float* G_2, float* B_2, float* R_out, float* G_out, float* B_out, int width, int height, int p);
extern int blend_planes(float* X_1, float* X_2, float* X_out, int width, int height, int p, float alpha);
extern int blend_images(float* R_1, float* G_1, float* B_1, float* R_2, float* G_2, float* B_2, float* R_out, float* G_out, float* B_out, int width, int height, int p, float alpha);

/* Linear filtering operations (dfx_filter.c): */
extern int filter_plane(float* X, float* X_out, int width, int height, int p, int n, int t, float fc);
extern int filter_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p, int n, int t, float fc);

/* Resampling operations: */
extern int resample_plane(float* X, float* X_out, int width_in, int height_in, int width_out, int height_out, int p, int n);
extern int resample_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width_in, int height_in, int width_out, int height_out, int p, int n);

/* DFT-related funcitons (dfx_dft.c): */
extern int dft_plane(float* x, float* reX, float* imX, int width, int height, int p, int center_dc);
extern int dft_magnitude(float* reX, float* imX, float* magX, int width, int height, int p);
extern int dft_phase(float* reX, float* imX, float* phaseX, int width, int height, int p);

/* Conversions between linear RGB, XYZ, L*u*v*, and L*a*b* spaces (dfx_colors.c): */
extern int linear_rgb_to_xyz(float* R, float* G, float* B, float* X, float* Y, float* Z, int width, int height, int p, int colors, float Y_white);
extern int xyz_to_linear_rgb(float* R, float* G, float* B, float* X, float* Y, float* Z, int width, int height, int p, int colors, float Y_white);
extern int xyz_to_lyv(float* X, float* Y, float* Z, float* L, float* u, float* v, int width, int height, int p, int colors, float Y_white);
extern int lyv_to_xyz(float* L, float* u, float* v, float* X, float* Y, float* Z, int width, int height, int p, int colors, float Y_white);
extern int xyz_to_lab(float* X, float* Y, float* Z, float* L, float* a, float* b, int width, int height, int p, int colors, float Y_white);
extern int lab_to_xyz(float* L, float* a, float* b, float* X, float* Y, float* Z, int width, int height, int p, int colors, float Y_white);

/* Luminance extraction and visualization: */
extern int linear_rgb_to_luminance(float* R, float* G, float* B, float* Y, int width, int height, int p, int colors, float Y_white);
extern int linear_to_luminance(float* R, float* G, float* B, float* Y, int width, int height, int p);
extern int luminance_to_grayscale_image(float* Y, float* R, float* G, float* B, int width, int height, int p);

/* Color adjustment operations: */
extern int adjust_brightness(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p, float brightness, int k);
extern int adjust_saturation(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p, float saturation, int k);
extern int adjust_hue(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p, float theta);

/* Signal-to-noise metric (dfx_snr.c): */
extern int snr_plane(float* X_ref, float* X_test, int width, int height, int p, float* p_MSE, float* p_SNR);
extern int snr_image(float* R_ref, float* G_ref, float* B_ref, float* R_test, float* G_test, float* B_test, int width, int height, int p, float* p_MSE, float* p_SNR);

#ifdef __cplusplus
}
#endif

#endif // _DFX_H
