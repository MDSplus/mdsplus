public fun TR10Handle(in _board)
{
  _handle_name = TR10HandleName(_board);
  if (!allocated()
  {
    if (!defined(_TR10Initialized) )
    {
      _TR10Initialized = TR10->TR10_InitLibrary();
    }
    execute(
