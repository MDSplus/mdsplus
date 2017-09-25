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
package mds.data;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runners.MethodSorters;
import mds.AllTests;
import mds.Mds;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.function.CONST.dA0;
import mds.data.descriptor_r.function.CONST.dAlpha;
import mds.data.descriptor_r.function.CONST.dAmu;
import mds.data.descriptor_r.function.CONST.dC;
import mds.data.descriptor_r.function.CONST.dCal;
import mds.data.descriptor_r.function.CONST.dDegree;
import mds.data.descriptor_r.function.CONST.dFalse;
import mds.data.descriptor_r.function.CONST.dFaraday;
import mds.data.descriptor_r.function.CONST.dG;
import mds.data.descriptor_r.function.CONST.dGas;
import mds.data.descriptor_r.function.CONST.dH;
import mds.data.descriptor_r.function.CONST.dHbar;
import mds.data.descriptor_r.function.CONST.dI;
import mds.data.descriptor_r.function.CONST.dK;
import mds.data.descriptor_r.function.CONST.dMe;
import mds.data.descriptor_r.function.CONST.dMissing;
import mds.data.descriptor_r.function.CONST.dMp;
import mds.data.descriptor_r.function.CONST.dN0;
import mds.data.descriptor_r.function.CONST.dNa;
import mds.data.descriptor_r.function.CONST.dP0;
import mds.data.descriptor_r.function.CONST.dPi;
import mds.data.descriptor_r.function.CONST.dQe;
import mds.data.descriptor_r.function.CONST.dRe;
import mds.data.descriptor_r.function.CONST.dRoprand;
import mds.data.descriptor_r.function.CONST.dRydberg;
import mds.data.descriptor_r.function.CONST.dT0;
import mds.data.descriptor_r.function.CONST.dTorr;
import mds.data.descriptor_r.function.CONST.dTrue;

@SuppressWarnings("static-method")
@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class CONST_Test{
    private static Mds mds;

    @BeforeClass
    public static final void setUpBeforeClass() throws Exception {
        CONST_Test.mds = AllTests.setUpBeforeClass();
    }

    @AfterClass
    public static final void tearDownAfterClass() throws Exception {
        AllTests.tearDownAfterClass(CONST_Test.mds);
    }

    @Test
    public void compare() throws Exception {
        Assert.assertEquals(new dA0().evaluate(), CONST_Test.mds.getDescriptor("$A0", Descriptor.class));
        Assert.assertEquals(new dAlpha().evaluate(), CONST_Test.mds.getDescriptor("$Alpha", Descriptor.class));
        Assert.assertEquals(new dAmu().evaluate(), CONST_Test.mds.getDescriptor("$Amu", Descriptor.class));
        Assert.assertEquals(new dC().evaluate(), CONST_Test.mds.getDescriptor("$C", Descriptor.class));
        Assert.assertEquals(new dCal().evaluate(), CONST_Test.mds.getDescriptor("$Cal", Descriptor.class));
        Assert.assertEquals(new dDegree().evaluate(), CONST_Test.mds.getDescriptor("$Degree", Descriptor.class));
        Assert.assertEquals(new dFalse().evaluate(), CONST_Test.mds.getDescriptor("$False", Descriptor.class));
        Assert.assertEquals(new dFaraday().evaluate(), CONST_Test.mds.getDescriptor("$Faraday", Descriptor.class));
        Assert.assertEquals(new dG().evaluate(), CONST_Test.mds.getDescriptor("$G", Descriptor.class));
        Assert.assertEquals(new dGas().evaluate(), CONST_Test.mds.getDescriptor("$Gas", Descriptor.class));
        Assert.assertEquals(new dH().evaluate(), CONST_Test.mds.getDescriptor("$H", Descriptor.class));
        Assert.assertEquals(new dHbar().evaluate(), CONST_Test.mds.getDescriptor("$Hbar", Descriptor.class));
        Assert.assertEquals(new dI().evaluate(), CONST_Test.mds.getDescriptor("$I", Descriptor.class));
        Assert.assertEquals(new dK().evaluate(), CONST_Test.mds.getDescriptor("$K", Descriptor.class));
        Assert.assertEquals(new dMe().evaluate(), CONST_Test.mds.getDescriptor("$Me", Descriptor.class));
        Assert.assertEquals(new dMissing().evaluate(), CONST_Test.mds.getDescriptor("$Missing", Descriptor.class));
        Assert.assertEquals(new dMp().evaluate(), CONST_Test.mds.getDescriptor("$Mp", Descriptor.class));
        Assert.assertEquals(new dN0().evaluate(), CONST_Test.mds.getDescriptor("$N0", Descriptor.class));
        Assert.assertEquals(new dNa().evaluate(), CONST_Test.mds.getDescriptor("$Na", Descriptor.class));
        Assert.assertEquals(new dP0().evaluate(), CONST_Test.mds.getDescriptor("$P0", Descriptor.class));
        Assert.assertEquals(new dPi().evaluate(), CONST_Test.mds.getDescriptor("$Pi", Descriptor.class));
        Assert.assertEquals(new dQe().evaluate(), CONST_Test.mds.getDescriptor("$Qe", Descriptor.class));
        Assert.assertEquals(new dRe().evaluate(), CONST_Test.mds.getDescriptor("$Re", Descriptor.class));
        Assert.assertEquals(new dRoprand().evaluate(), CONST_Test.mds.getDescriptor("$Roprand", Descriptor.class));
        Assert.assertEquals(new dRydberg().evaluate(), CONST_Test.mds.getDescriptor("$Rydberg", Descriptor.class));
        Assert.assertEquals(new dT0().evaluate(), CONST_Test.mds.getDescriptor("$T0", Descriptor.class));
        Assert.assertEquals(new dTorr().evaluate(), CONST_Test.mds.getDescriptor("$Torr", Descriptor.class));
        Assert.assertEquals(new dTrue().evaluate(), CONST_Test.mds.getDescriptor("$True", Descriptor.class));
    }

    @Before
    public void setUp() throws Exception {/*stub*/}

    @After
    public void tearDown() throws Exception {/*stub*/}
}
