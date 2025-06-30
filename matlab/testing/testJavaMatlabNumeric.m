classdef testJavaMatlabNumeric < matlab.unittest.TestCase

    properties
        use_python
    end

    properties(TestParameter)
        ints   = num2cell(-128:15:127);
        uints  = num2cell(0:15:255);
        floats = num2cell([-(2-2^-23)*2^127 sqrt(2) exp(1) pi (2-2^-23)*2^127]);
        arrays = {[4 5 6]};
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

    methods(Test)
        function test_scalar_int_from(testCase, ints)
            testCase.verifyEqual(mdsFromMatlab(int8(ints)),                MDSplus.Int8(ints))
            testCase.verifyEqual(mdsFromMatlab(int16(2^8*ints)),           MDSplus.Int16(2^8*ints))
            testCase.verifyEqual(mdsFromMatlab(int32(2^8*2^16*ints)),      MDSplus.Int32(2^8*2^16*ints))
            testCase.verifyEqual(mdsFromMatlab(int64(2^8*2^16*2^32*ints)), MDSplus.Int64(2^8*2^16*2^32*ints))
        end

        function test_scalar_uint_from(testCase, uints)
            testCase.verifyEqual(mdsFromMatlab(uint8(uints)),     MDSplus.Uint8(uints))
            t = uint16(2); zt = (t^8+1);
            testCase.verifyEqual(mdsFromMatlab(zt*uint16(uints)), MDSplus.Uint16(zt*uint16(uints)))
            t = uint32(2); zt = (t^8+1)*(t^16+1);
            testCase.verifyEqual(mdsFromMatlab(zt*uint32(uints)), MDSplus.Uint32(zt*uint32(uints)))
            t = uint64(2); zt = (t^8+1)*(t^16+1)*(t^32+1);
            testCase.verifyEqual(mdsFromMatlab(zt*uint64(uints)), MDSplus.Uint64(zt*uint64(uints)))
        end

        function test_float_from(testCase, floats)
            testCase.verifyEqual(mdsFromMatlab(single(floats)), MDSplus.Float32(floats))
            testCase.verifyEqual(mdsFromMatlab(double(floats)), MDSplus.Float64(floats))
        end
        
        function test_string_from(testCase)
            test_string = "my_string";
            test_char   = 'my_char';

            testCase.verifyEqual(mdsFromMatlab(test_string), MDSplus.String(test_string))
            testCase.verifyEqual(mdsFromMatlab(test_char),   MDSplus.String(test_char))
        end

        function test_array_from(testCase)
            testCase.verifyEqual(mdsFromMatlab(int8([4 5 6])), ...
                javaObject('MDSplus.Int8Array', reshape([4 5 6], [], 1), [1 3]))
            testCase.verifyEqual(mdsFromMatlab(int16([4; 5; 6])), ...
                javaObject('MDSplus.Int16Array', reshape([4; 5; 6], [], 1), [3 1]))
            testCase.verifyEqual(mdsFromMatlab(int32([1 2 3; 4 5 6])), MDSplus.Int32Array([1 4 2; 5 3 6]))
            testCase.verifyEqual(mdsFromMatlab(["my_string" "my_string_2"]), ...
                javaObject('MDSplus.StringArray', reshape(["my_string" "my_string_2"], [], 1), [1 2]))
        end

        function test_cell_and_struct_from(testCase)
            mycell = {1, 2, 3};
            mystruct = struct("field1", 1., "field2", "my_string");

            testCase.verifyEqual(mdsToMatlab(mdsFromMatlab(mycell)), mycell);
            testCase.verifyEqual(mdsToMatlab(mdsFromMatlab(mystruct)), mystruct);
        end

        function test_exceptions_from(testCase)
            test_handle = @sin;
            testCase.verifyError(@() mdsFromMatlab(test_handle), 'MDSplus:javaFromMatlab')
        end

        function test_scalar_int_to(testCase, ints)
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int8(ints)),           int8(ints))
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int16(2^8*ints)),      int16(2^8*ints))
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int32(2^8*2^16*ints)), int32(2^8*2^16*ints))
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int64(2^8*2^16*2^32*ints)), int64(2^8*2^16*2^32*ints))
        end

        function test_scalar_uint_to(testCase, uints)
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint8(uints)),     uint8(uints))
            t = uint16(2); zt = (t^8+1);
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint16(zt*uints)), uint16(zt*uints))
            t = uint32(2); zt = (t^8+1)*(t^16+1);
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint32(zt*uints)), uint32(zt*uints))
            t = uint64(2); zt = (t^8+1)*(t^16+1)*(t^32+1);
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint64(zt*uints)), uint64(zt*uints))
        end

        function test_float_to(testCase, floats)
            testCase.verifyEqual(mdsToMatlab(MDSplus.Float32(floats)), single(floats))
            testCase.verifyEqual(mdsToMatlab(MDSplus.Float64(floats)), double(floats))
        end

        function test_array_to(testCase, arrays)
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int8Array(arrays)),   int8(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int16Array(arrays)),  int16(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int32Array(arrays)),  int32(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Int64Array(arrays)),  int64(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint8Array(arrays)),  uint8(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint16Array(arrays)), uint16(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint32Array(arrays)), uint32(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Uint64Array(arrays)), uint64(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Float32Array(arrays)), single(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.Float64Array(arrays)), double(arrays)')
            testCase.verifyEqual(mdsToMatlab(MDSplus.StringArray(["my_string" "my_string_2"])), ["my_string" "my_string_2"]')
        end

        function test_exceptions_to(testCase)
            test_type = MDSplus.Range(MDSplus.Float64(1), MDSplus.Float64(10), MDSplus.Float64(1));
            testCase.verifyError(@() mdsToMatlab(test_type), 'MDSplus:mdsToMatlab')

            test_type = double(1);
            testCase.verifyEqual(mdsToMatlab(test_type), test_type);
        end
    end

end