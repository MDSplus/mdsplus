#include <stdio.h>
#include <mdsdescrip.h>
#include <strroutines.h>
#include <libroutines.h>
#include <stdlib.h>
#include <string.h>
#include <treeshr.h>
#include "gen_device.h"
static char prev_name[512];
int wrong = 0;
char *file_name, *lower_device_name, *upper_device_name, *routine;
NodeDescr *bottom_node_list;
extern int gen_deviceparse();
int set_offset;
int single_qual = 0;
int add_qual = 0;
int is_first = 1;
char *suffix = "\0";
FILE *descr_file=0;
FILE *diag_file=0;
FILE *file_h=0;
FILE *out_file=0;    
void Error(char *);

static void UpdateLibrary();

#define __tolower(c) (((c) >= 'A' && (c) <= 'Z') ? (c) | 0x20 : (c))
#define __toupper(c) (((c) >= 'a' && (c) <= 'z') ? (c) & 0xDF : (c))

static void FreeInList();

int main(int argc, char **argv)
{
    char *options;
    int status, i, next_arg;
    file_name = argv[1];
    if((descr_file = fopen(file_name, "r")) == 0) Error("Cannot open input file");
    options = argc > 1 ? argv[2] : "";
    for (i=0,next_arg=2;i<strlen(options);i++)
    {
      switch(options[i])
      {
      case 's': single_qual = 1; break;
      case 'x': { next_arg++; suffix = next_arg < argc ? argv[next_arg] : ""; break;}
      case 'a': add_qual = 1; break;
      default:
      }
    }
    TreeOpen("main",-1,0);
    printf("Parsing device description\n");
    gen_deviceparse();

    if(!wrong)
	fclose(file_h);

    if(single_qual)
	fclose(out_file);
    fclose(descr_file);
    TreeClose(0,0);
  return 0;
}
    


