import java.lang.Integer;
import java.awt.*;
//import java.awt.font.*;
import java.awt.event.*;
import java.util.Vector;
import java.io.*;


public class FontSelection extends ScopePositionDialog {
    Label fontLabel, sizeLabel, styleLabel, testLabel;
    Button ok, cancel, apply;
    FontPanel fontC;
    Choice fonts, sizes, styles;
    int index = 0;
    String fontchoice = "fontchoice";
    int stChoice = 0;
    String siChoice = "10";
    Font font = null;
    jScope main_scope;
    String envfonts[];
    String size_l[];
    String style_l[];

    public FontSelection(Frame dw, String title) {
        super(dw, title, true);
	    //setResizable(false);
        
	    main_scope = (jScope)dw;

        setLayout( new BorderLayout(5, 5) );

        Panel topPanel = new Panel();
        Panel fontPanel = new Panel();
        Panel sizePanel = new Panel();
        Panel stylePanel = new Panel();
        Panel sizeAndStylePanel = new Panel();
        Panel buttonPanel = new Panel();

        topPanel.setLayout( new BorderLayout(5, 5));
        fontPanel.setLayout( new GridLayout( 2, 1 ) );
        sizePanel.setLayout( new GridLayout( 2, 1 ) );
        stylePanel.setLayout( new GridLayout( 2, 1 ) );
        sizeAndStylePanel.setLayout( new BorderLayout() );
        buttonPanel.setLayout( new FlowLayout() );

        topPanel.add( BorderLayout.WEST, fontPanel );
        sizeAndStylePanel.add( BorderLayout.WEST, sizePanel );
        sizeAndStylePanel.add( BorderLayout.CENTER, stylePanel );
        topPanel.add( BorderLayout.CENTER, sizeAndStylePanel );

        add( BorderLayout.NORTH, topPanel );

        fontLabel = new Label();
        fontLabel.setText("Fonts");
        Font newFont = getFont();
        fontLabel.setFont(newFont);
        fontLabel.setAlignment(Label.CENTER);
        fontPanel.add(fontLabel);

        sizeLabel = new Label();
        sizeLabel.setText("Sizes");
        sizeLabel.setFont(newFont);
        sizeLabel.setAlignment(Label.CENTER);
        sizePanel.add(sizeLabel);

        styleLabel = new Label();
        styleLabel.setText("Styles");
        styleLabel.setFont(newFont);
        styleLabel.setAlignment(Label.CENTER);
        stylePanel.add(styleLabel);

//        if(System.getProperty("java.version").equals("1.2.2"))
//        {
//            GraphicsEnvironment gEnv = GraphicsEnvironment.getLocalGraphicsEnvironment();
//            envfonts = gEnv.getAvailableFontFamilyNames();
//        } else
            envfonts = getToolkit().getFontList();
        
        
        fonts = new Choice();
        for ( int i = 1; i < envfonts.length; i++ ) {
            fonts.addItem(envfonts[i]);
        }
               
        fonts.addItemListener(this);
        fontchoice = envfonts[0];
        fontPanel.add(fonts);

        size_l = new String[]{ "10", "12", "14", "16", "18"};
        sizes = new Choice();
        for ( int i = 0; i < size_l.length; i++ ) {
            sizes.addItem(size_l[i]);
        }
        
        sizes.addItemListener(this);
        sizePanel.add(sizes);

        style_l = new String[]{
                                "PLAIN",
                                "BOLD",
                                "ITALIC",
                                "BOLD & ITALIC"};
        styles = new Choice();
        for ( int i = 0; i < style_l.length; i++ ) {
            styles.addItem(style_l[i]);
        }

        styles.addItemListener(this);
        stylePanel.add(styles);

        fontC = new FontPanel();
        fontC.setBackground(Color.white);
        add( BorderLayout.CENTER, fontC);

	      ok = new Button("Ok");
	      ok.addActionListener(this);	
          buttonPanel.add(ok);

          apply = new Button("Apply");
	      apply.addActionListener(this);	
          buttonPanel.add(apply);

	      cancel = new Button("Cancel");
	      cancel.addActionListener(this);	
          buttonPanel.add(cancel);
	    		
	      
          add( BorderLayout.SOUTH, buttonPanel);
    }
    
    private void setFontChoice()
    {
        fonts.select(fontchoice);
        styles.select(stChoice);
        sizes.select(siChoice);
    }
    
    public Font StringToFont(String f)
    {
        String s;
        String style;
        int pos, i;
        
        
        if(f.indexOf("java.awt.Font[") == -1)
            return null;
        
        fontchoice = f.substring("java.awt.Font[Family=".length(), pos = f.indexOf(","));
        pos++;
        pos = f.indexOf(",", pos) + 1;
        style = f.substring(f.indexOf("style=")+6, pos = f.indexOf(",", pos));
        for(i = 0; i < this.style_l.length; i++)
            if(style_l[i].equals(style.toUpperCase()))
                break;
        if(i == this.style_l.length)
            stChoice = 0;
        else
            stChoice = i;
        siChoice = f.substring(f.indexOf("size=")+5, pos = f.indexOf("]", pos));
        return new Font(fontchoice, stChoice, Integer.parseInt(siChoice));
    }

    public int fromFile(ReaderConfig in, String prompt) throws IOException
    {
    	String str;
	    int error = 0;

	    while((str = in.readLine()) != null) {
	        if(str.indexOf(prompt) != -1)
	        {
	            font = StringToFont(str.substring(prompt.length()+1));
	        }
	    }
	    
	    if(font != null) {
	        setFontChoice();
	        this.fontC.changeFont(font);
	    }
	    
	    return error;
    }
    
    
    public void toFile(PrintWriter out, String prompt)
    {
        if(font != null)
	        jScope.writeLine(out, prompt +": ", font.toString());		
    }

    public Font GetFont()
    {
            fontchoice = (String)fonts.getSelectedItem();
            stChoice = styles.getSelectedIndex();
            siChoice = (String)sizes.getSelectedItem();
            Integer newSize = new Integer(siChoice);
            int size = newSize.intValue();
            Font f = new Font(fontchoice, stChoice, size);
            StringToFont(f.toString());
            
            return f;
    }

    public void itemStateChanged(ItemEvent e) {
        if ( e.getStateChange() != ItemEvent.SELECTED ) {
            return;
        }

        Object list = e.getSource();
        fontC.changeFont(GetFont());
    }
    
    public void actionPerformed(ActionEvent e) 
    {

	    Object ob = e.getSource();
	    int i;
    
	    if (ob == ok || ob == apply)
	    {
            font = GetFont();
            main_scope.setup.SetFontToWaves(font);
	        if(ob == ok)
		        setVisible(false);
        }  

	    if (ob == cancel)
	    {
	        setVisible(false);
	    }
    }  
}


class FontPanel extends Panel {

    Font thisFont;

    public FontPanel(){ 
        thisFont = new Font("Arial", Font.PLAIN, 10);
    }
    
    public Dimension getPreferredSize()
    {
        return new Dimension(100, 50);
    }
    
    public void changeFont(Font font){
        thisFont = font;
        repaint();
    }

    public void paint (Graphics g) {
        int w = getSize().width;//getWidth();
        int h = getSize().height;//getHeight();

        g.setColor(Color.darkGray);
        g.setFont(thisFont);
        String change = "Pick a font, size, and style to change me";
        FontMetrics metrics = g.getFontMetrics();
        int width = metrics.stringWidth( change );
        int height = metrics.getHeight();
//        System.out.println(thisFont.toString());
        g.drawString( change, w/2-width/2, h/2-height/2 );
    }
}