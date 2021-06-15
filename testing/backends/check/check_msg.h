/*
 * Check: a unit test framework for C
 * Copyright (C) 2001, 2002 Arien Malec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CHECK_MSG_NEW_H
#define CHECK_MSG_NEW_H

/* Functions implementing messaging during test runs */

void send_failure_info(const char *msg, int rtype);
void send_loc_info(const char *file, int line);
void send_ctx_info(enum ck_result_ctx ctx);
void send_duration_info(int duration);

TestResult *receive_test_result(int waserror);

void setup_messaging(void);
void teardown_messaging(void);

FILE *open_tmp_file(char **name);

#endif /*CHECK_MSG_NEW_H */
