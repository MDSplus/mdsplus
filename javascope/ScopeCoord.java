
import java.applet.Applet;
import java.awt.*;

public class ScopeCoord extends Applet
{
    public Label label;
    public void init()
    {
	setLayout(new GridLayout(1,1));
	add(label = new Label());
	validate();
    }
}

