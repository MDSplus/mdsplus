package mds.wave;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.Vector;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import mds.connection.MdsConnection;

public class ColorMapDialog extends JDialog
{
	public class ColorPalette extends JPanel
	{
		private static final long serialVersionUID = 1L;
		Color colors[];

		ColorPalette(Color colors[])
		{
			this.setBorder(BorderFactory.createLoweredBevelBorder());
			setColormap(colors);
		}

		@Override
		public void paintComponent(Graphics g)
		{
			final Dimension d = getSize();
			final float dx = (float) d.width / colors.length;
			int x;
			int width;
			for (int i = 0; i < colors.length; i++)
			{
				x = (int) (i * dx + 0.5);
				width = (int) (x + dx + 0.5);
				g.setColor(colors[i]);
				g.fillRect(x, 0, width, d.height);
			}
			super.paintBorder(g);
		}

		public void setColormap(Color colors[])
		{
			this.colors = colors;
			repaint();
		}
	}

	private static final long serialVersionUID = 1L;
	private static String lastMap = null;

	public static void exportPalette(final File file) throws IOException
	{
		try (final InputStream is = getResourcePalette())
		{
			try (final OutputStream os = new FileOutputStream(file))
			{
				final byte[] buffer = new byte[1024];
				int length;
				while ((length = is.read(buffer)) > 0)
					os.write(buffer, 0, length);
			}
		}
	}

	private static final InputStream getResourcePalette()
	{ return ColorMapDialog.class.getClassLoader().getResourceAsStream("mds/wave/colors.tbl"); }

	private final Vector<ActionListener> colorMapListener = new Vector<>();
	ColorMap colorMap;
	ColorPalette cp;
	JComboBox<ColorMap> cmComboBox;
	JTextField minVal, maxVal;
	JButton ok, apply, cancel;
	JSlider shiftSlider;
	JCheckBox bitClip;
	JPanel bitOptionPanel;
	boolean is16BitImage = false;
	// WaveformEditor weR, weG, weB;
	Waveform wave = null;
	String nameColorTables[];
	byte colorTables[];

