#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <dcl.h>
#include "dcl_p.h"

/*! Find the information associated with a command verb.
  - Locate all parameters and qualifiers defined for the command
  and load the associated information included for those params
  and qualifiers.
  - Locate a handler routine for the command.

  ** NOTE - findVerbInfo will recurse to find the command parts. **

 \param node [in] A pointer to a child node of a command verb node
                  found in the xml document.
 \param cmd [in] A pointer to a dclCommand structure.

*/

static void findVerbInfo(xmlNodePtr node, dclCommandPtr cmd)
{

  /* If the node ia a paramter (i.e. <parameter ... />) */

  if (node->name && (strcasecmp(node->name, "parameter") == 0)) {
    struct _xmlAttr *propNode;

    /* allocate an empty parameter structure */

    dclParameterPtr parameter = memset(malloc(sizeof(dclParameter)), 0, sizeof(dclParameter));

    /* Look at all the properties of the parameter */

    for (propNode = node->properties; propNode; propNode = propNode->next) {

      /* If this is a label property (i.e. <parameter label=gub ... />)
         duplicate the label value into the dclParameter structure. */

      if (propNode->name &&
	  (strcasecmp(propNode->name, "label") == 0) &&
	  propNode->children && propNode->children->content) {
	parameter->label = strdup(propNode->children->content);

	/* else if this is a name property duplicate the name value */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "name") == 0) &&
		 propNode->children && propNode->children->content) {
	parameter->name = strdup(propNode->children->content);

	/* else if this is a required property set required
	   if value is True. */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "required") == 0) &&
		 propNode->children && propNode->children->content) {
	parameter->required = strcasecmp(propNode->children->content, "True") == 0;

	/* else if this is a type property duplicate the type value
	   unless the type is "REST_OF_LINE" which signifies the parameter
	   should consume the rest of the line. In the rest of line case
	   the useRestOfLine flag is set.

	   ** Note this indicates an alternate command definition based
	   on the value specified by the user for this parameter.
	   The cmd definition may be completely replaced later when
	   user input is analyzed. ** */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "type") == 0) &&
		 propNode->children && propNode->children->content) {
	if (strcasecmp(propNode->children->content, "rest_of_line") == 0) {
	  parameter->useRestOfLine = 1;
	} else {
	  parameter->type = strdup(propNode->children->content);
	}

	/* else if this ia a prompt property duplicate into the parameter
	   structure. The prompt may be sent to the user if he failed
	   to provide this parameter in the command string. */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "prompt") == 0) &&
		 propNode->children && propNode->children->content) {
	parameter->prompt = strdup(propNode->children->content);

	/* else if this ia a list property set listOk in the parameter
	   if value is "True". When processing a command if the user
	   provided multiple values for the parameter a syntax error
	   will be issued. */
      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "list") == 0) &&
		 propNode->children &&
		 propNode->children->content &&
		 (strcasecmp(propNode->children->content, "true") == 0)) {
	parameter->listOk = 1;
      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "default") == 0) &&
		 propNode->children && propNode->children->content) {
	char *value = strdup(propNode->children->content);
	char *c;
	for (c = strchr(value, ','); c; c = strchr(value, ',')) {
	  *c = 0;
	  if (parameter->values) {
	    parameter->values =
		realloc(parameter->values, (parameter->value_count + 1) * sizeof(char *));
	  } else {
	    parameter->values = malloc(sizeof(char *));
	  }
	  parameter->values[parameter->value_count++] = strdup(value);
	}
	if (parameter->values) {
	  parameter->values =
	      realloc(parameter->values, (parameter->value_count + 1) * sizeof(char *));
	} else {
	  parameter->values = malloc(sizeof(char *));
	}
	parameter->values[parameter->value_count++] = strdup(value);
	free(value);
      }
    }

    /* Add this parameter to the dclCommand struture. */

    if (cmd->parameter_count == 0)
      cmd->parameters = malloc(sizeof(dclParameterPtr));
    else
      cmd->parameters =
	  realloc(cmd->parameters, sizeof(dclParameterPtr) * (cmd->parameter_count + 1));
    cmd->parameters[cmd->parameter_count] = parameter;
    cmd->parameter_count++;

    /* else if this is a qualifier node (i.e. <qualifier ... />) */

  } else if (node->name && (strcasecmp(node->name, "qualifier") == 0)) {

    /* allocate an empty dclQualifier structure. */

    dclQualifierPtr qualifier = memset(malloc(sizeof(dclQualifier)), 0, sizeof(dclQualifier));
    struct _xmlAttr *propNode;

    /* examine all the properties of this node */

    for (propNode = node->properties; propNode; propNode = propNode->next) {

      /* if this is a name property, duplicate to the qualifier structure. */
      if (propNode->name &&
	  (strcasecmp(propNode->name, "name") == 0) &&
	  propNode->children && propNode->children->content) {
	qualifier->name = strdup(propNode->children->content);

	/* else if this is a defaulted property, set the isDefault
	   flag if value is "True". If isDefault is true when
	   processing the command, this qualifier will be included unless
	   preceded by "no" */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "defaulted") == 0) &&
		 propNode->children && propNode->children->content) {
	qualifier->isDefault = strcasecmp(propNode->children->content, "True") == 0;

	/* else if this is a required property, set the valueRequired flag
	   if the value is "True". If valueRequired is true when
	   processing the command, this will check to make sure the user
	   provided a value with the qualifier (i.e. /qualifier=value).
	 */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "required") == 0) &&
		 propNode->children && propNode->children->content) {
	qualifier->valueRequired = strcasecmp(propNode->children->content, "True") == 0;

	/* else if this is a nonnegatable property, set the nonnegateable flag
	   if the value is "True". If nonnegatable is set then the user cannot
	   prefix the qualifier with "no" (i.e. cmd /nogub is disallowed). */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "nonnegatable") == 0) &&
		 propNode->children && propNode->children->content) {
	qualifier->nonnegatable = strcasecmp(propNode->children->content, "True") == 0;

	/* else if this is a list property, set the listOk flag if the value
	   is "True". The listOk flag indicates whether more than one value
	   is permitted with the qualifier. (i.e. cmd /gub=(val1,val2,val3)) */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "list") == 0) &&
		 propNode->children && propNode->children->content) {
	qualifier->listOk = strcasecmp(propNode->children->content, "True") == 0;

	/* else if this ia a type property, duplicate the type value in the
	   qualifier struct. The type property is used when processing the qualifier
	   to check for valid values or whether the value(s) should be converted
	   to integer during cli queries. */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "type") == 0) &&
		 propNode->children && propNode->children->content) {
	qualifier->type = strdup(propNode->children->content);

	/* else if this is a default property duplicate the value into
	   the defaultValue qualifier struct member. This will be used
	   to specify a default value if the user did not specify a value
	   with the qualifier */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "default") == 0) &&
		 propNode->children && propNode->children->content) {
	qualifier->defaultValue = strdup(propNode->children->content);

	/* else if this is a syntax property duplicate the syntax name
	   into the qualifier structure. This will cause a command syntax
	   replacement during command processing if the user included
	   this qualifier in the command. */

      } else if (propNode->name &&
		 (strcasecmp(propNode->name, "syntax") == 0) &&
		 propNode->children && propNode->children->content) {
	qualifier->syntax = strdup(propNode->children->content);
      }
    }
    /* Add the qualifier to the command definition */
    if (cmd->qualifier_count == 0)
      cmd->qualifiers = malloc(sizeof(dclQualifierPtr));
    else
      cmd->qualifiers =
	  realloc(cmd->qualifiers, sizeof(dclQualifierPtr) * (cmd->qualifier_count + 1));
    cmd->qualifiers[cmd->qualifier_count] = qualifier;
    cmd->qualifier_count++;

    /* else if this is a routine node (i.e. <routine ... /> 
       duplicate the routine name into the command routine element. */

  } else if (node->name && (strcasecmp(node->name, "routine") == 0)) {
    if (node->properties && node->properties->children && node->properties->children->content) {
      if (cmd->routine)
	free(cmd->routine);
      cmd->routine = strdup(node->properties->children->content);
    }
  }

  /* if this node has a next sibling, process it next */

  if (node->next != NULL)
    findVerbInfo(node->next, cmd);
}

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

