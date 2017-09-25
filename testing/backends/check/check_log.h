/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * Check: a unit test framework for C
 * Copyright (C) 2001,2002 Arien Malec
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

#ifndef CHECK_LOG_H
#define CHECK_LOG_H

void log_srunner_start(SRunner * sr);
void log_srunner_end(SRunner * sr);
void log_suite_start(SRunner * sr, Suite * s);
void log_suite_end(SRunner * sr, Suite * s);
void log_test_end(SRunner * sr, TestResult * tr);
void log_test_start(SRunner * sr, TCase * tc, TF * tfun);

void stdout_lfun(SRunner * sr, FILE * file, enum print_output,
                 void *obj, enum cl_event evt);

void lfile_lfun(SRunner * sr, FILE * file, enum print_output,
                void *obj, enum cl_event evt);

void xml_lfun(SRunner * sr, FILE * file, enum print_output,
              void *obj, enum cl_event evt);

void tap_lfun(SRunner * sr, FILE * file, enum print_output,
              void *obj, enum cl_event evt);

void subunit_lfun(SRunner * sr, FILE * file, enum print_output,
                  void *obj, enum cl_event evt);

void srunner_register_lfun(SRunner * sr, FILE * lfile, int close,
                           LFun lfun, enum print_output);

FILE *srunner_open_lfile(SRunner * sr);
FILE *srunner_open_xmlfile(SRunner * sr);
FILE *srunner_open_tapfile(SRunner * sr);
void srunner_init_logging(SRunner * sr, enum print_output print_mode);
void srunner_end_logging(SRunner * sr);

#endif /* CHECK_LOG_H */
