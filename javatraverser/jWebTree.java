import java.io.*;

import javax.servlet.*;

import javax.servlet.http.*;

import java.util.*;

import java.net.*;



public class jWebTree extends HttpServlet implements SingleThreadModel{

    Database tree = null;

    int shot;

    String experiment;

    boolean display_wave;

    boolean is_jscope = false;

    NidData default_nid;

    String default_path;

    HttpServletResponse response;

    static final String BASEREF = "jWebTree";

    static final int MAX_DISPLAY_LENGTH = 4096;

    static final String FIRST_PLUGIN_DEF = 

        "<OBJECT classid=\"clsid:8AD9C840-044E-11D1-B3E9-00805F499D93\" "+

        " NAME = \"WaveDisplay\" HEIGHT = \"200\" WIDTH = \"600\" ALIGN = \"baseline\"  codebase=\"http://java.sun.com/products/plugin/1.3/jinstall-13-win32.cab#Version=1,3,0,0\">"+"\n"+

        "<PARAM NAME = \"CODEBASE\" VALUE = \"/\""+"\n"+

        "<PARAM NAME = \"ARCHIVE\" VALUE = \"WaveDisplay.jar\" >"+"\n"+

        "<PARAM NAME = \"CODE\" VALUE = \"WaveDisplay.class\" >"+"\n"+

        "<PARAM NAME = \"NAME\" VALUE = \"WaveDisplay\" >"+"\n"+

        "<PARAM NAME=\"type\" VALUE=\"application/x-java-applet;version=1.3\">"+"\n"+

        "<PARAM NAME=\"scriptable\" VALUE=\"false\">"+"\n"+

        "<PARAM NAME = \"AUTENTICATION\" VALUE = \"b2JhcmFuYTpjbHVibGF0aW5v\">"+

        "<PARAM NAME = \"SIGNAL\" VALUE  = \" url = ";

        //mds://150.178.3.80/a/14000/\emra_it

        

    static final String SECOND_PLUGIN_DEF = "\">"+"\n"+

        "<COMMENT>"+"\n"+

        "<EMBED type=\"application/x-java-applet;version=1.3\""+"\n"+ 

        "CODEBASE = \"/\""+"\n"+

        "CODE = \"WaveDisplay.class\"+\"\n"+

        "ARCHIVE = \"WaveDisplay.jar\""+"\n"+ 

        "NAME = \"WaveDisplay\" WIDTH = \"700\" HEIGHT = \"250\" ALIGN = \"baseline\""+"\n"+ 

        "AUTENTICATION = \"b2JhcmFuYTpjbHVibGF0aW5v\""+

        "SIGNAL=\"url = ";

        //mds:://150.178.3.80/a/14000/\emra_it title = TITOLO "  

    static final String THIRD_PLUGIN_DEF = 

        "\"\nscriptable=false pluginspage=\"http://java.sun.com/products/plugin/1.3/plugin-install.html\">"+"\n"+

        "<NOEMBED>"+"\n"+

        "</COMMENT>"+"\n"+

        "No JDK 1.2 support for APPLET!!"+"\n"+

        "</NOEMBED></EMBED>"+"\n"+

        "</OBJECT>";



    protected String getPlugin(String path)

    {

        String url, temp;

        if(experiment.equals("jet"))

        {

            try{

                StringTokenizer st = new StringTokenizer(path,":");

                st.nextToken();

                st.nextToken();

                url = "rda://data.jet.uk/"+st.nextToken()+"/"+shot

                    +"/"+st.nextToken()+"/"+st.nextToken();

            }catch(Exception exc){return "<h1>Cannot define plugin</h1>";}

        }

        else 

        {

            try {

               url = "mds://"+InetAddress.getLocalHost().getHostAddress()+"/"+experiment+

                  "/"+shot+"/"+path;

    //        url = "mds://"+"150.178.3.80"+"/"+experiment+

    //           "/"+shot+"/"+path;

            }catch(Exception exc){return "<h1>Cannot define plugin</h1>";}

        }

        temp =  FIRST_PLUGIN_DEF+url+SECOND_PLUGIN_DEF+url+THIRD_PLUGIN_DEF;

        return temp;

    }



