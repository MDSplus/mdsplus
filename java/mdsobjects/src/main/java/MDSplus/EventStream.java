/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package MDSplus;
import java.math.BigDecimal;
import java.util.Vector;
import java.util.Hashtable;
import java.util.StringTokenizer;
import javax.json.*;
import java.io.Writer;
import java.io.StringWriter;
import java.io.StringReader;



/**
 *
 * @author mdsplus
 */
public class EventStream extends Event {
    static boolean debug = false;
    java.lang.String streamName;
    Hashtable<java.lang.String, Vector<DataStreamListener>> listenerHash = 
                    new Hashtable<java.lang.String, Vector<DataStreamListener>>();
    public EventStream() throws MdsException
    {
        super("STREAMING");
    }
    public EventStream(java.lang.String streamName) throws MdsException
    {
        super(streamName);
        this.streamName = streamName;
    }
    public synchronized void registerListener(DataStreamListener listener)
    {
        registerListener(streamName, listener);
    }
   
    public synchronized void registerListener(java.lang.String streamName, DataStreamListener listener)
    {
        Vector<DataStreamListener> listenerV = listenerHash.get(streamName);
        if(listenerV == null)
            listenerV = new Vector<DataStreamListener>();
        listenerV.addElement(listener);
        listenerHash.put(streamName, listenerV);
    }
     
    private static java.lang.String toJSON(int shot, java.lang.String chanName, float times[], float []samples) 
    {
        JsonBuilderFactory factory = Json.createBuilderFactory(null);
        JsonArrayBuilder timesArr = factory.createArrayBuilder();
        for(int i = 0; i < times.length; i++)
            timesArr.add(times[i]);
        JsonArrayBuilder samplesArr = factory.createArrayBuilder();
        for(int i = 0; i < samples.length; i++)
            samplesArr.add(samples[i]);
        JsonObjectBuilder streamObj = factory.createObjectBuilder();
        streamObj.add("name", chanName);
        streamObj.add("timestamp", 0);
        streamObj.add("shot", shot);
        streamObj.add("absolute_time", 0);
        streamObj.add("times", timesArr.build());
        streamObj.add("samples", samplesArr.build());
        java.lang.String jsonString = "";
        try {
            Writer writer = new StringWriter();
            Json.createWriter(writer).write(streamObj.build());
            jsonString = writer.toString();
        }catch(Exception exc)
        {
            System.out.println(exc);
         }
        return jsonString;
    }
   private static java.lang.String toJSON(int shot, java.lang.String chanName, long times[], float []samples) 
    {
        JsonBuilderFactory factory = Json.createBuilderFactory(null);
        JsonArrayBuilder timesArr = factory.createArrayBuilder();
        for(int i = 0; i < times.length; i++)
            timesArr.add(times[i]);
        JsonArrayBuilder samplesArr = factory.createArrayBuilder();
        for(int i = 0; i < samples.length; i++)
            samplesArr.add(samples[i]);
        JsonObjectBuilder streamObj = factory.createObjectBuilder();
        streamObj.add("name", chanName);
        streamObj.add("timestamp", 0);
        streamObj.add("shot", shot);
        streamObj.add("absolute_time", 1);
        streamObj.add("times", timesArr.build());
        streamObj.add("samples", samplesArr.build());
        java.lang.String jsonString = "";
        try {
            Writer writer = new StringWriter();
            Json.createWriter(writer).write(streamObj.build());
            jsonString = writer.toString();
        }catch(Exception exc)
        {
            System.out.println(exc);
         }
        return jsonString;
    }
    public static void send(int shot, java.lang.String streamName, float time, float sample)
    {
        java.lang.String payload = toJSON(shot, streamName, new float[]{time}, new float[]{sample}) ;
        seteventRaw(streamName, payload.getBytes());
    }
   public static void send(int shot, java.lang.String streamName, float time, float samples[])
    {
        java.lang.String payload = toJSON(shot, streamName, new float[]{time}, samples) ;
        seteventRaw(streamName, payload.getBytes());
    }
    
