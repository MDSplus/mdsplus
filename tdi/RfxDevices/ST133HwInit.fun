public fun ST133HwInit(in _shot_num)
{

	write(*, "_shot_num ", _shot_num);

	_u = fopen("C:\\Documents and settings\\spectroscopy\\CCD_experiment\\shotnum.txt","w");

	write(*, "Id file ", _u);

	_status = write(_u, _shot_num);

	fclose(_u);
		 
      return(_status);
}
