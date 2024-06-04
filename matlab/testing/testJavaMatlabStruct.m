classdef testJavaMatlabStruct < matlab.unittest.TestCase
    
    properties
        use_python
    end

    properties(TestParameter)
        test_struct
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
        function test_struct = initialize_parameters()
            test_struct.empty            = struct();
            test_struct.scalar           = struct("field1", 1.);
            test_struct.scalar_nested    = struct("field1", struct("field2", 1.));
            test_struct.scalar_nested2   = struct("field1", struct("field2", struct("field2", 1.)));
            test_struct.vector_row       = [struct("field1", 1.) struct("field1", 2.) struct("field1", 3.)];
            test_struct.vector_col       = [struct("field1", 1.); struct("field1", 2.); struct("field1", 3.)];
            test_struct.vector_cell      = [struct("field1", {1}) struct("field1", {2}) struct("field1", {3})];
            test_struct.vector_empty     = [struct() struct()];
            test_struct.matrix           = [struct("field1", 1) struct("field1", 2); struct("field1", 3) struct("field1", 4); struct("field1", 5) struct("field1", 6)];
            test_struct.matrix_square    = [test_struct.matrix [struct("field1", 7); struct("field1", 8); struct("field1", 9)]];
            test_struct.matrix_3d(:,:,1) = test_struct.matrix;
            test_struct.matrix_3d(:,:,2) = [struct("field1", 7) struct("field1", 8); struct("field1", 9) struct("field1", 10); struct("field1", 11) struct("field1", 12)];
            test_struct.matrix_4d(:,:,:,1) = test_struct.matrix_3d;
            test_struct.matrix_4d(:,:,:,2) = test_struct.matrix_3d;
            test_struct.matrix_4d(:,:,:,3) = test_struct.matrix_3d;
        end
    end

    methods(Test)
        function test_javaToMatlabStruct_exceptions(testCase)
            test_type = MDSplus.List();
            test_type.append(MDSplus.Float64(1));
            test_type.append(MDSplus.String("mystring"));
            testCase.verifyError(@() mdsToMatlab(test_type), 'MDSplus:javaToMatlabStruct')
        end

        function test_javaMatlabStruct(testCase, test_struct)
            mdsStruct = mdsFromMatlab(test_struct);
            mlStruct = mdsToMatlab(mdsStruct);

            testCase.verifyEqual(mlStruct, test_struct);
        end
    end

end