/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package mds;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.Int16Array;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.Call;
import mds.data.descriptor_s.CString;

@SuppressWarnings("static-method")
public class Mds_Test{
    private static Mds mds;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        Mds_Test.mds = AllTests.setUpBeforeClass();
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
        AllTests.tearDownAfterClass(Mds_Test.mds);
    }

    @Before
    public void setUp() throws Exception {/*stub*/}

    @After
    public void tearDown() throws Exception {/*stub*/}

    @Test
    public void test_getDescriptor() throws MdsException {
        Assert.assertEquals("Set_Range(1000,0. /*** etc. ***/)", Mds_Test.mds.getDescriptor("Array([1000],0.)").toString());
        Assert.assertEquals("[[[1.1],[2.1]],[[3.1],[4.1]]]", Mds_Test.mds.getDescriptor("[[[1.1],[2.1]],[[3.1],[4.1]]]").toString());
        final Descriptor<?> array = new Int16Array(new short[]{1, 2, 3, 4, 5});
        Assert.assertEquals(Mds_Test.mds.getDescriptor("WORD([1, 2, 3, 4, 5])").decompile(), array.decompile());
        Assert.assertEquals("Word([1,2,3,4,5])", Mds_Test.mds.getDescriptor("$", array).decompile());
        Assert.assertEquals("\"123456789\"", Mds_Test.mds.getDescriptor("concat", new CString("123"), new CString("456"), new CString("789")).decompile());
    }

    @Test
    public void test_Messages() throws MdsException {
        final Call call = new Call(51, "TreeShr", "TreeCtx");
        Assert.assertArrayEquals(Mds_Test.mds.getDescriptor("SerializeOut(TreeShr->TreeCtx:P())").toByteArray(), call.serializeArray());
        Assert.assertArrayEquals(call.serializeArray(), Mds_Test.mds.getDescriptor("SerializeOut($)", call).toByteArray());
        Assert.assertEquals("TreeShr->TreeCtx:P()", Mds_Test.mds.getDescriptor("AS_IS(TreeShr->TreeCtx:P())").decompile());
        Assert.assertEquals("[TreeShr->TreeCtx:P(),TreeShr->TreeCtx:P()]", Mds_Test.mds.getDescriptor(List.list + "As_Is(TreeShr->TreeCtx:P()),AS_IS($))", new Call(DTYPE.POINTER, "TreeShr", "TreeCtx")).decompile());
    }
}
