package mds.jtraverser;
//package jTraverser;
/**
 * This interface defined only method DataChanged(Data) to inform listeners dtah
 * the value of the ddatum is possibly changed
 */
interface DataListener
{
	public void DataChanged(MDSplus.Data data);
}
