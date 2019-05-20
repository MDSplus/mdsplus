public fun TimingGetEvents(out _event_names, out _event_codes)
{
    _expEvents = getenv("expEvents");

    if( _expEvents == "spider" )
    {
           _event_names = ['T_START_SPIDER', 'T_BEAM_ON', 'T_SWITCH_OFF', 'T_BEAM_OFF', 'T_BREAK_DOWN'];
           _event_codes = [1,2,3,4,5];
    } else {
           _event_names = ['CPST','CTST','CCT', 'T_START_RFX','T_START_PC','T_START_PM','T_START_PV','T_OPEN_PTSO','T_INSRT_PTCB','T_START_PR', 'T_CLOSE_PNSS', 'T_START_TF', 'T_START_GP', 'T_START_PLT', 'T_BLOW_OFF'];
           _event_codes = [1,2,3,4,5,6,7,8,9,10,11,12,13,33,14];
    }
}
