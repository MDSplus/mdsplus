package MDSplus;

/**
 * Class for DTYPE_DISPATCH
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.36
 */
public class Dispatch extends Compound
{
	public Dispatch(Data ident, Data phase, Data when, Data completion,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_DISPATCH;
            descs = new Data[]{ident, phase, when, completion};
	}
	public Dispatch(Data ident, Data phase, Data when, Data completion)
        {
            this(ident, phase, when, completion, null, null, null, null);
	}
        public Dispatch()
        {
            this(null, null, null, null);
        }

        public static Dispatch getData(Data help, Data units, Data error, Data validation)
        {
            return new Dispatch(null, null, null, null, help, units, error, validation);
        }

	public Data getIdent()
        {
            return descs[0];
	}

	public Data getPhase()
        {
            return descs[1];
	}

	public Data getWhen()
        {
            return descs[2];
	}

	public Data getCompletion()
        {
            return descs[3];
	}

	/**
	 *
	 * @param data
	 */
	public void setIdent(Data data)
        {
            descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setPhase(Data data)
        {
            descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setWhen(Data data)
        {
            descs[2] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setCompletion(Data data)
        {
            descs[3] = data;
	}

}