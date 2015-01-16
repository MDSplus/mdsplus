#ifndef DCL_H
#define DCL_H

typedef struct dclValue {
  char *value;
  char *restOfLine;
} dclValue, *dclValuePtr;

typedef struct dclValueList {
  int count;			/*!< number of values in values array */
  char *restOfLine;		/*!< rest of command line */
  char **values;		/*!< array of values */
} dclValueList, *dclValueListPtr;

typedef struct dclParameter {
  char *name;			/*!< Name of parameter for cli queries */
  char *prompt;			/*!< Prompt to present if interactive and parameter is missing but required */
  char *label;			/*!< Alternate name for cli queries */
  char *type;			/*!< Used for syntax switching */
  int required;			/*!< 1 if parameter is required */
  int value_count;		/*!< number of value string present */
  int value_idx;                /*!< index of next value to return */
  char **values;		/*!< array of values strings */
  char *restOfLine;		/*!< rest of command line */
  int useRestOfLine;		/*!< parameter uses rest of line */
  int listOk;			/*!< 1 if more than one value is allowed */
} dclParameter, *dclParameterPtr;

typedef struct dclQualifier {
  char *name;			/*!< Name of qualifier for cli queries */
  int value_count;		/*!< Number of value strings present */
  int value_idx;                /*|< index of next value to return */
  char **values;		/*!< array of value string */
  int valueRequired;		/*!< 1 if value is required for this parameter */
  int isDefault;		/*!< 1 if the qualifier should be included by default */
  int nonnegatable;		/*!< 1 if the qualifier is not negatable. i.e. invalid if preceded by no */
  int negated;			/*!< 1 if the qualifier was include preceded by "no" */
  int listOk;			/*!< 1 if more than one value is allowed */
  char *defaultValue;		/*!< default value if none provided */
  char *type;			/*!< refers to a type of value (i.e. number, usage_type, logging_type) */
  char *syntax;			/*!< triggers syntax switching */
} dclQualifier, *dclQualifierPtr;

typedef struct dclCommand {
  char *command_line;		/*!< full command line */
  char *verb;			/*!< verb name of command */
  int parameter_count;		/*!< number of parameters */
  dclParameterPtr *parameters;	/*!< array of parameters */
  int qualifier_count;		/*!< number of qualifiers */
  dclQualifierPtr *qualifiers;	/*!< array of qualifiers */
  char *routine;		/*!< routine providing implementation of the command */
} dclCommand, *dclCommandPtr;

typedef union YYSTYPE {		/*!< Used for types used during bison/flex parsing of commands */
  char *str;
  dclCommandPtr cmd;
  dclQualifierPtr qualifier;
  dclValueListPtr value_list;
  dclParameterPtr parameter;
  dclValuePtr pvalue;
} YYSTYPE;

/*** eventually turn these into mdsplus statuses ***/

#define CLI_STS_NORMAL 0
#define CLI_STS_IVVERB -1
#define AMBIGUOUS_COMMAND -2
#define TRY_NEXT_COMMAND -3
#define PROMPT_FOR_MORE_INPUT -4
#define MISSING_QUALIFIER_VALUE -5
#define CLI_STS_IVQUAL -6
#define TOO_MANY_PARAMETERS -7
#define TOO_MANY_VALUES_FOR_QUALIFIER -8
#define TOO_MANY_VALUES_FOR_PARAMETER -9
#define CLI_STS_NOTNEGATABLE -10
#define CLI_STS_ABSENT 2
#define CLI_STS_PRESENT 1
#define CLI_STS_NEGATED 20

#endif
