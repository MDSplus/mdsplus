package mds.jscope;

/* $Id$ */
import java.lang.Integer;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

import java.io.*;

class FontPanel extends JPanel
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	Font thisFont;

	public FontPanel()
	{
		thisFont = new Font("Arial", Font.PLAIN, 10);
	}

	public void changeFont(Font font)
	{
		thisFont = font;
		repaint();
	}

	@Override
	public Dimension getPreferredSize()
	{ return new Dimension(100, 50); }

	// Check if the font is visible on
	// this platform
	public boolean isFontAvailable(Font font)
	{
		int height;
		final Graphics g = this.getGraphics();
		final FontMetrics fm = g.getFontMetrics(font);
		height = fm.getHeight();
		return (height > 9 && height < 100);
	}

	@Override
	public void paint(Graphics g)
	{
		final int w = getSize().width;// getWidth();
		final int h = getSize().height;// getHeight();
		g.setColor(Color.darkGray);
		g.setFont(thisFont);
		final String change = "Pick a font, size, and style to change me";
		final FontMetrics metrics = g.getFontMetrics();
		final int width = metrics.stringWidth(change);
		final int height = metrics.getHeight();
//        System.out.println(thisFont.toString());
		g.drawString(change, w / 2 - width / 2, h / 2 - height / 2);
	}
}

