public class MdsContinuousDataProvider extends MdsDataProvider
{
  public boolean SupportsContinuous() {return true;}
  public boolean DataPending() {return true;}
  public WaveData GetWaveData(String inX, String inY)
  {
    try {
      Thread.currentThread().sleep(100);
    }catch(InterruptedException exc){}
    return super.GetWaveData(inX, inY);
  }
  public WaveData GetWaveData(String in)
  {
    try {
      Thread.currentThread().sleep(100);
    }catch(InterruptedException exc){}
    return super.GetWaveData(in);
  }

}