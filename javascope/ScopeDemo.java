import java.applet.Applet;
import java.awt.*;
import java.io.*;
//import Scope;


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
