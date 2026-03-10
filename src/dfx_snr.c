/*!
 *  \file   dfx_snr.c
 *  \brief  DFX library: Signal to Noise Ratio (SNR) and related metrics.
 * 
 *  Copyright (c) 2026 Yuriy A. Reznik
 *  Licensed under the MIT License: https://opensource.org/licenses/MIT
 *
 *  \author  Yuriy A. Reznik
 *  \version 1.01
 *  \date    March 7, 2026
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>	
#include <stdlib.h>
#include "dfx.h"

 /*!
  *  \brief Compute MSE and SNR metrics between two image planes.
  * 
  *  \param[in]      X_ref  - reference image plane (padded)
  *  \param[in]      X_test - test image plane (padded)
  *  \param[in]      width   - image width in pixels
  *  \param[in]      height  - image height in pixels
  *  \param[in]      p       - padding parameter
  *  \param[out]     p_MSE   - pointer to where to store the computed MSE value
  *  \param[out]     p_SNR   - pointer to where to store the computed SNR value
  * 
  *  \return         DFX_SUCCESS if success, or an error code otherwise.
  */
int snr_plane(float* X_ref, float* X_test, int width, int height, int p, float *p_MSE, float *p_SNR)
{
	float snr, mse, e_ref, ref, delta;
	int width_p = width + 2 * p;	/* width of padded plane */
	int x, y;

	/* check parameters: */
	if (X_ref == NULL || X_test == NULL || p_MSE == NULL || p_SNR == NULL || height < 0 || width < 0 || p < 0)
		return DFX_INVARG;

	/* compute energies: */
	mse = 0.; e_ref = 0.;
	for (y = 0; y < height; y++) for (x = 0; x < width; x++) {
		ref = X_ref[(p + y) * width_p + p + x];
		delta = ref - X_test[(p + y) * width_p + p + x];
		mse += delta * delta;
		e_ref += ref * ref;
	}

	/* normalize MSE and E_ref by the number of pixels: */
	mse /= (float)(width * height);
	e_ref /= (float)(width * height);

	/* compute SNR: */
	if (e_ref > 1e10 * mse) snr = 100.;        /* avoid divisions by 0 */
	else snr = 10.0f * log10f(e_ref / mse);

	/* store the results: */
	*p_MSE = mse;
	*p_SNR = snr;

	/* success: */
	return DFX_SUCCESS;
}

/*!
 *  \brief Compute MSE and SNR metrics between two images.
 */
int snr_image(float* R_ref, float* G_ref, float* B_ref, float* R_test, float* G_test, float* B_test, int width, int height, int p, float* p_MSE, float* p_SNR)
{
	float mse_R, mse_G, mse_B, mse, snr_R, snr_G, snr_B, snr;

	/* check parameters: */
	if (R_ref == NULL || G_ref == NULL || B_ref == NULL) return DFX_INVARG;
	if (R_test == NULL || G_test == NULL || B_test == NULL) return DFX_INVARG;
	if (height < 0 || width < 0 || p < 0) return DFX_INVARG;
	if (p_MSE == NULL || p_SNR == NULL) return DFX_INVARG;

	/* compute metrics for each plane: */
	snr_plane(R_ref, R_test, width, height, p, &mse_R, &snr_R);
	snr_plane(G_ref, G_test, width, height, p, &mse_G, &snr_G);
	snr_plane(B_ref, B_test, width, height, p, &mse_B, &snr_B);

	/* compute average metrics across planes: */
	mse = (mse_R + mse_G + mse_B) / 3.0f;
	snr = (snr_R + snr_G + snr_B) / 3.0f;

	/* store the results: */
	*p_MSE = mse;
	*p_SNR = snr;

	/* success: */
	return DFX_SUCCESS;
}

/* dfx_snr.c -- end of file */