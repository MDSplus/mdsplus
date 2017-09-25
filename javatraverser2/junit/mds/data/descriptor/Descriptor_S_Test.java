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
package mds.data.descriptor;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import mds.AllTests;
import mds.Mds;
import mds.MdsException;
import mds.data.descriptor_s.Int64;
import mds.data.descriptor_s.Uint64;

@SuppressWarnings("static-method")
public final class Descriptor_S_Test{
    private static Mds mds;

    @BeforeClass
    public static final void setUpBeforeClass() throws Exception {
        Descriptor_S_Test.mds = AllTests.setUpBeforeClass();
    }

    @AfterClass
    public static final void tearDownAfterClass() throws Exception {
        AllTests.tearDownAfterClass(Descriptor_S_Test.mds);
    }

    @Before
    public final void setUp() throws Exception {/*stub*/}

    @After
    public final void tearDown() throws Exception {/*stub*/}

    @Test
    public final void testComplex() throws MdsException {
        Assert.assertEquals("Cmplx(100.,30000.)", Descriptor_S_Test.mds.getDescriptor("cmplx(1.e2,3.e4)").decompile());
    }

    @Test
    public final void testFloat() throws MdsException {
        Assert.assertEquals("10D-6", Descriptor_S_Test.mds.getDescriptor("1D-5").decompile());
        Assert.assertEquals(".0001D0", Descriptor_S_Test.mds.getDescriptor("1D-4").decompile());
        Assert.assertEquals(".001D0", Descriptor_S_Test.mds.getDescriptor("1D-3").decompile());
        Assert.assertEquals(".01D0", Descriptor_S_Test.mds.getDescriptor("1D-2").decompile());
        Assert.assertEquals(".1D0", Descriptor_S_Test.mds.getDescriptor("1D-1").decompile());
        Assert.assertEquals("1D0", Descriptor_S_Test.mds.getDescriptor("1D0").decompile());
        Assert.assertEquals("10D0", Descriptor_S_Test.mds.getDescriptor("1D1").decompile());
        Assert.assertEquals("1000000000000000D0", Descriptor_S_Test.mds.getDescriptor("1D15").decompile());
        Assert.assertEquals("10D15", Descriptor_S_Test.mds.getDescriptor("1D16").decompile());
        Assert.assertEquals("100D15", Descriptor_S_Test.mds.getDescriptor("1D17").decompile());
        Assert.assertEquals("1D18", Descriptor_S_Test.mds.getDescriptor("1D18").decompile());
    }

    @Test
    public final void testInt64() {
        Assert.assertEquals("-123456789012345Q", new Int64(-123456789012345l).decompile());
        Assert.assertEquals("123456789012345QU", new Uint64(123456789012345l).decompile());
        Assert.assertEquals(100000, new Uint64(100000).toInt());
        Assert.assertEquals(100000l, new Uint64(100000).toLong());
        Assert.assertEquals(100000.f, new Uint64(100000).toFloat(), 1e-5f);
        Assert.assertEquals(100000., new Uint64(100000).toDouble(), 1e-9);
        Assert.assertEquals("100000", new Uint64(100000).getAtomic().toString());
    }

    @Test
    public final void testTString() throws MdsException {
        Assert.assertEquals("\"test\"", Descriptor_S_Test.mds.getDescriptor("'test'").decompile());
        Assert.assertEquals("'\"test\"'", Descriptor_S_Test.mds.getDescriptor("'\\\"test\\\"'").decompile());
    }
}
