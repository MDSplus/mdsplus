package mds.jtraverser;
import javax.swing.*;

/**
 * This class is used only to record the jTraverser frame \ and to communicate
 * it to devices
 */
class FrameRepository
{
	static JFrame frame = null;

	static void setFrame(JFrame frame)
	{ FrameRepository.frame = frame; }
}
