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

#include "test_suite.h"

#include "swima/swima_sw_id.h"
#include "ietf/swima/ietf_swima_attr_req.h"

static char* sw_id_str[] = {
	"strongswan.org_strongSwan_5.3.3",
	"strongswan.org_62251aa6-1a01-479b-aea6-f3dcf0ab1f1a"
};
static char sw_locator_str[] = "/usr/share/strongswan";


START_TEST(test_imcv_swima_sw_id)
{
	chunk_t sw_id, id, sw_locator, locator;
	swima_sw_id_t *swima_sw_id, *swima_sw_id_cp;

	sw_id = chunk_from_str(sw_id_str[0]);
	sw_locator = chunk_from_str(sw_locator_str);

	/* Software Identity with Software Locator */
	swima_sw_id = swima_sw_id_create(sw_id, sw_locator),
	ck_assert(swima_sw_id);
	swima_sw_id_cp = swima_sw_id->get_ref(swima_sw_id);

	id = swima_sw_id->get_sw_id(swima_sw_id, NULL);
	ck_assert_chunk_eq(id, sw_id);

	id = swima_sw_id->get_sw_id(swima_sw_id, &locator);
	ck_assert_chunk_eq(id, sw_id);
	ck_assert_chunk_eq(locator, sw_locator);

	swima_sw_id->destroy(swima_sw_id);
	swima_sw_id_cp->destroy(swima_sw_id);

	/* Software Identity without Software Locator */
	swima_sw_id = swima_sw_id_create(sw_id, chunk_empty),
	ck_assert(swima_sw_id);

	id = swima_sw_id->get_sw_id(swima_sw_id, &locator);
	ck_assert_chunk_eq(id, sw_id);
	ck_assert(locator.ptr == NULL && locator.len == 0);

	swima_sw_id->destroy(swima_sw_id);
}
END_TEST

typedef struct req_data_t req_data_t;

struct req_data_t {
	uint8_t flags;
	uint32_t request_id;
	uint32_t earliest_eid;
	uint32_t sw_id_count;
	chunk_t  value;
};

static req_data_t req_data[] = {
	{ IETF_SWIMA_ATTR_REQ_FLAG_NONE, 1,   0, 0, chunk_from_chars(
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00)
	},
	{ IETF_SWIMA_ATTR_REQ_FLAG_R,    2,  15, 1, chunk_from_chars(
		0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
		0x00, 0x0F, 0x00, 0x1f, 0x73, 0x74, 0x72, 0x6f, 0x6e, 0x67,
		0x73, 0x77, 0x61, 0x6e, 0x2e, 0x6f, 0x72, 0x67, 0x5f, 0x73,
		0x74, 0x72, 0x6f, 0x6e, 0x67, 0x53, 0x77, 0x61, 0x6e, 0x5f,
		0x35, 0x2e, 0x33, 0x2e, 0x33)
	},
	{ IETF_SWIMA_ATTR_REQ_FLAG_S,    3, 256, 2, chunk_from_chars(
		0x40, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x1f, 0x73, 0x74, 0x72, 0x6f, 0x6e, 0x67,
		0x73, 0x77, 0x61, 0x6e, 0x2e, 0x6f, 0x72, 0x67, 0x5f, 0x73,
		0x74, 0x72, 0x6f, 0x6e, 0x67, 0x53, 0x77, 0x61, 0x6e, 0x5f,
		0x35, 0x2e, 0x33, 0x2e, 0x33, 0x00, 0x33, 0x73, 0x74, 0x72,
		0x6f, 0x6e, 0x67, 0x73, 0x77, 0x61, 0x6e, 0x2e, 0x6f, 0x72,
		0x67, 0x5f, 0x36, 0x32, 0x32, 0x35, 0x31, 0x61, 0x61, 0x36,
		0x2d, 0x31, 0x61, 0x30, 0x31, 0x2d, 0x34, 0x37, 0x39, 0x62,
		0x2d, 0x61, 0x65, 0x61, 0x36, 0x2d, 0x66, 0x33, 0x64, 0x63,
		0x66, 0x30, 0x61, 0x62, 0x31, 0x66, 0x31, 0x61)
	},
};

