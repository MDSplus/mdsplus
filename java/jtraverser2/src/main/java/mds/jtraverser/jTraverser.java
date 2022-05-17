package mds.jtraverser;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class jTraverser
{
	static Pattern pattern = Pattern.compile("(.*)[\\\\/]([^\\\\/]+)_((model|[0-9]+))\\.tree");

	@SuppressWarnings("unused")
	public static void main(final String args[])
	{
		if (args.length >= 5)
			new jTraverserFacade(args[0], args[1], args[2], args[3], args[4]);
		else if (args.length == 4)
			new jTraverserFacade(args[0], args[1], args[2], args[3], null);
		else if (args.length == 3)
			new jTraverserFacade(args[0], args[1], args[2], null, null);
		else if (args.length == 2)
			new jTraverserFacade(args[0], args[1], null, null, null);
		else if (args.length == 1)
		{
			final Matcher matcher = jTraverser.pattern.matcher(args[0]);
			if (matcher.find())
				new jTraverserFacade(null, matcher.group(2), matcher.group(3), null, matcher.group(1));
			else
				new jTraverserFacade(args[0], null, null, null, null);
		}
		else
			new jTraverserFacade(null, null, null, null, null);
	}
}
