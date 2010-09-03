package jScope;

import jScope.Signal;
import jScope.CompositeWaveDisplay;
import java.awt.Component;

/**
 * <p>Title: </p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2006</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class TestCompositeWaveform
{
    CompositeWaveDisplay cd;
    TestCompositeWaveform(CompositeWaveDisplay cd)
    {
        new TestThread(cd).start();
    }

    public class TestThread extends Thread
    {
        CompositeWaveDisplay cd;

        TestThread(CompositeWaveDisplay cd)
        {
            this.cd = cd;
        }

        public void run()
        {
            int i = 0, k, numMsg = 0;
            int numPointPerSignal = 10;
            int numSignal = 4, command = CompositeWaveDisplay.CMND_ADD;
            float y[], x[], t[];

            float dx = (float)0.001;
            command = CompositeWaveDisplay.CMND_ADD;

            x = new float[numPointPerSignal * numSignal];

            while(true)
            {
/*
                if( (numMsg  % 30) == 0)
                {
                    cd.setLiveUpdate(false);
                    try
                    {
                        Thread.currentThread().sleep(3000);
                    }
                    catch (Exception exc){}
                    cd.setLiveUpdate(true);
                }
*/
                int numProfile = 5;
                int numPointsPerProfile = 50;

                t = new float[numProfile];
                x = new float[numPointsPerProfile * numProfile];
                y = new float[numPointsPerProfile * numProfile];

                for( i = 0, k = 0; i < numProfile; i++)
                {
                    for(int j = 0; j < numPointsPerProfile; j++)
                    {
                        x[j + i * numPointsPerProfile] = (numMsg+1) *  (float)Math.cos(j * 6.28/numPointsPerProfile);
                        y[j + i * numPointsPerProfile] = (numMsg+1) *  (float)Math.sin(j * 6.28/numPointsPerProfile);
                    }
                    t[i] = ( numMsg * numProfile  + i ) * 0.1f;
                    //cd.enqueueUpdateSignals(numPointsPerProfile, command, x, y, t);
                }

             //cd.enqueueUpdateSignal("Pippo", command, x, y, t);
//               cd.enqueueUpdateSignals(numPointsPerProfile, command, x, y, t);

                try
                {
                    Thread.currentThread().sleep(100);
                }
                catch (Exception exc){}

