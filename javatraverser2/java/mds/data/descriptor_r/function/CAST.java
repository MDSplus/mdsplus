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
package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_a.Float32Array;
import mds.data.descriptor_a.Float64Array;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Float32;
import mds.data.descriptor_s.Float64;
import mds.data.descriptor_s.Missing;

public abstract class CAST extends Function{
    public static final class Byte extends CAST{
        public Byte(final ByteBuffer b){
            super(b);
        }

        public Byte(final Descriptor<?> arg){
            super(OPC.OpcByte, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.bytes();
        }
    }
    public static final class Byte_Unsigned extends CAST{
        public Byte_Unsigned(final ByteBuffer b){
            super(b);
        }

        public Byte_Unsigned(final Descriptor<?> arg){
            super(OPC.OpcByteUnsigned, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.byteu();
        }
    }
    public static final class D_Complex extends CAST{
        public D_Complex(final ByteBuffer b){
            super(b);
        }

        public D_Complex(final Descriptor<?> arg){
            super(OPC.OpcDComplex, arg);
        }
    }
    public static final class D_Float extends CAST{
        public D_Float(final ByteBuffer b){
            super(b);
        }

        public D_Float(final Descriptor<?> arg){
            super(OPC.OpcDFloat, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.dfloat();
        }
    }
    public static final class Dble extends CAST{
        public Dble(final ByteBuffer b){
            super(b);
        }

        public Dble(final Descriptor<?> arg){
            super(OPC.OpcDble, arg);
        }
    }
    public static final class F_Complex extends CAST{
        public F_Complex(final ByteBuffer b){
            super(b);
        }

        public F_Complex(final Descriptor<?> arg){
            super(OPC.OpcFComplex, arg);
        }
    }
    public static final class F_Float extends CAST{
        public F_Float(final ByteBuffer b){
            super(b);
        }

        public F_Float(final Descriptor<?> arg){
            super(OPC.OpcFFloat, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.ffloat();
        }
    }
    public static final class Float extends CAST{
        public Float(final ByteBuffer b){
            super(b);
        }

        public Float(final Descriptor<?> arg){
            super(OPC.OpcFloat, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            if(data.length() > 4){
                if(data instanceof Descriptor_A) return new Float64Array(data.getShape(), data.toDoubleArray());
                return new Float64(data.toDouble());
            }
            if(data instanceof Descriptor_A) return new Float32Array(data.getShape(), data.toFloatArray());
            return new Float32(data.toFloat());
        }
    }
    public static final class FS_Complex extends CAST{
        public FS_Complex(final ByteBuffer b){
            super(b);
        }

        public FS_Complex(final Descriptor<?> arg){
            super(OPC.OpcFS_complex, arg);
        }
    }
    public static final class FS_Float extends CAST{
        public FS_Float(final ByteBuffer b){
            super(b);
        }

        public FS_Float(final Descriptor<?> arg){
            super(OPC.OpcFS_float, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.fsfloat();
        }
    }
    public static final class FT_Complex extends CAST{
        public FT_Complex(final ByteBuffer b){
            super(b);
        }

        public FT_Complex(final Descriptor<?> arg){
            super(OPC.OpcFT_complex, arg);
        }
    }
    public static final class FT_Float extends CAST{
        public FT_Float(final ByteBuffer b){
            super(b);
        }

        public FT_Float(final Descriptor<?> arg){
            super(OPC.OpcFT_float, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.ftfloat();
        }
    }
    public static final class G_Complex extends CAST{
        public G_Complex(final ByteBuffer b){
            super(b);
        }

        public G_Complex(final Descriptor<?> arg){
            super(OPC.OpcGComplex, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.gfloat();
        }
    }
    public static final class G_Float extends CAST{
        public G_Float(final ByteBuffer b){
            super(b);
        }

        public G_Float(final Descriptor<?> arg){
            super(OPC.OpcGFloat, arg);
        }
    }
    public static final class H_Complex extends CAST{
        public H_Complex(final ByteBuffer b){
            super(b);
        }

        public H_Complex(final Descriptor<?> arg){
            super(OPC.OpcHComplex, arg);
        }
    }
    public static final class H_Float extends CAST{
        public H_Float(final ByteBuffer b){
            super(b);
        }

        public H_Float(final Descriptor<?> arg){
            super(OPC.OpcHFloat, arg);
        }
    }
    public static final class Long extends CAST{
        public Long(final ByteBuffer b){
            super(b);
        }

        public Long(final Descriptor<?> arg){
            super(OPC.OpcLong, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.longs();
        }
    }
    public static final class Long_Unsigned extends CAST{
        public Long_Unsigned(final ByteBuffer b){
            super(b);
        }

        public Long_Unsigned(final Descriptor<?> arg){
            super(OPC.OpcLongUnsigned, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.longu();
        }
    }
    public static final class Octaword extends CAST{
        public Octaword(final ByteBuffer b){
            super(b);
        }

        public Octaword(final Descriptor<?> arg){
            super(OPC.OpcOctaword, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.octawords();
        }
    }
    public static final class Octaword_Unsigned extends CAST{
        public Octaword_Unsigned(final ByteBuffer b){
            super(b);
        }

