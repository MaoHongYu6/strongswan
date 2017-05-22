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
 * @defgroup ietf_swima_attr_sw_id_inv ietf_swima_attr_sw_id_inv
 * @{ @ingroup ietf_attr
 */

#ifndef IETF_SWIMA_ATTR_SW_ID_INV_H_
#define IETF_SWIMA_ATTR_SW_ID_INV_H_

typedef struct ietf_swima_attr_sw_id_inv_t ietf_swima_attr_sw_id_inv_t;

#include "ietf/ietf_attr.h"
#include "swima/swima_sw_id.h"
#include "swima/swima_inventory.h"

#include <pa_tnc/pa_tnc_attr.h>

#define IETF_SWIMA_SW_ID_INV_MIN_SIZE	16

/**
 * Class implementing the IETF SW Identifier Inventory attribute
 *
 */
struct ietf_swima_attr_sw_id_inv_t {

	/**
	 * Public PA-TNC attribute interface
	 */
	pa_tnc_attr_t pa_tnc_attribute;

	/**
	 * Add a Software Identifier to the attribute
	 *
	 * @param sw_id				Software ID to be added
	 */
	void (*add)(ietf_swima_attr_sw_id_inv_t *this, swima_sw_id_t *sw_id);

	/**
	 * Get Request ID
	 *
	 * @return					Request ID
	 */
	uint32_t (*get_request_id)(ietf_swima_attr_sw_id_inv_t *this);

	/**
	 * Get Last Event ID
	 *
	 * @param eid_epoch			Event ID Epoch
	 * @return					Last Event ID
	 */
	uint32_t (*get_last_eid)(ietf_swima_attr_sw_id_inv_t *this,
							 uint32_t *eid_epoch);

	/**
	 * Get number of Software Identifiers in the inventory
	 *
	 * @return					Software ID count
	 */
	uint32_t (*get_sw_id_count)(ietf_swima_attr_sw_id_inv_t *this);

	/**
	 * Get Inventory of Software Identifiers
	 *
	 * @result					Software Identifier Inventory
	 */
	swima_inventory_t* (*get_inventory)(ietf_swima_attr_sw_id_inv_t *this);

	/**
	 * Remove all Software Identifieres from the Inventory
	 */
	void (*clear_inventory)(ietf_swima_attr_sw_id_inv_t *this);


};

/**
 * Creates an ietf_swima_attr_sw_id_inv_t object
 *
 * @param request_id			Copy of the Request ID
 * @param eid_epoch				Event ID Epoch
 * @param eid					Last Event ID
 */
pa_tnc_attr_t* ietf_swima_attr_sw_id_inv_create(uint32_t request_id,
												uint32_t eid_epoch,
												uint32_t eid);

/**
 * Creates an ietf_swima_attr_sw_id_inv_t object from received data
 *
 * @param length				Total length of attribute value
 * @param value					Unparsed attribute value (might be a segment)
 */
pa_tnc_attr_t* ietf_swima_attr_sw_id_inv_create_from_data(size_t length,
														 chunk_t value);

#endif /** IETF_SWIMA_ATTR_SW_ID_INV_H_ @}*/