	/*
	 * ColorMapDialog(Frame f, Waveform wave) { // this(f, wave.getColorMap());
	 * this.wave = wave; }
	 */
	public ColorMapDialog(Frame f, String colorPaletteFile)
	{
		super(f, "Color Palette");
		readColorPalette(colorPaletteFile);
		getContentPane().setLayout(new GridLayout(4, 1));
		final JPanel pan1 = new JPanel();
		pan1.setLayout(new GridLayout(2, 1));
		final JPanel pan2 = new JPanel();
		final JPanel pan3 = new JPanel();
		pan2.add(new JLabel("MIN : "));
		pan2.add(minVal = new JTextField(6));
		minVal.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent ev)
			{
				float min, max;
				try
				{
					min = Float.parseFloat(minVal.getText().trim());
				}
				catch (final Exception exc)
				{
					min = Float.MIN_VALUE;
				}
				try
				{
					max = Float.parseFloat(maxVal.getText().trim());
				}
				catch (final Exception exc)
				{
					max = Float.MAX_VALUE;
				}
				ColorMapDialog.this.wave.setFrameMinMax(min, max);
			}
		});
		pan2.add(new JLabel("MAX : "));
		pan2.add(maxVal = new JTextField(6));
		maxVal.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent ev)
			{
				float min, max;
				try
				{
					min = Float.parseFloat(minVal.getText().trim());
				}
				catch (final Exception exc)
				{
					min = Float.MIN_VALUE;
				}
				try
				{
					max = Float.parseFloat(maxVal.getText().trim());
				}
				catch (final Exception exc)
				{
					max = Float.MAX_VALUE;
				}
				ColorMapDialog.this.wave.setFrameMinMax(min, max);
			}
		});
		cmComboBox = new JComboBox<>();
		pan3.add(cmComboBox);
		final int r[] = new int[256];
		final int g[] = new int[256];
		final int b[] = new int[256];
		for (int i = 0; i < nameColorTables.length; i++)
		{
			for (int j = 0; j < 256; j++)
			{
				r[j] = 0xFF & this.colorTables[i * (256 * 3) + j];
				g[j] = 0xFF & this.colorTables[i * (256 * 3) + 256 + j];
				b[j] = 0xFF & this.colorTables[i * (256 * 3) + 256 * 2 + j];
			}
			cmComboBox.addItem(new ColorMap(nameColorTables[i], r, g, b));
		}
		colorMap = (ColorMap) cmComboBox.getSelectedItem();
		cmComboBox.addItemListener(new ItemListener()
		{
			@Override
			public void itemStateChanged(ItemEvent ev)
			{
				cp.setColormap(((ColorMap) ev.getItem()).colors);
				ColorMapDialog.this.wave.applyColorModel((ColorMap) ev.getItem());
			}
		});
		if (colorMap == null)
			colorMap = new ColorMap();
		cp = new ColorPalette(colorMap.colors);
		getContentPane().add(cp);
		pan1.add(pan2);
		pan1.add(pan3);
		bitOptionPanel = new JPanel();
		bitOptionPanel.setBorder(BorderFactory.createTitledBorder("16 bit  Option"));
		bitOptionPanel.add(shiftSlider = new JSlider(-8, 8, 0));
		shiftSlider.setName("Bit Offset");
		shiftSlider.setMajorTickSpacing(1);
		shiftSlider.setPaintTicks(true);
		shiftSlider.setPaintLabels(true);
		shiftSlider.setSnapToTicks(true);
		shiftSlider.addChangeListener(new ChangeListener()
		{
			@Override
			public void stateChanged(ChangeEvent e)
			{
				ColorMapDialog.this.wave.setFrameBitShift(shiftSlider.getValue(), bitClip.isSelected());
			}
		});
		bitOptionPanel.add(bitClip = new JCheckBox("Bit Clip"));
		bitClip.addChangeListener(new ChangeListener()
		{
			@Override
			public void stateChanged(ChangeEvent e)
			{
				ColorMapDialog.this.wave.setFrameBitShift(shiftSlider.getValue(), bitClip.isSelected());
			}
		});
		final JPanel pan4 = new JPanel();
		pan4.add(ok = new JButton("Ok"));
		ok.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
//                if (ColorMapDialog.this.wave.IsImage())
				{
					final ColorMap cm = (ColorMap) cmComboBox.getSelectedItem();
					if (is16BitImage)
					{
						cm.bitClip = bitClip.isSelected();
						cm.bitShift = shiftSlider.getValue();
					}
					try
					{
						cm.setMin(Float.parseFloat(minVal.getText().trim()));
					}
					catch (final Exception exc)
					{
						cm.setMin(Float.MIN_VALUE);
					}
					try
					{
						cm.setMax(Float.parseFloat(maxVal.getText().trim()));
					}
					catch (final Exception exc)
					{
						cm.setMax(Float.MAX_VALUE);
					}
					ColorMapDialog.this.wave.setColorMap(cm);
					ColorMapDialog.this.setVisible(false);
				}
			}
		});
		/*
		 * ok.addActionListener(new ActionListener() { public void
		 * actionPerformed(ActionEvent e) { int max, min; try { min =
		 * Integer.parseInt(minVal.getText()); } catch(Exception exc) { min = 0; } try {
		 * max = Integer.parseInt(maxVal.getText()); } catch(Exception exc) { max = 255;
		 * } cm.createColorMap( 256, 0, min, max ); weR.setWaveform(cm.getRedPoints(),
		 * cm.getRedValues(), 0f, 255f); weR.repaint();
		 * weG.setWaveform(cm.getGreenPoints(), cm.getGreenValues(), 0f, 255f);
		 * weG.repaint(); weB.setWaveform(cm.getBluePoints(), cm.getBlueValues(), 0f,
		 * 255f); weB.repaint(); processActionEvents(new ActionEvent(this, 0, "")); } }
		 * );
		 */
		/*
		 * pan3.add(apply = new JButton("Apply")); apply.addActionListener(new
		 * ActionListener() { public void actionPerformed(ActionEvent e) {
		 * if(ColorMapDialog.this.wave.IsImage()) { ColorMap cm =
		 * (ColorMap)cmComboBox.getSelectedItem();
		 * ColorMapDialog.this.wave.changeColorModel(cm); } } } );
		 */
		pan4.add(cancel = new JButton("Cancel"));
		cancel.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
