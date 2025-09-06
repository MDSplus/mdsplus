package mds.connection;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

/**
 * Testable wrapper for MdsConnection that allows socket injection for testing
 * This enables testing of the resource cleanup logic without real network
 * connections
 */
public class MockMdsConnectionWithHangPMET extends MdsConnection
{
	private Socket injectedSocket;
	private InputStream injectedInputStream;
	private DataOutputStream injectedOutputStream;

	public MockMdsConnectionWithHangPMET(String provider)
	{
		super(provider);
	}

	/**
	 * Inject mock resources for testing
	 */
	public void injectResources(Socket socket, InputStream inputStream, DataOutputStream outputStream)
	{
		this.injectedSocket = socket;
		this.injectedInputStream = inputStream;
		this.injectedOutputStream = outputStream;
	}

	@Override
	public void connectToServer() throws IOException
	{
		// Use injected resources for testing
		this.sock = injectedSocket;
		this.dis = injectedInputStream;
		this.dos = injectedOutputStream;
		this.host = getProviderHost();
		this.port = getProviderPort();
		this.user = getProviderUser();
	}

	public void setReceiveThread(MRT thread)
	{ this.receiveThread = thread; }

	public void setConnected(boolean connected)
	{ this.connected = connected; }

	public MRT createTestMRT()
	{
		return new MRT();
	}

	@Override
	public PMET createPMET(int id)
	{
		return new PMET(id)
		{
			@Override
			public void run()
			{
				try
				{
					Thread.sleep(2_000);
				}
				catch (InterruptedException ignore)
				{}
			}
		};
	}
}
