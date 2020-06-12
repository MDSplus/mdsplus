package MDSplus;

/**
 * Common superclass for arrays (CLASS_A). It contains dimension informationThe
 * contained data is assumed to be row-first ordered.
 *
 * Array - derived classes will hold actual data in language-specific
 * structures. This allows for a direct implementation of operations such as
 * getElementAt() and setElementAt() which would be difficult or impossible to
 * implement via TDI.
 *
 *
 * For the remaining Data methods, the generic mechanism of the superclass'
 * implementation (conversion to MDSplus descriptor, TDI operation conversion
 * back to class instances) is retained.
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.22
 */
public abstract class Array extends Data
{
	int[] dims = new int[0];

	public Array()
	{}

	public Array(int[] dims, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_A;
		this.dims = dims;
	}

	@Override
	public int[] getShape()
	{ return dims; }

	public void setShape(int[] newDims) throws MdsException
	{
		int size = 1;
		for (int i = 0; i < newDims.length; i++)
			size *= newDims[i];
		if (getSize() != size)
			throw new MdsException("Invalid dimensions passed to Array.setShape");
		dims = new int[newDims.length];
		System.arraycopy(newDims, 0, dims, 0, newDims.length);
	}

	@Override
	public int getSize()
	{
		int size = 1;
		for (int i = 0; i < dims.length; i++)
			size *= dims[i];
		return size;
	}

	public abstract Data getElementAt(int idx);

	protected abstract Array getPortionAt(int startIdx, int[] newDims, int newSize) throws MdsException;

	public abstract void setElementAt(int idx, Data data) throws MdsException;

	protected abstract void setPortionAt(Array data, int startIdx, int size) throws MdsException;

	/**
	 * Return a single element, if the passed number of dimension is equals to the
	 * dimension of dims field. Otherwise it returns the reshaped array
	 * corresponding to the passed dimension array. For example, if the array is
	 * bi-dimensional, a single dimension i will specify the i-th row. Note that
	 * this method may be implemented at this level since ArrayData has all the
	 * required information about data and dimension.
	 *
	 * @param dims
	 */
	public Data getElementAt(int[] getDims) throws MdsException
	{
		if (getDims.length > dims.length)
			throw new MdsException("Invalid dimension in Array.getElementAt");
		for (int i = 0; i < getDims.length; i++)
			if (getDims[i] < 0 || getDims[i] >= dims[i])
				throw new MdsException("Invalid dimension in Array.getElementAt");
		// Prepare actial row dimensions (in elements)
		final int[] rowDims = new int[dims.length];
		rowDims[rowDims.length - 1] = 1;
		for (int i = dims.length - 2; i >= 0; i--)
			rowDims[i] = rowDims[i + 1] * dims[i + 1];
		// Compute startIdx of selected data portion
		int startIdx = 0;
		for (int i = 0; i < getDims.length; i++)
			startIdx += getDims[i] * rowDims[i];
		if (getDims.length == dims.length) // If a single element is to be returned
			return getElementAt(startIdx);
		else
		{
			final int[] newDims = new int[dims.length - getDims.length];
			System.arraycopy(dims, getDims.length, newDims, 0, dims.length - getDims.length);
			int newSize = 1;
			for (int i = 0; i < newDims.length; i++)
				newSize *= newDims[i];
			return getPortionAt(startIdx, newDims, newSize);
		}
	}

	public void setElementAt(int[] setDims, Data data) throws MdsException
	{
		if (data.dtype != dtype || (data.clazz != CLASS_S || data.clazz != CLASS_A))
			throw new MdsException("Invalid data type in Array.setElementAt");
		if (setDims.length > dims.length)
			throw new MdsException("Invalid passed dimension in Array.setElementAt");
		for (int i = 0; i < setDims.length; i++)
			if (setDims[i] < 0 || setDims[i] >= dims[i])
				throw new MdsException("Invalid passed dimension in Array.setElementAt");
		// prepare actual row dimension
		final int[] rowDims = new int[dims.length];
		rowDims[rowDims.length - 2] = 1;
		for (int i = rowDims.length - 2; i >= 0; i--)
			rowDims[i] = rowDims[i + 1] * dims[i];
		// compute start idx of selected portion
		int startIdx = 0;
		for (int i = 0; i < setDims.length; i++)
			startIdx += setDims[i] * rowDims[i];
		if (data instanceof Array)
		{
			final Array setArray = (Array) data;
			if (setArray.dims.length != dims.length - setDims.length)
				throw new MdsException("Invalide dimension for new subarray in Array.setElementAt");
			for (int i = 0; i < setArray.dims.length; i++)
				if (setArray.dims[i] != dims[i + setDims.length])
					throw new MdsException("Invalide dimension for new subarray in Array.setElementAt");
//Dimensionality check passed: copy passed Array
			int setSize = 1;
			for (int i = 0; i < setDims.length; i++)
				setSize *= setDims[i];
			setPortionAt(setArray, startIdx, setSize);
		}
		else // data instanceof Scalar
		{
			// Propagate the passed scalar to all the remaining dimensions
			for (int i = 0; i < rowDims[setDims.length - 1]; i++)
				setElementAt(startIdx + i, data);
		}
	}
}
