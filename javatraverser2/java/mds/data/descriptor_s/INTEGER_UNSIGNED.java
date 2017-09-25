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
package mds.data.descriptor_s;

import java.math.BigInteger;
import java.nio.ByteBuffer;

public abstract class INTEGER_UNSIGNED<T extends Number>extends INTEGER<T>{
    public INTEGER_UNSIGNED(final byte ou, final BigInteger value){
        super(ou, value);
    }

    public INTEGER_UNSIGNED(final byte bu, final byte value){
        super(bu, value);
    }

    public INTEGER_UNSIGNED(final byte lu, final int value){
        super(lu, value);
    }

    public INTEGER_UNSIGNED(final byte qu, final long value){
        super(qu, value);
    }

    public INTEGER_UNSIGNED(final byte wu, final short value){
        super(wu, value);
    }

    protected INTEGER_UNSIGNED(final ByteBuffer b){
        super(b);
    }

    @Override
    public final INTEGER_UNSIGNED<?> abs() {
        return this;
    }

    @Override
    protected final byte getRankClass() {
        return 0x00;
    }
}
