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

#include "lib/libcompat.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "check.h"
#include "check_list.h"
#include "check_impl.h"
#include "check_str.h"
#include "check_print.h"

static void srunner_fprint_summary(FILE * file, SRunner * sr,
                                   enum print_output print_mode);
static void srunner_fprint_results(FILE * file, SRunner * sr,
                                   enum print_output print_mode);


void srunner_print(SRunner * sr, enum print_output print_mode)
{
    srunner_fprint(stdout, sr, print_mode);
}

void srunner_fprint(FILE * file, SRunner * sr, enum print_output print_mode)
{
    if(print_mode == CK_ENV)
    {
        print_mode = get_env_printmode();
    }

    srunner_fprint_summary(file, sr, print_mode);
    srunner_fprint_results(file, sr, print_mode);
}

static void srunner_fprint_summary(FILE * file, SRunner * sr,
                                   enum print_output print_mode)
{
#if ENABLE_SUBUNIT
    if(print_mode == CK_SUBUNIT)
        return;
#endif

    if(print_mode >= CK_MINIMAL)
    {
        char *str;

        str = sr_stat_str(sr);
        fprintf(file, "%s\n", str);
        free(str);
    }
    return;
}

static void srunner_fprint_results(FILE * file, SRunner * sr,
                                   enum print_output print_mode)
{
    List *resultlst;

#if ENABLE_SUBUNIT
    if(print_mode == CK_SUBUNIT)
        return;
#endif

    resultlst = sr->resultlst;

    for(check_list_front(resultlst); !check_list_at_end(resultlst);
        check_list_advance(resultlst))
    {
        TestResult *tr = (TestResult *)check_list_val(resultlst);

        tr_fprint(file, tr, print_mode);
    }
    return;
}

void fprint_xml_esc(FILE * file, const char *str)
{
    for(; *str != '\0'; str++)
    {

        switch (*str)
        {

                /* handle special characters that must be escaped */
            case '"':
                fputs("&quot;", file);
                break;
            case '\'':
                fputs("&apos;", file);
                break;
            case '<':
                fputs("&lt;", file);
                break;
            case '>':
                fputs("&gt;", file);
                break;
            case '&':
                fputs("&amp;", file);
                break;

                /* regular characters, print as is */
            default:
                fputc(*str, file);
                break;
        }
    }
}

void tr_fprint(FILE * file, TestResult * tr, enum print_output print_mode)
{
    if(print_mode == CK_ENV)
    {
        print_mode = get_env_printmode();
    }

    if((print_mode >= CK_VERBOSE && tr->rtype == CK_PASS) ||
       (tr->rtype != CK_PASS && print_mode >= CK_NORMAL))
    {
        char *trstr = tr_str(tr);

        fprintf(file, "%s\n", trstr);
        free(trstr);
    }
}

void tr_xmlprint(FILE * file, TestResult * tr,
                 enum print_output print_mode CK_ATTRIBUTE_UNUSED)
{
    char result[10];
    char *path_name = NULL;
    char *file_name = NULL;
    char *slash = NULL;

    switch (tr->rtype)
    {
        case CK_PASS:
            snprintf(result, sizeof(result), "%s", "success");
            break;
        case CK_SKIP:
            snprintf(result, sizeof(result), "%s", "skip");
            break;            
        case CK_FAILURE:
            snprintf(result, sizeof(result), "%s", "failure");
            break;
        case CK_ERROR:
            snprintf(result, sizeof(result), "%s", "error");
            break;
        case CK_TEST_RESULT_INVALID:
        default:
            abort();
            break;
    }

    if(tr->file)
    {
        slash = strrchr(tr->file, '/');
        if(slash == NULL)
        {
            slash = strrchr(tr->file, '\\');
        }

        if(slash == NULL)
        {
            path_name = strdup(".");
            file_name = tr->file;
        }
        else
        {
            path_name = strdup(tr->file);
            path_name[slash - tr->file] = 0;    /* Terminate the temporary string. */
            file_name = slash + 1;
        }
    }


    fprintf(file, "    <test result=\"%s\">\n", result);
    fprintf(file, "      <path>%s</path>\n",
            (path_name == NULL ? "" : path_name));
    fprintf(file, "      <fn>%s:%d</fn>\n",
            (file_name == NULL ? "" : file_name), tr->line);
    fprintf(file, "      <id>%s</id>\n", tr->tname);
    fprintf(file, "      <iteration>%d</iteration>\n", tr->iter);
    fprintf(file, "      <duration>%d.%06d</duration>\n",
            tr->duration < 0 ? -1 : tr->duration / US_PER_SEC,
            tr->duration < 0 ? 0 : tr->duration % US_PER_SEC);
    fprintf(file, "      <description>");
    fprint_xml_esc(file, tr->tcname);
    fprintf(file, "</description>\n");
    fprintf(file, "      <message>");
    fprint_xml_esc(file, tr->msg);
    fprintf(file, "</message>\n");
    fprintf(file, "    </test>\n");

    free(path_name);
}

enum print_output get_env_printmode(void)
{
    char *env = getenv("CK_VERBOSITY");

    if(env == NULL)
        return CK_NORMAL;
    if(strcmp(env, "silent") == 0)
        return CK_SILENT;
    if(strcmp(env, "minimal") == 0)
        return CK_MINIMAL;
    if(strcmp(env, "verbose") == 0)
        return CK_VERBOSE;
    return CK_NORMAL;
}
