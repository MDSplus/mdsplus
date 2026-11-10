package mds.jdispatcher;

import static org.junit.Assert.assertFalse;
import static org.mockito.Mockito.atLeastOnce;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;

import java.lang.reflect.Field;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Hashtable;
import java.util.Vector;

import org.junit.Before;
import org.junit.Test;
import org.objenesis.ObjenesisStd;

import mds.connection.MdsConnection;

/**
 * Tests MdsServer.close() without a live dispatcher/mdsip server.
 *
 * Unlike MdsConnection (which has an I/O-free String constructor that
 * mds.connection.MockMdsConnectionWithHangPMET relies on), MdsServer's only
 * constructor unconditionally connects and opens a real receive socket, so
 * there is no safe way to construct a real instance here. Instead the
 * instance is created via Objenesis (bypassing every constructor, the same
 * mechanism Mockito itself uses under the hood to mock concrete classes),
 * and the handful of `final` collection fields that close()/shutdown()
 * touch - which a real constructor would have initialized - are set
 * directly via reflection so nothing NPEs.
 */
public class MdsServerMockTest
{
	private MdsServer server;
	private ServerSocket mockRcvSock;
	private Socket mockReadSock;
	private Socket mockListenSock;
	private Thread mockReceiveThread;

	@Before
	public void setUp() throws Exception
	{
		server = new ObjenesisStd().newInstance(MdsServer.class);

		// MdsConnection-level collections that close()/QuitFromMds() touch.
		setField(MdsConnection.class, server, "connection_listener", new Vector<>());
		setField(MdsConnection.class, server, "hashEventName", new Hashtable<>());
		setField(MdsConnection.class, server, "hashEventId", new Hashtable<>());

		// A mock receive thread, so we can prove super.close() (which is the
		// only thing that touches receiveThread) actually ran - unlike
		// `connected`, which shutdown()'s own QuitFromMds() call already
		// sets to false regardless of whether super.close() runs at all.
		final Field receiveThreadField = MdsConnection.class.getDeclaredField("receiveThread");
		mockReceiveThread = (Thread) mock(receiveThreadField.getType());
		setField(MdsConnection.class, server, "receiveThread", mockReceiveThread);

		// MdsServer-level sockets/collections that shutdown() touches.
		mockRcvSock = mock(ServerSocket.class);
		mockReadSock = mock(Socket.class);
		mockListenSock = mock(Socket.class);
		final Vector<Socket> curr_listen_sock = new Vector<>();
		curr_listen_sock.add(mockListenSock);

		setField(MdsServer.class, server, "server_event_listener", new Vector<>());
		setField(MdsServer.class, server, "rcv_sock", mockRcvSock);
		setField(MdsServer.class, server, "read_sock", mockReadSock);
		setField(MdsServer.class, server, "curr_listen_sock", curr_listen_sock);
	}

	@Test
	public void testCloseClosesLocalSocketsAndSuperClose() throws Exception
	{
		server.close();

		// shutdown()'s own cleanup ran
		verify(mockRcvSock, atLeastOnce()).close();
		verify(mockReadSock, atLeastOnce()).close();
		verify(mockListenSock, atLeastOnce()).close();
		// inherited MdsConnection.close() ran too - interrupting the receive
		// thread is the one thing in that method shutdown() doesn't already
		// do on its own, so this is what actually proves super.close() ran.
		verify(mockReceiveThread).interrupt();
		assertFalse("Connection should be disconnected after close()", server.isConnected());
	}

	@Test
	public void testCloseIsIdempotent() throws Exception
	{
		server.close();
		// Callers may legitimately close() more than once (e.g. explicit
		// close() followed by try-with-resources); must not throw.
		server.close();
	}

	private static void setField(Class<?> declaringClass, Object target, String name, Object value)
			throws Exception
	{
		final Field field = declaringClass.getDeclaredField(name);
		field.setAccessible(true);
		field.set(target, value);
	}
}
