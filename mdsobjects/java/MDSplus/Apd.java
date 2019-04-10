package MDSplus;


/**
 * Class for CLASS_APD, DTYPE_DSC
 * @author manduchi
 * @version 1.0
 * @updated 03-ott-2008 12.23.37
 */
public class Apd extends Data {

	protected Data[] descs;
        protected int nDescs = 0;
        final static int CHUNK_SIZE = 64;
        public Apd()
        {
            this(new Data[0]);
        }
        public Apd(Data [] descs)
        {
            this(descs, null, null, null, null);
        }
	public Apd(Data [] descs, Data help, Data units, Data error, Data validation)
        {
            super(help, units, error, validation);
            clazz = CLASS_APD;
            dtype = DTYPE_DSC;
            this.descs = descs;
            nDescs = descs.length;
	}

        public static Data getData(Data [] descs, Data help, Data units, Data error, Data validation)
        {
            return new Apd(descs, help, units, error, validation);
        }


	public Data[] getDescs()
        {
            Data []retDescs = new Data[nDescs];
            for(int i = 0; i < nDescs; i++)
                retDescs[i] = descs[i];
            return retDescs;
	}

	/**
	 *
	 * @param descs
	 */
	public void setDescs(Data[] inDescs)
        {
            descs = new Data[inDescs.length];
            for(int i = 0; i < inDescs.length; i++)
                descs[i] = inDescs[i];
            nDescs = descs.length;
        }
	/**
	 *
	 * @param idx
	 */
	public Data getDescAt(int idx)
        {
            return descs[idx];
	}

        protected void resizeDescs(int newDim)
        {
            if(descs == null)
            {
                descs = new Data[CHUNK_SIZE];
            }
            else
            {
                if(newDim <= descs.length) return;
                Data newDescs[] = new Data[(newDim/CHUNK_SIZE)*CHUNK_SIZE + CHUNK_SIZE];
                for(int i = 0; i  < nDescs; i++)
                    newDescs[i] = descs[i];
                descs = newDescs;
            }
        }
	/**
	 *
	 * @param idx
	 * @param dsc
	 */
	public void setDescAt(int idx, Data dsc)
        {
            resizeDescs(idx+1);
            descs[idx] = dsc;
        }

}
