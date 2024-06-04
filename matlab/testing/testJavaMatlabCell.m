classdef testJavaMatlabCell < matlab.unittest.TestCase
    
    properties
        use_python
    end

    properties(TestParameter)
        test_cell
    end
    
    methods (TestClassSetup)
        function set_use_java(testCase)
            % make sure these tests use the java interface
            testCase.use_python = mdsInfo().usePython;
            mdsUsePython(false);
        end
    end

    methods (TestClassTeardown)
        function reset_use_python(testCase)
            % reset the interface to the previous state
            mdsUsePython(testCase.use_python);
        end
    end

    methods(TestParameterDefinition, Static)
        function test_cell = initialize_parameters()
            test_cell.empty              = {};
            test_cell.scalar             = {1};
            test_cell.scalar_nested      = {{{1}}};
            test_cell.vector_row         = {1, {2}};
            test_cell.vector_row_nested  = {1, {2, 3}};
            test_cell.vector_col         = {1; 2; 3};
            test_cell.vector_col_nested  = {{1; 2}; {3; {4}}; {5, 6}};
            test_cell.vector_empty       = {{1, {}; {}, 4}};
            test_cell.matrix             = {1 2; 3 4; 5 6};
            test_cell.matrix_square      = {1 2 3; 4 5 6; 7 8 9};
            test_cell.matrix_struct      = {1, {2}; 3, struct("q",[4 5], "c", struct("a", {1, [1,2]})); 4, 5};
            test_cell.matrix_nested      = {1, {2, 3}; [3 4 5], struct("q",4, "c", {1, {2,3}}); {{3}}, int32(4)};
            test_cell.matrix_3d(:,:,1)   = {1 2 3; 4 5 6};
            test_cell.matrix_3d(:,:,2)   = {7 8 9; 10 11 12};
            test_cell.matrix_4d(:,:,:,1) = test_cell.matrix_3d;
            test_cell.matrix_4d(:,:,:,2) = test_cell.matrix_3d;
            test_cell.matrix_4d(:,:,:,3) = test_cell.matrix_3d;
        end
    end

    methods(Test)
        function test_javaMatlabCell(testCase, test_cell)
            mdsCell = mdsFromMatlab(test_cell);
            mlCell  = mdsToMatlab(mdsCell);

            testCase.verifyEqual(mlCell, test_cell);
        end
    end

end
