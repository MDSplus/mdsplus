/*------------------------------------------------------------------------------

	Name:	JAVA$RESAMPLE   

	Type:   C function

	Author:	Gabriele Manduchi
		Istituto Gas Ionizzati del CNR - Padova (Italy)

	Date:   7-JAN-1998

	Purpose: Resample a given signal into a vector of equally spaced samples
		 DO NOT interpolate when the total number of points is less than
		 the required one

--------------------------------------------------------------------------------

 Input  arguments:  struct descriptor *in : descriptor of the input signal
		    float *start : start time of resampling
		    float *freq: resampling frequency
		    int *max_samples: maximum number of samples
		    if *freq == 0, then freq and start time are adjusted such that all the
		    time range of the signal fits into max_samples
		    

 Output arguments: float *out: output vector, already allocated (max_samples elements)
		   float *ret_start: returned start time
		   int *out_samples: returned number of samples (<= max_samples)

-------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <mdsdescrip.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <mds_stdarg.h>

extern int TdiData();
extern int TdiFloat();
extern int TdiDimOf();

#define MAX_POINTS 1000

struct descriptor_xd *JavaResample(struct descriptor_xd  *y_xdptr, struct descriptor_xd *x_xdptr, 
    float *in_xmin, float *in_xmax)
{
    static struct descriptor_xd template_xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};
    struct descriptor_xd *xd;

int i;

    DESCRIPTOR_SIGNAL_1(sig_d, 0, 0, 0);
    DESCRIPTOR_A(a_d, sizeof(float), DTYPE_FLOAT, 0,0); 
    DESCRIPTOR_A(ax_d, sizeof(float), DTYPE_FLOAT, 0,0); 
    int out_points, x_points, y_points, min_points, act_points, start_idx, end_idx, curr_idx, out_idx, status;
    float out_array[1000], out_arrayx[1000], xmin, xmax, delta, curr, *x, *y;
    struct descriptor_xd x_xd = {0, DTYPE_DSC, CLASS_XD, 0, 0},
	y_xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};     

    xd = (struct descriptor_xd *)malloc(sizeof(struct descriptor_xd));
    memcpy(xd, &template_xd, sizeof(struct descriptor_xd));

    if(x_xdptr->class == CLASS_XD && 
	((struct descriptor *)(x_xdptr->pointer))->dtype == DTYPE_T)
    {
	((struct descriptor *)(x_xdptr->pointer))->dtype = DTYPE_PATH;

	 status = TdiData(x_xdptr, &x_xd MDS_END_ARG);
         if(!(status & 1)) return xd;
	((struct descriptor *)(x_xdptr->pointer))->dtype = DTYPE_T;

    }  else {

	status = TdiData(x_xdptr, &x_xd MDS_END_ARG);
	if(!(status & 1)) return xd;
    }

    if(y_xdptr->class == CLASS_XD && 
	((struct descriptor *)(y_xdptr->pointer))->dtype == DTYPE_T)
    {
	((struct descriptor *)(y_xdptr->pointer))->dtype = DTYPE_PATH;

	 status = TdiData(y_xdptr, &y_xd MDS_END_ARG);
         if(!(status & 1)) return xd;
	((struct descriptor *)(y_xdptr->pointer))->dtype = DTYPE_T;

    }  else {

	status = TdiData(y_xdptr, &y_xd MDS_END_ARG);
	if(!(status & 1)) return xd;
    }   
    status = TdiFloat(&x_xd, &x_xd MDS_END_ARG);
    if(!(status & 1)) return xd;
    status = TdiFloat(&y_xd, &y_xd MDS_END_ARG);
    if(!(status & 1)) return xd;

    if(x_xd.pointer->class != CLASS_A)
	return xd;
    if(y_xd.pointer->class != CLASS_A)
	return xd;

    x = (float *)((struct descriptor_a *)x_xd.pointer)->pointer;
    x_points = ((struct descriptor_a *)x_xd.pointer)->arsize / 4;
    y = (float *)((struct descriptor_a *)y_xd.pointer)->pointer;
    y_points = ((struct descriptor_a *)y_xd.pointer)->arsize / 4;
    xmin = *in_xmin;
    xmax = *in_xmax;
    if(x_points < y_points)
	min_points = x_points;
    else
	min_points = y_points;

    if(xmin < x[0])
	xmin = x[0];
    if(xmax > x[min_points - 1])
	xmax = x[min_points - 1];
    for(start_idx = 0; x[start_idx] < xmin; start_idx++);
    for(end_idx = 0; x[end_idx] < xmax; end_idx++);    	
    act_points = end_idx - start_idx + 1;
    if(act_points < MAX_POINTS)
    {
	for(curr_idx = start_idx, out_idx = 0; curr_idx <= end_idx; curr_idx++, out_idx++)
	{
	    out_arrayx[out_idx] = x[curr_idx];
	    out_array[out_idx] = y[curr_idx];
	}
	out_points = out_idx;
    }
    else /*Resample*/	    
    {
	delta = (xmax - xmin)/(MAX_POINTS - 1);
	curr = x[start_idx] + delta;    
	curr_idx = start_idx;
	out_idx = 1;
	out_array[0] = y[start_idx];
	out_arrayx[0] = x[start_idx];
	while(curr <= x[end_idx] && out_idx < MAX_POINTS && curr_idx <= x_points)
	{
	    while(x[curr_idx] < curr)
		curr_idx++;
	    if(curr_idx <= x_points)
	    {
		out_arrayx[out_idx] = curr;
		out_array[out_idx++] = y[curr_idx - 1] + (y[curr_idx] - y[curr_idx - 1]) *		
		    (curr - x[curr_idx - 1]) / (x[curr_idx] - x[curr_idx - 1]);
		curr = x[start_idx] + out_idx * delta;
	    }
	}
	for(;out_idx < MAX_POINTS; out_idx++)
	{
	    out_array[out_idx] = out_array[out_idx - 1];
	    out_arrayx[out_idx] = out_arrayx[out_idx - 1] + delta;
	}
	out_points = out_idx;
    }
    a_d.pointer = (char *)out_array;
    a_d.arsize = out_points * 4;
    ax_d.pointer = (char *)out_arrayx;
    ax_d.arsize = out_points * 4;
    sig_d.data = (struct descriptor *)&a_d;
    sig_d.dimensions[0] = (struct descriptor *)&ax_d;
    MdsCopyDxXd((struct descriptor *)&sig_d, xd );
    MdsFree1Dx(&x_xd, NULL);
    MdsFree1Dx(&y_xd, NULL);
    return xd;
}

