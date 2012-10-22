FUN PUBLIC JavaResample(as_is(_y))
{
   write(*, 'SONO LA JAVA RESAMPLE');
   write(*, _y);
   if(_jscope_dt == 0)
		return (_y);
   else
		return (JavaMds->JavaResample:DSC(GETNCI(_y, 'NID_NUMBER'), float(_jscope_xmin), float(_jscope_xmax), float(_jscope_dt)));
}



