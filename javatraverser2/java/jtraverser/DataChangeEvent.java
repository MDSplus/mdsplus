package jtraverser;

import java.awt.Event;

@SuppressWarnings("serial")
public class DataChangeEvent extends Event{
    public DataChangeEvent(final Object target, final int id, final Object arg){
	super(target, id, arg);
    }
}