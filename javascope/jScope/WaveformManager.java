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
import java.awt.Point;
import java.awt.Component;

/**
 * This interface can be implemented by (Multi)Waveform panel container to
 * perfom interaction between (Multi)Waveform object, autoscale action,
 * copy/paste action etc..
 *
 * @see WaveformContainer
 */
public interface  WaveformManager
{
    /**
     * Autoscale operation on all waveforms
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void     AutoscaleAll();

    /**
     * Autoscale operation on all images
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void     AutoscaleAllImages();

    /**
     * Autoscale y axis on all waveform
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void     AutoscaleAllY();

    /**
     * Set the same scale factor of the argument waveform to all waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void     AllSameScale(Waveform curr_w);

    /**
     * Autoscale y axis and set x axis equals to argument waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void     AllSameXScaleAutoY(Waveform curr_w);

    /**
     * Set y scale factor of all waveform equals to argument waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void     AllSameYScale(Waveform curr_w);

    /**
     * Set x scale factor of all waveform equals to argument waveform
     *
     * @param curr_w a waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void     AllSameXScale(Waveform curr_w);

    /**
     * Reset all waveform scale factor.
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void     ResetAllScales();

    /**
     * Remove a waveform.
     *
     * @param w waveform to remove
     */
    public void     removePanel(Waveform w);

    /**
     * Get current selected waveform.
     *
     * @return current selected waveform or null
     * @see Waveform
     * @see MultiWaveform
     */
    public Waveform GetSelected();

    /**
     * Select a waveform
     *
     * @param w waveform to select
     * @see Waveform
     * @see MultiWaveform
     */
    public void     Select(Waveform w);

    /**
     * Deselect waveform.
     *
     * @see Waveform
     * @see MultiWaveform
     */
    public void     Deselect();

    /**
     * Get the number of waveform in the container
     *
     * @return number of waveform in the container
     */
    public int      GetWaveformCount();

    /**
     * Update crosshair position
     *
     * @param curr_x x axis position
     * @param w a waveform to update cross
     * @see Waveform
     * @see MultiWaveform
     */
    public void     UpdatePoints(double curr_x, Waveform w);

    /**
     * Get current waveform selected as copy source
     *
     * @return copy source waveform
     */
    public Waveform GetCopySource();

    /**
     * Set copy source waveform
     *
     * @param w copy source waveform
     * @see Waveform
     * @see MultiWaveform
     */
    public void     SetCopySource(Waveform w);

	/**
	 * Perform copy operation
	 *
	 * @param dest destination waveform
	 * @param source source waveform
	 */
	public void     NotifyChange(Waveform dest, Waveform source);

    /**
     * Enable or disable measure mode
     *
     * @param state measure mode flag
     * @see Waveform
     * @see MultiWaveform
     */
    public void     SetShowMeasure(boolean state);

    /**
     * Return row, column position on the panel
     *
     * @param w a waveform
     */
    public Point     getWavePosition(Waveform w);

    public void maximizeComponent(Waveform w);
    public Component getMaximizeComponent();
    public boolean isMaximize();

}
