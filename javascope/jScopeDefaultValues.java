class jScopeDefaultValues {
   int	  shots[];
   String xmin, xmax, ymax, ymin;
   String title_str, xlabel, ylabel;
   String experiment_str, shot_str;
   String upd_event_str, def_node_str;
   boolean is_evaluated = false;
   
   
   public void Reset()
   {
        shots = null;
        xmin = xmax = ymax = ymin = null;
        title_str = xlabel = ylabel = null;
        experiment_str = shot_str = null;
        upd_event_str = def_node_str = null;
        is_evaluated = false;
   }
}