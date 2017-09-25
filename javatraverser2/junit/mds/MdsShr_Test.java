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
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.Int8Array;
import mds.data.descriptor_r.Range;

@SuppressWarnings("static-method")
public class MdsShr_Test{
    private static MdsShr mdsshr;
    private static Mds    mds;

    @BeforeClass
    public static final void setUpBeforeClass() throws Exception {
        MdsShr_Test.mds = AllTests.setUpBeforeClass();
        MdsShr_Test.mdsshr = new MdsShr(MdsShr_Test.mds);
    }

    @AfterClass
    public static final void tearDownAfterClass() throws Exception {
        AllTests.tearDownAfterClass(MdsShr_Test.mds);
    }

    @Before
    public void setUp() throws Exception {/*stub*/}

    @After
    public void tearDown() throws Exception {/*stub*/}

    @Test
    public final void testMdsCompress() throws MdsException {
        ARRAY<?> ca;
        Assert.assertEquals("Set_Range(15000,0 /*** etc. ***/)", MdsShr_Test.mds.getString("_a=DATA(0:29999:2);_s=MdsShr->MdsCompress(0,0,xd(_a),xd(_a));_s=TdiShr->TdiDecompile(xd(_a),xd(_a),val(-1));_a"));
        Assert.assertEquals("Set_Range(15000,0 /*** etc. ***/)", (ca = MdsShr_Test.mdsshr.mdsCompress(null, new Range(0, 29999, 2).getDataA())).decompile());
        Assert.assertEquals(10, ca.getDataA().toInt(5));
    }

    @Test
    public final void testMdsEvent() throws MdsException {
        Assert.assertEquals(1, MdsShr_Test.mdsshr.mdsEvent(null, "myevent"));
    }

    @Test
    public final void testMdsGetMsgDsc() throws MdsException {
        Assert.assertEquals("%SS-S-SUCCESS, Success", MdsShr_Test.mdsshr.mdsGetMsgDsc(null, 1));
        Assert.assertEquals("%SS-S-SUCCESS, Success", MdsShr_Test.mdsshr.mdsGetMsg(null, 1));
    }

    @Test
    public final void testMdsSerializeDsc() throws MdsException {
        final String result = "Build_With_Units(101325., \"Pa\")";
        final Int8Array serial = MdsShr_Test.mdsshr.mdsSerializeDscOut(null, "$P0");
        Assert.assertEquals(result, Descriptor.deserialize(serial.getBuffer()).decompile());
        Assert.assertEquals(result, MdsShr_Test.mdsshr.mdsSerializeDscIn(null, serial).decompile());
    }
}