void DevGenAdd(char *device, char *library, NodeDescr *bottom)
{
  int i;
    static char *add =  {"_add.c"};
    static char *dotc = {".c"};
    static char *zero = {"0"};
    static char *parameters = 
    {"(struct descriptor *name_d_ptr, struct descriptor *dummy_d_ptr, int *nid_ptr)\n"};  
    static char *test_status = {"\tif (!(status & 1)) return status;"};
    char *file_name = malloc(strlen(device) + strlen(suffix) + 7);
    char *routine;

    NodeDescr *curr_node;
    int num_nodes;
    lower_device_name = malloc(strlen(device) + 1);
    for (i=0;i<strlen(device);i++) lower_device_name[i] = __tolower(device[i]);
    lower_device_name[i]=0;
    upper_device_name = malloc(strlen(device) + 1);
    for (i=0;i<strlen(device);i++) upper_device_name[i] = __toupper(device[i]);
    upper_device_name[i]=0;
    if(!single_qual)
    {
	strcpy(file_name, lower_device_name);
	strcat(file_name, add);
    }
    else
    {
	strcpy(file_name, lower_device_name);
	strcat(file_name, suffix);    
	strcat(file_name, dotc);    
    }    
    printf("Building %s\n",file_name);    

    if((out_file = fopen(file_name, "w")) == 0)
	Error("Error opening file");

    fprintf(out_file, "#include <mds_gendevice.h>\n");
    fprintf(out_file, "#include \"%s_gen.h\"\n", lower_device_name); 
    if (add_qual)
      fprintf(out_file, "\textern int %s___add(int *nid);\n",lower_device_name );
    fprintf(out_file, "int %s__add%s{\n", lower_device_name, parameters);
    fprintf(out_file, "\tstatic DESCRIPTOR(library_d, \"%s\");\n", library);
    fprintf(out_file, "\tstatic DESCRIPTOR(model_d, \"%s\");\n", upper_device_name);
    fprintf(out_file, "\tstatic DESCRIPTOR_CONGLOM(conglom_d, &library_d, &model_d, 0, 0);\n");
    fprintf(out_file, "\tint usage = TreeUSAGE_DEVICE;\n"); 
    fprintf(out_file, "\tint curr_nid, old_nid, head_nid, status;\n");
    fprintf(out_file, "\tshort flags = NciM_WRITE_ONCE;\n\tNCI_ITM flag_itm[] = {{2, NciSET_FLAGS, 0, 0},{0, 0, 0, 0}};\n");
    fprintf(out_file, "\tchar *name_ptr = strncpy(malloc(name_d_ptr->length+1),name_d_ptr->pointer,name_d_ptr->length);\n");
    fprintf(out_file, "\tflag_itm[0].pointer = (unsigned char *)&flags;\n");
    fprintf(out_file, "\tname_ptr[name_d_ptr->length]=0;\n");

    for(num_nodes = 1, curr_node = bottom; curr_node->prv; curr_node = curr_node->prv, num_nodes++);

    fprintf(out_file, "\tstatus = TreeStartConglomerate(%s_K_CONG_NODES);\n%s\n", upper_device_name, test_status);
    fprintf(out_file, "\tstatus = TreeAddNode(name_ptr, &head_nid, usage);\n%s\n\t*nid_ptr = head_nid;\n",test_status);
    fprintf(out_file, "\t status = TreeSetNci(head_nid, flag_itm);\n");
    fprintf(out_file, "\tstatus = TreePutRecord(head_nid, (struct descriptor *)&conglom_d,0);\n");
    fprintf(out_file,"%s\n\tstatus = TreeGetDefaultNid(&old_nid);\n%s\n",test_status, test_status);
    fprintf(out_file, "\tstatus = TreeSetDefaultNid(head_nid);\n%s\n", test_status);
    for(;curr_node; curr_node = curr_node->nxt)
    {
	if(!curr_node->usage)
	    curr_node->usage = zero;
	switch (curr_node->type)  {
	    case UND :  fprintf(out_file, "\tADD_NODE(%s, %s)\n", curr_node->name, curr_node->usage); break;
	    case INT :  fprintf(out_file, "\tADD_NODE_INTEGER(%s, %s, %s)\n", curr_node->name, 
			    curr_node->value, curr_node->usage); break;
	    case FLO :  fprintf(out_file, "\tADD_NODE_FLOAT(%s, %s, %s)\n", curr_node->name, 
			    curr_node->value, curr_node->usage); break;
	    case STR :  fprintf(out_file, "\tADD_NODE_STRING(%s, %s, %s)\n", curr_node->name, 
			    curr_node->value, curr_node->usage); break;
	    case EXP :  fprintf(out_file, "#define expr \"%s\"\n", curr_node->value); 
			fprintf(out_file, "\tADD_NODE_EXPR(%s, %s)\n", curr_node->name, curr_node->usage);
			fprintf(out_file, "#undef expr\n");
			break;
	    case ACT : if(curr_node->completion)
			fprintf(out_file, "\tADD_NODE_ACTION(%s, %s, %s, %s, \"%s\", %s, %s, %s)\n",curr_node->name,  
			curr_node->method, curr_node->phase, curr_node->sequence, curr_node->completion, 
			curr_node->timout, curr_node->server, curr_node->usage);
			  else
			fprintf(out_file, "\tADD_NODE_ACTION(%s, %s, %s, %s, 0, %s, %s, %s)\n",curr_node->name,  curr_node->method, 
			  curr_node->phase, curr_node->sequence, curr_node->timout, curr_node->server, curr_node->usage);

	}
	if(curr_node->usage == zero)
	    curr_node->usage = 0;
	if(curr_node->state)
	    fprintf(out_file, "\tstatus = TreeTurnOff(curr_nid);\n%s\n", test_status);
	if(curr_node->tags)
	    fprintf(out_file, "\t{ \tDESCRIPTOR(tag_d, \"%s\");\n\t\tstatus = TreeAddTag(curr_nid, &tag_d);\n\t\t%s\n\t}\n",
		curr_node->tags, test_status);
	if(curr_node->nci_flags & OPT_WRITE_ONCE) 
	    fprintf(out_file, "\tflags |= NciM_WRITE_ONCE;\n");
	if(curr_node->nci_flags & OPT_COMPRESSIBLE) 
	    fprintf(out_file, "\tflags |= NciM_COMPRESS_ON_PUT;\n");
	if(curr_node->nci_flags & OPT_NO_WRITE_MODEL) 
	    fprintf(out_file, "\tflags |= NciM_NO_WRITE_MODEL;\n");
	if(curr_node->nci_flags & OPT_NO_WRITE_SHOT) 
	    fprintf(out_file, "\tflags |= NciM_NO_WRITE_SHOT;\n");
	if(curr_node->nci_flags & OPT_INCLUDE_IN_PULSE) 
	    fprintf(out_file, "\tflags |= NciM_INCLUDE_IN_PULSE;\n");
	if(curr_node->nci_flags)
	    fprintf(out_file, "\t status = TreeSetNci(curr_nid, flag_itm);\n");
    }
    if (add_qual)
      fprintf(out_file, "\tstatus = %s___add(&head_nid);\n",lower_device_name );
    fprintf(out_file, "\tstatus = TreeEndConglomerate();\n\t%s\n\treturn(TreeSetDefaultNid(old_nid));\n}\n", test_status);
    if(!single_qual)
	fclose(out_file);
    free(file_name);
}
 

