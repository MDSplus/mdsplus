/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package MDSplus;
import java.util.Vector;
import java.util.Hashtable;
import java.util.StringTokenizer;
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
    public synchronized void registerListener(java.lang.String streamName, DataStreamListener listener)
    {
        Vector<DataStreamListener> listenerV = listenerHash.get(streamName);
        if(listenerV == null)
            listenerV = new Vector<DataStreamListener>();
        listenerV.addElement(listener);
        listenerHash.put(streamName, listenerV);
    }
    public static void send(int shot, java.lang.String streamName, float time, float sample)
    {
        java.lang.String msgString = ""+shot+ " "+streamName+" F 1 "+time + " " + sample;
        seteventRaw("STREAMING", msgString.getBytes());
    }
    
    public static void send(int shot, java.lang.String streamName, long time, float sample)
    {
        java.lang.String msgString = ""+shot+ " "+streamName+" L 1 "+time + " " + sample;
        seteventRaw("STREAMING", msgString.getBytes());
    }
    
    public static void send(int shot, java.lang.String streamName, float[] times, float[] samples, 
            boolean oscilloscopeMode)
    {
        int numSamples = times.length;
        if(numSamples > samples.length)
            numSamples = samples.length;
        java.lang.String msgString;
        if(oscilloscopeMode)
            msgString = ""+shot+ " "+streamName+" A "+numSamples;
        else
            msgString = ""+shot+ " "+streamName+" F "+numSamples;
        for(int i = 0; i < numSamples; i++)
            msgString +=  " " + times[i];
        for(int i = 0; i < numSamples; i++)
            msgString +=  " " + samples[i];
        seteventRaw("STREAMING", msgString.getBytes());
    }
           
    public static void send(int shot, java.lang.String streamName, float[] times, float[] samples)
    {
        send(shot, streamName, times, samples, false);
    }
   public static void send(int shot, java.lang.String streamName, long[] times, float[] samples, 
            boolean oscilloscopeMode)
    {
        int numSamples = times.length;
        if(numSamples > samples.length)
            numSamples = samples.length;
        java.lang.String msgString;
        msgString = ""+shot+ " "+streamName+" L "+numSamples;
         for(int i = 0; i < numSamples; i++)
            msgString += " " + times[i];
        for(int i = 0; i < numSamples; i++)
            msgString +=  " " + samples[i];
        seteventRaw("STREAMING", msgString.getBytes());
    }
 
    public static void send(int shot, java.lang.String streamName, Data times, Data samples)
    {
        byte [] serTimes = times.serialize();
        byte [] serSamples = samples.serialize();
        int numSamples = serTimes.length;
        java.lang.String msgString = ""+shot+ " "+streamName+" B "+numSamples+" ";
        byte[] header = msgString.getBytes();
        byte [] message = new byte[header.length+serTimes.length+serSamples.length];
        System.arraycopy(header, 0, message, 0, header.length);
        System.arraycopy(serTimes, 0, message, header.length, serTimes.length);
        System.arraycopy(serSamples, 0, message, header.length+serTimes.length, serSamples.length);
        seteventRaw("STREAMING", message);
    }

    public synchronized void run()
    {
        java.lang.String evName = getName();
        if(!evName.equals("STREAMING"))
            return; //Should never happen
        byte[] rawMsg = getRaw();
        java.lang.String evMessage = new java.lang.String(rawMsg);
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
/*        try {
            EventStream es = new EventStream();
            es.registerListener("Channel1", new DataStreamListener() {
                public void dataReceived(java.lang.String streamName, int shot, Data times, Data samples)
                {
                    System.out.println(streamName+" Time: "+times + "Sample: " + samples);
                }
            });
            Thread.currentThread().sleep(1000000);
        }catch(Exception exc){System.out.println(exc);}
*/       
        
    EventStream.send(0, "Channel1", new Float32Array(new float[]{0,1}),new Float32Array(new float[]{10,20}));   
    }
}
