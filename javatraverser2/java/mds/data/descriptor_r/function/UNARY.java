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
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Ident;
import mds.data.descriptor_s.Missing;

public abstract class UNARY extends Function{
    public static final class Inot extends UNARY{
        public Inot(final ByteBuffer b){
            super(b);
        }

        public Inot(final Descriptor<?> arg){
            super(OPC.OpcInot, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.inot();
        }
    }
    public static final class Not extends UNARY{
        public Not(final ByteBuffer b){
            super(b);
        }

        public Not(final Descriptor<?> arg){
            super(OPC.OpcNot, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.not();
        }
    }
    public static final class Post_Dec extends UNARY{
        public Post_Dec(final ByteBuffer b){
            super(b);
        }

        public Post_Dec(final Descriptor<?> arg){
            super(OPC.OpcPostDec, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            if(this.getArgument(0) instanceof Ident){
                ((Ident)this.getArgument(0)).dec();
                return data.toDescriptor();
            }
            throw DATA.tdierror;
        }
    }
    public static final class Post_Inc extends UNARY{
        public Post_Inc(final ByteBuffer b){
            super(b);
        }

        public Post_Inc(final Descriptor<?> arg){
            super(OPC.OpcPostInc, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            if(this.getArgument(0) instanceof Ident){
                ((Ident)this.getArgument(0)).inc();
                return data.toDescriptor();
            }
            throw DATA.tdierror;
        }
    }
    public static final class Pre_Dec extends UNARY{
        public Pre_Dec(final ByteBuffer b){
            super(b);
        }

        public Pre_Dec(final Descriptor<?> arg){
            super(OPC.OpcPreDec, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            if(this.getArgument(0) instanceof Ident) return ((Ident)this.getArgument(0)).dec();
            throw DATA.tdierror;
        }
    }
    public static final class Pre_Inc extends UNARY{
        public Pre_Inc(final ByteBuffer b){
            super(b);
        }

        public Pre_Inc(final Descriptor<?> arg){
            super(OPC.OpcPreInc, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            if(this.getArgument(0) instanceof Ident) return ((Ident)this.getArgument(0)).inc();
            throw DATA.tdierror;
        }
    }
    public static final class Unary_Minus extends UNARY{
        public Unary_Minus(final ByteBuffer b){
            super(b);
        }

        public Unary_Minus(final Descriptor<?> arg){
            super(OPC.OpcUnaryMinus, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.neg();
        }
    }
    public static final class Unary_Plus extends UNARY{
        public Unary_Plus(final ByteBuffer b){
            super(b);
        }

        public Unary_Plus(final Descriptor<?> arg){
            super(OPC.OpcUnaryPlus, arg);
        }

        @Override
        protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException {
            return data.toDescriptor();
        }
    }
    private static final op_rec[] unary = new op_rec[]{ //
            new op_rec("~", OPC.OpcInot, (byte)8, (byte)1), // 0
            new op_rec("!", OPC.OpcNot, (byte)8, (byte)1), // 1
            new op_rec("--", OPC.OpcPreDec, (byte)8, (byte)1), // 2
            new op_rec("++", OPC.OpcPreInc, (byte)8, (byte)1), // 3
            new op_rec("-", OPC.OpcUnaryMinus, (byte)8, (byte)1), // 4
            new op_rec("+", OPC.OpcUnaryPlus, (byte)8, (byte)1), // 5
            new op_rec("--", OPC.OpcPostDec, (byte)4, (byte)-1), // 6
            new op_rec("++", OPC.OpcPostInc, (byte)4, (byte)-1), // 7
    };

    public static final boolean coversOpCode(final short opcode) {
        switch(opcode){
            default:
                return false;
            case OPC.OpcInot:
            case OPC.OpcNot:
            case OPC.OpcPreDec:
            case OPC.OpcPreInc:
            case OPC.OpcUnaryMinus:
            case OPC.OpcUnaryPlus:
            case OPC.OpcPostDec:
            case OPC.OpcPostInc:
                return true;
        }
    }

    public static UNARY deserialize(final ByteBuffer b) throws MdsException {
        final short opcode = b.getShort(b.getInt(Descriptor._ptrI));
        switch(opcode){
            case OPC.OpcInot:
                return new Inot(b);
            case OPC.OpcNot:
                return new Not(b);
            case OPC.OpcPostDec:
                return new Post_Dec(b);
            case OPC.OpcPostInc:
                return new Post_Inc(b);
            case OPC.OpcPreDec:
                return new Pre_Dec(b);
            case OPC.OpcPreInc:
                return new Pre_Dec(b);
            case OPC.OpcUnaryMinus:
                return new Unary_Minus(b);
            case OPC.OpcUnaryPlus:
                return new Unary_Plus(b);
        }
        throw new MdsException(MdsException.TdiINV_OPC);
    }

    public UNARY(final ByteBuffer b){
        super(b);
    }

    public UNARY(final short opcode, final Descriptor<?> arg){
        super(opcode, arg);
    }

    @Override
    public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode) {
        final short opcode = this.getOpCode();
        op_rec pop = null;
        for(final op_rec element : UNARY.unary)
            if(element.opcode == opcode){
                pop = element;
                break;
            }
        if(pop == null) return pout.append("/***error<unary opcode(").append(opcode).append(") not found>***/");
        final int newone = pop.prec;
        final int lorr = pop.lorr;
        if(lorr > 0) pout.append(pop.symbol);
        if(prec <= newone) pout.append('(');
        this.getDescriptor(0).decompile(newone + lorr, pout, mode & ~Descriptor.DECO_X);
        if(prec <= newone) pout.append(')');
        if(lorr < 0) pout.append(pop.symbol);
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

    protected abstract Descriptor<?> evaluate(final DATA<?> data) throws MdsException;

    @Override
    public Descriptor<?> getLocal_() {
        final Descriptor<?> X = this.getArgument(0).getLocal();
        try{
            return this.getClass().getConstructor(Descriptor.class).newInstance(X).setLocal();
        }catch(final Exception e){
            e.printStackTrace();
            return Missing.NEW;
        }
    }
}
