import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;

class JetDataProvider implements DataProvider
{
    
    String experiment;
    int shot;
    String username, passwd;
    String encoded_credentials;
    String error_string;
    URL url;
    byte []buffer;
    Base64 translator = new Base64();
    BufferedInputStream br;
    int content_len;
    private String last_url_name;
    private float [] last_y, last_x;
    Dialog inquiry_dialog;
    Frame owner_f;
    static final int LOGIN_OK = 1, LOGIN_ERROR = 2, LOGIN_CANCEL = 3;
    private int login_status;
    
    TextField user_text, passwd_text;
    
    JetDataProvider() {this(null, null);}
   
    JetDataProvider(String username, String passwd)
    {
        String credentials = username+":"+passwd;
        try{
            encoded_credentials = translator.encode(credentials);
        }catch(Exception e){}
    }
     
    public int GetLoginStatus()
    {
        return login_status;
    }
    
    void InquireCredentials(Frame f)
    {
        login_status = LOGIN_OK;
        owner_f = f;
        inquiry_dialog = new Dialog(f, "JET data server login", true);
        inquiry_dialog.setLayout(new BorderLayout());
        Panel p = new Panel();
        p.add(new Label("Username: "));
        user_text = new TextField(15);
        p.add(user_text);
        inquiry_dialog.add(p, "North");
        p = new Panel();
        p.add(new Label("Password: "));
        passwd_text = new TextField(15);
        passwd_text.setEchoChar('*');
        p.add(passwd_text);
        inquiry_dialog.add(p, "Center");
        p = new Panel();
        Button ok_b = new Button("Ok");
        ok_b.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
                {
                    username = user_text.getText();
                    passwd = passwd_text.getText();
                    if(!checkPasswd(username, passwd))
                    {
                        ErrorMessage.ShowErrorMessage(owner_f, "Incorrect password");
                        login_status = LOGIN_ERROR;
                        return;
                    }
                        
                    String credentials = username+":"+passwd;
                    try{
                        encoded_credentials = translator.encode(credentials);
                    } catch(Exception exc)
                    {
                        login_status = LOGIN_ERROR;
                    }
                    if(login_status == LOGIN_OK)
                        inquiry_dialog.setVisible(false);
                    else
                        login_status = LOGIN_OK;
                }});
        p.add(ok_b);
        Button clear_b = new Button("Clear");
        clear_b.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
                {
                    user_text.setText("");
                    passwd_text.setText("");
                }});
        p.add(clear_b);
        Button cancel_b = new Button("Cancel");
        cancel_b.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e)
                {
                    login_status = LOGIN_CANCEL;
                    inquiry_dialog.setVisible(false);
                }});
        p.add(cancel_b);
        inquiry_dialog.add(p, "South");
        Rectangle r = f.getBounds();
        inquiry_dialog.pack();
        inquiry_dialog.setLocation(r.x + r.width/2 - inquiry_dialog.getBounds().width/2,
			r.y + r.height/2 - inquiry_dialog.getBounds().height/2);
        inquiry_dialog.show();
    }
        
   boolean checkPasswd(String username, String passwd)
   {
        String credentials = username+":"+passwd;
        try{
            encoded_credentials = translator.encode(credentials);
            URLConnection urlcon;
            url = new URL("http://data.jet.uk/");
            urlcon = url.openConnection();
            urlcon.setRequestProperty("Authorization", "Basic "+encoded_credentials);
            br = new BufferedInputStream(urlcon.getInputStream());
            content_len = urlcon.getContentLength();
            if(content_len <= 0) return false;
            buffer = new byte[content_len];
            int num_read_bytes = 0;
            while(num_read_bytes < content_len)
                num_read_bytes += br.read(buffer, num_read_bytes, buffer.length - num_read_bytes);
            br.close();
        }catch(Exception e){return false;}
        String out = new String(buffer);
        if(out.indexOf("incorrect password") != -1)
            return false;
        return true;
    }
