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

#include "swima_inventory.h"
#include "swima_sw_id.h"

#include <collections/linked_list.h>
#include <utils/debug.h>


typedef struct private_swima_inventory_t private_swima_inventory_t;

/**
 * Private data of a swima_inventory_t object.
 *
 */
struct private_swima_inventory_t {

	/**
	 * Public swima_inventory_t interface.
	 */
	swima_inventory_t public;

	/**
	 * Full SW records or SW identifiers, only
	 */
	bool full_records;

	/**
	 * List of SWID tags or tag IDs
	 */
	linked_list_t *list;
};

METHOD(swima_inventory_t, add, void,
	private_swima_inventory_t *this, void *item)
{
	this->list->insert_last(this->list, item);
}

METHOD(swima_inventory_t, get_count, int,
	private_swima_inventory_t *this)
{
	return this->list->get_count(this->list);
}

METHOD(swima_inventory_t, create_enumerator, enumerator_t*,
	private_swima_inventory_t *this)
{
	return this->list->create_enumerator(this->list);
}

METHOD(swima_inventory_t, destroy, void,
	private_swima_inventory_t *this)
{
	if (this->full_records)
	{
		/* TODO */
	}
	else
	{
		this->list->destroy_offset(this->list, offsetof(swima_sw_id_t, destroy));
	}
	free(this);
}

/**
 * See header
 */
swima_inventory_t *swima_inventory_create(bool full_records)
{
	private_swima_inventory_t *this;

	INIT(this,
		.public = {
			.add = _add,
			.get_count = _get_count,
			.create_enumerator = _create_enumerator,
			.destroy = _destroy,
		},
		.full_records = full_records,
		.list = linked_list_create(),
	);

	return &this->public;
}