    public void doGet(HttpServletRequest request, HttpServletResponse response)

    throws IOException, ServletException

    {

        String curr_experiment = request.getParameter("experiment");

        String shot_str = request.getParameter("shot");

        String curr_path = request.getParameter("path");

        try {

            display_wave = request.getParameter("display").equals("true");

        }catch(Exception exc){display_wave = false; }

        try {

            is_jscope = request.getParameter("jscope").equals("true");

        }catch(Exception exc){is_jscope = false; }

        

        int i, curr_shot;

        

        this.response = response;      

        response.setContentType("text/html");

        PrintWriter out = response.getWriter();

        out.println("<html>");

        out.println("<body>");

        out.println("<head>");

        out.println("<title>Browse Experiment</title>");

        out.println("</head>");

        out.println("<body>");



      

        try {

            curr_shot = Integer.parseInt(shot_str);

        }catch(Exception exc) {curr_shot = -1; }

        if(experiment == null || !experiment.equals(curr_experiment)||curr_shot != shot)

        {

            try {

                if(tree != null)

                    tree.close();

                experiment = curr_experiment;

                shot = curr_shot;

                if(experiment.equals("jet"))

                    tree = new Database(experiment, -1);

                else

                    tree = new Database(experiment, shot);

                tree.open();

            }catch(Exception exc) 
	   {
		System.err.println(exc);
		tree = null; 
	    }

        }

        if(tree == null)

        {

            out.println("<h1>Cannot open " + "experiment" + " shot: " + shot + "</h1>");

            out.println("</body>");

            out.println("</html>");

            return;

        }

        if(curr_path == null)

        {

            out.println("<h1>Experiment: " + experiment.toUpperCase() + " Shot: " + shot + "</h1>");

            default_nid = new NidData(0);

            default_path = "\\TOP";

       }

       else

       {

            default_path = curr_path;

            default_nid = pathToNid(curr_path);

            try{

                NodeInfo node_info = tree.getInfo(default_nid);

                out.println("<h2>"+ node_info.getName().trim() + "</h2>");

                displayData(default_nid, out);

            }catch (Exception exc){out.println("<h1>Cannot open path" + curr_path+"</h1>");}

            out.println("<p>");

       }

       //Now we are ready to display the children list

       out.println("<hr>");

       out.println("<ul>");

       try {

            NidData sons[] = tree.getSons(default_nid);

            NidData [] members = tree.getMembers(default_nid);

            if(sons != null)

            {

                for(i = 0; i < sons.length; i++)

                {

                    NodeInfo node_info = tree.getInfo(sons[i]);

                    out.println("<li><a href="+BASEREF+"?path="+ default_path+"."+node_info.getName().trim()+

                        "&experiment="+curr_experiment+"&shot="+shot+"&display="+display_wave+"&jscope="+is_jscope+">"+

                        node_info.getName().trim()+"</a>");

                }

            }

            if(members != null)

            {

                for(i = 0; i < members.length; i++)

                {

                    NodeInfo node_info = tree.getInfo(members[i]);

                    out.println("<li><a href="+BASEREF+"?path="+ default_path+":"+node_info.getName().trim()+

                        "&experiment="+curr_experiment+"&shot="+shot+"&display="+display_wave+"&jscope="+is_jscope+">"+

                        node_info.getName().trim()+"</a>");

                }

            }

        }catch(Exception exc){}

        out.println("</ul>");

        out.println("</body>");

        out.println("</html>");

    }



    protected NidData pathToNid(String path)

    {

        try {

            return tree.resolve(new PathData(path));

        }catch(Exception exc) {return null; }

    }

        

    

    protected void displayData(NidData nid, PrintWriter out)