//DataProvider implementation

    public String GetDefaultTitle(String in_y[]){return null;}
    public String GetDefaultXLabel(String in_y[]){return null;}
    public String GetDefaultYLabel(String in_y[]){return null;}
    public float[] GetFrameTimes(String in_frame){return null;} 
    public byte[] GetFrameAt(String in_frame, int frame_idx){return null;} 
    
    
    public void SetEnvironment(String s) {}
    public void Update(String experiment, int shot) 
    {
        this.experiment = experiment;
        this.shot = shot;
        error_string = null;
    }
    public String GetString(String in) {return in; }
    public float GetFloat(String in){ return new Float(in).floatValue(); } 
    public float[] GetFloatArray(String in)
    {
        error_string = null;
        boolean is_time = in.startsWith("TIME:", 0);
        if(is_time)
            in = in.substring(5);
            
        String url_name;
        if(experiment == null)
        {
           StringTokenizer st = new StringTokenizer(in, "/", true);
           url_name = st.nextToken() + "/" + shot ;
           while(st.hasMoreTokens())
            url_name = url_name + st.nextToken();
        }    
//            url_name = in;
        else
            url_name = experiment + "/" + shot + "/" + in;
        if(last_url_name != null && url_name.equals(last_url_name))
        {
            if(is_time) 
                return last_x;
            else
                return last_y;
        }
        else
        {
            last_x = last_y = null;
            try{
                last_url_name = url_name;
                URLConnection urlcon;
                url = new URL("http://data.jet.uk/" + url_name);
                urlcon = url.openConnection();
                //urlcon.setRequestProperty("Connection", "Keep-Alive");
                urlcon.setRequestProperty("Authorization", "Basic "+encoded_credentials);
                br = new BufferedInputStream(urlcon.getInputStream());
                content_len = urlcon.getContentLength();
                if(content_len <= 0)
                {
                    last_url_name = null;
                    error_string = "Error reading URL " + url_name + " : null content length";
                    return null;
                }
                buffer = new byte[content_len];
                int num_read_bytes = 0;
                while(num_read_bytes < content_len)
                    num_read_bytes += br.read(buffer, num_read_bytes, buffer.length - num_read_bytes);
                br.close();

                JiNcSource jns = new JiNcSource("myname", new RandomAccessData(buffer));
                JiVar jvarTime = jns.getVar("TIME"), jvarData = jns.getVar("SIGNAL");

                JiDim[] dims = jvarTime.getDims();
                double[] time = jvarTime.readDouble(dims);
                last_x = new float[time.length];
                for(int i = 0; i < time.length; i++)
                    last_x[i] = (float)time[i];
                time = null;
                dims = jvarData.getDims();
                last_y = jvarData.readFloat(dims);
            }catch(Exception e)
            {
                error_string = "Error reading URL " + url_name + " : " + e;
                last_url_name = null;
                return null;
            }
            if(is_time)
                return last_x;
            else
                return last_y;
        }
    }
        
 public int[] GetIntArray(String in)
 {
    error_string = null;
    int [] result;
    String curr_in = in.trim();
    if(curr_in.startsWith("[", 0))
    {
        if(curr_in.endsWith("]"))
        {
            curr_in = curr_in.substring(1, curr_in.length() - 1);
            StringTokenizer st = new StringTokenizer(curr_in, ",", false);
            result = new int[st.countTokens()];
            int i = 0;
            try{
                while(st.hasMoreTokens())
                    result[i++] = Integer.parseInt(st.nextToken());
                return result;
            } catch(Exception e) {}
        }
    }
    else
    {
        if(curr_in.indexOf(":") != -1)
        {
            StringTokenizer st = new StringTokenizer(curr_in, ":");
            int start, end;
            if(st.countTokens() == 2)
            {
                try{
                    start = Integer.parseInt(st.nextToken());
                    end = Integer.parseInt(st.nextToken());
                    if(end < start) end = start;
                    result = new int[end-start+1];
                    for(int i = 0; i < end-start+1; i++)
                        result[i] = start+i;
                    return result;
                }catch(Exception e){}
            }
        }
        else
        {
            result = new int[1];
            try {
                result[0] = Integer.parseInt(curr_in);
                return result;
            }catch(Exception e){}
        }
    }
    error_string = "Error parsing shot number(s)";
    return null;
 }

 public String GetXSpecification(String in)
 {
    return "TIME:" + in;
 }
 public String ErrorString() { return error_string; }
 public boolean SupportsAsynch() { return false; } 
 public void addMdsEventListener(MdsEventListener l, String event){} 
 public void removeMdsEventListener(MdsEventListener l, String event){} 
 



public static void main(String args[])
{
    System.out.println("\nStart readout PPF/40000/MAGN/IPL");
    JetDataProvider dp = new JetDataProvider("obarana", "clublatino");
    float x[], y[];
    y = dp.GetFloatArray("PPF/40000/MAGN/IPLA");
    if(true) return;
    x = dp.GetFloatArray("TIME:PPF/40000/MAGN/IPLA");
        
    for(int i = 0; i < x.length; i++)
        System.out.println(x[i] + "  " +y[i]);
           
    System.out.println("Num. points: "+y.length);
 }
 
}

