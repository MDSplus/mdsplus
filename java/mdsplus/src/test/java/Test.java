import MdsPlus.*;
  import java.awt.*;
  public class Test {
	  public static void main(String[] args) {
	  try {
	          float xValue[];
	          int iValue[];
	          MdsPlus mds = new MdsPlus("cmoda", 8000);
	          iValue = new int[0];
	          xValue = new float[0];
	          xValue = mds.Value("[1.0D0, 2.0D0]").Float();
	          System.out.println("The values are "+xValue[0]+" "+xValue[1]);
	          iValue = mds.Value("1+2.5").Int();
	          System.out.println("The value is "+iValue[0]);
	          System.out.println("Here is a string "+mds.Value("'Now is the time'").String());
	          mds.OpenTree("CMOD,XX", 0); /* use a list of shots to prevent opening of whole cmod tree */
	          System.out.println("The time of the last shot was "+mds.Value("TIME_OF_SHOT").String());
	          mds.disconnect(mds);
	  } catch (Exception e) {
	      System.out.println("The exception was...\n"+e);
	  }
	  }
  }
