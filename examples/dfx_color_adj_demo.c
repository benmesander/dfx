/*!
 *  \file   dfx_color_adj_demo.c
 *  \brief  An example demonstrating color adjustement operations. 
 *
 *  This program generates color gradient test patterns and then applies adjustments of hue, saturation, and brightness. 
 *  It can also perform these operations with a user-supplied test file as an input.
 *
 *  Copyright (c) 2026 Yuriy A. Reznik
 *  Licensed under the MIT License: https://opensource.org/licenses/MIT
 *
 *  \author  Yuriy A. Reznik
 *  \version 1.02
 *  \date    March 15, 2026
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include "dfx.h"

 /*!
  *  \brief Generate color test image 1: changing brightness for primary and mixed colors.
  *
  *  \param[in,out]  R,G,B   - image where to place the pattern;
  *  \param[in]      width   - image width in pixels;
  *  \param[in]      height  - image height in pixels;
  *  \param[in]      p       - padding parameter
  *
  *  \return         DFX_SUCCESS if success, or an error code otherwise.
  */
static int generate_color_brightness_gradient_image(float* R, float* G, float* B, int width, int height, int p)
{
	float gamma, dx, L;
	int stripe_height, x, y;

	/* check args: */
	if (R == NULL || G == NULL || B == NULL || width <= 100 || height <= 7*10 || p < 0)
		return DFX_INVARG;

	/* test pattern parameters: */
	gamma = 3.0f;						/* gamma to use to produce color gradients */
	stripe_height = height / 7;			/* height of each stripe */

	/* produce test pattern: */
	for (x = 0; x < width; x++) 
	{
		/* compute luminance: */
		dx = (float)x / (width - 1);	/* delta in [0,1] */
		L = powf(dx, gamma);            /* converted to intended luminance, scaled to [0,1] */
		/* paint red stripe: */
		for (y = 0; y < stripe_height; y++) {
			R[(p + y) * (width + 2 * p) + p + x] = L;
			G[(p + y) * (width + 2 * p) + p + x] = 0.;
			B[(p + y) * (width + 2 * p) + p + x] = 0.;
		}
		/* paint yellow stripe: */
		for (; y < stripe_height * 2; y++) {
			R[(p + y) * (width + 2 * p) + p + x] = L;
			G[(p + y) * (width + 2 * p) + p + x] = L;
			B[(p + y) * (width + 2 * p) + p + x] = 0.;
		}
		/* paint green stripe: */
		for (; y < stripe_height * 3; y++) {
			R[(p + y) * (width + 2 * p) + p + x] = 0.;
			G[(p + y) * (width + 2 * p) + p + x] = L;
			B[(p + y) * (width + 2 * p) + p + x] = 0.;
		}
		/* paint cyan stripe: */
		for (; y < stripe_height * 4; y++) {
			R[(p + y) * (width + 2 * p) + p + x] = 0.;
			G[(p + y) * (width + 2 * p) + p + x] = L;
			B[(p + y) * (width + 2 * p) + p + x] = L;
		}
		/* paint blue stripe: */
		for (; y < stripe_height * 5; y++) {
			R[(p + y) * (width + 2 * p) + p + x] = 0.;
			G[(p + y) * (width + 2 * p) + p + x] = 0.;
			B[(p + y) * (width + 2 * p) + p + x] = L;
		}
		/* paint magenta stripe: */
		for (; y < stripe_height * 6; y++) {
			R[(p + y) * (width + 2 * p) + p + x] = L;
			G[(p + y) * (width + 2 * p) + p + x] = 0.;
			B[(p + y) * (width + 2 * p) + p + x] = L;
		}
		/* paint grayscale stripe: */
		for (; y < height; y++) {
			R[(p + y) * (width + 2 * p) + p + x] = L;
			G[(p + y) * (width + 2 * p) + p + x] = L;
			B[(p + y) * (width + 2 * p) + p + x] = L;
		}
	}

	/* pad image and return: */
	if (p > 0) pad_image(R, G, B, width, height, p, PAD_ZERO);
	return DFX_SUCCESS;
}

/*!
   *  \brief Generate color test image 2: changing saturation for primary and mixed colors.
   *
   *  \param[in,out]  R,G,B   - image where to place the pattern;
   *  \param[in]      width   - image width in pixels;
   *  \param[in]      height  - image height in pixels;
   *  \param[in]      p       - padding parameter
   *
   *  \return         DFX_SUCCESS if success, or an error code otherwise.
   */
