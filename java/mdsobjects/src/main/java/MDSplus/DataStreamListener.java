/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package MDSplus;

/**
 *
 * @author manduchi
 */
public interface DataStreamListener
{
	void dataReceived(java.lang.String streamName, int shot, Data times, Data samples);
}
