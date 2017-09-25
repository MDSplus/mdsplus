/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package jScope;

/* $Id$ */
import java.awt.Dimension;
import java.io.IOException;

/**
 * When a Frame sequence is requested to a DataProvider implementation, it is returned by means of an object
 * implementing the FrameData interface. FrameData defines therefore all the access methods required to handle 
 * a sequence of frames.
 * 
 * @see DataProvider
 */
public interface FrameData
{
    static final int BITMAP_IMAGE_8  = 1;
    static final int BITMAP_IMAGE_16 = 2;
    static final int BITMAP_IMAGE_32 = 4;
    static final int AWT_IMAGE = 5;
    static final int JAI_IMAGE = 6;
    
    /**
     * Returns the type of the corresponding frames. Returned frames can have either of the following types:
     * <br>
     * -FrameData.BITMAP_IMAGE meaning that method GetFrameAt will return a byte matrix.
     * <br>
     * -FrameData.AWT_IMAGE meaning that method GetFrameAt will return a byte vector representing the binary
     *  content of a gif or jpeg file.
     * <br>
     * -FrameData.JAI_IMAGE meaning that method GetFrameAt will return a byte vector representing the binary
     *  content of every image file supported by the JAI (Java Advanced Imaging) package. The JAI package needs not 
     *  to be installed unless file formats other than gif or jpeg are used.
     * <br>
     * Currently, only BITMAP_IMAGE is supported
     * 
     * @return The type of the corresponding frame.
     * @exception java.io.IOException
     */
    public int GetFrameType() throws Exception;

    /**
     * Returns the number of frames in the sequence.
     * 
     * @return The number of frames in the sequence.
     * @exception java.io.IOException
     */
    public int GetNumFrames() throws Exception;

    /**
     * Return the dimension of a frame. All the frames in the sequence must have the same dimension.
     * 
     * @return The frame dimension.
     * @exception java.io.IOException
     */
    public Dimension GetFrameDimension()throws Exception;

    /**
     * Return the times associated with every frame of the sequence. This information is required to correlate 
     * the frame sequence with the other signals displayed by jScope.
     * 
     * @return The time array for the frame sequence.
     */
    public float[] GetFrameTimes() throws Exception;

    /**
     * Return the frame at the given position.
     * 
     * @param idx The index of the requested frame in the frame sequence.
     * @return The frame as a byte array. If the frame type is FrameData.BITMAP_IMAGE, the matrix uses row major
     * ordering.
     * @exception java.io.IOException
     */
    public byte[] GetFrameAt(int idx) throws IOException;
}
