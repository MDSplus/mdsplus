package mds.connection;

import static org.junit.Assert.*;
import static org.mockito.Mockito.*;

import java.io.*;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.net.Socket;
import java.util.Vector;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

/**
 * Tests MdsConnection with mock socket
 */
public class MdsConnectionMockTest
{
	@Mock
	private Socket mockSocket;
	@Mock
	private ByteArrayInputStream mockInputStream;
	@Mock
	private DataOutputStream mockOutputStream;
	private MockMdsConnectionWithHangPMET connection;

	@Before
	public void setUp()
	{
		MockitoAnnotations.openMocks(this);
		connection = new MockMdsConnectionWithHangPMET("localhost:8000");
	}

	@After
	public void tearDown() throws Exception
	{
		if (connection != null)
		{
			connection.close();
		}
	}

	@Test
	public void testSocketClosedOnClose() throws Exception
	{
		// Inject mock resources
		connection.injectResources(mockSocket, mockInputStream, mockOutputStream);
		connection.connectToServer();
		// Verify resources are set
		assertNotNull("Socket should be set", connection.sock);
		assertNotNull("InputStream should be set", connection.dis);
		assertNotNull("OutputStream should be set", connection.dos);
		// When
		connection.close();
		// Then - Verify close was called on all resources
		assertFalse(connection.isConnected());
		verify(mockSocket, atLeastOnce()).close();
		verify(mockInputStream, times(1)).close();
		verify(mockOutputStream, times(1)).close();
	}

	@Test
	public void testReceiveThreadInterruptedOnClose() throws Exception
	{
		// Given
		MockMdsConnectionWithHangPMET.MRT mockReceiveThread = spy(connection.createTestMRT());
		connection.setReceiveThread(mockReceiveThread);
		connection.setConnected(true);
		mockReceiveThread.start();
		Thread.sleep(100);
		// When
		connection.close();
		// Then
		verify(mockReceiveThread, times(1)).interrupt();
		assertFalse("Connection should be disconnected", connection.isConnected());
	}

	@Test
	public void testExecutorServiceShutdownInMRT() throws Exception
	{
		// Get initial thread count
		ThreadMXBean threadBean = ManagementFactory.getThreadMXBean();
		long initialThreadCount = threadBean.getThreadCount();
		// Create a mock input stream that will provide event messages to trigger PMET
		// tasks
		InputStream dtypeEventMsg = createDummyMdsMessage();
		// Inject the mock stream that will provide event data
		connection.injectResources(mockSocket, dtypeEventMsg, mockOutputStream);
		connection.connectToServer();
		// PMET tasks
		MockMdsConnectionWithHangPMET.MRT receiveThread = spy(connection.createTestMRT());
		connection.setReceiveThread(receiveThread);
		receiveThread.start();
		long peakThreadCount = threadBean.getThreadCount();
		// When - interrupt the thread to trigger ExecutorService shutdown
		receiveThread.interrupt();
		Thread.sleep(600);
		long finalThreadCount = threadBean.getThreadCount();
		// Then - verify ExecutorService threads were created and cleaned up
		assertTrue("JVM Thread count must increase during execution", peakThreadCount > initialThreadCount);
		assertTrue("JVM Thread count must equal to init", initialThreadCount == finalThreadCount);
	}

	@Test
	public void testCreateDummyMdsMessageIsDTYPE_EVENT() throws IOException
	{
		MdsMessage msg = new MdsMessage("", new Vector<>());
		// When
		msg.Receive(createDummyMdsMessage());
		// Then
		assertEquals(msg.dtype, Descriptor.DTYPE_EVENT);
	}

	private InputStream createDummyMdsMessage()
	{
		ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
		try (DataOutputStream dos = new DataOutputStream(byteStream))
		{
			// Message header (48 bytes total)
			dos.writeInt(64); // msglen = header(48) + body(16)
			dos.writeInt(1); // status = success
			dos.writeShort(16); // length = 16 bytes for event data
			dos.writeByte(1); // nargs
			dos.writeByte(0); // descr_idx
			dos.writeByte(1); // message_id
			dos.writeByte(Descriptor.DTYPE_EVENT); // dtype = DTYPE_EVENT
			dos.writeByte((byte) 0x83); // client_type (JCJAVA_CLIENT)
			dos.writeByte(0); // ndims
			// Write 32 bytes of dimension data (8 ints)
			for (int i = 0; i < 8; i++)
			{
				dos.writeInt(0);
			}
			// Write event body - need at least 13 bytes to access body[12]
			for (int i = 0; i < 12; i++)
			{
				dos.writeByte(0); // padding bytes
			}
			dos.writeByte(5); // event ID at index 12
			dos.writeByte(0);
			dos.writeByte(0);
			dos.writeByte(0);
		}
		catch (IOException e)
		{
			throw new RuntimeException(e);
		}
		return new ByteArrayInputStream(byteStream.toByteArray());
	}
}
