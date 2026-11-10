package mds.provider;

import static org.junit.Assert.assertFalse;
import static org.mockito.Mockito.inOrder;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoInteractions;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InOrder;

import mds.connection.MdsConnection;

/**
 * Tests MdsDataProvider.close()'s JavaClose(...) logic (moved here from
 * finalize()) without a live mdsip server. MdsDataProvider's no-arg
 * constructor does no I/O of its own - it just calls the protected
 * getConnection() factory method - so a mock MdsConnection can be injected
 * by overriding that method in an anonymous subclass, no reflection needed.
 */
public class MdsDataProviderMockTest
{
	private MdsConnection mockMds;
	private MdsDataProvider provider;

	@Before
	public void setUp()
	{
		mockMds = mock(MdsConnection.class);
		provider = new MdsDataProvider()
		{
			@Override
			protected MdsConnection getConnection()
			{ return mockMds; }
		};
	}

	@Test
	public void testCloseSendsJavaCloseWhenOpen()
	{
		provider.open = true;
		provider.experiment = "test";
		provider.shot = 1;

		provider.close();

		verify(mockMds).MdsValue("JavaClose(\"test\",1)");
		assertFalse("open should be cleared after close()", provider.open);
	}

	@Test
	public void testCloseDoesNotSendJavaCloseWhenNotOpen()
	{
		provider.open = false;

		provider.close();

		// No JavaClose(...) - and, since connected/is_tunneling are also
		// false by default, no interaction with mds at all.
		verifyNoInteractions(mockMds);
	}

	@Test
	public void testCloseSendsJavaCloseBeforeDisconnectingWhenBothOpenAndConnected()
	{
		// This is the ordering finalize() used to guarantee: close the
		// experiment on the server while the connection is still live,
		// *before* tearing the connection down.
		provider.open = true;
		provider.experiment = "test";
		provider.shot = 1;
		provider.connected = true;

		provider.close();

		final InOrder inOrder = inOrder(mockMds);
		inOrder.verify(mockMds).MdsValue("JavaClose(\"test\",1)");
		inOrder.verify(mockMds).DisconnectFromMds();
	}
}
