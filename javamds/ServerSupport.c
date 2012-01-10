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
#include <string.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <mds_stdarg.h>


extern int TdiData();
extern int TdiFloat();
extern int TdiDimOf();

#define MAX_POINTS 1000

#ifdef HAVE_WINDOWS_H
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT struct descriptor_xd *JavaResample(struct descriptor_xd  *y_xdptr, struct descriptor_xd *x_xdptr, 
    float *in_xmin, float *in_xmax, float *dt)
{
    static struct descriptor_xd template_xd = {0, DTYPE_DSC, CLASS_XD, 0, 0};
	int i;

	EMPTYXD(emptyXd);
	EMPTYXD(x_xd);
	EMPTYXD(y_xd);
    DESCRIPTOR_SIGNAL_1(sig_d, 0, 0, 0);
	int status;
	static struct descriptor_xd retXd;

	struct descriptor startD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)in_xmin};
	struct descriptor endD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)in_xmax};
	struct descriptor deltaD = {sizeof(float), DTYPE_FLOAT, CLASS_S, (char *)dt};


	printf("PARTE JavaResample start %f end %f\n", *in_xmin, *in_xmax);

	retXd = emptyXd;
	if(*in_xmax <= *in_xmin) return &retXd; 

	TreeSetTimeContext(&startD, &endD, &deltaD);
    if(x_xdptr->class == CLASS_XD && ((struct descriptor *)(x_xdptr->pointer))->dtype == DTYPE_T)
    {
		((struct descriptor *)(x_xdptr->pointer))->dtype = DTYPE_PATH;
		status = TdiData(x_xdptr, &x_xd MDS_END_ARG);
        if(!(status & 1))
		{
			TreeSetTimeContext(&emptyXd, &emptyXd, &emptyXd);
			return &retXd;
		}
		((struct descriptor *)(x_xdptr->pointer))->dtype = DTYPE_T;
    }  
	else 
	{
		status = TdiData(x_xdptr, &x_xd MDS_END_ARG);
		if(!(status & 1))
		{
			TreeSetTimeContext(&emptyXd, &emptyXd, &emptyXd);
			return &retXd;
		}
	}

	printf("JavaResample: Fatta X\n");

    if(y_xdptr->class == CLASS_XD && ((struct descriptor *)(y_xdptr->pointer))->dtype == DTYPE_T)
    {
		((struct descriptor *)(y_xdptr->pointer))->dtype = DTYPE_PATH;
		status = TdiData(y_xdptr, &y_xd MDS_END_ARG);
		if(!(status & 1))
		{
			TreeSetTimeContext(&emptyXd, &emptyXd, &emptyXd);
			return &retXd;
		}
		((struct descriptor *)(y_xdptr->pointer))->dtype = DTYPE_T;
	}  
	printf("JavaResample: Fatta Y\n");
/*	TreeSetTimeContext(&emptyXd, &emptyXd, &emptyXd);
	status = TdiFloat(&x_xd, &x_xd MDS_END_ARG);
	if(!(status & 1)) return &retXd;
	status = TdiFloat(&y_xd, &y_xd MDS_END_ARG);
	if(!(status & 1)) return &retXd;
    if(x_xd.pointer->class != CLASS_A)
		return &retXd;
	if(y_xd.pointer->class != CLASS_A)
		return &retXd;
    sig_d.data = (struct descriptor *)&y_xd;
    sig_d.dimensions[0] = (struct descriptor *)&x_xd;
    MdsCopyDxXd((struct descriptor *)&sig_d, &retXd);
*/
	MdsCopyDxXd((struct descriptor *)&y_xd, &retXd);
	MdsFree1Dx(&x_xd, NULL);
    MdsFree1Dx(&y_xd, NULL);
	printf("JavaResample: FINITA\n");
    return &retXd;
}
		
		
/*************OLD VERSION **********************/		
		
