public fun GetLastError()
{

/* Initialize Library if the first time */
    if_error(_last_device_error, (public _last_device_error = "Success";));

	return( _last_device_error);
}