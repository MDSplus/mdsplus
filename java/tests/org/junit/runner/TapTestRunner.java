package org.junit.runner;
import org.junit.internal.JUnitSystem;
import org.junit.internal.RealSystem;
import org.junit.runner.notification.Failure;
import org.tap4j.ext.junit.listener.TapListenerClass;
import junit.runner.Version;

public class TapTestRunner extends JUnitCore{
	public static void main(String... args) {
		System.setProperty("tap.junit.results",".");
		Result result = new TapTestRunner().runMain(new RealSystem(), args);
		if (!result.wasSuccessful()) {
			for (Failure f:result.getFailures()) {
				System.err.println(f.getTrace());
			}
		}
		System.exit(result.wasSuccessful() ? 0 : 1);
	}
	@Override
	Result runMain(JUnitSystem system, String... args) {
		TapListenerClass l = new TapListenerClass();
		system.out().println("JUnit version " + Version.id() + " with " + l.getClass().getName());
		addListener(l);
		JUnitCommandLineParseResult jUnitCommandLineParseResult = JUnitCommandLineParseResult.parse(args);
		Result result = run(jUnitCommandLineParseResult.createRequest(defaultComputer()));
		try{
			l.testRunFinished(result);
		}catch(Exception e){
			e.printStackTrace();
		}
		return result;
	}
}