static struct descriptor_xd *JavaResampleClassic(struct descriptor_xd  *y_xdptr, struct descriptor_xd *x_xdptr, 
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
    if(x_xdptr->class == CLASS_XD && ((struct descriptor *)(x_xdptr->pointer))->dtype == DTYPE_T)
    {
		((struct descriptor *)(x_xdptr->pointer))->dtype = DTYPE_PATH;
		status = TdiData(x_xdptr, &x_xd MDS_END_ARG);
        if(!(status & 1)) return xd;
		((struct descriptor *)(x_xdptr->pointer))->dtype = DTYPE_T;
    }  
	else 
	{
		status = TdiData(x_xdptr, &x_xd MDS_END_ARG);
		if(!(status & 1)) return xd;
	}
    if(y_xdptr->class == CLASS_XD && ((struct descriptor *)(y_xdptr->pointer))->dtype == DTYPE_T)
    {
		((struct descriptor *)(y_xdptr->pointer))->dtype = DTYPE_PATH;
		status = TdiData(y_xdptr, &y_xd MDS_END_ARG);
		if(!(status & 1)) return xd;
		((struct descriptor *)(y_xdptr->pointer))->dtype = DTYPE_T;
	}  
	else
	{
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

static int traverseNodeMinMax(int nid, float *xMin, float *xMax);

static int traverseExprMinMax(struct descriptor *dsc, float *xMin, float *xMax)
{
	int status, nid, size, i, isUpdated;
	struct descriptor_a *arrD;
	struct descriptor_r *recD;
	char *currName;
	float currMin, currMax, min, max;
	EMPTYXD(xd);

	if(!dsc) return 0;
	switch(dsc->class)  {
		case CLASS_XD:
			return traverseExprMinMax(((struct descriptor_xd *)dsc)->pointer, xMin, xMax);
		case CLASS_S:
		case CLASS_D:
			switch (dsc->dtype)  {
				case DTYPE_PATH:
					currName = malloc(dsc->length + 1);
					memcpy(currName, dsc->pointer, dsc->length);
					currName[dsc->length] = 0;
					status = TreeFindNode(currName, &nid);
					free(currName);
					if(status & 1) return traverseNodeMinMax(nid, xMin, xMax);
					return 0;
				case DTYPE_NID:
					return traverseNodeMinMax(*((int *)dsc->pointer), xMin, xMax);
				default: 
					return 0; //Other Scalar values do not affect limits
			}
			break;
		case CLASS_A:
			arrD = (struct descriptor_a *)dsc;
			size = arrD->arsize/arrD->length;
			switch (arrD->dtype){
				case DTYPE_B:
					*xMin = ((char *)arrD->pointer)[0];
					*xMax = ((char *)arrD->pointer)[size - 1];
					break;
				case DTYPE_BU:
					*xMin = ((unsigned char *)arrD->pointer)[0];
					*xMax = ((unsigned char *)arrD->pointer)[size - 1];
					break;
				case DTYPE_W:
					*xMin = ((short *)arrD->pointer)[0];
					*xMax = ((short *)arrD->pointer)[size - 1];
					break;
				case DTYPE_WU:
					*xMin = ((unsigned short *)arrD->pointer)[0];
					*xMax = ((unsigned short *)arrD->pointer)[size - 1];
					break;
				case DTYPE_L:
					*xMin = ((int *)arrD->pointer)[0];
					*xMax = ((int *)arrD->pointer)[size - 1];
					break;
				case DTYPE_LU:
					*xMin = ((unsigned int *)arrD->pointer)[0];
					*xMax = ((unsigned int *)arrD->pointer)[size - 1];
					break;
				case DTYPE_Q:
					*xMin = ((_int64 *)arrD->pointer)[0];
					*xMax = ((_int64 *)arrD->pointer)[size - 1];
					break;
				case DTYPE_QU:
					*xMin = ((_int64u *)arrD->pointer)[0];
					*xMax = ((_int64u *)arrD->pointer)[size - 1];
					break;
				case DTYPE_FLOAT:
					*xMin = ((float *)arrD->pointer)[0];
					*xMax = ((float *)arrD->pointer)[size - 1];
					break;
				case DTYPE_DOUBLE:
					*xMin = ((double *)arrD->pointer)[0];
					*xMax = ((double *)arrD->pointer)[size - 1];
					break;
				default: 
					printf("JavaGetMinMax: unsupported scalar type: %d\n", arrD->dtype);
					return 0;
			}
			return 1;
			break;
		case CLASS_R:
			recD = (struct descriptor_r *)dsc;
			size = recD->ndesc;
			min = -1E30;
			max = 1E30;
			isUpdated = 0;
			switch (recD->dtype) {
				case DTYPE_PARAM:
				case DTYPE_WITH_UNITS:
				case DTYPE_WITH_ERROR:
					return (recD->dscptrs[0])?traverseExprMinMax(recD->dscptrs[0], xMin, xMax):0;
				case DTYPE_SIGNAL:
					if(!recD->dscptrs[2]) return 0;
					status = TdiData(recD->dscptrs[2], &xd MDS_END_ARG);
					if(!(status & 1) || !xd.pointer) return 0;
					status = traverseExprMinMax(xd.pointer, xMin, xMax);
					MdsFree1Dx(&xd, 0);
					return status;
				case DTYPE_FUNCTION:
					for(i = 0; i < size; i++)
					{
						status = traverseExprMinMax(recD->dscptrs[i], &currMin, &currMax);
						if(status) //something meaningful has been returned
						{
							if(currMin > min)
								min = currMin;
							if(currMax < max)
								max = currMax;
							isUpdated = 1;
						}
					}
					if(!isUpdated) return 0;
					*xMin = min;
					*xMax = max;
					return 1;
				default:
					return 0;
			}
		default:
			printf("JavaGetMinMax: Unsupported class: %d\n", dsc->class);
			return 0;
	}
}

static int traverseNodeMinMax(int nid, float *xMin, float *xMax)
{
	EMPTYXD(xd);
	EMPTYXD(startXd);
	EMPTYXD(endXd);
	int numSegments, i, status;

	status = TreeGetNumSegments(nid, &numSegments);
	if(!(status & 1)) return 0;
	if(numSegments == 0)
	{
		status = TreeGetRecord(&xd);
		if(!(status & 1)) return 0;
		status = traverseExprMinMax(xd.pointer, xMin, xMax);
		MdsFree1Dx(&xd, 0);
		return status;
	}
	status = TreeGetSegmentLimits(nid, 0, &startXd, &endXd);
	if(!(status & 1)) return 0;
	status = TdiData(&startXd, &startXd MDS_END_ARG);
	if(status & 1)
		status = TdiFloat(&startXd, &startXd MDS_END_ARG);
	if(!(status & 1)) return 0;
	*xMin = *((float *)startXd.pointer->pointer);
	MdsFree1Dx(&startXd, 0);
	MdsFree1Dx(&endXd, 0);
	status = TreeGetSegmentLimits(nid, numSegments - 1, &startXd, &endXd);
	if(!(status & 1)) return 0;
	status = TdiData(&endXd, &endXd MDS_END_ARG);
	if(status & 1)
		status = TdiFloat(&endXd, &endXd MDS_END_ARG);
	if(!(status & 1)) return 0;
	*xMax = *((float *)endXd.pointer->pointer);
	MdsFree1Dx(&startXd, 0);
	MdsFree1Dx(&endXd, 0);
	return 1;
}






//Find minimum and maximum time for an expression involving signals 
EXPORT int JavaGetMinMax(char *sigExpr, float *xMin, float *xMax)
{
	EMPTYXD(xd);
	int status;
	struct descriptor sigD = {strlen(sigExpr), DTYPE_T, CLASS_S, sigExpr};
	 
	status = TdiCompile(&sigD, &xd MDS_END_ARG);
	if(!(status & 1)) return 0;

	status = traverseExprMinMax(xd.pointer, xMin, xMax);
	MdsFree1Dx(&xd, 0);
	return status;
}