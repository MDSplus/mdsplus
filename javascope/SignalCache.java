import java.security.*;
import java.io.*;


public class SignalCache {
 private long min_last_modified = Long.MAX_VALUE;    
 private long max_last_modified = Long.MIN_VALUE;
 private long purge_modified = 0L;
 private long min_cache_size = 0x800000;
 private long max_cache_size = 0x1000000;
 private MessageDigest md;
 private String cache_directory = "";
 private long cache_size = 0L;
  

 SignalCache()
 {
    try
    {
        md = MessageDigest.getInstance("MD5");
        cache_directory = System.getProperty("Signal.cache_directory");
        
        try
        {   
            String s = System.getProperty("Signal.cache_size");
            //Long size = new Long(s);
            max_cache_size = Long.parseLong(s);
        } catch (NumberFormatException e){}
        
        if(cache_directory == null)
            cache_directory = System.getProperty("user.home")+ File.separator + "jScopeCache";
        File f = new File(cache_directory);
        if(!f.exists() || !f.isDirectory()) 
            f.mkdir();
        cache_size = getCacheSize();
    } catch (NoSuchAlgorithmException e)
    {
        System.out.println(""+e);
    }
 }
 
 public long getCacheSize()
 {
    long last_modified;
    long size = 0L;
    File fc;
    File f = new File(cache_directory);
    String list[] = f.list();
    for(int i = 0; i < list.length; i++)
    {
        fc = new File(cache_directory + File.separator + list[i]);
        last_modified = fc.lastModified();
        if(last_modified < min_last_modified)
            min_last_modified = last_modified;
        if(last_modified > max_last_modified)
            max_last_modified = last_modified;
        size += fc.length();
    }
    purge_modified = min_last_modified + (max_last_modified  - min_last_modified)/2;
    return size;
 }
 
 public String cacheFileName(String name)
 {
     byte[] n = name.getBytes();
     byte[] d = md.digest(n);
     StringBuffer out = new StringBuffer(32);
     char ch[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'L', 'M', 'N', 'O', 'P', 'Q', 'R'};
     
     for(int i = 0; i < d.length; i++)
     {
        out.append(ch[(d[i]& 0xF0)>> 4]);
        out.append(ch[(d[i]& 0x0F)]);
     }
     return out.toString();
 }

 public void putCacheData(String expression, String experiment, int shot, Object data)
 {
    if(expression == null || data == null) return;
    
    if(experiment == null) 
        experiment = "";
        
    String name = expression.toUpperCase()+experiment.toUpperCase()+shot;
    
    if(isInCache(name)) return;
    
    if(cache_size > max_cache_size)
        purgeCache();
    
    String cache_file = cache_directory + File.separator + cacheFileName(name);
    DataCached cd = new DataCached(expression, experiment, shot, data);
    
    try
    {
        FileOutputStream ostream = new FileOutputStream(cache_file);
        ObjectOutputStream p = new ObjectOutputStream(ostream);

        p.writeObject(cd);
        p.flush();
        ostream.close();
        File fc = new File(cache_file);
        cache_size += fc.length();
        max_last_modified = fc.lastModified();
    } catch (IOException e) {System.out.println(e);}
 }

 public Object getCacheData(String expression,String experiment, int shot)
 {
    if(expression == null) return null;

    if(experiment == null) 
        experiment = "";

    String name = expression.toUpperCase()+experiment.toUpperCase()+shot;
    
    if(!isInCache(name)) return null;
    
    String cache_file = cache_directory + File.separator + cacheFileName(name);
    
    try
    {
        FileInputStream istream = new FileInputStream(cache_file);
        ObjectInputStream p = new ObjectInputStream(istream);
        DataCached dc = (DataCached)p.readObject();
        istream.close();
        
        if(dc.equals(expression, experiment, shot))
            return dc.data;
        
     } catch (IOException e) {System.out.println(e);}
       catch (ClassNotFoundException e) {System.out.println(e);}
     
     return null;
 }
 
 public void purgeCache()
 {
    String name;
    long modified;
    File fc;
    File f = new File(cache_directory);
    String list[] = f.list();
   
    for(int i = 0; i < list.length; i++)
    {
        name = cache_directory + File.separator + list[i];
        fc = new File(name);
        modified = fc.lastModified();
        if(modified < purge_modified) {
            cache_size -= fc.length();
            fc.delete();
        } 
    }
    min_last_modified = purge_modified;
    purge_modified = min_last_modified + (max_last_modified  - min_last_modified)/2;
 }
 
 public boolean isInCache(String name)
 {
    String cache_name = cacheFileName(name);
    File f = new File(cache_directory + File.separator + cache_name);
    return f.exists();
 }
 
 public static void main(String[] arg)
 {
    SignalCache sc = new SignalCache();
    
    
    sc.putCacheData("emra_it", "rfx", 12450, new Signal());
 
    Signal pippo = (Signal)sc.getCacheData("emra_it", "rfx", 12450);
    
    pippo = pippo;

 }
 
}