void DevGenH(char *device, char *library, NodeDescr *bottom)
{
    static char *dot_h = {"_gen.h"};
    static char curr_name[512];
    char *file_name = malloc(strlen(lower_device_name) + 7),
	 *curr_char,
	 *routine;

    NodeDescr *curr_node;
    int num_nodes, current;
    
    printf("Building %s_gen.h\n", lower_device_name); 
    strcpy(file_name, lower_device_name);
    strcat(file_name, dot_h);    
    
    if((file_h = fopen(file_name, "w")) == 0)
	Error("Error opening file");

    for(num_nodes = 1, curr_node = bottom; curr_node->prv; curr_node = curr_node->prv, num_nodes++);
    fprintf(file_h, "#define %s_K_CONG_NODES %d\n", upper_device_name, num_nodes + 1);
    fprintf(file_h, "#define %s_N_HEAD 0\n", upper_device_name);
    for(current = 1; curr_node; curr_node = curr_node->nxt, current++)
    {
	strcpy(curr_name, curr_node->name);
	for(curr_char = curr_name; *curr_char; curr_char++)
	    if((*curr_char == '.')||(*curr_char == ':'))
		*curr_char = '_';
	fprintf(file_h, "#define %s_N%s %d\n", upper_device_name, curr_name, current);
    }
    free(file_name);
}


void DevGenPartName(char *device, char *library, NodeDescr *bottom)
{

    static char *test_status = {"\tif (!(status & 1)) return status;"},
		*part_name = {"_part_name.c"};
    char *file_name = malloc(strlen(device) + 13), *routine;
    NodeDescr *curr_node;
    int num_nodes;
    char curr_name[512], *curr_char;


    if(!single_qual)
    {
	printf("Building %s_part_name.c\n", lower_device_name); 
	strcpy(file_name, lower_device_name);
	strcat(file_name, part_name);    
    
	if((out_file = fopen(file_name, "w")) == 0)
	    Error("Error opening file");
	fprintf(out_file, "#include <mds_gendevice.h>\n");
	fprintf(out_file, "#include \"%s_gen.h\"\n", lower_device_name); 
    }
    fprintf(out_file, 
"\nint %s__part_name(struct descriptor *nid_d_ptr, struct descriptor *method_d_ptr, struct descriptor *out_d)\n{\n", 
	lower_device_name);
    fprintf(out_file, "\tint element = 0, status;\n \tNCI_ITM nci_list[] = {{4, NciCONGLOMERATE_ELT, 0, 0},{0,0,0,0}};\n");
    fprintf(out_file, "\tnci_list[0].pointer = (unsigned char *)&element;\n");
    fprintf(out_file, "\tstatus = TreeGetNci(*(int *)nid_d_ptr->pointer, nci_list);\n\t%s\n", test_status);
    fprintf(out_file, "\tswitch(element)  {\n");

    for(num_nodes = 1, curr_node = bottom; curr_node->prv; curr_node = curr_node->prv, num_nodes++);
    fprintf(out_file, "\t\tcase(%s_N_HEAD + 1) : StrFree1Dx(out_d); break;\n", upper_device_name);
    for(; curr_node; curr_node = curr_node->nxt)
    {
	strcpy(curr_name, curr_node->name);
	for(curr_char = curr_name; *curr_char; curr_char++)
	    if((*curr_char == '.')||(*curr_char == ':'))
		*curr_char = '_';
	fprintf(out_file, "\t\tcase(%s_N%s + 1) : COPY_PART_NAME(%s) break;\n", upper_device_name, curr_name, curr_node->name);
    }
    fprintf(out_file, "\t\tdefault : status = TreeILLEGAL_ITEM;\n\t}\n\treturn status;\n}");
    free(file_name);
    if(!single_qual)
    {
	fclose(out_file);
    }
}                                   



NodeDescr* DevConcat(NodeDescr *node_1, NodeDescr *node_2)
{
    NodeDescr *curr_node;
    curr_node = node_2;
    while(curr_node->prv)
	curr_node = curr_node->prv;
    node_1->nxt = curr_node;
    curr_node->prv = node_1;
    return node_2;
} 

InDescr* DevInConcat(InDescr *node_1, InDescr *node_2)
{
    char abc[128];
    InDescr *curr_node;
    for(curr_node = node_1; curr_node; curr_node = curr_node->prv)
	if(!strcmp(curr_node->path_name, node_2->path_name))
	{
	    sprintf(abc, "Path name %s redeclared", node_2->path_name);
	    Error(abc);
	}
    curr_node = node_2;
    node_1->nxt = curr_node;
    curr_node->prv = node_1;
    return node_2;
} 
 
