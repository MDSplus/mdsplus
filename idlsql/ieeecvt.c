#include <stdio.h>
#include "export.h"
extern int IDL_IsIeee();
extern void IDL_CvtFloatToVax();
extern void IDL_CvtDoubleToVax();
extern void IDL_CvtVaxToFloat();
extern void IDL_CvtVaxToDouble();

extern void IEEE_TO_FLOAT(int argc, IDL_VPTR argv[])
{
  if (IDL_IsIeee()) {
    int i;
    for (i=0; i<argc; i++) {
      if (argv[i]->type == IDL_TYP_FLOAT) {
        if (argv[i]->flags & IDL_V_NOT_SCALAR)
          IDL_CvtFloatToVax(argv[i]->value.arr->data, argv[i]->value.arr->n_elts);
        else
          IDL_CvtFloatToVax(&(argv[i]->value.f), 1);
      } 
      else if (argv[i]->type == IDL_TYP_DOUBLE) {
        if (argv[i]->flags & IDL_V_NOT_SCALAR)
          IDL_CvtDoubleToVax(argv[i]->value.arr->data, argv[i]->value.arr->n_elts);
        else
          IDL_CvtDoubleToVax(&(argv[i]->value.d), 1);
      } 
    }
  }
}

extern void FLOAT_TO_IEEE(int argc, IDL_VPTR argv[])
{
  if (IDL_IsIeee()) {
    int i;
    for (i=0; i<argc; i++) {
      if (argv[i]->type == IDL_TYP_FLOAT) {
        if (argv[i]->flags & IDL_V_NOT_SCALAR)
          IDL_CvtVaxToFloat(argv[i]->value.arr->data, argv[i]->value.arr->n_elts);
        else
          IDL_CvtVaxToFloat(&(argv[i]->value.f), 1);
      } 
      else if (argv[i]->type == IDL_TYP_DOUBLE) {
        if (argv[i]->flags & IDL_V_NOT_SCALAR)
          IDL_CvtVaxToDouble(argv[i]->value.arr->data, argv[i]->value.arr->n_elts);
        else
          IDL_CvtVaxToDouble(&(argv[i]->value.d), 1);
      } 
    }
  }
}
