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

#include "ietf_swima_attr_req.h"

#include <pa_tnc/pa_tnc_msg.h>
#include <bio/bio_writer.h>
#include <bio/bio_reader.h>
#include <utils/debug.h>
#include <collections/linked_list.h>

typedef struct private_ietf_swima_attr_req_t private_ietf_swima_attr_req_t;

/**
 * SW Request
 * see section 5.7 of IETF SW Inventory Message and Attributes for PA-TNC
 *
 *                       1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |C|S|R| Reserved|           Software Identifier Count           |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                          Request ID                           |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                         Earliest EID                          |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Software Identifier Length  | Software Identifier (var len) ~
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ~             Software Identifier (variable length)             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

#define SW_REQ_RESERVED_MASK			0xE0

/**
 * Private data of an ietf_swima_attr_req_t object.
 */
struct private_ietf_swima_attr_req_t {

	/**
	 * Public members of ietf_swima_attr_req_t
	 */
	ietf_swima_attr_req_t public;

	/**
	 * Vendor-specific attribute type
	 */
	pen_type_t type;

	/**
	 * Length of attribute value
	 */
	size_t length;

	/**
	 * Attribute value or segment
	 */
	chunk_t value;

	/**
	 * Noskip flag
	 */
	bool noskip_flag;

	/**
	 * SWID request flags
	 */
	uint8_t flags;

	/**
	 * Request ID
	 */
	uint32_t request_id;

	/**
	 * Earliest EID
	 */
	uint32_t earliest_eid;

	/**
	 * List of Target Software Identifiers
	 */
	linked_list_t *targets;

	/**
	 * Reference count
	 */
	refcount_t ref;
};

METHOD(pa_tnc_attr_t, get_type, pen_type_t,
	private_ietf_swima_attr_req_t *this)
{
	return this->type;
}

METHOD(pa_tnc_attr_t, get_value, chunk_t,
	private_ietf_swima_attr_req_t *this)
{
	return this->value;
}

METHOD(pa_tnc_attr_t, get_noskip_flag, bool,
	private_ietf_swima_attr_req_t *this)
{
	return this->noskip_flag;
}

METHOD(pa_tnc_attr_t, set_noskip_flag,void,
	private_ietf_swima_attr_req_t *this, bool noskip)
{
	this->noskip_flag = noskip;
}

METHOD(pa_tnc_attr_t, build, void,
	private_ietf_swima_attr_req_t *this)
{
	bio_writer_t *writer;
	swima_sw_id_t *swima_sw_id;
	chunk_t sw_id;
	enumerator_t *enumerator;

	if (this->value.ptr)
	{
		return;
	}

	writer = bio_writer_create(IETF_SWIMA_REQ_MIN_SIZE);
	writer->write_uint8 (writer, this->flags);
	writer->write_uint24(writer, this->targets->get_count(this->targets));
	writer->write_uint32(writer, this->request_id);
	writer->write_uint32(writer, this->earliest_eid);

	enumerator = this->targets->create_enumerator(this->targets);
	while (enumerator->enumerate(enumerator, &swima_sw_id))
	{
		sw_id = swima_sw_id->get_sw_id(swima_sw_id, NULL);
		writer->write_data16(writer, sw_id);
	}
	enumerator->destroy(enumerator);

	this->value = writer->extract_buf(writer);
	this->length = this->value.len;
	writer->destroy(writer);
}

METHOD(pa_tnc_attr_t, process, status_t,
	private_ietf_swima_attr_req_t *this, uint32_t *offset)
{
	bio_reader_t *reader;
	swima_sw_id_t *swima_sw_id;
	uint32_t sw_id_count;
	chunk_t sw_id;

	*offset = 0;

	if (this->value.len < this->length)
	{
		return NEED_MORE;
	}
	if (this->value.len < IETF_SWIMA_REQ_MIN_SIZE)
	{
		DBG1(DBG_TNC, "insufficient data for SW Request");
		return FAILED;
	}

	reader = bio_reader_create(this->value);
	reader->read_uint8 (reader, &this->flags);
	reader->read_uint24(reader, &sw_id_count);
	reader->read_uint32(reader, &this->request_id);
	reader->read_uint32(reader, &this->earliest_eid);

	*offset = IETF_SWIMA_REQ_MIN_SIZE;
	this->flags &= SW_REQ_RESERVED_MASK;

	while (sw_id_count--)
	{
		if (!reader->read_data16(reader, &sw_id))
		{
			DBG1(DBG_TNC, "insufficient data for Software ID");
			reader->destroy(reader);
			return FAILED;
		}
		*offset += 2 + sw_id.len;
		
		swima_sw_id = swima_sw_id_create(sw_id, chunk_empty);
		this->targets->insert_last(this->targets, swima_sw_id);
	}
	reader->destroy(reader);

	return SUCCESS;
}

