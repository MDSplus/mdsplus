package mds.provider.jet;

public class Base64
{
	static byte[] Base64Alphabet = new String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=")
			.getBytes();
	byte decodingTable[] = new byte[256];

	public Base64()
	{
		int i;
		for (i = 0; i < 256; i++)
			decodingTable[i] = (byte) 0xFF;
		for (i = 0; i < 64; i++)
			decodingTable[Base64Alphabet[i]] = (byte) i;
		decodingTable['='] = (byte) 0xFE;
	}

	public String encode(String in)
	{
		return encode(in.getBytes());
	}

	String encode(byte[] in)
	{
		final StringBuffer out_s = new StringBuffer();
		int size = in.length, idx = 0;
		int word = 0;
		int code;
		while (size > 0)
		{
			word = in[idx];
			code = word >> 2;
			word &= 0x3;
			out_s.append((char) Base64Alphabet[code]);
			if (size > 1)
			{
				word <<= 8;
				word |= in[idx + 1];
				code = word >> 4;
				word &= 0xF;
				out_s.append((char) Base64Alphabet[code]);
				if (size > 2)
				{
					word <<= 8;
					word |= in[idx + 2];
					code = word;
					word &= 0x3F;
					code >>= 6;
					out_s.append((char) Base64Alphabet[code]);
					out_s.append((char) Base64Alphabet[word]);
				}
				else
				{
					word <<= 2;
					out_s.append((char) Base64Alphabet[word]);
					out_s.append("=");
				}
			}
			else
			{
				out_s.append((char) Base64Alphabet[word << 4]);
				out_s.append("==");
			}
			size -= 3;
			idx += 3;
		}
		return new String(out_s);
	}

	byte[] decode(byte[] in)
	{
		int size = in.length;
		final StringBuffer out_s = new StringBuffer();
		int idx = 0, word = 0;
		int code;
		byte key;
		byte c;
		while (size >= 4)
		{
			c = in[0];
			if (c == '=')
				return new String(out_s).getBytes();
			key = decodingTable[c];
			if (key == 0xFF)
				return null;
			word = key;
			c = in[idx + 1];
			if (c == '=')
			{
				word <<= 2;
				out_s.append(word);
				return new String(out_s).getBytes();
			}
			key = decodingTable[c];
			if (key == 0xFF)
				return null;
			word <<= 6;
			word |= key;
			code = word >> 4;
			word &= 0xF;
			out_s.append(code);
			c = in[idx + 2];
			if (c == '=')
			{
				word <<= 4;
				out_s.append(word);
				return new String(out_s).getBytes();
			}
			key = decodingTable[c];
			if (key == 0xFF)
				return null;
			word <<= 6;
			word |= key;
			code = word >> 2;
			word &= 0x3;
			out_s.append(code);
			c = in[idx + 3];
			if (c == '=')
			{
				return new String(out_s).getBytes();
			}
			key = decodingTable[c];
			if (key == 0xFF)
				return null;
			word <<= 6;
			word |= key;
			out_s.append(word);
			size -= 4;
			idx += 4;
		}
		return new String(out_s).getBytes();
	}
}
