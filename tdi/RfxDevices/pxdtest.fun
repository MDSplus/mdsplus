public fun pxdtest()
{
    write(*, 'BEO');
    _name = "PXI7::11::INSTR";

    _handle = long(0);
    _status = LcPXD1xxx_32->LcPXD1xxx_InitWithOptions (_name, val(long(1)), val(long(1)),
			"Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1",ref(_handle));

    write(*, 'Init: status', _status);
    write(*, _handle);

    _input_impedance = 50.d0;
    _max_input_frequency = 500.0d6;
    _ch_name = 'C1';

    _status = LcPXD1xxx_32->LcPXD1xxx_ConfigureChanCharacteristics (val(_handle), 'C1', 
	val(_input_impedance), val(_max_input_frequency));
    
    write(*, 'ConfigureChanCharacteristics, status: ', _status);
 
}