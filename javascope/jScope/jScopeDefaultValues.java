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
package jScope;

/* $Id$ */
class jScopeDefaultValues 
{
   long	  shots[];
   String xmin, xmax, ymax, ymin;
   String title_str, xlabel, ylabel;
   String experiment_str, shot_str;
   String upd_event_str, def_node_str;
   private boolean is_evaluated = false;
   private String public_variables = null;
   boolean upd_limits = true;

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
   
   public boolean getIsEvaluated()
   {
       return is_evaluated || public_variables == null || public_variables.length() == 0 ;
   }
   
   public void setIsEvaluated(boolean evaluated)
   {
       is_evaluated = evaluated;
   }
   
   public void setPublicVariables(String public_variables)
   {
       if( this.public_variables == null || public_variables == null || ! this.public_variables.equals(public_variables) )
       {
           is_evaluated = false;
           this.public_variables = public_variables.trim(); 
       }
   }
   
   public String getPublicVariables()
   {
       return public_variables;
   }
   
   public boolean isSet()
   {
       return ( public_variables != null && public_variables.length() > 0 );
   }
   
}
