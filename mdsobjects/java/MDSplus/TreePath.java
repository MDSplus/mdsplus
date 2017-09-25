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
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package MDSplus;

/**
 *
 * @author manduchi
 */
public class TreePath extends TreeNode
{
    java.lang.String path;
    public TreePath(java.lang.String path, Data help, Data units, Data error, Data validation) throws MdsException
    {
        super(help, units, error, validation);
        clazz = CLASS_S;
        dtype = DTYPE_PATH;
        this.path = path;
    }
    public TreePath(java.lang.String path, Tree tree, Data help, Data units, Data error, Data validation) throws MdsException
    {
        super(help, units, error, validation);
        clazz = CLASS_S;
        dtype = DTYPE_PATH;
        this.path = path;
        this.tree = tree;
    }

    public TreePath(java.lang.String path, Tree tree) throws MdsException
    {
        this(path, tree, null, null, null, null);
    }
    public TreePath(java.lang.String path) throws MdsException
    {
        this(path, null, null, null, null);
    }

    public static Data getData(java.lang.String path,  Data help, Data units, Data error, Data validation) throws MdsException
    {
        return new TreePath(path,  help,  units, error, validation);
    }
    
    public java.lang.String getString()
    {
        return path;
    }

    protected void resolveNid() throws MdsException
    {
        nid = Tree.findNode(tree.getCtx1(), tree.getCtx2(), path);
    }
}
