public fun TR10HWClose(in _handle)
{

	write(*, 'TR10HWClose', _handle);
	return (1);


	TR10->TR10_Close(val(_handle));
}
