/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jScope;

public class FakeTWUProperties extends TWUProperties {
  public FakeTWUProperties (int len) {
    super (null) ;
    signalProps.put ("TWU.properties.version", "0.7");
    signalProps.put ("Dimensions", "1");
    signalProps.put ("Length.total",       String.valueOf(len) );
    signalProps.put ("Length.dimension.0", String.valueOf(len) );
    signalProps.put ("Equidistant", "Incrementing");
    signalProps.put ("Signal.Minimum", "0.0");

    signalProps.put ("Signal.Maximum", String.valueOf( (double)(len-1) ) );
  }
    public boolean valid() {return true;}
}

