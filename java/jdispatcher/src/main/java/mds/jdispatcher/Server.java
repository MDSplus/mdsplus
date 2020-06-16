package mds.jdispatcher;

public interface Server
/**
 * Abstracts Server functionality
 */
{
	/**
	 * returns the number of doing actions
	 */
	public void abort(boolean flush);

	/**
	 * aborts action currently being executed. If flush, empties waiting queue;
	 */
	public boolean abortAction(Action action);

	/**
	 * Handles the termination of the sequence (e.g. for closing trees)
	 */
	public void addServerListener(ServerListener listener);

	/**
	 * Collects action information to be used by the dispatcher
	 */
	public void beginSequence(int shot);

	/**
	 * Collects action information to be used by the dispatcher, starting from
	 * specified subtree
	 */
	public Action[] collectActions();

	/**
	 * Removes the lastly inserted action form the waiting queue. If no such action
	 * is found return null
	 */
	public Action[] collectActions(String rootPath);

	/**
	 * Handles the beginning of a sequence (e.g. for opening trees)
	 */
	public void endSequence(int shot);

	/**
	 * returns the number of waiting actions
	 */
	public int getDoingAction();

	/**
	 * returns the classes to which the server belongs
	 */
	public int getQueueLength();

	/**
	 * Add a new listener for actions handled by the server
	 */
	public String getServerClass();

	/**
	 * aborts selected action
	 */
	public boolean isActive();

	/**
	 * true if server actively participating to dispatching
	 */
	public boolean isReady();

	/**
	 * Inserts a new action in the waiting queue
	 */
	public Action popAction();

	public void pushAction(Action action);

	/**
	 * true if server can participate to the next shot
	 */
	public void setTree(String tree);

	public void setTree(String tree, int shot);
}
