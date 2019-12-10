package org.junit.runner;
import java.lang.reflect.Constructor;
import org.junit.internal.JUnitSystem;
import org.junit.internal.RealSystem;
import org.junit.internal.TextListener;
import org.junit.runner.notification.RunListener;
import junit.runner.Version;

public class TapTestRunner extends JUnitCore{
	public static void main(String... args) {
		Result result = new TapTestRunner().runMain(new RealSystem(), args);
		System.exit(result.wasSuccessful() ? 0 : 1);
	}
	@Override
	Result runMain(JUnitSystem system, String... args) {
		system.out().println("Tap JUnit version " + Version.id());
		JUnitCommandLineParseResult jUnitCommandLineParseResult = JUnitCommandLineParseResult.parse(args);
		// Add TAP Reporter Listener to the core object executor
		RunListener listener;
		try{
			@SuppressWarnings("unchecked")
			Class<RunListener> JUnitTestTapReporter = (Class<RunListener>)Class.forName("org.tap4j.ext.junit.JUnitTestTapReporter");
			if (JUnitTestTapReporter==null) throw new Exception();
			Constructor<RunListener> constructor = JUnitTestTapReporter.getConstructor();
			listener = constructor.newInstance();
		}catch(Exception e){
			e.printStackTrace();
			listener = new TextListener(system);
		}
		addListener(listener);
		return run(jUnitCommandLineParseResult.createRequest(defaultComputer()));
	}
}