        public Octaword_Unsigned(final Descriptor<?> arg){
            super(OPC.OpcOctawordUnsigned, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.octawordu();
        }
    }
    public static final class Quadword extends CAST{
        public Quadword(final ByteBuffer b){
            super(b);
        }

        public Quadword(final Descriptor<?> arg){
            super(OPC.OpcQuadword, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.quadwords();
        }
    }
    public static final class Quadword_Unsigned extends CAST{
        public Quadword_Unsigned(final ByteBuffer b){
            super(b);
        }

        public Quadword_Unsigned(final Descriptor<?> arg){
            super(OPC.OpcQuadwordUnsigned, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.quadwordu();
        }
    }
    public static final class Text extends CAST{
        public Text(final ByteBuffer b){
            super(b);
        }

        public Text(final Descriptor<?> arg){
            super(OPC.OpcText, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.text();
        }
    }
    public static final class Word extends CAST{
        public Word(final ByteBuffer b){
            super(b);
        }

        public Word(final Descriptor<?> arg){
            super(OPC.OpcWord, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.words();
        }
    }
    public static final class Word_Unsigned extends CAST{
        public Word_Unsigned(final ByteBuffer b){
            super(b);
        }

        public Word_Unsigned(final Descriptor<?> arg){
            super(OPC.OpcWordUnsigned, arg);
        }

        @Override
        protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.wordu();
        }
    }

    public static final boolean coversOpCode(final short opcode) {
        switch(opcode){
            default:
                return false;
            case OPC.OpcText:
            case OPC.OpcDble:
            case OPC.OpcByte:
            case OPC.OpcByteUnsigned:
            case OPC.OpcWord:
            case OPC.OpcWordUnsigned:
            case OPC.OpcLong:
            case OPC.OpcLongUnsigned:
            case OPC.OpcQuadword:
            case OPC.OpcQuadwordUnsigned:
            case OPC.OpcOctaword:
            case OPC.OpcOctawordUnsigned:
            case OPC.OpcFloat:
            case OPC.OpcDFloat:
            case OPC.OpcFFloat:
            case OPC.OpcFS_float:
            case OPC.OpcFT_float:
            case OPC.OpcGFloat:
            case OPC.OpcHFloat:
            case OPC.OpcDComplex:
            case OPC.OpcFComplex:
            case OPC.OpcFS_complex:
            case OPC.OpcFT_complex:
            case OPC.OpcGComplex:
            case OPC.OpcHComplex:
                return true;
        }
    }

    public static CAST deserialize(final ByteBuffer b) throws MdsException {
        final short opcode = b.getShort(b.getInt(Descriptor._ptrI));
        switch(opcode){
            case OPC.OpcText:
                return new Text(b);
            case OPC.OpcDble:
                return new Dble(b);
            case OPC.OpcByte:
                return new Byte(b);
            case OPC.OpcByteUnsigned:
                return new Byte_Unsigned(b);
            case OPC.OpcWord:
                return new Word(b);
            case OPC.OpcWordUnsigned:
                return new Word_Unsigned(b);
            case OPC.OpcLong:
                return new Long(b);
            case OPC.OpcLongUnsigned:
                return new Long_Unsigned(b);
            case OPC.OpcQuadword:
                return new Quadword(b);
            case OPC.OpcQuadwordUnsigned:
                return new Quadword_Unsigned(b);
            case OPC.OpcOctaword:
                return new Octaword(b);
            case OPC.OpcOctawordUnsigned:
                return new Octaword_Unsigned(b);
            case OPC.OpcFloat:
                return new Float(b);
            case OPC.OpcDFloat:
                return new D_Float(b);
            case OPC.OpcFFloat:
                return new F_Float(b);
            case OPC.OpcFS_float:
                return new FS_Float(b);
            case OPC.OpcFT_float:
                return new FT_Float(b);
            case OPC.OpcGFloat:
                return new G_Float(b);
            case OPC.OpcHFloat:
                return new H_Float(b);
            case OPC.OpcDComplex:
                return new Octaword(b);
            case OPC.OpcFComplex:
                return new Octaword(b);
            case OPC.OpcFS_complex:
                return new Octaword(b);
            case OPC.OpcFT_complex:
                return new Octaword(b);
            case OPC.OpcGComplex:
                return new Octaword(b);
            case OPC.OpcHComplex:
                return new Octaword(b);
        }
        throw new MdsException(MdsException.TdiINV_OPC);
    }

    protected CAST(final ByteBuffer b){
        super(b);
    }

    private CAST(final short opcode, final Descriptor<?> arg){
        super(opcode, arg);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        pout.append(this.getName());
        this.addArguments(0, "(", ")", pout, mode);
        return pout;
    }

    @Override
    public final Descriptor<?> evaluate() {
        try{
            return this.evaluate(this.getArgument(0).getData());
        }catch(final MdsException e){
            e.printStackTrace();
            return Missing.NEW;
        }
    }

    protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
        return this.mds.getDescriptor(this.getName(), this.getArguments());
    }
}