public class FontSelection extends JDialog implements ActionListener, ItemListener
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	JLabel fontLabel, sizeLabel, styleLabel, testLabel;
	JRadioButton application_i, waveform_i;
	JButton ok, cancel, apply;
	FontPanel fontC;
	JComboBox<String> fonts;
	JComboBox<String> sizes;
	JComboBox<String> styles;
	String[] size_l;
	String[] style_l;
	int index = 0;
	String fontchoice = "fontchoice";
	int stChoice = 0;
	String siChoice = "10";
	jScopeFacade main_scope;
	Font font;

	public FontSelection(Frame dw, String title)
	{
		super(dw, title, true);
		main_scope = (jScopeFacade) dw;
		getContentPane().setLayout(new BorderLayout(5, 5));
		final JPanel topPanel = new JPanel();
		final JPanel fontPanel = new JPanel();
		final JPanel sizePanel = new JPanel();
		final JPanel stylePanel = new JPanel();
		final JPanel sizeAndStylePanel = new JPanel();
		final JPanel buttonPanel = new JPanel();
		final JPanel fontSelectionPanel = new JPanel();
		topPanel.setLayout(new BorderLayout(5, 5));
		fontPanel.setLayout(new GridLayout(2, 1));
		sizePanel.setLayout(new GridLayout(2, 1));
		stylePanel.setLayout(new GridLayout(2, 1));
		fontSelectionPanel.setLayout(new GridLayout(2, 1));
		sizeAndStylePanel.setLayout(new BorderLayout());
		buttonPanel.setLayout(new FlowLayout());
		topPanel.add(BorderLayout.WEST, fontPanel);
		sizeAndStylePanel.add(BorderLayout.WEST, sizePanel);
		sizeAndStylePanel.add(BorderLayout.CENTER, stylePanel);
		topPanel.add(BorderLayout.CENTER, sizeAndStylePanel);
		topPanel.add(BorderLayout.SOUTH, fontSelectionPanel);
		getContentPane().add(BorderLayout.NORTH, topPanel);
		fontLabel = new JLabel();
		fontLabel.setText("Fonts");
		final Font newFont = getFont();
		fontLabel.setFont(newFont);
		fontLabel.setHorizontalAlignment(SwingConstants.CENTER);
		fontPanel.add(fontLabel);
		sizeLabel = new JLabel();
		sizeLabel.setText("Sizes");
		sizeLabel.setFont(newFont);
		sizeLabel.setHorizontalAlignment(SwingConstants.CENTER);
		sizePanel.add(sizeLabel);
		styleLabel = new JLabel();
		styleLabel.setText("Styles");
		styleLabel.setFont(newFont);
		styleLabel.setHorizontalAlignment(SwingConstants.CENTER);
		stylePanel.add(styleLabel);
		final ButtonGroup font_selection = new ButtonGroup();
		application_i = new JRadioButton("Application");
		fontSelectionPanel.add(application_i);
		waveform_i = new JRadioButton("Waveform", true);
		fontSelectionPanel.add(waveform_i);
		font_selection.add(application_i);
		font_selection.add(waveform_i);
		final GraphicsEnvironment gEnv = GraphicsEnvironment.getLocalGraphicsEnvironment();
		final String[] envfonts = gEnv.getAvailableFontFamilyNames();
		fonts = new JComboBox<>(envfonts);
		fonts.addItemListener(this);
		fontchoice = envfonts[0];
		fontPanel.add(fonts);
		size_l = new String[]
		{ "10", "12", "14", "16", "18", "20", "22", "24", "26", "28" };
		sizes = new JComboBox<>(size_l);
		sizes.addItemListener(this);
		sizePanel.add(sizes);
		style_l = new String[]
		{ "PLAIN", "BOLD", "ITALIC", "BOLD & ITALIC" };
		styles = new JComboBox<>(style_l);
		styles.addItemListener(this);
		stylePanel.add(styles);
		fontC = new FontPanel();
		fontC.setBackground(Color.white);
		getContentPane().add(BorderLayout.CENTER, fontC);
		ok = new JButton("Ok");
		ok.addActionListener(this);
		buttonPanel.add(ok);
		apply = new JButton("Apply");
		apply.addActionListener(this);
		buttonPanel.add(apply);
		cancel = new JButton("Cancel");
		cancel.addActionListener(this);
		buttonPanel.add(cancel);
		getContentPane().add(BorderLayout.SOUTH, buttonPanel);
		pack();
		GetPropertiesValue();
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		final Object ob = e.getSource();
		if (ob == ok || ob == apply)
		{
			if (waveform_i.isSelected())
			{
				font = GetFont();
				main_scope.UpdateFont();
				main_scope.RepaintAllWaves();
				main_scope.setChange(true);
			}
			if (application_i.isSelected())
			{
				main_scope.SetApplicationFonts(GetFont());
				// SwingUtilities.updateComponentTreeUI(main_scope);
			}
			if (ob == ok)
				setVisible(false);
		}
		if (ob == cancel)
		{
			setVisible(false);
		}
	}

	public void fromFile(Properties pr, String prompt) throws IOException
	{
		String prop;
		if ((prop = pr.getProperty(prompt)) != null)
		{
			font = StringToFont(prop);
		}
		if (font != null)
		{
			setFontChoice();
			fontC.changeFont(font);
		}
	}

	public Font GetFont()
	{
		fontchoice = (String) fonts.getSelectedItem();
		stChoice = styles.getSelectedIndex();
		siChoice = (String) sizes.getSelectedItem();
		Font f = new Font(fontchoice, stChoice, Integer.parseInt(siChoice));
		f = StringToFont(f.toString());
		return f;
	}

	private void GetPropertiesValue()
	{
		final Properties js_prop = main_scope.js_prop;
		if (js_prop == null)
			return;
		String prop = js_prop.getProperty("jScope.font.application");
		if (prop != null)
			main_scope.SetApplicationFonts(StringToFont(prop));
		prop = js_prop.getProperty("jScope.font");
		if (prop == null)
			font = StringToFont(getFont().toString());
		else
			font = StringToFont(prop);
		if (font != null)
		{
			setFontChoice();
			fontC.changeFont(font);
		}
	}

	@Override
	public void itemStateChanged(ItemEvent e)
	{
		if (e.getStateChange() != ItemEvent.SELECTED)
			return;
		fontC.changeFont(GetFont());
	}

	private void setFontChoice()
	{
		fonts.removeItemListener(this);
		styles.removeItemListener(this);
		sizes.removeItemListener(this);
		fonts.setSelectedItem(fontchoice);
		styles.setSelectedIndex(stChoice);
		sizes.setSelectedItem(siChoice);
		fonts.addItemListener(this);
		styles.addItemListener(this);
		sizes.addItemListener(this);
	}

	public Font StringToFont(String f)
	{
		String style;
		int pos, i;
		if (f.indexOf("java.awt.Font[") == -1)
			return null;
		if (jScopeFacade.is_debug)
			System.out.println("Font desiderato " + f);
		fontchoice = f.substring(f.indexOf("family=") + 7, pos = f.indexOf(",name="));
		pos++;// Index on the string after comma before style
		style = f.substring(f.indexOf("style=", pos) + 6, pos = f.indexOf(",size", pos));
		for (i = 0; i < this.style_l.length; i++)
			if (style_l[i].equals(style.toUpperCase()))
				break;
		if (i == this.style_l.length)
			stChoice = 0;
		else
			stChoice = i;
		siChoice = f.substring(f.indexOf("size=") + 5, f.indexOf("]", pos));
		Font font = new Font(fontchoice, stChoice, Integer.parseInt(siChoice));
		// Check if selected font can be displayed
		// on the current patform
		if (!fontC.isFontAvailable(font))
		{
			// Set font to default font
			fontchoice = "Default";
			stChoice = 0;
			siChoice = "12";
			font = new Font(fontchoice, stChoice, Integer.parseInt(siChoice));
		}
		if (jScopeFacade.is_debug)
			System.out.println("Font ottenuto " + font);
		return font;
	}

	public void toFile(PrintWriter out, String prompt)
	{
		if (font != null)
			out.println(prompt + ": " + font.toString());
		out.println("");
	}
}