    {

        try 

        {

            NodeInfo info = tree.getInfo(nid);

            out.println("<hr>");

            out.println("path = "+info.getFullPath()+"<br>");

            out.println("time = "+info.getDate()+"<br>");

            out.println("length = "+info.getLength()+"<br>");

            Data data = tree.getData(nid);

            if(data != null)

            {

                if(data instanceof WithUnitsData)

                {

                    try {

                        String units = ((WithUnitsData)data).getUnits().toString();

                        out.println("units = " + units+"<br>");

                        data = ((WithUnitsData)data).getDatum();

                    }catch(Exception exc) {out.println("Cannot display units: " + exc);}

                }

                

                if(data instanceof ParameterData)

                {

                    try {

                        String comment = ((ParameterData)data).getHelp().toString();

                        out.println("comment = " + comment+"<br>");

                        data = ((ParameterData)data).getDatum();

                    }catch(Exception exc) {out.println("Cannot display help: " + exc);}

                }

            

                if(info.getLength() <= MAX_DISPLAY_LENGTH)   //display decompiled

                {

                    data = convertPaths(data);

                    try {

                        out.println("<p>"+ "expression = "+data.toString() +"<p>");

                    }catch(Exception exc){out.println("The expression cannot be decompiled: "+exc);}

                }

                else if(!display_wave && !is_jscope)

                    out.println("<p>The expression is too long to be displayed<p>");

                if(!isScalar(data))

                {

                    try {

                        Data evaluated_data = tree.evaluateData(nid);

                        if( isScalar(evaluated_data))

                            out.println("evaluated = "+evaluated_data.toString());

                        else if( isArray(evaluated_data))

                        {

                            if(display_wave)

                            {

                                out.println("<hr>");

                                out.println(getPlugin(info.getFullPath()));

                            }

                            if(is_jscope)

                            {

                                out.println("<hr>");

                                out.println("<strong>This node can be added to jScope</strong>");

                            }

                            

                        }

                    }catch(Exception exc)

                    {

                        if(experiment.equals("jet"))

                        {

                            out.println("<hr>");

                            out.println(getPlugin(info.getFullPath()));

                        }

                        else if(!is_jscope)

                            out.println("The expression cannot be evaluated <br>");

                    }

                }

            }

        }catch(Exception exc){}

    }



    protected Data convertPaths(Data data)

    {

        if(data instanceof PathData)

        {

            try {

                return new PathData("<a href="+BASEREF+"?path="+data.toString()+

                    "&experiment="+experiment+"&shot="+shot+"&display="+display_wave+"&jscope="+is_jscope+">"+

                    data.toString()+"</a>");

            }catch(Exception exc) {return data; }

        }

        if(data instanceof NidData)

        {

            try {

                NodeInfo info = tree.getInfo((NidData)data);

                return new PathData("<a href="+BASEREF+"?path="+info.getFullPath()+

                    "&experiment="+experiment+"&shot="+shot+"&display="+display_wave+"&jscope="+is_jscope+">"+

                    info.getFullPath()+"</a>");

            }catch(Exception exc) {return data; }

        }

        if(data instanceof CompoundData)

        {

            Data [] descs = ((CompoundData)data).getDescs();

            for(int i = 0; i < descs.length; i++)

                descs[i] = convertPaths(descs[i]);

        }

        return data;

    }

    

    boolean isScalar(Data data)

    {

        return  data instanceof IntData ||

                data instanceof ShortData ||

                data instanceof ByteData ||            

                data instanceof OctaData ||            

                data instanceof DoubleData ||            

                data instanceof FloatData ||

                data instanceof StringData;

    }

    boolean isArray(Data data)

    {

        return  data instanceof IntArray ||

                data instanceof ShortArray ||

                data instanceof ByteArray ||            

                data instanceof OctaArray ||            

                data instanceof DoubleArray ||            

                data instanceof FloatArray;

    }









}