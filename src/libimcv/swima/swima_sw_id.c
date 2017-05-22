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

#include "swima_sw_id.h"

typedef struct private_swima_sw_id_t private_swima_sw_id_t;

/**
 * Private data of a swima_sw_id_t object.
 *
 */
struct private_swima_sw_id_t {

	/**
	 * Public swima_sw_id_t interface.
	 */
	swima_sw_id_t public;


	/**
	 * Software Identity
	 */
	chunk_t sw_id;

	/**
	 * Optional Software Locator
	 */
	chunk_t sw_locator;

	/**
	 * Reference count
	 */
	refcount_t ref;
};

METHOD(swima_sw_id_t, get_sw_id, chunk_t,
	private_swima_sw_id_t *this, chunk_t *sw_locator)
{
	if (sw_locator)
	{
		*sw_locator = this->sw_locator;
	}
	return this->sw_id;
}

METHOD(swima_sw_id_t, get_ref, swima_sw_id_t*,
	private_swima_sw_id_t *this)
{
	ref_get(&this->ref);
	return &this->public;
}

METHOD(swima_sw_id_t, destroy, void,
	private_swima_sw_id_t *this)
{
	if (ref_put(&this->ref))
	{
		free(this->sw_id.ptr);
		free(this->sw_locator.ptr);
		free(this);
	}
}

/**
 * See header
 */
swima_sw_id_t *swima_sw_id_create(chunk_t sw_id, chunk_t sw_locator)
{
	private_swima_sw_id_t *this;

	INIT(this,
		.public = {
			.get_sw_id = _get_sw_id,
			.get_ref = _get_ref,
			.destroy = _destroy,
		},
		.sw_id = chunk_clone(sw_id),
		.ref = 1,
	);

	if (sw_locator.len > 0)
	{
		this->sw_locator = chunk_clone(sw_locator);
	}

	return &this->public;
}

