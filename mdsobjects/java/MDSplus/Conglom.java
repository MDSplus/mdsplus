package MDSplus;

/**
 * Class for DTYPE_CONGLOM
 * @author manduchi
 * @version 1.0
 * @updated 03-ott-2008 12.23.50
 */
public class Conglom extends Compound
{
	public Conglom(Data image, Data model, Data name, Data qualifiers,
                Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_R;
            dtype = DTYPE_CONGLOM;
            descs = new Data[]{image, model, name, qualifiers};
	}
	public Conglom(Data image, Data model, Data name, Data qualifiers)
        {
            this(image, model, name, qualifiers, null, null, null, null);
	}

        public static Conglom getData(Data help, Data units, Data error, Data validation)
        {
            return new Conglom(null, null, null, null, help, units, error, validation);
        }

	public Data getImage()
        {
            return descs[0];
	}

	public Data getModel()
        {
           return descs[1];
	}

	public Data getName()
        {
           return descs[2];
	}

	public Data getQualifiers()
        {
           return descs[3];
	}

	/**
	 *
	 * @param data
	 */
	public void setImage(Data data)
        {
            descs[0] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setModel(Data data)
        {
            descs[1] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setName(Data data)
        {
            descs[2] = data;
	}

	/**
	 *
	 * @param data
	 */
	public void setQualifiers(Data data)
        {
            descs[3] = data;
	}

}