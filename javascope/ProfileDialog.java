import java.awt.Dialog;
import java.awt.Frame;
import java.awt.event.*;


public class ProfileDialog extends Dialog
{
    static final String TITLE[] = {"X profile", "Y profile", "Pixel time profile"};
    private WaveformContainer profile_container;
    int row[] = {3};
    Waveform wave[] = new Waveform[3];
    
    ProfileDialog(Frame f, String title)
    {
        super(f, title, false);
        profile_container = new WaveformContainer(row, false);
        WavePopup wp = new WavePopup();
        profile_container.setPopupMenu(wp);
        for(int i = 0; i < 3; i++)
        {
            wave[i] = new Waveform();
            wave[i].SetTitle(TITLE[i]);
        }
        profile_container.add(wave);
        add(profile_container);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {dispose();}});
    }
    
    public void updateProfiles(int pixels_x[], int start_pixel_x, 
                               int pixels_y[], int start_pixel_y,
                               int pixels_signal[], float frames_time[])
    {
        
        if(pixels_x != null && pixels_x.length > 0)
        {
            float x[] = new float[pixels_x.length];
            float xt[] = new float[pixels_x.length];
            for(int i = 0; i < pixels_x.length; i++)
            {
                x[i] = (float)(pixels_x[i] & 0xff);
                xt[i] = (float)start_pixel_x + i;
            }
            wave[0].Update(xt, x);
       }
        
        if(pixels_x != null && pixels_x.length > 0)
        {
            float y[] = new float[pixels_y.length];
            float yt[] = new float[pixels_y.length];
            for(int i = 0; i < pixels_y.length; i++)
            {
                y[i] = (float)(pixels_y[i] & 0xff);
                yt[i] = (float)start_pixel_y + i;
            }
            wave[1].Update(yt, y);
        }
        
        
        if(pixels_signal != null && pixels_x.length > 0 &&
           frames_time != null && frames_time.length > 0)
        {
            float s[] = new float[pixels_signal.length];
            for(int i = 0; i < pixels_signal.length; i++)
            {
                s[i] = (float)(pixels_signal[i] & 0xff);
            }
            wave[2].Update(frames_time, s);
        }
    }
}