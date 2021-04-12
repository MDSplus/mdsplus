package mds.jtraverser;

import java.awt.Component;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;

import mds.data.TREE;
import mds.jtraverser.TreeManager.ExtrasMenu;
import mds.jtraverser.TreeManager.Menu;
import mds.mdsip.MdsIp.Provider;

public final class jTraverserFacade extends JFrame
{
	private class MenuChecker implements MenuListener
	{
		private final JMenu jmenu;
		private final Menu menu;

		public MenuChecker(final JMenu jmenu, final Menu menu)
		{
			this.jmenu = jmenu;
			this.menu = menu;
		}

		@Override
		public void menuCanceled(final MenuEvent e)
		{/* stub */}

		@Override
		public void menuDeselected(final MenuEvent e)
		{/* stub */}

		@Override
		public void menuSelected(final MenuEvent e)
		{
			if (this.jmenu.isEnabled())
				this.menu.checkSupport();
		}
	}

	private static final long serialVersionUID = 1L;
	private static final String TitleNoTree = "jTraverser - no tree open";
	private final List<JMenuItem> jmenus = new ArrayList<>(5);
	private final TreeManager treeman;
	private ExtrasMenu extras;

	public jTraverserFacade(final String server, final String exp_name, final String shot_name, String access,
			final String path)
	{
		JOptionPane.setRootFrame(this);
		this.setTitle("jTraverser - no tree open");
		TREE.MODE mode = TREE.READONLY;
		if (access != null && !access.isEmpty())
		{
			access = access.toLowerCase();
			if ("-normal".startsWith(access))
				mode = TREE.NORMAL;
			else if ("-edit".startsWith(access))
				mode = TREE.EDITABLE;
		}
		this.getContentPane().add(this.treeman = new TreeManager(this));
		final JMenuBar menu_bar = new JMenuBar();
		this.setJMenuBar(menu_bar);
		JMenu jmenu;
		menu_bar.add(jmenu = new JMenu("File"));
		jmenu.addMenuListener(new MenuChecker(jmenu, new TreeManager.FileMenu(this.treeman, jmenu, -1)));
		this.jmenus.add(jmenu);
		menu_bar.add(jmenu = new JMenu("Display"));
		jmenu.addMenuListener(new MenuChecker(jmenu, new TreeManager.DisplayMenu(this.treeman, jmenu, -1)));
		this.jmenus.add(jmenu);
		jmenu.setEnabled(false);
		menu_bar.add(jmenu = new JMenu("Modify"));
		jmenu.addMenuListener(new MenuChecker(jmenu, new TreeManager.ModifyMenu(this.treeman, jmenu, -1)));
		this.jmenus.add(jmenu);
		jmenu.setEnabled(false);
		menu_bar.add(jmenu = new JMenu("Edit"));
		jmenu.addMenuListener(new MenuChecker(jmenu, new TreeManager.EditMenu(this.treeman, jmenu, -1)));
		this.jmenus.add(jmenu);
		jmenu.setEnabled(false);
		menu_bar.add(jmenu = new JMenu("Extras"));
		jmenu.addMenuListener(new MenuChecker(jmenu, this.extras = new TreeManager.ExtrasMenu(this.treeman, jmenu)));
		this.jmenus.add(jmenu);
		jmenu.setEnabled(true);
		this.addWindowListener(new WindowAdapter()
		{
			@Override
			public void windowClosing(final WindowEvent we)
			{
				try
				{
					jTraverserFacade.this.treeman.quit();
				}
				catch (final Exception e)
				{
					e.printStackTrace();
					System.exit(1);
				}
			}
		});
		this.pack();
		this.setVisible(true);
		try
		{
			final MdsView mdsview;
			if (server == null || server.isEmpty() && this.treeman.getCurrentMdsView() != null)
				mdsview = this.treeman.getCurrentMdsView();
			else
				mdsview = this.treeman.openMds(new Provider(server));
			if (exp_name != null && mdsview != null)
			{
				if (path != null)
					this.treeman.setTreePathEnv(exp_name, path);
				mdsview.openTree(exp_name.toUpperCase(),
						(shot_name == null || shot_name.equalsIgnoreCase("model")) ? -1 : Integer.parseInt(shot_name),
						mode);
			}
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
	}

	@Override
	public Component add(final Component component)
	{
		return this.getContentPane().add(component);
	}

	void reportChange(final MdsView mdsview)
	{
		if (mdsview != null)
		{
			final TreeView treeview = mdsview.getCurrentTreeView();
			if (treeview != null)
			{
				this.setTitle(new StringBuilder(256).append("jTraverser - ").append(treeview).append(" on ")
						.append(mdsview).toString());
				this.jmenus.get(1).setEnabled(true);
				this.jmenus.get(2).setEnabled(!treeview.isReadOnly());
				this.jmenus.get(3).setEnabled(treeview.isEditable());
				this.jmenus.get(4).setEnabled(true);
				return;
			}
		}
		this.setTitle(jTraverserFacade.TitleNoTree);
		for (final JMenuItem jm : this.jmenus.subList(1, 4))
			jm.setEnabled(false);
		this.extras.checkSupport();
	}
}
