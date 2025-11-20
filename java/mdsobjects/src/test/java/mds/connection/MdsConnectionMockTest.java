package mds.connection;

import static org.junit.Assert.*;
import static org.mockito.Mockito.*;

import java.io.ByteArrayInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.net.Socket;
import java.util.concurrent.CountDownLatch;

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
	private CountDownLatch latch;

	@Before
	public void setUp()
	{
		MockitoAnnotations.openMocks(this);
		connection = new MockMdsConnectionWithHangPMET("localhost:8000");
		latch = new CountDownLatch(1);
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
		connection.injectResources(mockSocket, mockInputStreamStopWhenRead(), mockOutputStream);
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
	public void testReceiveThreadLifecycle() throws Exception
	{
		// Given
		connection.injectResources(mockSocket, mockInputStreamStopWhenRead(), mockOutputStream);
		connection.connectToServer();
		MockMdsConnectionWithHangPMET.MRT receiveThread = connection.createTestMRT();
		connection.setReceiveThread(receiveThread);
		connection.setConnected(true);
		// Start the thread with some time to complete the creation
		receiveThread.start();
		Thread.sleep(100);
		assertTrue("Thread should be alive after start", receiveThread.isAlive());
		assertEquals("Await on read", 1, latch.getCount());
		// When
		connection.close();
		// Then
		receiveThread.join(1400);
		assertFalse("Connection should be disconnected", connection.isConnected());
		assertFalse("Thread should be terminated after close", receiveThread.isAlive());
		assertEquals("Thread should be in TERMINATED state", Thread.State.TERMINATED, receiveThread.getState());
	}

	@Test
	public void testExecutorServiceResourceCleanup() throws Exception
	{
		// Get initial thread count
		ThreadMXBean threadBean = ManagementFactory.getThreadMXBean();
		long initialThreadCount = threadBean.getThreadCount();
		connection.injectResources(mockSocket, mockInputStreamStopWhenRead(), mockOutputStream);
		connection.connectToServer();
		MockMdsConnectionWithHangPMET.MRT receiveThread = connection.createTestMRT();
		connection.setReceiveThread(receiveThread);
		// Start the thread with some time to complete the creation
		receiveThread.start();
		Thread.sleep(100);
		long peakThreadCount = threadBean.getThreadCount();
		// When
		connection.close();
		// Wait for all cleanup to complete
		receiveThread.join(1400);
		Thread.sleep(200); // Additional time for ExecutorService cleanup
		long finalThreadCount = threadBean.getThreadCount();
		// Then
		assertFalse("Connection should be disconnected", connection.isConnected());
		assertTrue("Thread count should increase during event processing", peakThreadCount > initialThreadCount);
		assertEquals("Thread count should return to initial after cleanup", initialThreadCount, finalThreadCount);
	}

	////// Helper methods
	private ByteArrayInputStream mockInputStreamStopWhenRead()
	{
		when(mockInputStream.read()).thenAnswer(invocation ->
		{
			latch.await();
			return -1; // EOF
		});
		try
		{
			when(mockInputStream.read(any(byte[].class))).thenAnswer(invocation ->
			{
				latch.await();
				return -1; // EOF
			});
		}
		catch (IOException e)
		{
			throw new RuntimeException(e);
		}
		when(mockInputStream.read(any(byte[].class), anyInt(), anyInt())).thenAnswer(invocation ->
		{
			latch.await();
			return -1; // EOF
		});
		return mockInputStream;
	}
}
