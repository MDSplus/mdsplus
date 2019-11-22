package MDSplus;

import org.junit.runner.JUnitCore;
import org.junit.runner.Computer;
import org.junit.runner.Result;
import org.tap4j.ext.junit.JUnitTestTapReporter;
import org.junit.runner.JUnitCommandLineParseResultWrapper;



public class TestRunner
{
   public static void main(java.lang.String[] args) 
	{
      // // Create a JUnit suite
		// TestSuite suite = new TestSuite();		
		// suite.addTestSuite(MyJUnitTestClass.class);
		
		// Instantiate a JUniteCore object
		JUnitCore core = new JUnitCore();
		
		// Add TAP Reporter Listener to the core object executor
		core.addListener(new JUnitTestTapReporter());
		// Run the test suite
      // core.run(suite);
      
      Computer defaultComputer = new Computer();

      JUnitCommandLineParseResultWrapper jUnitCommandLineParseResult = JUnitCommandLineParseResultWrapper.parse(args);
      Result res = core.run(jUnitCommandLineParseResult.createRequest(defaultComputer));
      System.exit(res.wasSuccessful() ? 0 : 1);
   }
}



   // import org.junit.runner.JUnitCore;
   // import org.junit.runner.Result;
   // import org.junit.runner.notification.Failure;
   
   // public class TestRunner {
   //    public static void main(String[] args) {
   //       Result result = JUnitCore.runClasses(new Class[]{MdsDataTest.class, MdsConglomTest.class, MdsConnectionTest.class, MdsDimensionTest.class, 
   // 	    MdsEventTest.class, MdsExpressionCompileTest.class, MdsFunctionTest.class,  MdsRangeTest.class,  MdsSignalTest.class, 
   // 	    MdsStringTest.class, MdsTreeTest.class, MdsTreeNodeTest.class, MdsWindowTest.class});
        
   //       for (Failure failure : result.getFailures()) {
   //          System.out.println(failure.toString());
   //       }
   
   //       System.out.println(result.wasSuccessful());
   //    }
   // } 


