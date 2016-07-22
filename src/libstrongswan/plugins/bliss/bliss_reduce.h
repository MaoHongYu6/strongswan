/*
 * Copyright (C) 2016 Andreas Steffen
 * HSR Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

/**
 * @defgroup bliss_fft bliss_fft
 * @{ @ingroup bliss_p
 */

#ifndef BLISS_REDUCE_H_
#define BLISS_REDUCE_H_

#include "bliss_fft_params.h"

/**
 * Montgomery Reduction
 *
 * Montgomery, P. L. Modular multiplication without trial division.
 * Mathematics of Computation 44, 170 (1985), 519–521.
 */
static inline uint32_t bliss_mreduce(uint32_t x, bliss_fft_params_t *p)
{
	uint32_t m, t;
	
	m = (x * p->q_inv) & p->rmask;
	t = (x + m * p->q) >> p->rlog;

	return (t < p->q) ? t : t - p->q;
}

#endif /** BLISS_REDUCE_H_ @}*/
