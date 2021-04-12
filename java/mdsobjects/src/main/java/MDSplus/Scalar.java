package MDSplus;

/**
 * Scalar is the common superclass for all scalar data types (descriptor class
 * CLASS_S).
 *
 *
 * Note that neither this nor the derived classes define or implement new
 * methods, except a constructor.
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.41
 */
public class Scalar extends Data
{
	public Scalar(Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
	}

	@Override
	public int getSize()
	{ return 1; }
}
