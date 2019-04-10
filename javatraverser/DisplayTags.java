//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DisplayTags
    extends NodeEditor
    implements ActionListener
{
  JPanel tagsPanel;
  JLabel tagsLabel;
  public DisplayTags()
  {
    setLayout(new BorderLayout());
    tagsPanel = new JPanel();
    tagsPanel.add(tagsLabel = new JLabel());
    add(tagsPanel, "North");
    JPanel jp1 = new JPanel();
    JButton cancel = new JButton("Cancel");
    jp1.add(cancel);
    add(jp1, "South");
    cancel.addActionListener(this);
  }

  public void actionPerformed(ActionEvent e)
  {
    frame.dispose();
  }

  public void setNode(Node _node)
  {
    String tags[] = new String[0];
    node = _node;
    frame.setTitle("Display Node Tags");
    try
    {
      tags = node.getTags();
    }
    catch (Exception e)
    {
      System.out.println("Error retieving Tags");
      return;
    }
    String tagNames = "";
    if (tags == null || tags.length == 0)
      tagNames = "No Tags";
    else
    {
      for (int i = 0; i < tags.length; i++)
      {
	tagNames += tags[i];
	if (i < tags.length - 1)
	  tagNames += ", ";
      }
    }
    tagsLabel.setText(tagNames);
  }

}