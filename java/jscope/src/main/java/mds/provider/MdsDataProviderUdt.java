/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package mds.provider;

import mds.connection.MdsConnection;
import mds.provider.mds.MdsConnectionUdt;

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
