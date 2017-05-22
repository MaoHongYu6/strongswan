/*
 * Copyright (C) 2017 Andreas Steffen
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
 * @defgroup swima_sw_id swima_sw_id
 * @{ @ingroup libimcv_swid
 */

#ifndef SWIMA_SW_ID_H_
#define SWIMA_SW_ID_H_

#include <library.h>

typedef struct swima_sw_id_t swima_sw_id_t;


/**
 * Class storing a Software Identity
 */
struct swima_sw_id_t {

	/**
	 * Get Software ID and optional Tag File Path
	 *
	 * @param sw_locator	Optional Software Locator
	 * @return				Software Identity
	 */
	chunk_t (*get_sw_id)(swima_sw_id_t *this, chunk_t *sw_locator);

	/**
	 * Get a new reference to a swima_sw_id object
	 *
	 * @return			this, with an increased refcount
	 */
	swima_sw_id_t* (*get_ref)(swima_sw_id_t *this);

	/**
	 * Destroys a swima_sw_id_t object.
	 */
	void (*destroy)(swima_sw_id_t *this);

};

/**
 * Creates a swima_sw_id_t object
 *
 * @param sw_id				Software Identity
 * @param sw_locator		Software Locator or empty chunk
 */
swima_sw_id_t* swima_sw_id_create(chunk_t sw_id, chunk_t sw_locator);

#endif /** SWIMA_SW_ID_H_ @}*/