    public static void send(int shot, java.lang.String streamName, long time, float sample)
    {
        java.lang.String payload = toJSON(shot, streamName, new long[]{time}, new float[]{sample}) ;
        seteventRaw(streamName, payload.getBytes());
    }
     public static void send(int shot, java.lang.String streamName, long time, float samples[])
    {
        java.lang.String payload = toJSON(shot, streamName, new long[]{time}, samples) ;
        seteventRaw(streamName, payload.getBytes());
    }
    
             
    public static void send(int shot, java.lang.String streamName, long[] times, float[] samples)
    {
        java.lang.String payload = toJSON(shot, streamName, times, samples) ;
        seteventRaw(streamName, payload.getBytes());
   }
   public static void send(int shot, java.lang.String streamName, float[] times, float[] samples)
    {
        java.lang.String payload = toJSON(shot, streamName, times, samples) ;
        seteventRaw(streamName, payload.getBytes());
   }
  
    public static void send(int shot, java.lang.String streamName, Data timesD, Data samplesD)
    {
        long absTimes[];
        float relTimes[];
        float samples[];
        java.lang.String payload;
        try {
            if(samplesD instanceof Array)
                samples = samplesD.getFloatArray();
            else
                samples = new float[]{samplesD.getFloat()};
        
            if(timesD instanceof Uint64 )
            {
                absTimes = new long[]{timesD.getLong()};
                payload = toJSON(shot, streamName, absTimes, samples) ;
            }
            else if(timesD instanceof Uint64Array)
            {
                absTimes = timesD.getLongArray();
                payload = toJSON(shot, streamName, absTimes, samples) ;
            }
            else if (timesD instanceof Array)
            {
                relTimes = timesD.getFloatArray();
                payload = toJSON(shot, streamName, relTimes, samples) ;
            }
            else
            {
                relTimes = new float[]{timesD.getFloat()};
                payload = toJSON(shot, streamName, relTimes, samples) ;
            }
            System.out.println(payload);
            seteventRaw(streamName, payload.getBytes());
        }catch(Exception exc) //If scalar
        {
            System.out.println("Cannot send stream: " + exc);
        
        }
    }
    private void handleEventJSON(java.lang.String evString)
    {
        JsonReader reader = Json.createReader(new StringReader(evString));
        JsonObject evObj = reader.readObject();
        int shot = evObj.getInt("shot");
        java.lang.String streamName = evObj.getString("name");
        boolean isAbsTime = evObj.getInt("absolute_time") == 1;
        JsonArray samplesArr = evObj.getJsonArray("samples");
        JsonArray timesArr = evObj.getJsonArray("times");
        float samples[] = new float[samplesArr.size()];
        for(int i = 0; i < samplesArr.size(); i++)
            samples[i] = (float)samplesArr.getJsonNumber(i).doubleValue();
        Data samplesD;
        if(samples.length == 1)
            samplesD = new Float32(samples[0]);
        else
            samplesD = new Float32Array(samples);
        Data timesD;
        if (isAbsTime)
        {
            long times[] = new long[timesArr.size()];
            for(int i = 0; i < timesArr.size(); i++)
                times[i] = timesArr.getJsonNumber(i).longValue();
            if(times.length == 1)
                timesD = new Uint64(times[0]);
            else
                timesD = new Uint64Array(times);
        }
        else
        {
            float times[] = new float[timesArr.size()];
            for(int i = 0; i < timesArr.size(); i++)
                times[i] = (float)timesArr.getJsonNumber(i).doubleValue();
            if(times.length == 1)
                timesD = new Float32(times[0]);
            else
                timesD = new Float32Array(times);
        }
        Vector<DataStreamListener>listeners = listenerHash.get(streamName);
        for(int i = 0; i < listeners.size(); i++)
            listeners.elementAt(i).dataReceived(streamName, shot,timesD, samplesD);
   }
    public synchronized void run()
    {
        java.lang.String evName = getName();
        byte[] rawMsg = getRaw();
        java.lang.String evMessage = new java.lang.String(rawMsg);
        if(evMessage.charAt(0) == '{')
        {
            handleEventJSON(evMessage);
            return;
        }
        int byteIdx = 0;
        if(debug) System.out.println(evMessage);
        StringTokenizer st = new StringTokenizer(evMessage);
        try {
            java.lang.String currStr = st.nextToken();
            byteIdx = currStr.length() + 1; //A single blank between header fields
            int shot = Integer.parseInt(currStr);
            java.lang.String streamName = st.nextToken();
            byteIdx += streamName.length() + 1; 
            Vector<DataStreamListener>listeners = listenerHash.get(streamName);
            if(listeners != null && listeners.size() > 0)
            {
                java.lang.String mode = st.nextToken();
                byteIdx += 2;
                currStr = st.nextToken();
                byteIdx += currStr.length() + 1;
                int numSamples = Integer.parseInt(currStr);
                if(mode.equals("B")) //Serialized times and values
                {
                    byte [] serTimes = new byte[numSamples];
                    byte [] serSamples = new byte[rawMsg.length - byteIdx - numSamples];
                    System.arraycopy(rawMsg, byteIdx, serTimes, 0, numSamples);
                    System.arraycopy(rawMsg, byteIdx + numSamples, serSamples, 0, rawMsg.length - byteIdx - numSamples);
                    Data timesD = Data.deserialize(serTimes);
                    Data samplesD = Data.deserialize(serSamples);
                    for(int i = 0; i < listeners.size(); i++)
                        listeners.elementAt(i).dataReceived(streamName, shot,timesD, samplesD);
                    return; //All done
                }
                if(numSamples == 1)
                {
                    Data timeD, sampleD;
                    if(mode.equals("A") || mode.equals("F")) //Float times
                        timeD = new Float32(Float.parseFloat(st.nextToken()));
                    else
                        timeD = new Uint64(Long.parseLong(st.nextToken()));
                    sampleD = new Float32(Float.parseFloat(st.nextToken()));
                    for(int i = 0; i < listeners.size(); i++)
                    {
                        DataStreamListener listener = listeners.elementAt(i);
                        listener.dataReceived(streamName, shot,timeD, sampleD);
                    }
                }
                else
                {
                    Data timesD, samplesD;
                    if(mode.equals("A") || mode.equals("F"))
                    {
                        float times[] = new float[numSamples];
                        for(int i = 0; i < numSamples; i++)
                            times[i] = Float.parseFloat(st.nextToken());
                        timesD = new Float32Array(times);
                    }
                    else
                    {
                        long times[] = new long[numSamples];
                        for(int i = 0; i < numSamples; i++)
                            times[i] = Long.parseLong(st.nextToken());
                        timesD = new Uint64Array(times);
                    }
                    float samples[] = new float[numSamples];
                    for(int i = 0; i < numSamples; i++)
                        samples[i] = Float.parseFloat(st.nextToken());
                    samplesD = new Float32Array(samples);
                    for(int i = 0; i < listeners.size(); i++)
                        listeners.elementAt(i).dataReceived(streamName, shot,timesD, samplesD);
                    
                }
            }
       }catch(Exception exc)
       {
           System.out.println("Error getting data from stream: "+exc);
       }
    }
    public static void main(java.lang.String args[])
    {
       try {
            EventStream es = new EventStream("ciccior");
            es.registerListener(new DataStreamListener() {
                public void dataReceived(java.lang.String streamName, int shot, Data times, Data samples)
                {
                    System.out.println(streamName+" Time: "+times + "Sample: " + samples);
                }
            });
            Thread.currentThread().sleep(1000000);
        }catch(Exception exc){System.out.println(exc);}
      
        
/*    EventStream.send(0, "Channel1", new Float32Array(new float[]{0,1}),new Float32Array(new float[]{10,20}));   
    EventStream.send(0, "Channel2", new Uint64Array(new long[]{0,1}),new Float32Array(new float[]{10,20}));   
    EventStream.send(0, "Channel3", new Uint64(123),new Float32Array(new float[]{10,20}));   
    EventStream.send(0, "Channel4", new Float32(666),new Float32Array(new float[]{10,20}));   
    EventStream.send(0, "Channel5", new Float32(666),new Float32(212));   */ 
    }
}
