/* Copyright 2013-present Barefoot Networks, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

int _logs_on = 1;


void pi_logs_on() { _logs_on = 1; }

void pi_logs_off() { _logs_on = 0; }
