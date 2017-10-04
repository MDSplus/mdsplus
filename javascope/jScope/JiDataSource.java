package jScope;

/* $Id$ */
import java.io.IOException;

/**
 * Interface for reading scientific data sets
 * Looks a lot like netCDF
 */ 

public interface JiDataSource {
	/**
	 * Get a global attribute named 'name'
	 *
	 * @exception IOException
	 * @return the global attribute named 'name'
	 */
	public JiVar getGlobalAtt(String name) throws IOException ;
	/**
	 * Get all the global attributes for this source
	 *
	 * @return array containing global attributes
	 */
	public JiVar[] getGlobalAtts();
	/**
	 * Get a variable named 'name'
	 *
	 * @exception IOException
	 * @return the variable named 'name'
	 */
	public JiVar getVar(String name) throws IOException ;
	/**
	 * Get all the variables for this source
	 *
	 * @return array containing variables
	 */
	public JiVar[] getVars();
}
