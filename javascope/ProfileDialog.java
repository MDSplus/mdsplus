import javax.swing.JDialog;
import javax.swing.JFrame;
import java.awt.event.*;


public class ProfileDialog extends JDialog
{
    static final String TITLE[] = {"X profile", "Y profile", "Pixel time profile"};
    private WaveformContainer profile_container;
    int row[] = {3};
    Waveform wave[] = new Waveform[3];
    Waveform w_profile_line = null;
    private String name;
    private jScopeMultiWave source_profile = null;
    
    ProfileDialog(jScopeMultiWave source_profile)
    {
        this.source_profile = source_profile;
        setTitle("Profile Dialog");
        profile_container = new WaveformContainer(row, false);
        WavePopup wp = new WavePopup();
        profile_container.setPopupMenu(wp);
        for(int i = 0; i < 3; i++)
        {
            wave[i] = new Waveform();
            wave[i].SetTitle(TITLE[i]);
        }
        profile_container.add(wave);
        getContentPane().add(profile_container);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) 
            {
                if(ProfileDialog.this.source_profile != null)
                    ProfileDialog.this.source_profile.setSendProfile(false);
                dispose();
            }
        });
    }
    
    public void addProfileLine()
    {        
        w_profile_line = new Waveform();
        profile_container.add(w_profile_line, 4, 1);
        w_profile_line.SetTitle("Line Profile");
        profile_container.update();
    }

    public void removeProfileLine()
    {   
        if(w_profile_line == null) return;
        profile_container.removeComponent(w_profile_line);
        w_profile_line = null;
        profile_container.update();        
    }

    public synchronized void updateProfileLine(int pixels_line[])
    {
        float x[] = new float[pixels_line.length];
        float xt[] = new float[pixels_line.length];
        
        if(w_profile_line == null)
          addProfileLine();  
        for(int i = 0; i < pixels_line.length; i++)
        {
            x[i] = (float)(pixels_line[i] & 0xff);
            xt[i] = (float)i;
        }
        w_profile_line.Update(xt, x);
    }
    
    public synchronized void updateProfiles(String name,
                                            int pixels_x[], int start_pixel_x, 
                                            int pixels_y[], int start_pixel_y,
                                            int pixels_signal[], float frames_time[])
    {
        float x_null[] = {0.0F, 0.1F};
        float y_null[] = {0.0F, 0.0F};
        
        if(!name.equals(this.name))
        {
            this.name = new String(name);
            setTitle("Profile Dialog - "+name);
        }
        
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