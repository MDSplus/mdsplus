public fun BoloHWReset(in _chan_id)
{

     write(*, "BOLO HW reset");

    _errors_id = zero([48],0);

    Tomo->RESET_SW_BOLO( val(48), _chan_id, ref(_errors_id));

    return (_errors_id);
}