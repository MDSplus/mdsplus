import matlab.unittest.TestSuite
import matlab.unittest.TestRunner
import matlab.unittest.plugins.CodeCoveragePlugin
% import matlab.unittest.plugins.codecoverage.CoverageResult
import matlab.unittest.plugins.XMLPlugin

%% add tests in the current dir
test_folder = '.';
suite = TestSuite.fromFolder(test_folder,'IncludingSubfolders', false);

%% set up the runner
runner = TestRunner.withNoPlugins;

%% set up the runner plugins
% this part commented since coverage and xml results is currently not supported by the testing framework

% format = CoverageResult;
% coveragePlugin = CodeCoveragePlugin.forFolder(test_folder, ...
%     Producing=format,MetricLevel="mcdc");

% coveragePlugin = CodeCoveragePlugin.forFolder([getenv('MDSPLUS_DIR') '/matlab']);
% runner.addPlugin(coveragePlugin)

% xmlPlugin = XMLPlugin.producingJUnitFormat('testResults.xml');
% runner.addPlugin(xmlPlugin)

%% run the tests
results = runner.run(suite);

t = table(results);

%% print results
% required to compare the result output to a string in run_tests.py
fprintf("PASSED:     %3d\n", nnz(t.Passed))
fprintf("FAILED:     %3d\n", nnz(t.Failed))
fprintf("INCOMPLETE: %3d\n", nnz(t.Incomplete))