NodeDescr* DevNewNode(char *level_str, char *name, FlagsDescr *flags)
{
    static int prev_level = 0;
    static char *dot = {"."};
    NodeDescr *new_node = calloc(1, sizeof(NodeDescr));
    int i, level;
    char *curr_ptr;
    sscanf(level_str, "%d", &level);

    if((level > prev_level + 1)||(level < 1)) Error("Error in node levels");
    if((level <= prev_level) && prev_name)
    {
	curr_ptr = prev_name + strlen(prev_name) - 1;
	for(i = 0; i < level - prev_level + 1; i++)    
	{
	    while(*curr_ptr && *curr_ptr != '.')
	    {
		*curr_ptr = '\0';
		curr_ptr--;
	    }
	    if(*curr_ptr == '.')
	    {
		*curr_ptr = '\0';
		curr_ptr--;
	    }
	}
    }
    strcat(prev_name, dot);
    strcat(prev_name, name);
    new_node->name = malloc(strlen(prev_name) + 1);
    strcpy(new_node->name, prev_name);
    new_node->state = flags->state;
    new_node->nci_flags = flags->nci_flags;
    new_node->tags = flags->tags;
    free(flags);
    new_node->type = UND;
    new_node->usage = malloc(strlen("TreeUSAGE_NONE") + 1);
    strcpy(new_node->usage, "TreeUSAGE_NONE");
    prev_level = level;
    return new_node;
}
    

FlagsDescr* DevNewFlag(int state, int nci_flags, char *usage, char *tags)
{
    FlagsDescr *new_flags =  calloc(1, sizeof(FlagsDescr));
    new_flags->state = state;
    new_flags->nci_flags = nci_flags;
    if(usage)
    {
	new_flags->usage = malloc(strlen(usage) + 1);
	strcpy(new_flags->usage, usage);
    }
    new_flags->tags = tags;
    return new_flags;
}

NodeDescr* DevNewMember(char *name, void *data, FlagsDescr *flags, int type)
{
    ActionDescr *act;
    NodeDescr *new_node = calloc(1, sizeof(NodeDescr));
    static char *colon = {":"};

    new_node->name = malloc(strlen(prev_name) + strlen(name) + 2);
    if((name[0] != ':') && (name[0] != '.'))
    {
	strcpy(new_node->name, prev_name);
	strcat(new_node->name, colon);
	strcat(new_node->name, name);
    }
    else
	strcpy(new_node->name, name);
    new_node->state = flags->state;
    new_node->nci_flags = flags->nci_flags;
    new_node->tags = flags->tags;
    if(flags->usage)
	new_node->usage = flags->usage;
    else
	new_node->usage = 0;
/*    else
    {
	switch(type)  {
	    case EXP :
	    case INT : 
	    case FLO :	new_node->usage = malloc(strlen("TREE$K_USAGE_NUMERIC") + 1);
			strcpy(new_node->usage, "TREE$K_USAGE_NUMERIC"); break;
	    case STR :	new_node->usage = malloc(strlen("TREE$K_USAGE_TEXT") + 1);
			strcpy(new_node->usage, "TREE$K_USAGE_TEXT"); break;
	    case ACT :	new_node->usage = malloc(strlen("TREE$K_USAGE_ACTION") + 1);
			strcpy(new_node->usage, "TREE$K_USAGE_ACTION"); break;
	    default :	new_node->usage = malloc(strlen("TREE$K_USAGE_ANY") + 1);
			strcpy(new_node->usage, "TREE$K_USAGE_ANY"); break;
	}
    }
*/    new_node->type = type;
    if(type != ACT)
    {
	new_node->value = malloc(strlen(data)+1);
	strcpy(new_node->value, data);
    }
    else if(type != UND)
    {
	act = data;
	new_node->method = act->method;
	new_node->phase = act->phase;
	new_node->sequence = act->sequence;
	new_node->completion = act->completion;
	new_node->timout = act->timout;
	new_node->server = act->server;
	free(act);
    }
    free(name);
    if (file_h) fprintf(file_h, "\tstruct descriptor_xd *__xds;\n\tint __num_xds;\n");
return new_node;
}



ActionDescr* DevNewAction(char *method, char *timout, char *sequence, char *phase, char *server, char *completion)
{
    ActionDescr *new_act = calloc(1, sizeof(ActionDescr));

    new_act->method = malloc(strlen(method) + 1);
    strcpy(new_act->method, method);
    if(timout)
    {
	new_act->timout = malloc(strlen(timout) + 1);
	strcpy(new_act->timout, timout);
    }
    else
    {
	new_act->timout = malloc(2);
	strcpy(new_act->timout, "0");
    }
    new_act->sequence = malloc(strlen(sequence) + 1);
    strcpy(new_act->sequence, sequence);
    new_act->phase = malloc(strlen(phase) + 1);
    strcpy(new_act->phase, phase);
    new_act->server = malloc(strlen(server) + 1);
    strcpy(new_act->server, server);
    new_act->completion = completion;
    return new_act;
}


