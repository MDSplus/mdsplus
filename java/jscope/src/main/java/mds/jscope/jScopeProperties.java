package mds.jscope;

/* $Id$ */
import java.io.*;
import java.util.Properties;

public class jScopeProperties extends Properties
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	private static final String keyValueSeparators = "=: \t\r\n\f";
	private static final String strictKeyValueSeparators = "=:";
	// private static final String specialSaveChars = "=: \t\r\n\f#!";
	private static final String whiteSpaceChars = " \t\r\n\f";

	/*
	 * Returns true if the given line is a line that must be appended to the next
	 * line
	 */
	/*
	 * private boolean continueLine (String line) { int slashCount = 0; int index =
	 * line.length() - 1; while((index >= 0) && (line.charAt(index--) == '\\'))
	 * slashCount++; return (slashCount % 2 == 1); }
	 */
	@Override
	public String getProperty(String key)
	{
		String p = super.getProperty(key);
		if (p == null)
			return null;
		p = p.trim();
		if (p.length() == 0)
			return null;
		return p;
	}

	@Override
	public synchronized void load(InputStream inStream) throws IOException
	{
		final BufferedReader in = new BufferedReader(new InputStreamReader(inStream, "8859_1"));
		while (true)
		{
			// Get next line
			final String line = in.readLine();
			if (line == null)
				return;
			if (line.length() > 0)
			{
				// Continue lines that end in slashes if they are not comments
				final char firstChar = line.charAt(0);
				if ((firstChar != '#') && (firstChar != '!'))
				{
					/*
					 * while (continueLine(line)) { String nextLine = in.readLine(); if(nextLine ==
					 * null) nextLine = new String(""); String loppedLine = line.substring(0,
					 * line.length()-1); // Advance beyond whitespace on new line int startIndex=0;
					 * for(startIndex=0; startIndex<nextLine.length(); startIndex++) if
					 * (whiteSpaceChars.indexOf(nextLine.charAt(startIndex)) == -1) break; nextLine
					 * = nextLine.substring(startIndex,nextLine.length()); line = new
					 * String(loppedLine+nextLine); }
					 */
					// Find start of key
					final int len = line.length();
					int keyStart;
					for (keyStart = 0; keyStart < len; keyStart++)
					{
						if (whiteSpaceChars.indexOf(line.charAt(keyStart)) == -1)
							break;
					}
					// Find separation between key and value
					int separatorIndex;
					for (separatorIndex = keyStart; separatorIndex < len; separatorIndex++)
					{
						final char currentChar = line.charAt(separatorIndex);
						if (currentChar == '\\')
							separatorIndex++;
						else if (keyValueSeparators.indexOf(currentChar) != -1)
							break;
					}
					// Skip over whitespace after key if any
					int valueIndex;
					for (valueIndex = separatorIndex; valueIndex < len; valueIndex++)
						if (whiteSpaceChars.indexOf(line.charAt(valueIndex)) == -1)
							break;
					// Skip over one non whitespace key value separators if any
					if (valueIndex < len)
						if (strictKeyValueSeparators.indexOf(line.charAt(valueIndex)) != -1)
							valueIndex++;
					// Skip over white space after other separators if any
					while (valueIndex < len)
					{
						if (whiteSpaceChars.indexOf(line.charAt(valueIndex)) == -1)
							break;
						valueIndex++;
					}
					final String key = line.substring(keyStart, separatorIndex);
					final String value = (separatorIndex < len) ? line.substring(valueIndex, len) : "";
					// Convert then store key and value
					// key = loadConvert(key);
					// value = loadConvert(value);
					put(key, value);
				}
			}
		}
	}
}
