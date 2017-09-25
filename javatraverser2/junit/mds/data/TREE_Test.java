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

import java.util.Arrays;
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
import mds.TreeShr.TagRefStatus;
import mds.data.descriptor_a.Float32Array;
import mds.data.descriptor_a.Float64Array;
import mds.data.descriptor_a.Uint64Array;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_s.Float32;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;
import mds.data.descriptor_s.Path;

@SuppressWarnings("static-method")
@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class TREE_Test{
    private static final String expt = AllTests.tree;
    private static final int    shot = 7633;
    private static Mds          mds;

    @BeforeClass
    public static final void setUpBeforeClass() throws Exception {
        TREE_Test.mds = AllTests.setUpBeforeClass();
    }

    @AfterClass
    public static final void tearDownAfterClass() throws Exception {
        AllTests.tearDownAfterClass(TREE_Test.mds);
    }

    @Test
    public void construct() throws Exception {
        NODE<?> node, node1;
        final TREE tree = new TREE(TREE_Test.mds, TREE_Test.expt, TREE_Test.shot);
        Assert.assertEquals(TREE_Test.shot, tree.setCurrentShot().getCurrentShot());
        Assert.assertTrue(tree.open(TREE.NEW).isOpen());
        Assert.assertEquals("Tree(\"TEST\", 7633, edit)", tree.toString());
        Assert.assertTrue(tree.getCtx().decompile().matches("Pointer\\(0x[a-f0-9]+\\)"));
        Assert.assertEquals("\\TEST::TOP.STRUCT", (node = tree.addNode("STRUCT", NODE.USAGE_STRUCTURE)).decompile());
        Assert.assertEquals("\\TEST::TOP.STRUCT:SIGNAL", (node = node.addNode("SIGNAL", NODE.USAGE_SIGNAL)).decompile());
        Assert.assertEquals("\\TEST::TOP:DATA", (node1 = tree.addNode("DATA", NODE.USAGE_SIGNAL)).getNciPath());
        Assert.assertEquals("[\\TEST::TOP:DATA, \\TEST::TOP.STRUCT:SIGNAL]", Arrays.toString(tree.findNodesWild(NODE.USAGE_SIGNAL)));
        Assert.assertArrayEquals(new Nid[]{new Nid(3), new Nid(2)}, tree.findNodesWildLL(NODE.USAGE_SIGNAL));
        Assert.assertTrue(node.makeSegment(new Float32(.1f), new Float32(.3f), new Float32Array(.1f, .2f, .3f), new Float64Array(1., 2., 3.), -1, 3).isSegmented());
        Assert.assertEquals(2, node.makeSegment(new Float32Array(.4f, .5f, .6f), new Float64Array(4., 5., 6.)).getNumSegments());
        Assert.assertArrayEquals(new double[]{6., 4., 3.}, node.putSegment(0, new Float64Array(6., 4., 3.)).getSegment(1).toDoubleArray(), 1e-9);
        Assert.assertArrayEquals(new double[]{7., 8., 0.}, node.makeSegment(new Float32(.7f), new Float32(.9f), new Float32Array(.7f, .8f, .9f), new Float64Array(7., 8., 0.), -1, 2).getSegment(2).toDoubleArray(), 1e-9);
        Assert.assertArrayEquals(new double[]{7., 8., 9.}, node.putRow(0, new Float64Array(9.)).getSegment(2).toDoubleArray(), 1e-9);
        Assert.assertArrayEquals(new double[]{.7, .9}, node.getSegmentLimits(2).toDoubleArray(), 1e-6);
        Assert.assertEquals("53;1;3,0,0,0,0,0,0,0,3;265389633", node.getSegmentInfo(2).toString());
        Assert.assertEquals("[]", node.getNciChildrenNids().toString());
        Assert.assertEquals("[\\TEST::TOP.STRUCT]", tree.getTop().getNciChildrenNids().toString());
        Assert.assertEquals("[\\TEST::TOP:DATA]", tree.getTop().getNciMemberNids().toString());
        Assert.assertTrue(node1.putRecord(node).isNidReference());
        Assert.assertTrue(tree.isNidReference(node1.getNidNumber()));
        Assert.assertTrue(node1.putRecord(node.toFullPath()).isPathReference());
        Assert.assertTrue(tree.isPathReference(node1.getNidNumber()));
        Assert.assertEquals(node.getNciMinPath(), node1.followReference().getNciMinPath());
        Assert.assertEquals("\\TEST::TOP:SIGNAL", node.setPath(":SIGNAL").toPath().toString());
        Assert.assertEquals("Build_Conglom(*, \"E1429\", *, *)", new Path(".STRUCT").addConglom("E1429", "E1429").getRecord().decompile());
        Assert.assertEquals("[DAT, SIG]", Arrays.toString(node.addTag("SIG").addTag("DAT").getTags()));
        Assert.assertEquals("[SIG, DAT]", Arrays.toString(node.setTags("SIG", "DAT").getTags()));
        Assert.assertEquals("[SIG, DAT]", Arrays.toString(node.getTagsLL()));
        Assert.assertEquals("[\\TEST::TOP, \\TEST::SIG, \\TEST::DAT]", tree.findTagsWildLL().keySet().toString());
        Assert.assertEquals("\\TEST::DAT", tree.findTagWild("*A*", TagRefStatus.init).data);
        Assert.assertEquals("[1.0, 2.0, 3.0]", Arrays.toString(tree.findNodesWild("***", NODE.USAGE_SIGNAL)[2].putRecord(new Signal(new Float64Array(1., 2., 3.), null, new Uint64Array(1, 2, 3))).getData().toDoubleArray()));
        Assert.assertEquals(NODE.Flags.COMPRESS_ON_PUT | NODE.Flags.NO_WRITE_MODEL, node.clearFlags(-1).setFlags(NODE.Flags.COMPRESS_ON_PUT | NODE.Flags.NO_WRITE_MODEL).getNciFlags());
        Assert.assertFalse(node.setOn(false).isOn());
        Assert.assertTrue(node.setOn(true).isOn());
        Assert.assertEquals("TreeUSAGE_SUBTREE", (node = tree.addNode("SUBTREE", NODE.USAGE_SUBTREE)).getNciUsageStr());
        Assert.assertEquals(2, tree.getNciMemberNids(0).getLength());
        Assert.assertEquals(2, tree.getNciChildrenNids(0).getLength());
        Assert.assertEquals(1, node.deleteInitialize());
        Assert.assertEquals("[\\TEST::TOP.SUBTREE]", Arrays.toString(tree.deleteNodeGetNids()));
        Assert.assertEquals(0, tree.deleteNodeExecute().findNodesWild(NODE.USAGE_SUBTREE).length);
        Assert.assertFalse(tree.writeTree().quitTree().isOpen());
    }

    @Before
    public void setUp() throws Exception {/*stub*/}

    @After
    public void tearDown() throws Exception {/*stub*/}
}
