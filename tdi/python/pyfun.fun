/* Call a python function. Arguments:
- Function name
- Module name
- up to 8 arguments
*/ 
fun public pyfun(in _function, optional in _module, optional in _in1, optional in _in2, optional in _in3, 
	optional in _in4, optional in _in5, optional in _in6, optional in _in7, optional in _in8)
{
	if(present(_module))
		_lines = ['from '//_module // ' import ' // _function];
	else
		_lines = [];
	_lines = [_lines, 'from MDSplus import Data'];
	_args = '(';
    if(present(_in1))
	{
		public _a1 = _in1;
		_lines = [_lines, 'a1 = Data.execute("_a1")'];
		_args = _args // 'a1';
	}
    if(present(_in2))
	{
		public _a2 = _in2;
		_lines = [_lines, 'a2 = Data.execute("_a2")'];
		_args = _args // ',a2';
	}
    if(present(_in3))
	{
		public _a3 = _in3;
		_lines = [_lines, 'a3 = Data.execute("_a3")'];
		_args = _args // ',a3';
	}
    if(present(_in4))
	{
		public _a4 = _in4;
		_lines = [_lines, 'a4 = Data.execute("_a4")'];
		_args = _args // ',a4';
	}
    if(present(_in5))
	{
		public _a5 = _in5;
		_lines = [_lines, 'a5 = Data.execute("_a5")'];
		_args = _args // ',a5';
	}
    if(present(_in6))
	{
		public _a6 = _in6;
		_lines = [_lines, 'a6 = Data.execute("_a6")'];
		_args = _args // ',a6';
	}
    if(present(_in7))
	{
		public _a7 = _in7;
		_lines = [_lines, 'a7 = Data.execute("_a7")'];
		_args = _args // ',a7';
	}
    if(present(_in8))
	{
		public _a8 = _in8;
		_lines = [_lines, 'a8 = Data.execute("_a8")'];
		_args = _args // ',a8';
	}
	_args = _args // ')';
	return(py([_lines, 'res = '//_function //_args], 'res'));
}