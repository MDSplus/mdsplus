public fun NI6259AIGetStartEnd(as_is _clock_nid, in _idx)
{
    return (begin_of(_clock_nid) + _idx * slope_of(_clock_nid));
}
