public fun BoloHWZero(in _chan_id)
{

     write(*, "BOLO HW zero");

    _errors_id = zero([48],0);

    Tomo->ZERO_SW_BOLO( val(48), _chan_id, ref(_errors_id));

    return (_errors_id);
}