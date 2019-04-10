import java.beans.*;

public class FloatArrayEditor extends PropertyEditorSupport
{
    public String getJavaInitializationString()
    {
	float [] array = (float [])getValue();
	if(array == null) return "null";
	StringBuffer sb = new StringBuffer("new float[] {");
	for(int i = 0; i < array.length; i++)
	{
	    sb.append("(float)" + (new Float(array[i])).toString());
	    if(i < (array.length - 1))
	        sb.append(",");
	}
	sb.append("}");
	return sb.toString();
    }
}