START_TEST(test_imcv_swima_sw_req)
{
	pen_type_t type;
	pen_type_t pen_type = { PEN_IETF, IETF_ATTR_SW_REQUEST };
	pa_tnc_attr_t *attr, *attr1, *attr2;
	ietf_swima_attr_req_t *c_attr;
	swima_sw_id_t *swima_sw_id;
	chunk_t sw_id, value;
	enumerator_t *enumerator;
	uint32_t offset;
	int n;

	attr = ietf_swima_attr_req_create(req_data[_i].flags,
									  req_data[_i].request_id,
									  req_data[_i].earliest_eid);
	ck_assert(attr);

	type = attr->get_type(attr);
	ck_assert(pen_type_equals(type, pen_type));

	ck_assert(attr->get_noskip_flag(attr) == FALSE);
	attr->set_noskip_flag(attr, TRUE);
	ck_assert(attr->get_noskip_flag(attr) == TRUE);

	for (n = 0; n < req_data[_i].sw_id_count; n++)
	{
		sw_id = chunk_from_str(sw_id_str[n]);
		swima_sw_id = swima_sw_id_create(sw_id, chunk_empty),
		c_attr = (ietf_swima_attr_req_t*)attr;
		c_attr->add_target(c_attr, swima_sw_id);
	}
	attr->build(attr);
	attr->build(attr);
	value = attr->get_value(attr);
	ck_assert_chunk_eq(value, req_data[_i].value);

	attr1 = attr->get_ref(attr);
	attr->destroy(attr);

	attr2 = ietf_swima_attr_req_create_from_data(value.len, value);
	ck_assert(attr2);

	attr1->destroy(attr1);
	ck_assert(attr2->process(attr2, &offset) == SUCCESS);

	type = attr2->get_type(attr2);
	ck_assert(pen_type_equals(type, pen_type));

	c_attr = (ietf_swima_attr_req_t*)attr2;
	ck_assert(c_attr->get_flags(c_attr) == req_data[_i].flags);
	ck_assert(c_attr->get_request_id(c_attr) == req_data[_i].request_id);
	ck_assert(c_attr->get_earliest_eid(c_attr) == req_data[_i].earliest_eid);

	enumerator = c_attr->create_target_enumerator(c_attr);
	ck_assert(enumerator);
	n = 0;
	while (enumerator->enumerate(enumerator, &swima_sw_id))
	{
		sw_id = swima_sw_id->get_sw_id(swima_sw_id, NULL);
		ck_assert(chunk_equals(sw_id, chunk_from_str(sw_id_str[n++])));
	}
	enumerator->destroy(enumerator);

	attr2->destroy(attr2);
}
END_TEST

START_TEST(test_imcv_swima_sw_req_trunc)
{
	pa_tnc_attr_t *attr;
	chunk_t data;
	uint32_t offset = 100;

	/* Data smaller than minimum size */
	attr = ietf_swima_attr_req_create_from_data(0, chunk_empty);
	ck_assert(attr);
	ck_assert(attr->process(attr, &offset) == FAILED && offset == 0);
	attr->destroy(attr);

	/* Truncate first SW ID */
	data = req_data[2].value;
	data.len = 14;
	attr = ietf_swima_attr_req_create_from_data(data.len, data);
	ck_assert(attr);
	ck_assert(attr->process(attr, &offset) == FAILED && offset == 12);
	attr->destroy(attr);

	/* Truncate second SW ID */
	data = req_data[2].value;
	data.len = 47;
	attr = ietf_swima_attr_req_create_from_data(data.len, data);
	ck_assert(attr);
	ck_assert(attr->process(attr, &offset) == FAILED && offset == 45);
	attr->destroy(attr);

	/* Segmentation */
	data = req_data[2].value;
	data.len = 50;
	attr = ietf_swima_attr_req_create_from_data(req_data[2].value.len, data);
	ck_assert(attr);
	ck_assert(attr->process(attr, &offset) == NEED_MORE);
	data = chunk_skip(req_data[2].value, 50);
	attr->add_segment(attr, data);
	ck_assert(attr->process(attr, &offset) == SUCCESS);
	attr->destroy(attr);
}
END_TEST

Suite *imcv_swima_suite_create()
{
	Suite *s;
	TCase *tc;

	s = suite_create("imcv_swima");

	tc = tcase_create("sw_id");
	tcase_add_test(tc, test_imcv_swima_sw_id);
	suite_add_tcase(s, tc);

	tc = tcase_create("sw_req");
	tcase_add_loop_test(tc, test_imcv_swima_sw_req, 0, countof(req_data));
	suite_add_tcase(s, tc);

	tc = tcase_create("sw_req_trunc");
	tcase_add_test(tc, test_imcv_swima_sw_req_trunc);
	suite_add_tcase(s, tc);

	return s;
}
