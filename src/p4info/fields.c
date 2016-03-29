/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "PI/p4info/fields.h"
#include "p4info/p4info_struct.h"
#include "pi_int.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct _field_data_s {
  char *name;
  pi_p4_id_t field_id;
  size_t bitwidth;
  char byte0_mask;
} _field_data_t;

static size_t get_field_idx(pi_p4_id_t field_id) {
  assert(PI_GET_TYPE_ID(field_id) == PI_FIELD_ID);
  return field_id & 0xFFFF;
}

static _field_data_t *get_field(const pi_p4info_t *p4info,
                                pi_p4_id_t field_id) {
  size_t field_idx = get_field_idx(field_id);
  assert(field_idx < p4info->num_fields);
  return &p4info->fields[field_idx];
}

void pi_p4info_field_init(pi_p4info_t *p4info, size_t num_fields) {
  p4info->num_fields = num_fields;
  p4info->fields = calloc(num_fields, sizeof(_field_data_t));
  p4info->field_name_map = (Pvoid_t) NULL;
}

void pi_p4info_field_free(pi_p4info_t *p4info) {
  for (size_t i = 0; i < p4info->num_fields; i++) {
    _field_data_t *field = &p4info->fields[i];
    if (!field->name) continue;
    free(field->name);
  }
  free(p4info->fields);
  Word_t Rc_word;
  JSLFA(Rc_word, p4info->field_name_map);
}

static char get_byte0_mask(size_t bitwidth) {
  if (bitwidth % 8 == 0) return 0xff;
  int nbits = bitwidth % 8;
  return ((1 << nbits) - 1);
}

void pi_p4info_field_add(pi_p4info_t *p4info, pi_p4_id_t field_id,
                         const char *name, size_t bitwidth) {
  _field_data_t *field = get_field(p4info, field_id);
  field->name = strdup(name);
  field->field_id = field_id;
  field->bitwidth = bitwidth;
  field->byte0_mask = get_byte0_mask(bitwidth);

  Word_t *field_id_ptr;
  JSLI(field_id_ptr, p4info->field_name_map, (const uint8_t *) field->name);
  *field_id_ptr = field_id;
}

pi_p4_id_t pi_p4info_field_id_from_name(const pi_p4info_t *p4info,
                                        const char *name) {
  Word_t *field_id_ptr;
  JSLG(field_id_ptr, p4info->field_name_map, (const uint8_t *) name);
  assert (field_id_ptr);
  return *field_id_ptr;
}

const char *pi_p4info_field_name_from_id(const pi_p4info_t *p4info,
                                         pi_p4_id_t field_id) {
  _field_data_t *field = get_field(p4info, field_id);
  return field->name;
}

size_t pi_p4info_field_bitwidth(const pi_p4info_t *p4info,
                                pi_p4_id_t field_id) {
  _field_data_t *field = get_field(p4info, field_id);
  return field->bitwidth;
}

char pi_p4info_field_byte0_mask(const pi_p4info_t *p4info,
                                pi_p4_id_t field_id) {
  _field_data_t *field = get_field(p4info, field_id);
  return field->byte0_mask;
}