#include <ctype.h>

void DevGenOperation(char *operation, InDescr *input_list, int is_operation)
{
    InDescr *curr_in, *top_list;
    char *underscore = "_", *dot_c = ".c", low_path[512], *error_messages[512], abc[512], upper_path_name[32];
    char *out_file_name = malloc(strlen(operation) + strlen(lower_device_name) + 9);
    char *curr_char, *low_case_operation = malloc(strlen(operation) + 1),*mixed_case_operation = malloc(strlen(operation) + 1);
    char *routine;
    int xd_count, i, num_err_messages, set_count;
    
    for (i=0; operation[i] != 0; i++)
    {
      low_case_operation[i] = __tolower(operation[i]);
      mixed_case_operation[i] = __tolower(operation[i]);
    }
    low_case_operation[i]=0;
    mixed_case_operation[i]=0;
    mixed_case_operation[0]=__toupper(mixed_case_operation[0]);
    if (!single_qual)
    {
	if(is_operation)
	    printf("Building %s_%s.c\n", lower_device_name, low_case_operation); 
	else
	    printf("Building %s__%s.c\n", lower_device_name, low_case_operation); 
	strcpy(out_file_name, lower_device_name);
	strcat(out_file_name, underscore);
	if(!is_operation)
	    strcat(out_file_name, underscore);
	strcat(out_file_name, low_case_operation);
	strcat(out_file_name, dot_c);

	if((out_file = fopen(out_file_name, "w")) == 0)
	    Error("File cannot be opened");
    }
    for(curr_in = input_list, xd_count = 0; curr_in->prv; curr_in = curr_in->prv)
	if(!curr_in->type || (curr_in->type == STR) || (curr_in->type == INT_ARRAY) || (curr_in->type == FLO_ARRAY)) xd_count++;

    if(!curr_in->type || (curr_in->type == STR) || (curr_in->type == INT_ARRAY) || (curr_in->type == FLO_ARRAY)) xd_count++;

    top_list = curr_in;
    if(!single_qual)
    {
	fprintf(out_file, "#include <mds_gendevice.h>\n");
	fprintf(out_file, "#include \"%s_gen.h\"\n", lower_device_name);
    }
    fprintf(out_file, "\n\n\textern int %s___%s();\n", lower_device_name, low_case_operation);
    fprintf(out_file, 
      "#define free_xd_array { int i; for(i=0; i<%d;i++) if(work_xd[i].l_length) MdsFree1Dx(&work_xd[i],0);}\n", xd_count);


    if(!single_qual || is_first)
    {
	is_first = 0;
	if(is_operation)
	    fprintf(out_file, 
"#define error(nid,code,code1) {free_xd_array return GenDeviceSignal(nid,code,code1);}\n");
	else
	    fprintf(out_file, "#define error(nid,code,code1) error_code = code1;"); 
    }
    if(is_operation)
	fprintf(out_file, "\nint %s__%s(struct descriptor *nid_d_ptr, struct descriptor *method_d_ptr)\n{\n", 
           lower_device_name, low_case_operation);
    else
	fprintf(out_file, "\nint %s___%s(struct descriptor *nid_d_ptr, In%sStruct *in_ptr)\n{\n", 
	    lower_device_name, low_case_operation, mixed_case_operation);

    for(num_err_messages = 0; curr_in; curr_in = curr_in->nxt)
    {	
	if(curr_in->error_code)
	{
	    for(i = 0; (i < num_err_messages) && strcmp(curr_in->error_code, error_messages[i]); i++);
	    if(i >= num_err_messages)
	    {
		error_messages[num_err_messages] = malloc(strlen(curr_in->error_code) + 1);
		strcpy(error_messages[num_err_messages++], curr_in->error_code);
	    }
	}
    }    
    curr_in = top_list;

    fprintf(out_file, "\tdeclare_variables(In%sStruct)\n", mixed_case_operation);

    for(; curr_in; curr_in = curr_in->nxt)
    {
    	curr_char = curr_in->path_name;
	while(*curr_char)
	{
	    if((*curr_char == ':')||(*curr_char == '.'))
		*curr_char = '_';
	    curr_char++;
	}
	curr_char = curr_in->path_name;	
	if(*curr_char == '_')
	    for(i = 0; curr_char[i]; i++)
		curr_char[i] = curr_char[i+1];

	for(i = 0; curr_in->path_name[i]; i++)
	    low_path[i] = tolower(curr_in->path_name[i]);
	low_path[i] = '\0';	


	if(curr_in->mode == SET)
	    switch(curr_in->type) {
		case INT :
		    fprintf(out_file, "static struct {\n\tshort code;\n\tint value;} %s_t[] = {\n", low_path);
		    for(i = 0; curr_in->set[i]; i++)
			fprintf(out_file, "\t%d, %s,\n", i, curr_in->set[i]);
		    fprintf(out_file, "\t0, 0 };\n");
		    break;
		case FLO :
		    fprintf(out_file, "static struct {\n\tshort code;\n\tfloat value;} %s_t[] = {\n", low_path);
		    for(i = 0; curr_in->set[i]; i++)
			fprintf(out_file, "\t%d, %s,\n", i, curr_in->set[i]);
		    fprintf(out_file, "\t0, 0 };\n");
		    break;
		case STR :
		    fprintf(out_file, "static struct {\n\tshort code;\n\tchar *value;} %s_t[] = {\n", low_path);
		    for(i = 0; curr_in->set[i]; i++)
			fprintf(out_file, "\t%d, \"%s\",\n", i, curr_in->set[i]);
		    fprintf(out_file, "\t0, 0 };\n");
		    break;
	    }
	else if(curr_in->mode == CONV_SET)
	    switch(curr_in->type) {
		case INT :
		    fprintf(out_file, "static struct {\n\tshort code;\n\tint value;} %s_t[] = {\n", low_path);
		    for(i = 0; curr_in->set[i]; i++)
			fprintf(out_file, "\t%s, %s,\n", curr_in->codes[i], curr_in->set[i]);
		    fprintf(out_file, "\t0, 0 };\n");
		    break;	
		case FLO :
		    fprintf(out_file, "static struct {\n\tshort code;\n\tfloat value;} %s_t[] = {\n", low_path);
		    for(i = 0; curr_in->set[i]; i++)
			fprintf(out_file, "\t%s, %s,\n", curr_in->codes[i], curr_in->set[i]);
		    fprintf(out_file, "\t0, 0 };\n");
		    break;	
		case STR :
		    fprintf(out_file, "static struct {\n\tshort code;\n\tchar *value;} %s_t[] = {\n", low_path);
		    for(i = 0; curr_in->set[i]; i++)
			fprintf(out_file, "\t%s, \"%s\",\n", curr_in->codes[i], curr_in->set[i]);
		    fprintf(out_file, "\t0, 0 };\n");
		    break;	
	    }
    }
    if(xd_count)
    {
	fprintf(out_file, "\tstruct descriptor_xd work_xd[%d];\n\tint xd_count = 0;\n", xd_count);
	fprintf(out_file, "\tmemset((char *)work_xd, \'\\0\', sizeof(struct descriptor_xd) * %d);\n", xd_count);
    }
    else
	fprintf(out_file, "\tstruct descriptor_xd work_xd[1];\n", xd_count);


    fprintf(out_file, "\tinitialize_variables(In%sStruct)\n", mixed_case_operation);

    curr_in = top_list;

    fprintf(out_file, "\n");
    for(; curr_in; curr_in = curr_in->nxt)
    {

	for(set_count = 0; curr_in->set[set_count]; set_count++);
	strcpy(upper_path_name, curr_in->path_name);
	for(curr_char = curr_in->path_name; *curr_char; curr_char++) 
	    *curr_char = tolower(*curr_char);

	switch(curr_in->type)  {
	    case 0 :
		if(!curr_in->error_code)
		    fprintf(out_file, "\tread_descriptor(%s_N_%s, %s);\n", upper_device_name, upper_path_name,curr_in->path_name); 
		else
		    fprintf(out_file, "\tread_descriptor_error(%s_N_%s, %s, %s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->error_code);
		break;
	    case INT :

		if(curr_in->error_code)
		    fprintf(out_file, "\tread_integer_error(%s_N_%s, %s, %s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->error_code);
		else
		    fprintf(out_file, "\tread_integer(%s_N_%s, %s);\n", upper_device_name, upper_path_name, curr_in->path_name);
		switch(curr_in->mode)  {
		    case NONE : 
		      break;
		    case RANGE :
		      fprintf(out_file, "\tcheck_range(%s, %s, %s, %s);\n", curr_in->path_name,
			curr_in->set[0], curr_in->set[1], curr_in->error_code);
		      break;
		    case SET :
		      fprintf(out_file, "\tcheck_integer_set(%s, %s_t, %d, %s);\n",  
			curr_in->path_name, curr_in->path_name, set_count, curr_in->error_code);
		      break;
		    case CONV_SET : 
		      fprintf(out_file, "\tcheck_integer_conv_set(%s, %s_convert, %s_t, %d, %s);\n",
			curr_in->path_name, curr_in->path_name, curr_in->path_name, set_count, curr_in->error_code);
		      break;	
		    };
		break;




	    case INT_ARRAY :
		if(curr_in->error_code)
		    fprintf(out_file, "\tread_integer_array_error(%s_N_%s,%s, %s_count, %s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->path_name, curr_in->error_code);
		else
		    fprintf(out_file, "\tread_integer_array(%s_N_%s,%s, %s_count);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->path_name);
		break;



	    case FLO_ARRAY :
		if(curr_in->error_code)
		    fprintf(out_file, "\tread_float_array_error(%s_N_%s,%s, %s_count, %s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->path_name, curr_in->error_code);
		else
		    fprintf(out_file, "\tread_float_array(%s_N_%s,%s, %s_count);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->path_name);
		break;
	    case FLO :
		if(curr_in->error_code)
		    fprintf(out_file, "\tread_float_error(%s_N_%s,%s, %s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->error_code);
		else
		    fprintf(out_file, "\tread_float(%s_N_%s,%s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name);
		switch(curr_in->mode)  {
		    case NONE : 
		      break;
		    case RANGE :
		      fprintf(out_file, "\tcheck_range(%s, %s, %s, %s);\n", curr_in->path_name,
			curr_in->set[0], curr_in->set[1], curr_in->error_code);
		      break;
		    case SET :
		      fprintf(out_file, "\tcheck_float_set(%s, %s_t, %d, %s);\n",  
			curr_in->path_name, curr_in->path_name, set_count, curr_in->error_code);
		      break;
		    case CONV_SET :
		      fprintf(out_file, "\tcheck_float_conv_set(%s, %s_convert, %s_t, %d, %s);\n",
			curr_in->path_name, curr_in->path_name, curr_in->path_name, set_count, curr_in->error_code);
		      break;	
		};
		break;


	    case STR :
		if(curr_in->error_code)
		    fprintf(out_file, "\tread_string_error(%s_N_%s,%s, %s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name, curr_in->error_code);
		else
		    fprintf(out_file, "\tread_string(%s_N_%s,%s);\n", 
			upper_device_name, upper_path_name, curr_in->path_name);
		switch(curr_in->mode)  {
		    case NONE : 
		      break;
		    case SET :
		      fprintf(out_file, "\tcheck_string_set(%s, %s_t, %d, %s);\n",  
			curr_in->path_name, curr_in->path_name, set_count, curr_in->error_code);
		      break;
		    case CONV_SET :
		      fprintf(out_file, "\tcheck_string_conv_set(%s, %s_convert, %s_t, %d, %s);\n",
			curr_in->path_name, curr_in->path_name, curr_in->path_name, set_count, curr_in->error_code);
		      break;	
		}
		break;
	}    
    }
    if(is_operation)
    {
	fprintf(out_file, "\tstatus = %s___%s(nid_d_ptr, &in_struct);\n", lower_device_name, low_case_operation);
	fprintf(out_file, "\tfree_xd_array\n");
	fprintf(out_file, "\treturn status;\n}");
    }
    else
    {
	if(xd_count)
	    fprintf(out_file, "\tbuild_results_with_xd_and_return(%d);\n}\n", xd_count);
	else
	    fprintf(out_file, "\tbuild_results_and_return;\n}\n");
    }
    fprintf(out_file, "\n#undef free_xd_array\n");
    if(!single_qual)
	fclose(out_file);

    fprintf(file_h, "typedef struct {\n");

    fprintf(file_h, "\tstruct descriptor_xd *__xds;\n\tint __num_xds;\n\tint head_nid;\n");
    curr_in = top_list;
    for(; curr_in; curr_in = curr_in->nxt)
    {
	switch (curr_in->type)  {
	    case INT : fprintf(file_h, "\tint %s;\n", curr_in->path_name); break; 
	    case FLO : fprintf(file_h, "\tfloat %s;\n", curr_in->path_name); break;
	    case INT_ARRAY : 
		       fprintf(file_h, "\tint *%s;\n", curr_in->path_name);
		       fprintf(file_h, "\tint  %s_count;\n", curr_in->path_name); break;
	    case FLO_ARRAY : 
		       fprintf(file_h, "\tfloat *%s;\n", curr_in->path_name);
		       fprintf(file_h, "\tint  %s_count;\n", curr_in->path_name); break;
	    case STR : fprintf(file_h, "\tchar *%s;\n", curr_in->path_name); break;
	    case NONE :fprintf(file_h, "\tstruct descriptor *%s;\n", curr_in->path_name); break;
	}
	if(curr_in->mode == CONV_SET)
	    fprintf(file_h, "\tint %s_convert;\n", curr_in->path_name); 
    }
    fprintf(file_h, "} In%sStruct;\n", mixed_case_operation);
    free(low_case_operation);
    free(mixed_case_operation);
    FreeInList(top_list);
}

InDescr *DevNewInElement(char *path, InDescr *in_descr)
{
    char abc[128];
    NodeDescr *curr_node;
    int found = 0;
    for(curr_node = bottom_node_list; !found && curr_node; curr_node = curr_node->prv)
	if(*curr_node->name == ':')
	    found = !strcmp(&curr_node->name[1], path);
	else
	    found = !strcmp(curr_node->name, path);
    if(!found)
    {
	sprintf(abc, "Path name %s has not been declared in device", path);
	Error(abc);
    }
    in_descr->path_name = path;
    return in_descr;
}


InDescr *DevNewInElementNoConv(char *path, char *error)
{
    char abc[128];
    InDescr *in_descr = calloc(1, sizeof(InDescr));
    NodeDescr *curr_node;
    int found = 0;
    for(curr_node = bottom_node_list; !found && curr_node; curr_node = curr_node->prv)
	if(*curr_node->name == ':')
	    found = !strcmp(&curr_node->name[1], path);
	else
	    found = !strcmp(curr_node->name, path);
    if(!found)
    {
	sprintf(abc, "Path name %s has not been declared in device", path);
	Error(abc);
    }
    in_descr->path_name = path;
    in_descr->error_code = error;
    return in_descr;
}

InDescr *DevNewInElementErr(char *error)
{
    InDescr *in_descr = calloc(1, sizeof(InDescr));
    in_descr->error_code = error;
    return in_descr;
}


InDescr *DevNewConvert(InDescr *in_descr, char *error)
{
    in_descr->error_code = error;
    return in_descr;
}


InDescr *DevSetType(InDescr *in_descr, int type)
{
    if(!in_descr)
    {
	in_descr = calloc(1, sizeof(InDescr));
	in_descr->mode = NONE;
    }
    in_descr->type = type;
    return in_descr;
}


InDescr *DevNewRange(char *from, char *to)
{
    InDescr *in_descr = calloc(1, sizeof(InDescr));

    in_descr->mode = RANGE;
    in_descr->set[0] = from;
    in_descr->set[1] = to;
    return in_descr;
}

InDescr *DevNewSet(char *val, char *code, int convert)
{
    InDescr *in_set = calloc(1, sizeof(InDescr));

    if(convert)
    {
	in_set->mode = CONV_SET;
	in_set->codes[0] = code;
    }
    else
	in_set->mode = SET;
    in_set->set[0] = val;
    set_offset = 1;
    return in_set;
}

InDescr *DevAddInSet(InDescr *in_set, char *val, char *code, int convert)
{
    if(convert)
	in_set->codes[set_offset] = code;
    in_set->set[set_offset++] = val;
    return in_set;
}
    

char *DevConcatString(char *str1, char *str2)
{
    char *new_str;
    if(!str2) return str1;
    if(!str1) return str2;
    new_str = malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(new_str, str1);
    strcpy(&new_str[strlen(str1)], str2);
    free(str1);
    free(str2);
    return new_str;
}


static void FreeInList(InDescr *top)
{
    InDescr *prev = top;
    int i;

    while(top)
    {
	prev = top; 
	top = top->nxt;
	free(prev->path_name);
	for(i = 0; prev->set[i]; i++)
	    free(prev->set[i]);
	for(i = 0; prev->codes[i]; i++)
	    free(prev->codes[i]);
	if(prev->error_code)
	    free(prev->error_code);
	free(prev);
    }    
}


void DevFreeNodeList(NodeDescr *bottom)
{
    NodeDescr *prev = bottom;

    while(bottom)
    {
	prev = bottom;
	bottom = bottom->prv;
	free(prev->name);
	if(prev->tags)
	    free(prev->tags);
	if(prev->usage)
	    free(prev->usage);
	if(prev->value)
	    free(prev->value);
	if(prev->method)
	    free(prev->method);
	if(prev->phase)
	    free(prev->phase);
	if(prev->sequence)
	    free(prev->sequence);
	if(prev->completion)
	    free(prev->completion);
	if(prev->timout)
	    free(prev->timout);
	if(prev->server)
	    free(prev->server);

	free(prev);
    }
} 
