
class prova
{
    public static void main(String args[])
    {
	LocalProvider dp = new LocalProvider();
System.out.println("Update da fare");
	dp.Update("a", -1);
System.out.println("Update fatto");

	if(args.length > 0)
	{
	    double [] arr = dp.GetDoubleArray(args[0]);
	    for(int i = 0; i < arr.length; i++)
	    	System.out.println(arr[i]);
	    return;
	} 



	System.out.println(dp.GetString("titolo"));
	double [] arr = dp.GetDoubleArray("seno");
	for(int i = 0; i < arr.length; i++)
	    System.out.println(arr[i]); 

	arr = dp.GetDoubleArray("pippera");
	System.out.println(dp.ErrorString()); 

    }
}    