//               if(numMsg < 1000 || numMsg > 3000)
               if(numMsg != numMsg)
                {

                    y = new float[numPointPerSignal * numSignal];
                    x = new float[numPointPerSignal * numSignal];

                    for (int j = 0; j < numPointPerSignal; j++)
                    {
                        y[j] = (float) (numMsg / 10000.);
                        y[j +     numPointPerSignal] = (float) Math.sin(numMsg / 300.);
                        y[j + 2 * numPointPerSignal] = (float) Math.cos(numMsg / 300.);
                        y[j + 3 * numPointPerSignal] = (float) Math.sin(numMsg / 300.) * (float) Math.sin(numMsg / 300.);
                        //               if (numMsg == 0) {
                        x[j] =
                        x[j +     numPointPerSignal] =
                        x[j + 2 * numPointPerSignal] =
                        x[j + 3 * numPointPerSignal] = (float) (0.1 * numMsg);
                        //              } else x[0] = 0.1f;
                        i++;
                    }
//                    cd.enqueueUpdateSignals(numPointPerSignal, command, x, y);
                }
                else
                {
                    y = new float[numPointPerSignal];
                    x = new float[numPointPerSignal];
                    for (int j = 0; j < numPointPerSignal; j++)
                    {
                        y[j] = (float) Math.sin( ((numMsg * numPointPerSignal)  + i)   / 3000.);
                        x[j] = (float) (0.1 * ( (numMsg * numPointPerSignal)  + i ) );
                        i++;
                    }
                    cd.enqueueUpdateSignal("Terzo", command, x, y);
//                  cd.enqueueUpdateSignal("Primo", command, x, y);

               }
                // x = new float[1];

                numMsg++;

                /*
                if( (numMsg % 100) == 0 )
                 command = CompositeWaveDisplay.CMND_CLEAR;
                else
                 command = CompositeWaveDisplay.CMND_ADD;
                 */



                try
                {
                    Thread.currentThread().sleep(100);
                }
                catch (Exception exc){}
            }
        }
    }


    public static void main(String args[])
    {
        CompositeWaveDisplay cd = CompositeWaveDisplay.createWindow("prova");
        cd.showWindow(50, 50, 500, 400);

        float y[] = new float[1000], y1[] = new float[1000],
                                            y2[] = new float[1000],
                x[] = new float[1000];
        for (int i = 0; i < 1000; i++) {
            x[i] = (float) (i / 1000.);
            y[i] = (float) Math.sin(i / 300.);
            y1[i] = (float) Math.cos(i / 300.);
            y2[i] = (float) Math.sin(i / 300.) * (float) Math.sin(i / 300.);
        }

        /*
                float z2D[] = new float[100*100], x2D[] = new float[100], y2D[] = new float[100];

                for(int i = 0; i < 100; i++)
                {
                    x2D[i] = (float)(i/100.);
                  //data[i] = (float)Math.cos(i/300.);
                    for(int j = 0; j < 100; j++)
                    {
                        z2D[j * 100+i] = (float)((i + 1)/100.) * (float)Math.cos(j * 6.28/100.) * (float)Math.cos(i * 6.28/100.);
                    }
                }
                for(int i = 0; i < 100; i++)
                    y2D[i] = i;// (float)Math.sin(i * 6.28/100.);
         */
        int numProfile = 20;
        int numPointsPerProfile = 50;
        float z2D[] = new float[numProfile * 2],
                      x2D[] = new float[numPointsPerProfile * numProfile],
                              y2D[] = new float[numPointsPerProfile *
                                      numProfile];

        for (int i = 0, k = 0; i < numProfile; i++) {
            for (int j = 0; j < numPointsPerProfile; j++) {
                x2D[j +
                        i *
                        numPointsPerProfile] = 2 * (i + 1) *
                                               (float) Math.
                                               cos(j * 6.28 /
                        numPointsPerProfile);
                y2D[j +
                        i *
                        numPointsPerProfile] = 2 * (i + 1) *
                                               (float) Math.
                                               sin(j * 6.28 /
                        numPointsPerProfile);
            }
            z2D[k] = i;
            z2D[k + 1] = numPointsPerProfile;
            k += 2;
        }

        String sig_name1[] = {"Primo", "Secondo"};
        for (int i = 0; i < sig_name1.length; i++) {
            ((CompositeWaveDisplay) cd).addSignal(1, 1, sig_name1[i], 0xff0000, 10000,
                                                  Signal.TYPE_1D);
        }
        String sig_name2[] = {"Terzo", "Quarto"};
        for (int i = 0; i < sig_name2.length; i++) {
            ((CompositeWaveDisplay) cd).addSignal(1, 2, sig_name2[i], 0x00ff00, 10000,
                                                  Signal.TYPE_1D);
        }
        ((CompositeWaveDisplay) cd).setLimits(1, 1, 0, 10, -1, 1);
        ((CompositeWaveDisplay) cd).setLimits(1, 2, 0, 10, -1, 1);

        ((CompositeWaveDisplay) cd).addSignal(2, 1, "Pippo", 0x0000ff, 20000, Signal.TYPE_2D);

        /*
         Signal  sig_profile = new Signal(z2D, y2D, x2D, Signal.TYPE_2D);
                  sig_profile.setMode2D(Signal.MODE_PROFILE);
                  ((CompositeWaveDisplay)cd).addSignal(sig_profile, 1, 1);
         */

//          Signal  sig_2d = new Signal(z2D, y2D, x2D, Signal.TYPE_2D);
//          sig_2d.setMode2D(Signal.MODE_XZ);
//          ((CompositeWaveDisplay)cd).addSignal(sig_2d, 1, 2);
//          ((CompositeWaveDisplay)cd).addSignal(x, y, 1,1,"green", "seno");
//          ((CompositeWaveDisplay)cd).setXYLimits(1, 1, 0, 10, -1, 1);
//        Signal  sig_2d1 = new Signal(sig_2d);
//        ((CompositeWaveDisplay)cd).addSignal(sig_2d1, 1, 1);
//        ((CompositeWaveDisplay)cd).addSignal(x, y, 1,1,"green", "seno");
//        ((CompositeWaveDisplay)cd).addSignal(x, y1, 1,1,"red", "coseno");

//        ((CompositeWaveDisplay)cd).addSignal(x, y2, 2,1,"blue", "seno**2");
//        ((CompositeWaveDisplay)cd).addSignal("mds://150.178.3.80/rfx/13000/\\RFX::TOP.RFX.A.SIGNALS.EMRA:IT", 2,3, "blue", "emra_it", true, 0);
//        ((CompositeWaveDisplay)cd).addSignal("twu://ipp333.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star", 2,3, "blue", "twu", true, 0);
//        ((CompositeWaveDisplay)cd).addSignal("rda://data.jet.uk/PPF/40000/MAGN/BPOL", 1,3, "blue", "jet", true, 0);
//        ((CompositeWaveDisplay)cd).addSignal("demo://sin", 1,1, "blue", "demo", true, 0);
//        ((CompositeWaveDisplay)cd).setLimits(1,1, -100, 100, -2, 2);
//        ((CompositeWaveDisplay)cd).addFrames("mds://150.178.3.80/prova/-1/\\PROVA::TOP:VIDEO", 2,2);



        try {
            Thread.currentThread().sleep(500);
        } catch (Exception exc) {}

        TestCompositeWaveform testThread = new TestCompositeWaveform(cd);


    }

}