METHOD(pa_tnc_attr_t, add_segment, void,
	private_ietf_swima_attr_req_t *this, chunk_t segment)
{
	this->value = chunk_cat("mc", this->value, segment);
}

METHOD(pa_tnc_attr_t, get_ref, pa_tnc_attr_t*,
	private_ietf_swima_attr_req_t *this)
{
	ref_get(&this->ref);
	return &this->public.pa_tnc_attribute;
}

METHOD(pa_tnc_attr_t, destroy, void,
	private_ietf_swima_attr_req_t *this)
{
	if (ref_put(&this->ref))
	{
		this->targets->destroy_offset(this->targets,
									  offsetof(swima_sw_id_t, destroy));
		free(this->value.ptr);
		free(this);
	}
}

METHOD(ietf_swima_attr_req_t, get_flags, uint8_t,
	private_ietf_swima_attr_req_t *this)
{
	return this->flags;
}

METHOD(ietf_swima_attr_req_t, get_request_id, uint32_t,
	private_ietf_swima_attr_req_t *this)
{
	return this->request_id;
}

METHOD(ietf_swima_attr_req_t, get_earliest_eid, uint32_t,
	private_ietf_swima_attr_req_t *this)
{
	return this->earliest_eid;
}

METHOD(ietf_swima_attr_req_t, add_target, void,
	private_ietf_swima_attr_req_t *this, swima_sw_id_t *swima_sw_id)
{
	this->targets->insert_last(this->targets, swima_sw_id);
}

METHOD(ietf_swima_attr_req_t, create_target_enumerator, enumerator_t*,
	private_ietf_swima_attr_req_t *this)
{
	return this->targets->create_enumerator(this->targets);
}

/**
 * Described in header.
 */
pa_tnc_attr_t *ietf_swima_attr_req_create(uint8_t flags, uint32_t request_id,
										  uint32_t eid)
{
	private_ietf_swima_attr_req_t *this;

	INIT(this,
		.public = {
			.pa_tnc_attribute = {
				.get_type = _get_type,
				.get_value = _get_value,
				.get_noskip_flag = _get_noskip_flag,
				.set_noskip_flag = _set_noskip_flag,
				.build = _build,
				.process = _process,
				.add_segment = _add_segment,
				.get_ref = _get_ref,
				.destroy = _destroy,
			},
			.get_flags = _get_flags,
			.get_request_id = _get_request_id,
			.get_earliest_eid = _get_earliest_eid,
			.add_target = _add_target,
			.create_target_enumerator = _create_target_enumerator,
		},
		.type = { PEN_IETF, IETF_ATTR_SW_REQUEST },
		.flags = flags & SW_REQ_RESERVED_MASK,
		.request_id = request_id,
		.earliest_eid = eid,
		.targets = linked_list_create(),
		.ref = 1,
	);

	return &this->public.pa_tnc_attribute;
}

/**
 * Described in header.
 */
pa_tnc_attr_t *ietf_swima_attr_req_create_from_data(size_t length, chunk_t data)
{
	private_ietf_swima_attr_req_t *this;

	INIT(this,
		.public = {
			.pa_tnc_attribute = {
				.get_type = _get_type,
				.get_value = _get_value,
				.get_noskip_flag = _get_noskip_flag,
				.set_noskip_flag = _set_noskip_flag,
				.build = _build,
				.process = _process,
				.add_segment = _add_segment,
				.get_ref = _get_ref,
				.destroy = _destroy,
			},
			.get_flags = _get_flags,
			.get_request_id = _get_request_id,
			.get_earliest_eid = _get_earliest_eid,
			.add_target = _add_target,
			.create_target_enumerator = _create_target_enumerator,
		},
		.type = { PEN_IETF, IETF_ATTR_SW_REQUEST },
		.length = length,
		.value = chunk_clone(data),
		.targets = linked_list_create(),
		.ref = 1,
	);

	return &this->public.pa_tnc_attribute;
}