#define QUITE_SIMILAR(x,y) ((x)>(y)*(1-1E-10)&&(x)<(y)*(1+1E-10))

struct descriptor_xd *JavaDim(float *x, int *in_xsamples, float *in_xmin, float *in_xmax)
{
    static struct descriptor_xd xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};
    DESCRIPTOR_A(a_d, sizeof(float), DTYPE_FLOAT, 0,0); 
    int x_points, act_points, i, j, start_idx, end_idx, curr_idx, out_idx;
    float out_array[3*MAX_POINTS+1], xmin, xmax, delta, curr;
    

    xd.length = 0;
    xd.pointer = 0;

    xmin = *in_xmin;
    xmax = *in_xmax;
    x_points = *in_xsamples;

    if(xmin < x[0])
	xmin = x[0];
    if(xmax > x[x_points - 1])
	xmax = x[x_points - 1];
	
    for(start_idx = 0; x[start_idx] < xmin; start_idx++);
    for(end_idx = 0; x[end_idx] < xmax; end_idx++);    	
    act_points = end_idx - start_idx + 1;
    if(act_points < MAX_POINTS) /*Code time axis*/
    {
	curr_idx = start_idx + 1;
	out_idx = 1;
	while(curr_idx <= end_idx)
	{
	    out_array[out_idx++] = x[curr_idx-1];
	    delta = x[curr_idx + 1] - x[curr_idx];
	    for(;curr_idx<=end_idx && QUITE_SIMILAR(x[curr_idx],x[curr_idx - 1]+delta); curr_idx++);
	    out_array[out_idx++] = x[curr_idx - 1];
	    out_array[out_idx++] = delta;
	    curr_idx++;
	}
	out_array[0] = 1;
    }
    else /*Resample*/	    
    {
	out_idx = 1;
	out_array[out_idx++] = x[start_idx];
	out_array[out_idx++] = x[end_idx];
	out_array[out_idx++] = (x[end_idx] - x[start_idx])/(MAX_POINTS - 1);
	out_array[0] = 1;
    }
    if(out_idx >=  act_points) /* Coding is not convenient*/
    {
	out_array[0] = -1;
	out_idx = 1;
	for(curr_idx = start_idx; curr_idx <= end_idx; curr_idx++)
	    out_array[out_idx++] = x[curr_idx];
    }
    a_d.pointer = (char *)out_array;
    a_d.arsize = out_idx * 4;
    MdsCopyDxXd((struct descriptor *)&a_d, &xd );
    return &xd;

}

