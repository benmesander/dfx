/*!
 *  \file   dfx_image.c
 *  \brief  Basic operations with linear RGB images. 
 *
 *  We use planar format for linear RGB images, where each channel is stored in a separate plane. 
 *  Each plane is allocated as a single block of memory, containnig pixel values in row-major order, 
 *  with padding around the boundary.
 * 
 *  This is a preferred format for image filtering, blending, and resampling operations. 
 *  By Grassmann's laws of color mixing, all such operations on linear RGB images can be performed 
 *  independently in each channel.
 * 
 *  Copyright (c) 2026 Yuriy A. Reznik
 *  Licensed under the MIT License: https://opensource.org/licenses/MIT
 *
 *  \author  Yuriy A. Reznik
 *  \version 1.01
 *  \date    March 7, 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dfx.h"

/**************
 *
 *  Allocation/free operations:
 *
 *   plane_size()
 *   alloc_plane()
 *   alloc_image()
 *   free_plane()
 *   free_image()
 */

/*!
 * \brief Compute size of a padded plane of linear image (in pixels):
 */
static unsigned int plane_size(int width, int height, int p)
{
	unsigned int size = (2 * p + height) * (2 * p + width);	        /* size of padded plane, in pixels */
	return size;
}

/*!
 *  \brief Allocate a channel of linear RGB image.
 *
 *  \param[in,out]  pX     - pointer to a pointer to allocated plane
 *  \param[in]      height - image height
 *  \param[in]      width  - image width
 *  \param[in]      p      - padding parameter
 *
 *  \returns        DFX_X error code
 */
int alloc_plane(float** pX, int width, int height, int p)
{
	unsigned int size = plane_size(width, height, p);
	if (pX == NULL || height < 0 || width < 0 || p < 0)  return DFX_INVARG;
	if ((*pX = (float*)malloc(size * sizeof(float))) == NULL) return DFX_NOMEM;
	return DFX_SUCCESS;
}

/*!
 *  \brief Free image plane.
 */
int free_plane(float* X)
{
	if (X != NULL) free(X);
	return DFX_SUCCESS;
}

/*!
 *  \brief Allocate linear RGB image.
 *
 *  \param[in,out]  pR, pG, pB  - pointers to a pointers to allocated R,G,B planes
 *  \param[in]      height - image height
 *  \param[in]      width  - image width
 *  \param[in]      p      - padding parameter
 *
 *  \returns        DFX_X error code
 */
int alloc_image(float** pR, float** pG, float** pB, int width, int height, int p)
{
	float* R = NULL, * G = NULL, * B = NULL;

	/* check arguments */
	if (pR == NULL || pG == NULL || pB == NULL || height < 0 || width < 0 || p < 0)
		return DFX_INVARG;

	/* allocate planes: */
	if (alloc_plane(&R, width, height, p) != DFX_SUCCESS
		|| alloc_plane(&G, width, height, p) != DFX_SUCCESS
		|| alloc_plane(&B, width, height, p) != DFX_SUCCESS) {
		/* free memory & exit: */
		free_image(R, G, B);
		return DFX_NOMEM;
	}

	/* pass pointers & exit: */
	*pR = R; *pG = G; *pB = B;
	return DFX_SUCCESS;
}
 
/*!
 *  \brief Free planes of linear RGB image.
 */
int free_image(float* R, float* G, float* B)
{
	free_plane(R);
	free_plane(G);
	free_plane(B);
	return DFX_SUCCESS;
}

/**************
 *
 *  Zero & unit initializations:
 *
 *   zero_plane()
 *   zero_image()
 * 
 *   unit_plane()
 *   unit_image()
 */

 /*!
  *  \brief Fill channel of linear RGB image with zeros.
  */
