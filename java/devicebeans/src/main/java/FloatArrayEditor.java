import java.beans.*;

public class FloatArrayEditor extends PropertyEditorSupport
{
	@Override
	public String getJavaInitializationString()
	{
		final float[] array = (float[]) getValue();
		if (array == null)
			return "null";
		final StringBuffer sb = new StringBuffer("new float[] {");
		for (int i = 0; i < array.length; i++)
		{
			sb.append("(float)" + (new Float(array[i])).toString());
			if (i < (array.length - 1))
				sb.append(",");
		}
		sb.append("}");
		return sb.toString();
	}
}
