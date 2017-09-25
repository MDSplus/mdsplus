/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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