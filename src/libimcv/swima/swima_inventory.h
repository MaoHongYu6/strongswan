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
 * @defgroup swima_inventory swima_inventory
 * @{ @ingroup libimcv_swid
 */

#ifndef SWIMA_INVENTORY_H_
#define SWIMA_INVENTORY_H_

#include <library.h>

/* Maximum size of a SWID Tag Inventory: 100 MB */
#define SWID_MAX_ATTR_SIZE	100000000

typedef struct swima_inventory_t swima_inventory_t;

/**
 * Class managing SWID tag inventory
 */
struct swima_inventory_t {

	/**
	 * Collect the SWID tags stored on the endpoint
	 *
	 * @param item			SW evidence record to be added
	 */
	void (*add)(swima_inventory_t *this, void *item);

	/**
	 * Get the number of collected SWID tags
	 *
	 * @return				Number of collected records
	 */
	int (*get_count)(swima_inventory_t *this);

	/**
	  * Create a SWID tag inventory enumerator
	  *
	  * @return				Enumerator returning SW evidence records
	  */
	enumerator_t* (*create_enumerator)(swima_inventory_t *this);

	/**
	 * Destroys a swima_inventory_t object.
	 */
	void (*destroy)(swima_inventory_t *this);

};

/**
 * Creates a swima_inventory_t object
 *
 * @param full_records			TRUE if full records, FALSE if SW IDs only
 */
swima_inventory_t* swima_inventory_create(bool full_records);

#endif /** SWIMA_INVENTORY_H_ @}*/
