import java.applet.Applet;
import java.awt.*;
import java.io.*;
import Scope;

public class PopScopes extends Applet
{
	Button button;
	Label label;
	Class window_class;
	Scope scopes[];
	Checkbox synch;
	boolean is_synch;
	int num_scopes;


    public void init()
    {
		try {
			window_class = Class.forName("ScopeDialog");
		} catch(Exception e) {}
		add(label = new Label("Speremo ben..."));
//		add(button = new Button("Click here to get a new Scope"));
//		add(synch = new Checkbox("Synchronized", null,  false));
//		is_synch = false;
//		scopes = new Scope[64];
		validate();
//		num_scopes = 0;
		try {
		ScopeDialog dscope = (ScopeDialog)window_class.newInstance();
		dscope.show();
		} catch(Exception ex){System.out.println(ex);}
    }

	public boolean action(Event e, Object arg)
	{
		Object target = e.target;

		if(target == button)
		{
			try{
				ScopeDialog dscope = (ScopeDialog)window_class.newInstance();
				dscope.show();
				dscope.controller = this;
				dscope.scope.controller = this;
				scopes[num_scopes++] = dscope.scope;

				dscope.scope.Synchronize(is_synch);

			} catch(Exception ex){}
			return true;
		}
		if(target == synch)
		{
			is_synch = ((Boolean)arg).booleanValue();
			for(int i = 0; i < num_scopes; i++)
				scopes[i].Synchronize(is_synch);
			return true;
		}
		return false;
	}

	public void Remove(Scope sc)
	{
		int i;
		for(i = 0; i < num_scopes && scopes[i] != sc; i++);
		for(; i < num_scopes - 1; i++)
			scopes[i] = scopes[i+1];
		num_scopes--;
	}

	public void UpdatePoint(float curr_x, Waveform wave)
	{
	}
}




class ScopeDialog extends Frame
{

	public Scope scope;
	public PopScopes controller;
	public ScopeDialog()
	{
		super("Scope");
		resize(350,250);
		scope = new Scope((Frame)this);
		scope.controller = controller;
		add("Center", scope);
	}

	
   public boolean handleEvent(Event event) {
        if (event.id == Event.WINDOW_DESTROY) {

			dispose();
        } 
	if(event.id == Event.WINDOW_ICONIFY)
	    scope.setup.Hide();  
        return super.handleEvent(event);
    }
}
