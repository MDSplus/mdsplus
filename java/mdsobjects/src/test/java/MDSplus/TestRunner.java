package MDSplus;

import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;

public class TestRunner {
   public static void main(String[] args) {
      Result result = JUnitCore.runClasses(new Class[]{MdsDataTest.class, MdsConglomTest.class, MdsConnectionTest.class, MdsDimensionTest.class, 
	    MdsEventTest.class, MdsExpressionCompileTest.class, MdsFunctionTest.class,  MdsRangeTest.class,  MdsSignalTest.class, 
	    MdsStringTest.class, MdsTreeTest.class, MdsTreeNodeTest.class, MdsWindowTest.class});
		
      for (Failure failure : result.getFailures()) {
         System.out.println(failure.toString());
      }
		
      System.out.println(result.wasSuccessful());
   }
} 