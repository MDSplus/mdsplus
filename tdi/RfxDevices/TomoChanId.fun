public fun TomoChanId(in _logical, in _enable, in _table)
{
    _id = 0UQ; 

    /* VME RACK*/
    _id =   (   _table[ 1, _logical -1] & 0x1UQ);
    /* VME MODULE */
    _id +=  ((  _table[ 2, _logical -1] & 0xFUQ)      << 1);
    /* VME CHANNEL */
    _id +=  ((( _table[ 3, _logical -1] - 1) & 0x3UQ) << 5);
    /* DIODE */
    _id +=  ((  _table[ 4, _logical -1] & 0x1FF)      << 7);
    /* AMPLIFIER TYPE */
    _id +=  (((_table[ 5, _logical -1]) & 0x1FUQ)      << 16); 
    /* AMPLIFIER ID */
    _id +=  (( _table[ 6, _logical -1] & 0x7FUQ)       << 21);
    /* GPIB MODULE*/
    _id +=  QUADWORD(( _table[ 7, _logical -1] & 0x1FUQ))       << 28;
    /* GPIB CHANNEL*/
    _id +=  QUADWORD((( _table[ 8, _logical -1] - 1) & 0x3UQ))       << 33;
    /* AMPLIFIER RACK */
    _id +=  QUADWORD(( _table[ 9, _logical -1] & 0x1FUQ))       << 36;
    /* SOFTWARE ABIL*/
    _id +=  QUADWORD(_enable & 0x1UQ)   << 41;

    return (QUADWORD(_id));
}
