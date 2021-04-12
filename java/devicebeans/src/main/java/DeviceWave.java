import java.awt.*;
import java.awt.datatransfer.*;
import java.awt.event.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.text.NumberFormat;
import java.util.*;

import javax.swing.*;
import javax.swing.event.TableModelListener;
import javax.swing.table.DefaultTableModel;

import mds.wave.*;

public class DeviceWave extends DeviceComponent
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	static final int MAX_POINTS = 500;
	static final double MIN_STEP = 1E-5;
	protected static float savedMinX, savedMinY, savedMaxX, savedMaxY;
	protected static float savedWaveX[] = null, savedWaveY[] = null;

	public static void main(String args[])
	{
		new DeviceWave();
		System.out.println("Istanziato");
	}

	public boolean maxXVisible = false;
	public boolean minXVisible = false;
	public boolean maxYVisible = true;
	public boolean minYVisible = false;
	public boolean waveEditable = false;
	public String updateExpression = null;
	protected int prefHeight = 200;
	protected boolean initializing = false;
	protected WaveformEditor waveEditor;
	protected JTable table;
	protected JTextField maxXField = null, minXField = null, maxYField = null, minYField = null;
	protected JCheckBox editCB;
	protected JScrollPane scroll;
	protected int numPoints;
	protected float[] waveX = null, waveY = null;
	protected float[] waveXOld = null, waveYOld = null;
	protected float maxX, minX, maxY, minY;
	protected float maxXOld, minXOld, maxYOld, minYOld;
	private final NumberFormat nf = NumberFormat.getInstance(Locale.ENGLISH);
	JPopupMenu copyPastePopup;
	JMenuItem copyI, copyC, pasteI, pasteC;

	public DeviceWave()
	{}

	@Override
	public Component add(Component c)
	{
		if (!initializing)
		{
			JOptionPane.showMessageDialog(null,
					"You cannot add a component to a Device Wave. Please remove the component.",
					"Error adding Device field", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return super.add(c);
	}

	@Override
	public Component add(Component c, int intex)
	{
		if (!initializing)
		{
			JOptionPane.showMessageDialog(null,
					"You cannot add a component to a Device Wave. Please remove the component.",
					"Error adding Device field", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return super.add(c);
	}

	@Override
	public Component add(String name, Component c)
	{
		if (!initializing)
		{
			JOptionPane.showMessageDialog(null,
					"You cannot add a component to a Device Wave. Please remove the component.",
					"Error adding Device field", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return super.add(c);
	}

	@Override
	public void apply() throws Exception
	{
		final CellEditor ce = table.getCellEditor();
		if (ce != null)
			ce.stopCellEditing();
		super.apply();
		updateLimits();
		if (minXVisible)
		{
			try
			{
				subtree.putDataExpr(nidData + 1, "" + minX);
			}
			catch (final Exception exc)
			{
				System.out.println("Error storing min X value: " + exc);
			}
		}
		if (maxXVisible)
		{
			try
			{
				subtree.putDataExpr(nidData + 2, "" + maxX);
			}
			catch (final Exception exc)
			{
				System.out.println("Error storing max X value: " + exc);
			}
		}
		if (minYVisible)
		{
			try
			{
				subtree.putDataExpr(nidData + 3, "" + minY);
			}
			catch (final Exception exc)
			{
				System.out.println("Error storing min Y value: " + exc);
			}
		}
		if (maxYVisible)
		{
			try
			{
				subtree.putDataExpr(nidData + 4, "" + maxY);
			}
			catch (final Exception exc)
			{
				System.out.println("Error storing max Y value: " + exc);
			}
		}
	}

	private void create()
	{
		savedWaveX = null;
		waveEditor = new WaveformEditor();
		nf.setMaximumFractionDigits(4);
		nf.setGroupingUsed(false);
		waveEditor.setPreferredSize(new Dimension(300, prefHeight));
		waveEditor.addWaveformEditorListener(new WaveformEditorListener()
		{
			@Override
			public void waveformUpdated(float[] waveX, float[] waveY, int newIdx)
			{
				numPoints = waveX.length;
				DeviceWave.this.waveX = waveX;
				DeviceWave.this.waveY = waveY;
				if (newIdx >= 0)
				{
					table.setRowSelectionInterval(newIdx, newIdx);
					table.setEditingRow(newIdx);
					int centerIdx;
					if (newIdx > 8)
						centerIdx = newIdx - 4;
					else
						centerIdx = 0;
					final int rowY = centerIdx * table.getRowHeight();
					scroll.getViewport().setViewPosition(new Point(0, rowY));
				}
				if (maxYVisible)
				{
					try
					{
						maxY = (new Float(maxYField.getText()).floatValue());
						for (int i = 0; i < waveY.length; i++)
							if (waveY[i] > maxY)
								waveY[i] = maxY;
					}
					catch (final Exception exc)
					{}
				}
				table.repaint();
			}
		});
		waveEditor.setEditable(false);
		table = new JTable();
		/*
		 * table.setColumnModel(new DefaultTableColumnModel() { public int
		 * getTotalColumnWidth() {return 150;} });
		 */
		table.setModel(new DefaultTableModel()
		{
			/**
			 *
			 */
			private static final long serialVersionUID = 1L;

			@Override
			public void addTableModelListener(TableModelListener l)
			{}

			@Override
			public Class<?> getColumnClass(int col)
			{
				return new String().getClass();
			}

			@Override
			public int getColumnCount()
			{ return 2; }

			@Override
			public String getColumnName(int col)
			{
				if (col == 0)
					return "Time";
				else
					return "Value";
			}

			@Override
			public int getRowCount()
			{ return MAX_POINTS; }

			@Override
			public Object getValueAt(int row, int col)
			{
				if (waveX == null || row >= waveX.length)
					return "";
				final float currVal = (col == 0) ? waveX[row] : waveY[row];
				// return (new Float(currVal)).toString();
				return (nf.format(currVal));
			}

			@Override
			public boolean isCellEditable(int row, int col)
			{
				if (!waveEditable)
					return false;
				if (row == 0 && col == 0)
					return false;
				if (row == waveX.length - 1 && col == 0)
					return false;
				return true;
			}

			@Override
			public void removeTableModelListener(TableModelListener l)
			{}

			@Override
			public void setValueAt(Object val, int row, int col)
			{
				if (row >= waveX.length)
				{
					JOptionPane.showMessageDialog(DeviceWave.this,
							"There are misssing points in the waveform definition", "Incorrect waveform definition",
							JOptionPane.WARNING_MESSAGE);
					return;
				}
				float valFloat;
				try
				{
					valFloat = (new Float((String) val)).floatValue();
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(DeviceWave.this,
							"The value is not a correct floating point representation", "Incorrect waveform definition",
							JOptionPane.WARNING_MESSAGE);
					return;
				}
				if (col == 0)
				{
					if (valFloat > maxX)
						waveX[row] = maxX;
					else if (valFloat < minX)
						waveX[row] = minX;
					else
						waveX[row] = valFloat;
					if (row == 0 || row == waveX.length - 1)
						return;
					if (waveX[row] < waveX[row - 1] + MIN_STEP)
						waveX[row] = waveX[row - 1] + (float) MIN_STEP;
					if (waveX[row] > waveX[row + 1] - MIN_STEP)
						waveX[row] = waveX[row + 1] - (float) MIN_STEP;
				}
				else
				{
					// if(valFloat > maxY)
					// waveY[row] = maxY;
					// else if(valFloat < minY)
					// waveY[row] = minY;
					// else
					waveY[row] = valFloat;
				}
				waveEditor.setWaveform(waveX, waveY, minY, maxY);
			}
		});
		setLayout(new BorderLayout());
		add(waveEditor, "Center");
		scroll = new JScrollPane(table);
		scroll.setPreferredSize(new Dimension(150, 200));
		add(scroll, "East");
		final JPanel jp = new JPanel();
		if (minXVisible)
		{
			jp.add(new JLabel("Min X: "));
			jp.add(minXField = new JTextField("" + minX, 6));
		}
		if (maxXVisible)
		{
			jp.add(new JLabel("Max X: "));
			jp.add(maxXField = new JTextField("" + maxX, 6));
		}
		if (minYVisible)
		{
			jp.add(new JLabel("Min Y: "));
			jp.add(minYField = new JTextField("" + minY, 6));
		}
		if (maxYVisible)
		{
			jp.add(new JLabel("Max Y: "));
			jp.add(maxYField = new JTextField("" + maxY, 6));
		}
		editCB = new JCheckBox("Edit", false);
		editCB.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				waveEditable = editCB.isSelected();
				waveEditor.setEditable(editCB.isSelected());
			}
		});
		jp.add(editCB);
		final JButton updateB = new JButton("Update");
		updateB.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				updateLimits();
				waveEditor.setWaveform(waveX, waveY, minY, maxY);
			}
		});
		jp.add(updateB);
		add(jp, "North");
		// Add popup for copy/paste
		copyPastePopup = new JPopupMenu();
		copyI = new JMenuItem("Copy");
		copyI.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				savedMinX = minX;
				savedMinY = minY;
				savedMaxX = maxX;
				savedMaxY = maxY;
				savedWaveX = waveX;
				savedWaveY = waveY;
			}
		});
		copyPastePopup.add(copyI);
		pasteI = new JMenuItem("Paste");
		pasteI.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if (savedWaveX == null)
					return;
				if (minXVisible)
					minX = savedMinX;
				if (minYVisible)
					minY = savedMinY;
				if (maxXVisible)
					maxX = savedMaxX;
				if (maxYVisible)
					maxY = savedMaxY;
				try
				{
					waveX = new float[savedWaveX.length];
					waveY = new float[savedWaveY.length];
					for (int i = 0; i < savedWaveX.length; i++)
					{
						waveX[i] = savedWaveX[i];
						waveY[i] = savedWaveY[i];
					}
				}
				catch (final Exception exc)
				{}
				displayData(null, true);
			}
		});
		copyPastePopup.add(pasteI);
		pasteC = new JMenuItem("Paste from Clipboard");
		pasteC.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				int numPoint;
				boolean yxColumn = false;
				boolean setMinX = false, setMaxX = false, setMinY = false, setMaxY = false;
				try
				{
					final Clipboard clip = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
					final Transferable contents = clip.getContents(null);
					final DataFlavor[] flavors = contents.getTransferDataFlavors();
					String valType = "";
					for (final DataFlavor flavor : flavors)
					{
						try
						{
							// System.out.println( flavors[k].getHumanPresentableName());
							if (!flavor.getHumanPresentableName().equals("Unicode String"))
								continue;
							final BufferedReader in = new BufferedReader(flavor.getReaderForText(contents));
							String s = in.readLine();
							while (s != null) // && s.trim().length() != 0 )
							{
								if (s.trim().length() != 0)
								{
									final StringTokenizer st = new StringTokenizer(s);
									valType = st.nextToken();
									if (valType.trim().equalsIgnoreCase("xmax"))
									{
										savedMaxX = Float.parseFloat(st.nextToken());
										setMaxX = true;
									}
									else if (valType.trim().equalsIgnoreCase("xmin"))
									{
										savedMinX = Float.parseFloat(st.nextToken());
										setMinX = true;
									}
									else if (valType.trim().equalsIgnoreCase("ymax"))
									{
										savedMaxY = Float.parseFloat(st.nextToken());
										setMaxY = true;
									}
									else if (valType.trim().equalsIgnoreCase("ymin"))
									{
										savedMinY = Float.parseFloat(st.nextToken());
										setMinY = true;
									}
									else if (valType.trim().equalsIgnoreCase("x"))
									{
										int i = 0;
										numPoint = st.countTokens();
										savedWaveX = new float[numPoint];
										while (st.hasMoreTokens())
										{
											savedWaveX[i] = Float.parseFloat(st.nextToken());
											i++;
										}
									}
									else if (valType.trim().equalsIgnoreCase("y"))
									{
										int i = 0;
										numPoint = st.countTokens();
										savedWaveY = new float[numPoint];
										while (st.hasMoreTokens())
										{
											savedWaveY[i] = Float.parseFloat(st.nextToken());
											i++;
										}
									}
									else if (valType.trim().equalsIgnoreCase("xc")
											&& st.nextToken().equalsIgnoreCase("yc")
											|| (yxColumn = (valType.trim().equalsIgnoreCase("yc")
													&& st.nextToken().equalsIgnoreCase("xc"))))
									{
										int i = 0;
										final Vector<Float> vectX = new Vector<>();
										final Vector<Float> vectY = new Vector<>();
										float fv1, fv2;
										// numPoint = st.countTokens();
										// savedWaveY = new float[numPoint];
										while ((s = in.readLine()) != null)
										{
											final StringTokenizer st1 = new StringTokenizer(s);
											fv1 = Float.parseFloat(st1.nextToken());
											fv2 = Float.parseFloat(st1.nextToken());
											vectX.addElement(yxColumn ? new Float(fv2) : new Float(fv1));
											vectY.addElement(yxColumn ? new Float(fv1) : new Float(fv2));
										}
										savedWaveX = new float[vectX.size()];
										savedWaveY = new float[vectY.size()];
										for (final Float val : vectX)
											savedWaveX[i++] = val.floatValue();
										i = 0;
										for (final Float val : vectY)
											savedWaveY[i++] = val.floatValue();
									}
									else
									{
										JOptionPane.showMessageDialog(DeviceWave.this,
												"Clipboard contents syntax error\nSyntax : \nx nx1 nx2 nx3 ..... nxm\ny ny1 ny2 ny3 ..... nym",
												"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
										return;
									}
									s = in.readLine();
								}
							}
							break;
						}
						catch (final NumberFormatException ex)
						{
							JOptionPane.showMessageDialog(DeviceWave.this,
									"Clipboard contents syntax error on " + valType + " value",
									"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
							return;
						}
						catch (final IOException ex)
						{
							JOptionPane.showMessageDialog(DeviceWave.this, "Error on clipboard data reading",
									"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
							return;
						}
					}
				}
				catch (final UnsupportedFlavorException ex)
				{
					JOptionPane.showMessageDialog(DeviceWave.this, "Clipboard data type not supported",
							"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
					return;
				}
				/* Clip Waveform */
				try
				{
					numPoint = savedWaveX.length;
					if (savedWaveX.length != savedWaveY.length)
					{
						JOptionPane.showMessageDialog(DeviceWave.this, "x and y array must be equal in length",
								"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
						return;
					}
					for (int i = 1; i < numPoint; i++)
						if (savedWaveX[i] < savedWaveX[i - 1])
						{
							JOptionPane.showMessageDialog(DeviceWave.this, "X must be monotonic increasing array",
									"Invalid clipboard contents", JOptionPane.WARNING_MESSAGE);
							return;
						}
					if (minXVisible && setMinX)
						minX = savedMinX;
					if (minYVisible && setMinY)
						minY = savedMinY;
					if (maxXVisible && setMaxX)
						maxX = savedMaxX;
					if (maxYVisible && setMaxY)
						maxY = savedMaxY;
					final Vector<Float> vectX = new Vector<>();
					final Vector<Float> vectY = new Vector<>();
					final String warningMessage = "";
					float valX;
					float valY;
					boolean minXfound = false;
					boolean maxXfound = false;
					/*
					 *
					 * x -6 100 200 300 360 y 10 20 20 30 100
					 *
					 * x -5 100 200 300 350 y 10 20 20 30 100
					 *
					 * x -7 -5 100 200 300 350 370 y 100 10 20 20 30 100 200
					 *
					 * x -7 -6 100 200 300 360 370 y 100 10 20 20 30 100 200
					 *
					 *
					 * x -8 -7 -6 100 200 300 360 370 380 y 200 100 10 20 20 30 100 200 300
					 *
					 * x -8 -7 -5 100 200 300 350 370 380 y 200 100 10 20 20 30 100 200 300
					 *
					 *
					 */
					for (int i = 0; i < numPoint; i++)
					{
						valX = savedWaveX[i];
						valY = savedWaveY[i];
						if (valX < minX)
							continue;
						if (valX > maxX)
							break;
						if (valX >= minX || valX <= maxX)
						{
							if (valY < minY)
							{
								valY = minY;
							}
							else if (valY > maxY)
							{
								valY = maxY;
							}
						}
						else
							valY = 0;
						if (valX <= minX)
						{
							if (minXfound)
								continue;
							valX = minX;
							// valY = 0; first values should be not 0
							minXfound = true;
						}
						else
						{
							if (valX >= maxX)
							{
								if (maxXfound)
									break;
								valX = maxX;
								// valY = 0; first values should be not 0
								maxXfound = true;
							}
							else
							{
								if (!minXfound && (i == 0 || savedWaveX[i - 1] < minX))
								{
									vectX.addElement(new Float(minX));
									vectY.addElement(new Float(0));
									vectX.addElement(new Float(valX));
									vectY.addElement(new Float(0));
								}
								if (!maxXfound && (i == (numPoint - 1) || savedWaveX[i + 1] > maxX))
								{
									vectX.addElement(new Float(valX));
									vectY.addElement(new Float(valY));
									vectX.addElement(new Float(valX));
									vectY.addElement(new Float(0));
									valX = maxX;
									valY = 0;
								}
							}
						}
						vectX.addElement(new Float(valX));
						vectY.addElement(new Float(valY));
					}
					int i = 0;
					waveX = new float[vectX.size()];
					waveY = new float[vectY.size()];
					for (final Float val : vectX)
						waveX[i++] = val.floatValue();
					i = 0;
					for (final Float val : vectY)
						waveY[i++] = val.floatValue();
					if (warningMessage.length() > 0)
						JOptionPane.showMessageDialog(DeviceWave.this, warningMessage, "WARNING clipboard contents",
								JOptionPane.WARNING_MESSAGE);
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(DeviceWave.this, "Invalid content in clipboard",
							"Parsing clipboard data error", JOptionPane.WARNING_MESSAGE);
				}
				displayData(null, true);
			}
		});
		copyPastePopup.add(pasteC);
		pasteC.setEnabled(isClipboardText(Toolkit.getDefaultToolkit().getSystemClipboard()));
		Toolkit.getDefaultToolkit().getSystemClipboard().addFlavorListener(new FlavorListener()
		{
			@Override
			public void flavorsChanged(FlavorEvent e)
			{
				// System.out.println("ClipBoard UPDATED: " + e.getSource() + " " +
				// e.toString());
				final Clipboard clip = (Clipboard) (e.getSource());
				pasteC.setEnabled(isClipboardText(clip));
			}
		});
		copyC = new JMenuItem("Copy to Clipboard");
		copyC.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				String myString = "";
				if (minXVisible)
					myString = myString + "xmin\t" + minX + "\n";
				if (minYVisible)
					myString = myString + "ymin\t" + minY + "\n";
				if (maxXVisible)
					myString = myString + "xmax\t" + maxX + "\n";
				if (maxYVisible)
					myString = myString + "ymax\t" + maxY + "\n";
				myString = myString + "x";
				for (final float val : waveX)
					myString = myString + "\t" + val;
				myString = myString + "\ny";
				for (final float val : waveY)
					myString = myString + "\t" + val;
				// System.out.println(myString);
				final StringSelection stringSelection = new StringSelection(myString);
				final Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
				clipboard.setContents(stringSelection, null);
			}
		});
		copyPastePopup.add(copyC);
		copyPastePopup.pack();
		copyPastePopup.setInvoker(this);
		addMouseListener(new MouseAdapter()
		{
			@Override
			public void mousePressed(MouseEvent e)
			{
				if ((e.getModifiers() & Event.META_MASK) != 0)
				{
					if (savedWaveX == null)
						pasteI.setEnabled(false);
					else
						pasteI.setEnabled(true);
					copyPastePopup.setInvoker(DeviceWave.this);
					copyPastePopup.show(DeviceWave.this, e.getX(), e.getY());
				}
			}
		});
	}

	@Override
	protected void dataChanged(int offsetNid, Object data)
	{
		if (offsetNid != getOffsetNid())
			return;
		if (!(data instanceof Vector<?>))
		{
			System.err.println("Internal error: wrong data passed to DeviceWave.dataChanged");
			return;
		}
		final Vector<?> inVect = (Vector<?>) data;
		try
		{
			minX = ((Number) inVect.elementAt(0)).floatValue();
			maxX = ((Number) inVect.elementAt(1)).floatValue();
			minY = ((Number) inVect.elementAt(2)).floatValue();
			maxY = ((Number) inVect.elementAt(3)).floatValue();
			final float[] currX = (float[]) inVect.elementAt(4);
			final float[] currY = (float[]) inVect.elementAt(5);
			waveX = Arrays.copyOf(currX, currX.length);
			waveY = Arrays.copyOf(currY, currY.length);
		}
		catch (final Exception exc)
		{}
		displayData(null, true);
	}

	@Override
	protected void displayData(String data, boolean is_on)
	{
		waveEditor.setWaveform(waveX, waveY, minY, maxY);
		if (maxXVisible)
		{
			maxXField.setText("" + maxX);
		}
		if (minXVisible)
		{
			minXField.setText("" + minX);
		}
		if (maxYVisible)
		{
			maxYField.setText("" + maxY);
		}
		if (minYVisible)
		{
			minYField.setText("" + minY);
		}
		table.repaint();
	}

	@Override
	public void fireUpdate(String updateId, String newExpr)
	{
		if (updateIdentifier != null && updateExpression != null && updateIdentifier.equals(updateId))
		{
			// Substitute $ in expression with the new value
			final StringTokenizer st = new StringTokenizer(updateExpression, "$");
			String newExprStr = "";
			try
			{
				final String newVal = newExpr.toString();
				while (st.hasMoreTokens())
				{
					newExprStr += st.nextToken();
					if (st.hasMoreTokens())
						newExprStr += newVal;
				}
				// System.out.println(newExprStr);
				// Update first current id TDI variables
				master.updateIdentifiers();
				// Compute new Max
				final String newData = subtree.execute(newExprStr);
				maxY = subtree.getFloat(newData);
				// System.out.println(""+maxY);
				if (maxYVisible)
					maxYField.setText("" + maxY);
				waveEditor.setWaveform(waveX, waveY, minY, maxY);
			}
			catch (final Exception exc)
			{
				System.err.println("Error updating Max Y: " + exc);
			}
		}
	}

	protected String getArrayExpr(float[] vals)
	{
		String retExpr = "";
		for (int i = 0; i < vals.length; i++)
		{
			if (i < vals.length - 1)
				retExpr += vals[i] + ",";
			else
				retExpr += vals[i];
		}
		return "[" + retExpr + "]";
	}

	@Override
	protected String getData()
	{
//System.out.println("waveY length " + waveY.length);
		final String dims = getArrayExpr(waveX);
		final String values = getArrayExpr(waveY);
		return "BUILD_SIGNAL(" + values + ",," + dims + ")";
	}

	@Override
	protected Object getFullData()
	{
		final Vector<Object> res = new Vector<>();
		res.add(new Float(minX));
		res.add(new Float(maxX));
		res.add(new Float(minY));
		res.add(new Float(maxY));
		res.add(waveX);
		res.add(waveY);
		return res;
	}

	public boolean getMaxXVisible()
	{ return maxXVisible; }

	public boolean getMaxYVisible()
	{ return maxYVisible; }

	public boolean getMinXVisible()
	{ return minXVisible; }

	public boolean getMinYVisible()
	{ return minYVisible; }

	public int getPrefHeight()
	{ return prefHeight; }

	@Override
	protected boolean getState()
	{ return true; }

	public String getUpdateExpression()
	{ return updateExpression; }

	public boolean getWaveEditable()
	{ return waveEditable; }

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		create();
		initializing = true;
		// Read X and Y extremes
		int currNid;
		String currData;
		float[] currX, currY;
		// Min X
		try
		{
			currNid = nidData + 1;
			currData = subtree.execute(subtree.getDataExpr(currNid));
			minX = minXOld = subtree.getFloat(currData);
		}
		catch (final Exception exc)
		{
			minX = minXOld = 0;
		}
		// Max X
		try
		{
			currNid = nidData + 2;
			currData = subtree.execute(subtree.getDataExpr(currNid));
			maxX = maxXOld = subtree.getFloat(currData);
		}
		catch (final Exception exc)
		{
			maxX = maxXOld = 1;
		}
		// Min Y
		try
		{
			currNid = nidData + 3;
			currData = subtree.execute(subtree.getDataExpr(currNid));
			minY = minYOld = subtree.getFloat(currData);
		}
		catch (final Exception exc)
		{
			minY = minYOld = 0;
		}
		// Max Y
		try
		{
			currNid = nidData + 4;
			currData = subtree.execute(subtree.getDataExpr(currNid));
			maxY = maxYOld = subtree.getFloat(currData);
		}
		catch (final Exception exc)
		{
			maxY = maxYOld = 1;
		}
		// Prepare waveX and waveY
		try
		{
			// yData = subtree.evaluateData(subtree.dataFromExpr("FLOAT(" +
			// subtree.dataToString(data) + ")"), 0);
			// xData = subtree.evaluateData(subtree.dataFromExpr("FLOAT(DIM_OF(" +
			// subtree.dataToString(data) + "))"), 0);
			currNid = nidData;
			currData = subtree.getDataExpr(currNid);
			currY = subtree.getFloatArray("VALUE_OF(" + currData + ")");
			currX = subtree.getFloatArray("DIM_OF(" + currData + ")");
		}
		catch (final Exception exc)
		{
			currX = new float[]
			{ minX, maxX };
			currY = new float[]
			{ 0, 0 };
		}
		// Check that the stored signal lies into valid X range
		if (currX[0] <= minX - MIN_STEP || currX[currX.length - 1] >= maxX + MIN_STEP)
		{
			currX = new float[]
			{ minX, maxX };
			currY = new float[]
			{ 0, 0 };
			JOptionPane.showMessageDialog(DeviceWave.this,
					"The stored signal lies outside the valid X range. Hit apply to override the incorrect values.",
					"Incorret waveform limits", JOptionPane.WARNING_MESSAGE);
		}
		// set extreme points, if not present
		int nPoints = currX.length;
		if (Math.abs(currX[0] - minX) > MIN_STEP)
			nPoints++;
		else
			currX[0] = minX;
		if (Math.abs(currX[currX.length - 1] - maxX) > MIN_STEP)
			nPoints++;
		else
			currX[currX.length - 1] = maxX;
		waveX = new float[nPoints];
		waveY = new float[nPoints];
		int currIdx = 0;
		if (Math.abs(currX[0] - minX) > MIN_STEP)
		{
			waveX[0] = minX;
			waveY[0] = 0;
			currIdx++;
		}
		for (int i = 0; i < currX.length; i++)
		{
			waveX[currIdx] = currX[i];
			waveY[currIdx++] = currY[i];
		}
		if (Math.abs(currX[currX.length - 1] - maxX) > MIN_STEP)
		{
			waveX[currIdx] = maxX;
			waveY[currIdx] = 0;
		}
		waveXOld = new float[waveX.length];
		waveYOld = new float[waveX.length];
		for (int i = 0; i < waveX.length; i++)
		{
			waveXOld[i] = waveX[i];
			waveYOld[i] = waveY[i];
		}
		// updateLimits();
		displayData(data, is_on);
		initializing = false;
	}

	private boolean isClipboardText(Clipboard clip)
	{
		final Transferable contents = clip.getContents(null);
		final DataFlavor[] flavors = contents.getTransferDataFlavors();
		// System.out.println("isClipboardText: " + flavors);
		boolean isText = false;
		for (final DataFlavor flavor : flavors)
		{
			// System.out.println( flavors[k].getHumanPresentableName());
			if (!flavor.getHumanPresentableName().equals("Unicode String"))
				continue;
			isText = true;
			break;
		}
		return isText;
	}

	@Override
	public void reset()
	{
		minX = minXOld;
		maxX = maxXOld;
		minY = minYOld;
		maxY = maxYOld;
		waveX = new float[waveXOld.length];
		waveY = new float[waveXOld.length];
		for (int i = 0; i < waveXOld.length; i++)
		{
			waveX[i] = waveXOld[i];
			waveY[i] = waveYOld[i];
		}
		super.reset();
	}

	@Override
	public void setEnabled(boolean state)
	{}

	@Override
	public void setHighlight(boolean highlighted)
	{
		if (highlighted)
		{
			Waveform.SetColors(new Color[]
			{ Color.red }, new String[]
			{ "Red" });
		}
		else
		{
			Waveform.SetColors(new Color[]
			{ Color.black }, new String[]
			{ "Black" });
		}
		super.setHighlight(highlighted);
	}

	public void setMaxXVisible(boolean visible)
	{ maxXVisible = visible; }

	public void setMaxYVisible(boolean visible)
	{ maxYVisible = visible; }

	public void setMinXVisible(boolean visible)
	{ minXVisible = visible; }

	public void setMinYVisible(boolean visible)
	{ minYVisible = visible; }

	public void setPrefHeight(int prefHeight)
	{ this.prefHeight = prefHeight; }

	public void setUpdateExpression(String updateExpression)
	{ this.updateExpression = updateExpression; }

	public void setWaveEditable(boolean editable)
	{ waveEditable = editable; }

    	public void updateXLimit(float minX, float maxX)
    	{
        
        if( this.maxX == maxX && this.minX == minX )
            return;
        
        if( waveX[0] > minX )
            waveX[0] = minX;

        if( waveX[waveX.length - 1] < maxX )
            waveX[waveX.length - 1] = maxX;
        
        
        if(waveX[0] < minX - (float)MIN_STEP || waveX[waveX.length - 1] > maxX + (float)MIN_STEP)
        {
            int minIndex=0;
            int maxIndex=waveX.length;
            
            for( int i = 0; i < waveX.length; i++ )
            {
                if( waveX[i] <= minX - (float)MIN_STEP )
                {
                    minIndex=i+1;
                    continue;                           
                }
                if( waveX[i] >= maxX + (float)MIN_STEP )
                {
                    maxIndex=i;
                    break;
                }
            }
            int newLen = maxIndex - minIndex;
            int extremePoint = 0, stIdx = 0; 
            if(minIndex > 0)
            {
                extremePoint++;
                stIdx=1;
            }
            if(maxIndex < waveX.length)
                extremePoint++;
            
            float newX[] = new float[newLen+extremePoint];
            float newY[] = new float[newLen+extremePoint];

            System.arraycopy(waveX, minIndex, newX, stIdx, newLen);
            System.arraycopy(waveY, minIndex, newY, stIdx, newLen);
            
            if(minIndex > 0)
            {
                newX[0] = minX;
                newY[0] = 0;
            }
            
            if(maxIndex < waveX.length)
            {
                newX[newX.length-1] = maxX;
                newY[newX.length-1] = 0;
            }
            waveX = newX;
            waveY = newY;
            
        }
        this.minX = minX;
        this.maxX = maxX;            
        if(maxXVisible) {maxXField.setText(""+maxX);}
        if(minXVisible) {minXField.setText(""+minX);}
        waveEditor.setWaveform(waveX, waveY, minY, maxY);
        table.repaint();
        waveEditor.repaint();
        repaint();
    	}

	protected void updateLimits()
	{
		if (minXVisible)
		{
			while (true)
			{
				try
				{
					minX = (new Float(minXField.getText()).floatValue());
					break;
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(DeviceWave.this, "Invalid value for min X", "Incorret limit",
							JOptionPane.WARNING_MESSAGE);
				}
			}
		}
		if (maxXVisible)
		{
			while (true)
			{
				try
				{
					maxX = (new Float(maxXField.getText()).floatValue());
					break;
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(DeviceWave.this, "Invalid value for max X", "Incorret limit",
							JOptionPane.WARNING_MESSAGE);
				}
			}
		}
		if (minYVisible)
		{
			while (true)
			{
				try
				{
					minY = (new Float(minYField.getText()).floatValue());
					break;
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(DeviceWave.this, "Invalid value for min Y", "Incorret limit",
							JOptionPane.WARNING_MESSAGE);
				}
			}
		}
		if (maxYVisible)
		{
			while (true)
			{
				try
				{
					maxY = (new Float(maxYField.getText()).floatValue());
					break;
				}
				catch (final Exception exc)
				{
					JOptionPane.showMessageDialog(DeviceWave.this, "Invalid value for max Y", "Incorret limit",
							JOptionPane.WARNING_MESSAGE);
				}
			}
			for (int i = 0; i < waveX.length; i++)
			{
				if (waveY[i] > maxY)
					waveY[i] = maxY;
			}
			repaint();
		}
	}
}