static void findEntity(xmlNodePtr node, char *category, char *name, dclNodeListPtr list,
		       int *exactFound)
{

  /* If exact match already found just return */

  if (*exactFound)
    return;

  /* else if all the characters in the name argument match the property content consider it a match. */

  else if (node->name &&
	   (strcasecmp(node->name, category) == 0) &&
	   ((name == NULL) ||
	    (node->properties &&
	     node->properties->children &&
	     node->properties->children->content &&
	     (strncasecmp(name, node->properties->children->content, strlen(name)) == 0)))) {

    /* Check if it is an exact match */
    if ((name != NULL) && (strlen(name) == strlen(node->properties->children->content))) {	// if exact command match use it!

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
  for (line = strsep(&hlp, "\n"); line; line = strsep(&hlp, "\n")) {
    if (strlen(line) > 0) {
      char *nline;
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
      ans = strcat(realloc(ans, strlen(ans) + 2), "\n");
    }
  }
  free(help);
  return ans;
}

int mdsdcl_do_help(char *command)
{
  int status = CLI_STS_IVVERB;
  char *prompt = 0;
  char *error = 0;
  char *output = strcpy(malloc(1), "");
  dclDocListPtr doc_l;
  int helpFound = 0;
  dclDocListPtr dclDocs = mdsdcl_getdocs();
  if (dclDocs == NULL)
    mdsdclAddCommands("mdsdcl_commands", &error);
  for (doc_l = dclDocs; (status == CLI_STS_IVVERB) && (doc_l != NULL); doc_l = doc_l->next) {
    int exactFound = 0;
    dclNodeList matchingHelp = { 0, 0 };
    if (command != 0) {
      findEntity(((xmlDocPtr) doc_l->doc)->children, "help", command, &matchingHelp, &exactFound);
      if ((matchingHelp.count == 0) || (matchingHelp.count > 1)) {
	if (matchingHelp.nodes != NULL)
	  free(matchingHelp.nodes);
	status = CLI_STS_IVVERB;
      } else {
	char *content = ((xmlNodePtr) matchingHelp.nodes[0])->children->content;
	if (content != NULL) {
	  char *help = formatHelp(content);
	  helpFound = 1;
	  output = strcat(realloc(output, strlen(output) + strlen(help) + 1), help);
	  free(help);
	}
	free(matchingHelp.nodes);
	break;
      }
    } else {
      findEntity(((xmlDocPtr) doc_l->doc)->children, "helpall", 0, &matchingHelp, &exactFound);
      if ((matchingHelp.count == 1) &&
	  matchingHelp.nodes &&
	  matchingHelp.nodes[0] &&
	  ((xmlNodePtr) matchingHelp.nodes[0])->children &&
	  ((xmlNodePtr) matchingHelp.nodes[0])->children->content) {
	char *content = ((xmlNodePtr) matchingHelp.nodes[0])->children->content;
	char *help = formatHelp(content);
	helpFound = 1;
	output = strcat(realloc(output, strlen(output) + strlen(help) + 1), help);
	free(help);
      };
      if ((matchingHelp.count > 0) && (matchingHelp.nodes))
	free(matchingHelp.nodes);
    }
    output = strcat(realloc(output, strlen(output) + 3), "\n\n");
  }
  if (helpFound == 0) {
    if (output)
      free(output);
    output=strdup("No help available for that command.");
  }
  if (command == NULL)
    output =
	strcat(realloc(output, strlen(output) + 80), "Type 'help command-name' for more info\n\n");
  printf(output);
  printf("\n");
  free(output);
  return 1;
}