int zero_plane(float* X, int width, int height, int p)
{
	unsigned int size = plane_size(width, height, p);
	if (X == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;
	/* set everything to 0s */
	memset((void*)X, 0, size * sizeof(float));
	return DFX_SUCCESS;
}

/*!
 *  \brief Fill linear RGB image with zeros.
 */
int zero_image(float* R, float* G, float* B, int width, int height, int p)
{
	if (R == NULL || G == NULL || B == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;
	zero_plane(R, width, height, p);
	zero_plane(G, width, height, p);
	zero_plane(B, width, height, p);
	return DFX_SUCCESS;
}

/*!
 *  \brief Fill channel of linear RGB image with ones.
 */
int unit_plane(float* X, int width, int height, int p)
{
	int width_p = width + 2 * p;	/* width of padded plane */
	int height_p = height + 2 * p;	/* height of padded plane */
	int x, y;
	
	/* check parameters: */
	if (X == NULL || height < 0 || width < 0 || p < 0) 
		return DFX_INVARG;

	/* set everything to 1s: */
	for (y = 0; y < height_p; y++) for (x = 0; x < width_p; x++) 
		X[y * width_p + x] = 1.0f;


	return DFX_SUCCESS;
}

/*!
 *  \brief Fill linear RGB image with ones.
 */
int unit_image(float* R, float* G, float* B, int width, int height, int p)
{
	/* check parameters: */
	if (R == NULL || G == NULL || B == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* initialize planes: */
	unit_plane(R, width, height, p);
	unit_plane(G, width, height, p);
	unit_plane(B, width, height, p);
	return DFX_SUCCESS;
}

/************** 
 *
 *  Copy, scale, add, subtract, and blend images:
 * 
 *   copy_plane()
 *   copy_image()
 *   scale_plane()
 *   scale_image()
 *   add_planes()
 *   add_images()
 *   subtract_planes()
 *   subtract_images()
 *   blend_planes()
 *   blend_images()
 */

/*!
 *  \brief Copy channel/plain of a linear RGB image.
 */
int copy_plane(float* X_in, float* X_out, int width, int height, int p)
{
	unsigned int size = plane_size(width, height, p);

	/* check parameters: */
	if (X_in == NULL || X_out == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* copy plane: */
	memcpy((void*)X_out, (void*)X_in, size * sizeof(float));
	return DFX_SUCCESS;
}

/*!
 *  \brief Copy linear RGB image.
 */
int copy_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p)
{
	/* check parameters: */
	if (R_in == NULL || G_in == NULL || B_in == NULL) return DFX_INVARG;
	if (R_out == NULL || G_out == NULL || B_out == NULL) return DFX_INVARG;
	if (height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* copy planes: */
	copy_plane(R_in, R_out, width, height, p);
	copy_plane(G_in, G_out, width, height, p);
	copy_plane(B_in, B_out, width, height, p);
	return DFX_SUCCESS;
}

/*!
 *  \brief Scale channel of linear RGB image by a constant.
 */
int scale_plane(float* X_in, float* X_out, int width, int height, int p, float scale)
{
	int width_p = width + 2 * p;	/* width of padded plane */
	int height_p = height + 2 * p;	/* height of padded plane */
	int x, y;

	/* check parameters: */
	if (X_in == NULL || X_out == NULL || height < 0 || width < 0 || p < 0) 
		return DFX_INVARG;

	/* scale image: */
	for (y = 0; y < height_p; y++) for (x = 0; x < width_p; x++)
		X_out[y * width_p + x] = X_in[y * width_p + x] * scale;

	return DFX_SUCCESS;
}

/*!
 *  \brief Scale linear RGB image by a constant.
 */
int scale_image(float* R_in, float* G_in, float* B_in, float* R_out, float* G_out, float* B_out, int width, int height, int p, float scale)
{
	/* check parameters: */
	if (R_in == NULL || G_in == NULL || B_in == NULL) return DFX_INVARG;
	if (R_out == NULL || G_out == NULL || B_out == NULL) return DFX_INVARG;
	if (height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* scale planes: */
	scale_plane(R_in, R_out, width, height, p, scale);
	scale_plane(G_in, G_out, width, height, p, scale);
	scale_plane(B_in, B_out, width, height, p, scale);
	return DFX_SUCCESS;
}

/*!
 *  \brief Add two linear RGB image planes.
 */
int add_planes(float* X_1, float* X_2, float* X_out, int width, int height, int p)
{
	int width_p = width + 2 * p;	/* width of padded plane */
	int height_p = height + 2 * p;	/* height of padded plane */
	int x, y;

	/* check parameters: */
	if (X_1 == NULL || X_2 == NULL || X_out == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* add two images: */
	for (y = 0; y < height_p; y++) for (x = 0; x < width_p; x++)
		X_out[y * width_p + x] = X_1[y * width_p + x] + X_2[y * width_p + x];

	return DFX_SUCCESS;
}

/*!
 *  \brief Add two linear RGB images.
 */
int add_images(float* R_1, float* G_1, float* B_1, float* R_2, float* G_2, float* B_2, float* R_out, float* G_out, float* B_out, int width, int height, int p)
{
	/* check parameters: */
	if (R_1 == NULL || G_1 == NULL || B_1 == NULL) return DFX_INVARG;
	if (R_2 == NULL || G_2 == NULL || B_2 == NULL) return DFX_INVARG;
	if (R_out == NULL || G_out == NULL || B_out == NULL) return DFX_INVARG;
	if (height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* add planes: */
	add_planes(R_1, R_2, R_out, width, height, p);
	add_planes(G_1, G_2, G_out, width, height, p);
	add_planes(B_1, B_2, B_out, width, height, p);
	return DFX_SUCCESS;
}

/*!
 *  \brief Subtract two linear RGB image planes.
 */
int subtract_planes(float* X_1, float* X_2, float* X_out, int width, int height, int p)
{
	int width_p = width + 2 * p;	/* width of psubtracted plane */
	int height_p = height + 2 * p;	/* height of psubtracted plane */
	int x, y;

	/* check parameters: */
	if (X_1 == NULL || X_2 == NULL || X_out == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* subtract two images: */
	for (y = 0; y < height_p; y++) for (x = 0; x < width_p; x++)
		X_out[y * width_p + x] = X_1[y * width_p + x] + X_2[y * width_p + x];

	return DFX_SUCCESS;
}

/*!
 *  \brief Subtract two linear RGB images.
 */
int subtract_images(float* R_1, float* G_1, float* B_1, float* R_2, float* G_2, float* B_2, float* R_out, float* G_out, float* B_out, int width, int height, int p)
{
	/* check parameters: */
	if (R_1 == NULL || G_1 == NULL || B_1 == NULL) return DFX_INVARG;
	if (R_2 == NULL || G_2 == NULL || B_2 == NULL) return DFX_INVARG;
	if (R_out == NULL || G_out == NULL || B_out == NULL) return DFX_INVARG;
	if (height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* subtract planes: */
	subtract_planes(R_1, R_2, R_out, width, height, p);
	subtract_planes(G_1, G_2, G_out, width, height, p);
	subtract_planes(B_1, B_2, B_out, width, height, p);
	return DFX_SUCCESS;
}

/*!
 *  \brief Blend two linear RGB image planes.
 */
int blend_planes(float* X_1, float* X_2, float* X_out, int width, int height, int p, float alpha)
{
	int width_p = width + 2 * p;	/* width of padded plane */
	int height_p = height + 2 * p;	/* height of padded plane */
	int x, y;

	/* check parameters: */
	if (X_1 == NULL || X_2 == NULL || X_out == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;

	/* blend two images: */
	for (y = 0; y < height_p; y++) for (x = 0; x < width_p; x++)
		X_out[y * width_p + x] = alpha * X_1[y * width_p + x] + (1.0f - alpha) * X_2[y * width_p + x];

	return DFX_SUCCESS;
}

/*!
 *  \brief Blend two linear RGB images.
 */
int blend_images(float* R_1, float* G_1, float* B_1, float* R_2, float* G_2, float* B_2, float* R_out, float* G_out, float* B_out, int width, int height, int p, float alpha)
{
	/* check parameters: */
	if (R_1 == NULL || G_1 == NULL || B_1 == NULL) return DFX_INVARG;
	if (R_2 == NULL || G_2 == NULL || B_2 == NULL) return DFX_INVARG;
	if (R_out == NULL || G_out == NULL || B_out == NULL) return DFX_INVARG;
	if (height < 0 || width < 0 || p < 0) return DFX_INVARG;
	if (alpha < 0.f || alpha > 1.0f) return DFX_INVARG;

	/* add planes: */
	blend_planes(R_1, R_2, R_out, width, height, p, alpha);
	blend_planes(G_1, G_2, G_out, width, height, p, alpha);
	blend_planes(B_1, B_2, B_out, width, height, p, alpha);
	return DFX_SUCCESS;
}

/*******************
 * 
 *  Image padding functions 
 * 
 *   pad_plane()
 *   pad_image()
 */

/*!
 *  \brief Fill boundary for a given image plane/channel.
 * 
 *  \param[in,out]  X      - pointer to a channel to process
 *  \param[in]      height - image height
 *  \param[in]      width  - image width
 *  \param[in]      p      - padding parameter
 *  \param[in]      t      - padding type
 *
 *  \returns        DFX_X error code
 */
int pad_plane(float* X, int width, int height, int p, int t)
{
	int w_lin = width + 2 * p;			    /* w_lin = width of padded liner RGB image */
	int x, y;

	/* check parameters: */
	if (X == NULL || height < 0 || width < 0 || p < 0) return DFX_INVARG;
	
	/* check padding type: */
	if (t == PAD_ZERO) 
	{
		/* zero-pad left and right edges: */
		for (y = 0; y < height; y++) for (x = 0; x < p; x++) {
			X[(p + y) * w_lin + x] = 0;
			X[(p + y) * w_lin + p + width + x] = 0;
		}
		/* zero-pad top and bottom edges: */
		for (y = 0; y < p; y++) for (x = 0; x < w_lin; x++) {
			X[y * w_lin + x] = 0;
			X[(p + height + y) * w_lin + x] = 0;
		}
	} 
	else if (t == PAD_REPLICATE) 
	{
		/* replicate-pad left and right edges: */
		for (y = 0; y < height; y++) for (x = 0; x < p; x++) {
			X[(p + y) * w_lin + x] = X[(p + y) * w_lin + p];
			X[(p + y) * w_lin + p + width + x] = X[(p + y) * w_lin + p + width - 1];
		}
		/* replicate-pad top and bottom edges: */
		for (y = 0; y < p; y++) for (x = 0; x < w_lin; x++) {
			X[y * w_lin + x] = X[p * w_lin + x];
			X[(p + height + y) * w_lin + x] = X[(p + height - 1) * w_lin + x];
		}
	}
	else /* PAD_REFLECT */
	{
		/* reflect-pad left and right edges: */
		for (y = 0; y < height; y++) for (x = 0; x < p; x++) {
			X[(p + y) * w_lin + p - 1 - x] = X[(p + y) * w_lin + p + x];                  /* flip horisontal */
			X[(p + y) * w_lin + p + width + x] = X[(p + y) * w_lin + p + width - 1 - x];
		}
		/* repflect-pad top and bottom edges: */
		for (y = 0; y < p; y++) for (x = 0; x < w_lin; x++) {
			X[(p - 1 - y) * w_lin + x] = X[(p + y) * w_lin + x];                          /* flip vertical */
			X[(p + height + y) * w_lin + x] = X[(p + height - 1 - y) * w_lin + x];
		}
	}

	return DFX_SUCCESS;
}

/*!
 *  \brief Fill boundary for an image.
 */
int pad_image(float* R, float* G, float* B, int width, int height, int p, int t)
{
	if (R == NULL || G == NULL || B == NULL || height < 0 || width < 0 || p < 0 || t < 0) return DFX_INVARG;
	pad_plane(R, width, height, p, t);
	pad_plane(G, width, height, p, t);
	pad_plane(B, width, height, p, t);
	return DFX_SUCCESS;
}

/* dfx_image.c -- end of file */
