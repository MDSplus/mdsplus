
import java.awt.*;
import javax.swing.*;
import javax.swing.text.*;
import java.awt.event.*;



/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

public class ServerShowDialog extends JDialog {
  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jPanel1 = new JPanel();
  JToggleButton jToggleButton1 = new JToggleButton();
  JScrollPane jScrollPane1 = new JScrollPane();
  JTextPane jTextPane1 = new JTextPane();
  public ServerShowDialog(Frame owner, String title) throws HeadlessException {
    super(owner, title);
    try {
      jbInit();
    }
    catch(Exception e) {
      e.printStackTrace();
    }
  }
  private void jbInit() throws Exception
  {
    this.setModal(false);
    this.getContentPane().setLayout(borderLayout1);
    jToggleButton1.setText("Cancel");
    jToggleButton1.addActionListener(new ServerShowDialog_jToggleButton1_actionAdapter(this));
    this.getContentPane().add(jPanel1, BorderLayout.SOUTH);
    jPanel1.add(jToggleButton1, null);
    this.getContentPane().add(jScrollPane1, BorderLayout.CENTER);
    jScrollPane1.getViewport().add(jTextPane1, null);
  }

  public void addString(String serverClass, String Address, boolean active)
  {
    // Start with the current input attributes for the JTextPane. This
// should ensure that we do not wipe out any existing attributes
// (such as alignment or other paragraph attributes) currently
// set on the text area.
    MutableAttributeSet attrs = jTextPane1.getInputAttributes();

// Set the font family, size, and style, based on properties of
// the Font object. Note that JTextPane supports a number of
// character attributes beyond those supported by the Font class.
// For example, underline, strike-through, super- and sub-script.
/*
    StyleConstants.setFontFamily(attrs, font.getFamily());
    StyleConstants.setFontSize(attrs, font.getSize());
    StyleConstants.setItalic(attrs, (font.getStyle() & Font.ITALIC) != 0);
    StyleConstants.setBold(attrs, (font.getStyle() & Font.BOLD) != 0);
*/
// Set the font color
    if(active)
      StyleConstants.setForeground(attrs, Color.black);
    else
      StyleConstants.setForeground(attrs, Color.red);

// Retrieve the pane's document object
    StyledDocument doc = jTextPane1.getStyledDocument();

// Replace the style for the entire document. We exceed the length
// of the document by 1 so that text entered at the end of the
// document uses the attributes.
//    doc.setCharacterAttributes(0, doc.getLength() + 1, attrs, false);
    try {
      doc.insertString(doc.getLength(), serverClass + "\t\t" + Address + "\n", attrs );
    } catch(Exception exc){}
  }

  void jToggleButton1_actionPerformed(ActionEvent e) {
    this.dispose();
  }
}

class ServerShowDialog_jToggleButton1_actionAdapter implements java.awt.event.ActionListener {
  ServerShowDialog adaptee;

  ServerShowDialog_jToggleButton1_actionAdapter(ServerShowDialog adaptee) {
    this.adaptee = adaptee;
  }
  public void actionPerformed(ActionEvent e) {
    adaptee.jToggleButton1_actionPerformed(e);
  }
}
