package org.junit.runner;

public class JUnitCommandLineParseResultWrapper extends JUnitCommandLineParseResult {

    public static JUnitCommandLineParseResultWrapper parse(String[] args) {
        JUnitCommandLineParseResultWrapper result = new JUnitCommandLineParseResultWrapper();
        result.parseParameters(args);
        return result;
    }

}
