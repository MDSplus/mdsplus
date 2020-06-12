package mds.jscope;

/* $Id$ */
class jScopeDefaultValues
{
	long shots[];
	String xmin, xmax, ymax, ymin;
	String title_str, xlabel, ylabel;
	String experiment_str, shot_str;
	String upd_event_str, def_node_str;
	private boolean is_evaluated = false;
	private String public_variables = null;
	boolean upd_limits = true;

	public boolean getIsEvaluated()
	{ return is_evaluated || public_variables == null || public_variables.length() == 0; }

	public String getPublicVariables()
	{ return public_variables; }

	public boolean isSet()
	{ return (public_variables != null && public_variables.length() > 0); }

	public void Reset()
	{
		shots = null;
		xmin = xmax = ymax = ymin = null;
		title_str = xlabel = ylabel = null;
		experiment_str = shot_str = null;
		upd_event_str = def_node_str = null;
		is_evaluated = false;
		upd_limits = true;
	}

	public void setIsEvaluated(boolean evaluated)
	{ is_evaluated = evaluated; }

	public void setPublicVariables(String public_variables)
	{
		if (this.public_variables == null || public_variables == null
				|| !this.public_variables.equals(public_variables))
		{
			is_evaluated = false;
			this.public_variables = public_variables.trim();
		}
	}
}
