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
#include <mdsplus/mdsconfig.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <dcl.h>
#include <mdsdcl_messages.h>
#include "dcl_p.h"

/*| Find all nodes in the xml hierarchy with an xml "name" matching the category argument with it's content matching
   the name argument stopping if there is an exact match. The matching is case insensitive and only checks the number
   of characters provided by the name parameter. If their is a match and the node property has exacly the same
   length as the name parameter then it is considered as an exact match.

   \param node [in] Node to begin search with.
   \param category [in] XML Node name to search for (i.e. "verb" would look for xml <verb .../> items).
   \param name [in] Value to match with the content of the first property of the node found based on category.
   \param list [in] Pointer to a dclNodList which describes an array of xmlNodes
   \param exactFound [in,out] Pointer to an int flag which is set if the name parameter exactly matches the
          property of the xml node.

   *** NOTE: This is only applicable for use on xml nodes which look like <category name="name"/> ***
   *** NOTE: This routine recurses on node siblings and children. ****
   */

static void findEntity(xmlNodePtr node, const char *category, const char *name, dclNodeListPtr list,
		       int *exactFound)
{

  /* If exact match already found just return */

  if (*exactFound)
    return;

  /* else if all the characters in the name argument match the property content consider it a match. */

  else if (node->name &&
	   (strcasecmp((const char *)node->name, category) == 0) &&
	   ((name == NULL) ||
	    (node->properties &&
	     node->properties->children &&
	     node->properties->children->content &&
	     (strncasecmp(name, (const char *)node->properties->children->content, strlen(name)) ==
	      0)))) {

    /* Check if it is an exact match */
    if ((name != NULL) && (strlen(name) == strlen((const char *)node->properties->children->content))) {	// if exact command match use it!

      /* if already found other nodes but not exact match then free the "array" of nodes. */

      if (list->count > 0 && list->nodes)
	free(list->nodes);

      /* allocate memory for one node ptr and load this node and return */
      list->nodes = malloc(sizeof(xmlNodePtr));
      list->nodes[0] = node;
      list->count = 1;
      *exactFound = 1;
      return;
    }

    /* Add this node to the list of nodes */

    if (list->count == 0) {
      list->nodes = malloc(sizeof(xmlNodePtr));
    } else {
      list->nodes = realloc(list->nodes, sizeof(xmlNodePtr) * (list->count + 1));
    }
    list->nodes[list->count] = node;
    list->count++;
  }

  /* If this node has a sibling see if it matches. */

  if (node->next)
    findEntity(node->next, category, name, list, exactFound);

  /* If this node has children see if they match. */

  if (node->children)
    findEntity(node->children, category, name, list, exactFound);
}

static char *formatHelp(char *content)
{
  int indentation = -1;
  int offset;
  char *ans = strdup("");
  char *help = strdup(content);
  char *hlp = help;
  char *line;
  int leading = 1;
#ifdef HAVE_STRSEP
  for (line = strsep(&hlp, "\n"); line; line = strsep(&hlp, "\n")) {
#else
  char *saveptr = 0;
  for (line = strtok_r(hlp, "\n", &saveptr); line; line = strtok_r(0, "\n", &saveptr)) {
#endif
    if ((strlen(line) > 0) && (strspn(line, " \t") != strlen(line))) {
      char *nline;
      leading = 0;
      if (indentation == -1) {
	for (indentation = 0; line[indentation] == ' '; indentation++) ;
	offset = indentation - 2;
      }
      if (offset < 0) {
	nline = strcpy(malloc(strlen(line) - offset + 1), "  ");
	strcat(nline, line);
      } else {
	nline = strcpy(malloc(strlen(line) - offset + 1), line + offset);
      }
      ans = strcat(realloc(ans, strlen(ans) + strlen(nline) + 2), nline);
      free(nline);
      strcat(ans, "\n");
    } else {
      if (leading == 0)
	ans = strcat(realloc(ans, strlen(ans) + 2), "\n");
    }
  }
  free(help);
  return ans;
}

int mdsdcl_do_help(const char *command, char **error, char **output)
{
  int status = MdsdclIVVERB;
  dclDocListPtr doc_l;
  int helpFound = 0;
  dclDocListPtr dclDocs = mdsdcl_getdocs();
  int docIdx;
  xmlDocPtr *docs;
  int numDocs = 0;
  for (doc_l = dclDocs; doc_l != NULL; doc_l = doc_l->next, numDocs++) ;
  docs = malloc(sizeof(void *) * numDocs);
  for (doc_l = dclDocs, docIdx = numDocs - 1; doc_l != NULL; doc_l = doc_l->next, docIdx--)
    docs[docIdx] = doc_l->doc;
  *output = strdup("\n");
  for (docIdx = 0; (status == MdsdclIVVERB) && (docIdx < numDocs); docIdx++) {
    xmlDocPtr doc = docs[docIdx];
    int exactFound = 0;
    dclNodeList matchingHelp = { 0, 0 };
    if (command != 0) {
      findEntity(doc->children, "help", command, &matchingHelp, &exactFound);
      if (matchingHelp.count == 0) {
	status = MdsdclIVVERB;
      } else {
	int i;
	for (i = 0; i < matchingHelp.count; i++) {
	  char *content = (char *)(((xmlNodePtr) matchingHelp.nodes[i])->children->content);
	  if (content != NULL) {
	    char *help = formatHelp(content);
	    helpFound = 1;
	    *output = strcat(realloc(*output, strlen(*output) + strlen(help) + 1), help);
	    free(help);
	  }
	}
	free(matchingHelp.nodes);
      }
    } else {
      findEntity(doc->children, "helpall", 0, &matchingHelp, &exactFound);
      if ((matchingHelp.count == 1) &&
	  matchingHelp.nodes &&
	  matchingHelp.nodes[0] &&
	  ((xmlNodePtr) matchingHelp.nodes[0])->children &&
	  ((xmlNodePtr) matchingHelp.nodes[0])->children->content) {
	char *content = (char *)(((xmlNodePtr) matchingHelp.nodes[0])->children->content);
	char *help = formatHelp(content);
	helpFound = 1;
	*output = strcat(realloc(*output, strlen(*output) + strlen(help) + 1), help);
	free(help);
      };
      if ((matchingHelp.count > 0) && (matchingHelp.nodes))
	free(matchingHelp.nodes);
    }
  }
  if (helpFound == 0) {
    if (*output) {
      strcpy(*output,"");
    }
    *error = strdup("No help available for that command.\n");
  }
  if (command == NULL)
    *output =
	strcat(realloc(*output, strlen(*output) + 80),
	       "Type 'help command-name' for more info\n\n");
  if (docs)
    free(docs);
  return 1;
}