static int generate_color_saturation_gradient_image(float* R, float* G, float* B, int width, int height, int p)
{
	float gamma, dy, S, L;
	int stripe_height, x, y;

	/* check args: */
	if (R == NULL || G == NULL || B == NULL || width <= 100 || height <= 7 * 10 || p < 0)
		return DFX_INVARG;

	/* test pattern parameters: */
	gamma = 3.0f;						/* gamma to use to produce brightness gradients */
	stripe_height = height / 6;			/* height of each stripe */

	/* produce test pattern: */
	for (x = 0; x < width; x++)
	{
		/* compute saturation: */
		S = (float)x / (width - 1);											/* S = distance from 0, normalized to [0,1] */
		/* paint red stripe: */
		for (y = 0; y < stripe_height; y++) {
			dy = (float)(y % stripe_height) / (stripe_height - 1);
			L = powf(1.f - dy, gamma);										/* L = function of vertical position withn a stripe */
			R[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
			G[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
			B[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
		}
		/* paint yellow stripe: */
		for (; y < stripe_height * 2; y++) {
			dy = (float)(y % stripe_height) / (stripe_height - 1);
			L = powf(1.f - dy, gamma);
			R[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
			G[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
			B[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
		}
		/* paint green stripe: */
		for (; y < stripe_height * 3; y++) {
			dy = (float)(y % stripe_height) / (stripe_height - 1);
			L = powf(1.f - dy, gamma);
			R[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
			G[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
			B[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
		}
		/* paint cyan stripe: */
		for (; y < stripe_height * 4; y++) {
			dy = (float)(y % stripe_height) / (stripe_height - 1);
			L = powf(1.f - dy, gamma);
			R[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
			G[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
			B[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
		}
		/* paint blue stripe: */
		for (; y < stripe_height * 5; y++) {
			dy = (float)(y % stripe_height) / (stripe_height - 1);
			L = powf(1.f - dy, gamma);
			R[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
			G[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
			B[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
		}
		/* paint magenta stripe: */
		for (; y < height; y++) {
			dy = (float)(y % stripe_height) / (stripe_height - 1);
			L = powf(1.f - dy, gamma);
			R[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
			G[(p + y) * (width + 2 * p) + p + x] = 0 * S + L * (1 - S);
			B[(p + y) * (width + 2 * p) + p + x] = L * S + L * (1 - S);
		}
	}

	/* pad image and return: */
	if (p > 0) pad_image(R, G, B, width, height, p, PAD_ZERO);
	return DFX_SUCCESS;
}

/*!
 *  \brief Color operations / adjustments demo program.
 */
int main(int argc, char* argv[])
{
	/* image parameters: */
	int width = 1120, height = 630, ppm_x = 2835, ppm_y = 2835; /* 72dpi*/
	int p = 16; /* padding parameter */

	/* working images / color planes: */
	unsigned char *sRGB = NULL, *sRGB_in = NULL;
	float *R = NULL, *G = NULL, *B = NULL;
	float *R2 = NULL, *G2 = NULL, *B2 = NULL;

	/* color space and brightness to use in comversions to XYZ, LUV, and LAB: */
	int colors = COLORS_SRGB;
	float Y_white = 1.0;

	/* allocate memory: */
	if (alloc_srgb_image(&sRGB, width, height) != DFX_SUCCESS
	 || alloc_image(&R, &G, &B, width, height, p) != DFX_SUCCESS
	 || alloc_image(&R2, &G2, &B2, width, height, p) != DFX_SUCCESS) {
		/* print error, free allocated memory & exit: */
		printf("Error: cannot allocate memory for images\n");
		if (sRGB != NULL) free_srgb_image(sRGB);
		if (R != NULL || G != NULL || B != NULL) free_image(R, G, B);
		if (R2 != NULL || G2 != NULL || B2 != NULL) free_image(R2, G2, B2);
		return 1;
	}

	/* 
	 * Load or generate test pattern to work with: 
	 */
	if (argc >= 2) {
		/* read bitmap file: */
		if (read_bitmap(argv[1], &sRGB_in, &width, &height, &ppm_x, &ppm_y) != DFX_SUCCESS) {
			printf("Cannot open input file: %s\n", argv[1]);
			printf("Only 24-bit RGB bitmap files are supported.\n");
			return 1;
		}
		/* convert it to linear & visualize: */
		srgb_to_linear(sRGB_in, R, G, B, width, height, p);
		linear_to_srgb(R, G, B, sRGB, width, height, p);
		write_bitmap("test_image.bmp", sRGB, width, height, ppm_x, ppm_y);
	}
	else {
		/* generate test pattern: */
		generate_color_brightness_gradient_image(R, G, B, width, height, p);
		linear_to_srgb(R, G, B, sRGB, width, height, p);
		write_bitmap("color_gradient_test_image_1.bmp", sRGB, width, height, ppm_x, ppm_y);
	}

	/*
	 * Test brightness adjustements:
	 */

	/* adjust brightness by factor of 0.5: */
	adjust_brightness(R, G, B, R2, G2, B2, width, height, p, 0.5f, 0);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_05x_brightness_hard.bmp", sRGB, width, height, ppm_x, ppm_y);
	/* with soft transition: */
	adjust_brightness(R, G, B, R2, G2, B2, width, height, p, 0.5f, 2);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_05x_brightness_soft.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* adjust brightness by factor of 2x: */
	adjust_brightness(R, G, B, R2, G2, B2, width, height, p, 2.0f, 0);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_2x_brightness_hard.bmp", sRGB, width, height, ppm_x, ppm_y);
	/* with soft transition: */
	adjust_brightness(R, G, B, R2, G2, B2, width, height, p, 2.0f, 2);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_2x_brightness_soft.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* adjust brightness by factor of 4x: */
	adjust_brightness(R, G, B, R2, G2, B2, width, height, p, 4.0f, 0);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_4x_brightness_hard.bmp", sRGB, width, height, ppm_x, ppm_y);
	/* with soft transition: */
	adjust_brightness(R, G, B, R2, G2, B2, width, height, p, 4.0f, 2);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_4x_brightness_soft.bmp", sRGB, width, height, ppm_x, ppm_y);

	/*
	 * Test saturation adjustements:
	 */
	if (argc < 2) {
		/* generate saturation test pattern; */
		generate_color_saturation_gradient_image(R, G, B, width, height, p);
		linear_to_srgb(R, G, B, sRGB, width, height, p);
		write_bitmap("color_gradient_test_image_2.bmp", sRGB, width, height, ppm_x, ppm_y);
	}

	/* adjust saturation by factor of 0.5: */
	adjust_saturation(R, G, B, R2, G2, B2, width, height, p, 0.5f, 0);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_05x_saturation_hard.bmp", sRGB, width, height, ppm_x, ppm_y);
	/* with soft transition: */
	adjust_saturation(R, G, B, R2, G2, B2, width, height, p, 0.5f, 2);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_05x_saturation_soft.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* adjust saturation by factor of 2x: */
	adjust_saturation(R, G, B, R2, G2, B2, width, height, p, 2.0f, 0);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_2x_saturation_hard.bmp", sRGB, width, height, ppm_x, ppm_y);
	/* with soft transition: */
	adjust_saturation(R, G, B, R2, G2, B2, width, height, p, 2.0f, 2);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_2x_saturation_soft.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* adjust saturation by factor of 4x: */
	adjust_saturation(R, G, B, R2, G2, B2, width, height, p, 4.0f, 0);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_4x_saturation_hard.bmp", sRGB, width, height, ppm_x, ppm_y);
	/* with soft transition: */
	adjust_saturation(R, G, B, R2, G2, B2, width, height, p, 4.0f, 2);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_4x_saturation_soft.bmp", sRGB, width, height, ppm_x, ppm_y);

	/*
	 * Test hue adjustements:
	 */

	/* +15 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI/12.f);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_15deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* +30 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI / 6.f);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_30deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* +45 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI / 4.f);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_45deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* +60 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI / 3.f);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_60deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* +75 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI * 5.f/12.f);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_75deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* +90 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI / 2.f);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_90deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* +135 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI*3./4.);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_135deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* +180 degrees */
	adjust_hue(R, G, B, R2, G2, B2, width, height, p, M_PI);
	linear_to_srgb(R2, G2, B2, sRGB, width, height, p);
	write_bitmap("test_hue_plus_180deg.bmp", sRGB, width, height, ppm_x, ppm_y);

	/* free memory and exit: */
	if (sRGB_in) free_srgb_image(sRGB_in);
	free_srgb_image(sRGB);
	free_image(R, G, B);
	free_image(R2, G2, B2);

	return 0;
}

/* dfx_color_adj_demo.c -- end of file */