//                if (ColorMapDialog.this.wave.IsImage())
				{
					ColorMapDialog.this.wave.setColorMap(colorMap);
					ColorMapDialog.this.setVisible(false);
					if (is16BitImage)
					{
						bitClip.setSelected(colorMap.bitClip);
						shiftSlider.setValue(colorMap.bitShift);
					}
				}
			}
		});
		getContentPane().add(pan1);
//        getContentPane().add(bitOptionPanel);
		getContentPane().add(pan4);
		pack();
		setSize(330, 380);
	}

	public void addColorMapListener(ActionListener l)
	{
		colorMapListener.addElement(l);
	}

	public ColorMap getColorMap(String name)
	{
		for (int i = 0; i < cmComboBox.getItemCount(); i++)
		{
			final ColorMap cm = cmComboBox.getItemAt(i);
			if (cm.name.equals(name))
				return cm;
		}
		return new ColorMap();
	}

	public void processActionEvents(ActionEvent avtionEvent)
	{
		for (int i = 0; i < colorMapListener.size(); i++)
			colorMapListener.elementAt(i).actionPerformed(avtionEvent);
	}

	public void readColorPalette(String cmap)
	{
		if (cmap == null)
			cmap = lastMap;
		DataInputStream dis = null;
		try
		{
			try
			{
				dis = new DataInputStream(new FileInputStream(new File(cmap)));
			}
			catch (final IOException exc)
			{
				dis = new DataInputStream(getResourcePalette());
			}
			final byte nColorTables = dis.readByte();
			nameColorTables = new String[nColorTables];
			final byte name[] = new byte[32];
			colorTables = new byte[nColorTables * 3 * 256];
			dis.readFully(colorTables);
			for (int i = 0; i < nColorTables; i++)
			{
				dis.readFully(name);
				nameColorTables[i] = (new String(name)).trim();
			}
			lastMap = cmap;
			MdsConnection.tryClose(dis);
		}
		catch (final Exception exc)
		{
			nameColorTables = new String[0];
			colorTables = new byte[0];
			MdsConnection.tryClose(dis);
		}
	}

	public void removeMapListener(ActionListener l)
	{
		colorMapListener.remove(l);
	}

	@SuppressWarnings("unused")
	public void setWave(Waveform wave)
	{
		this.wave = wave;
		colorMap = wave.getColorMap();
		minVal.setText("" + colorMap.getMin());
		maxVal.setText("" + colorMap.getMax());
		cmComboBox.setSelectedItem(colorMap);
		// shiftSlider and bitClip to be remove
		// color scaled on min max value
		if (false)
		{
			if (!is16BitImage)
			{
				getContentPane().setLayout(new GridLayout(4, 1));
				getContentPane().add(bitOptionPanel, 2);
				setSize(380, 350);
			}
			is16BitImage = true;
			shiftSlider.setValue(colorMap.bitShift);
			bitClip.setSelected(colorMap.bitClip);
		}
		else
		{
			is16BitImage = false;
			getContentPane().remove(bitOptionPanel);
			getContentPane().setLayout(new GridLayout(3, 1));
			setSize(380, 250);
		}
	}
}
