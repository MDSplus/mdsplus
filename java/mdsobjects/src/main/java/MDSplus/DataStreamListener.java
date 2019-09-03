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
    void dataReceived(Data samples, Data times);
}
