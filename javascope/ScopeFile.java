import java.awt.*;


public class scopeFile extends FileDialog {

    private String load_path;
    private String save_path;

    scopeFile(Frame fw, String title, int mode)
    {
	super(fw, title, mode);

	int i, comp;
	comp = getComponentCount();
	System.out.println("COMPONENT"+comp);
	for(i = 0; i < comp; i++)
	    System.out.println("FILE COMPO -> "  + getComponent(i));
 
    }
 		
}
 
