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
import MDSplus.Tree;
import MDSplus.TreeNode;
import MDSplus.Int32;
import java.io.*;
import java.util.*;

/*
xsltproc sdd2mds.xsl SI_4.xml > si.xml
javac -cp /usr/local/mdsplus/java/classes/mdsobjects.jar Xml2Mds.java
java -cp /usr/local/mdsplus/java/classes/mdsobjects.jar:. Xml2Mds si   
*/

class Xml2Mds
{
    public static void main(String args[])
    {
	boolean debug = false;
	int num_pv_node = 0;

	if( args.length != 2 && args.length != 3  )
	{
	    System.out.println("Usage: java Xml2Mds <file xml> <ExperimentName> [debug] ");
	    System.exit(0);
	}
	Vector groupV = new Vector();
        String xmlFile = args[0];
	String expName = args[1].trim();
	if(args.length == 3 && args[2].compareToIgnoreCase("debug") == 0 ) debug = true;
	Tree tree = null;
	try {
	    BufferedReader br = new BufferedReader(new FileReader(xmlFile+".xml"));
	    tree = new Tree(expName, -1, "NEW");
	    tree.addNode(":FILE_SIZE", "NUMERIC");
	    TreeNode currNode = tree.getNode(":FILE_SIZE");
	    currNode.putData(new Int32(1000000000));
	    tree.addNode(":GET_TRESH", "NUMERIC");
	    currNode = tree.getNode(":GET_TRESH");
	    currNode.putData(new Int32(1));
	    tree.addNode(":IGN_FUTURE", "NUMERIC");
	    currNode = tree.getNode(":IGN_FUTURE");
	    currNode.putData(new Int32(6));
	    tree.addNode(":MAX_REPEAT", "NUMERIC");
	    tree.addNode(":WRITE_PERIOD", "NUMERIC");
	    String currVarName, currLine, currDescription, currSignalName;
	    while((currLine = br.readLine()) != null)
	    {
		//Variable to skip
		if( currLine.contains("HWCF") ) continue;
		if( currLine.contains("COS") ) continue;

		StringTokenizer st0 = new StringTokenizer(currLine, ";");
		currVarName = st0.nextToken();
		try {
		    currDescription = st0.nextToken();
		}catch(Exception exc){currDescription = null;}	
		try {
		    currSignalName = st0.nextToken();
		}catch(Exception exc){currSignalName = null;}	

		StringTokenizer st = new StringTokenizer(currVarName, ":");
		String firstPart = st.nextToken();
		StringTokenizer st1 = new StringTokenizer(firstPart, "-");
		st1.nextToken();
		String systemName = st1.nextToken();
		if(groupV.indexOf(systemName) == -1)
		{
                    if (debug) System.out.println("System : " + systemName);
		    tree.addNode("."+systemName, "STRUCTURE");
		    tree.addNode("."+systemName+":DISABLE", "NUMERIC");
		    groupV.addElement(systemName);
		}

		String subsystemName;
		String subsystemPartName;
		try {
			subsystemPartName = st1.nextToken();
			subsystemName = systemName+"."+subsystemPartName;
		}catch(Exception exc) {continue;} //In case of non correct breakdown

		if(groupV.indexOf(subsystemName) == -1)
		{
                    if (debug) System.out.println("SubSystem : "+subsystemName);
		    tree.addNode("."+subsystemName, "STRUCTURE");
		    tree.addNode("."+subsystemName+":DISABLE", "NUMERIC");
		    groupV.addElement(subsystemName);
		}
		String secondPart = st.nextToken();
		StringTokenizer st2 = new StringTokenizer(secondPart, "-");
		String componentName = st2.nextToken();
		String componentPath = subsystemName+"."+componentName;
		if(groupV.indexOf(componentPath) == -1)
		{
		    if (debug) System.out.println("Component : "+componentPath);
		    tree.addNode("."+componentPath, "STRUCTURE");
		    tree.addNode("."+componentPath+":DISABLE", "NUMERIC");
		    groupV.addElement(componentPath);
		}
	   	//Group hierarchy checked. Add var specific nodes
                num_pv_node++;
		String name = st2.nextToken();
 		String path = ("."+componentPath+"."+name).trim();

		if (debug) System.out.println("Var name : "+name);

		tree.addNode(path, "STRUCTURE");
		tree.addNode(path+":REC_NAME", "TEXT");
		currNode = tree.getNode(path+":REC_NAME");
		currNode.putData(new MDSplus.String(currVarName));

		tree.addNode(path+":REC_SIGNAL", "TEXT");
		tree.addNode(path+":EGU", "TEXT");
		tree.addNode(path+":HOPR", "NUMERIC");
		tree.addNode(path+":LOPR", "NUMERIC");
		tree.addNode(path+":PERIOD", "NUMERIC");
		tree.addNode(path+":IS_DISABLE", "NUMERIC");
		tree.addNode(path+":SCAN_MODE", "TEXT");
		currNode = tree.getNode(path+":SCAN_MODE");
		currNode.putData(new MDSplus.String("MONITOR"));
		tree.addNode(path+":DESCRIPTION", "TEXT");
		if(currDescription != null)
		{
  		    currNode = tree.getNode(path+":DESCRIPTION");
		    currNode.putData(new MDSplus.String(currDescription));
		}
		if(currSignalName != null)
		{
  		    currNode = tree.getNode(path+":REC_SIGNAL");
		    currNode.putData(new MDSplus.String(currSignalName));
		}
		tree.addNode(path+":ALARM", "SIGNAL");
		tree.addNode(path+":VAL", "SIGNAL");

		//String tagName=currVarName.replaceFirst("-","");
		//tagName=tagName.replaceFirst("-","");
		//tagName=tagName.replaceFirst("-","_");
 
		String tagName=currVarName.replace("-","_");
		tagName=tagName.replace(':','_').trim();
		if(!systemName.equals("SKPC") && !systemName.equals("SPIS") )
		  tagName=tagName.replaceFirst("_"+subsystemPartName,"");

		currNode = tree.getNode(path+":VAL");
		if( debug ) System.out.println("Tag name " + tagName + " subsystemPartName " + subsystemPartName );
		currNode.addTag(tagName);
	    }
	    tree.write();
            System.out.println("Added " + num_pv_node + " process variable nodes ");
	}
	catch(Exception exc)
	{
	     try{
	    	tree.write();
	     }catch(Exception exc1){}
	     System.out.println("Error on eperiment "+expName+" : "+exc);
	}
    }
}

