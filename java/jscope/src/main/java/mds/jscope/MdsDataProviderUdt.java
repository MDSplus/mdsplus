/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package mds.jscope;

import mds.connection.MdsConnection;

/**
 *
 * @author manduchi
 */
public class MdsDataProviderUdt extends MdsDataProvider
{
     protected MdsConnection getConnection()
    {
	return new MdsConnectionUdt();
    }

}
