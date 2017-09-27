# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

def treeshr_exceptions(s=True):
    from MDSplus import mdsExceptions as me
    for k,v in me.treeshrExceptions.__dict__.iteritems():
        try:
            e = v()
            status = e.status;
            message = str(e)
        except:
            continue
        if s:
            print('public static final int %s = %d;'%(k,status))
        else:
            print('case %s: return "%s";'%(k,message))
    if s:
        treeshr_exceptions(False);


def tdishr_exceptions(s=True):
    from MDSplus import mdsExceptions as me
    for k,v in me.tdishrExceptions.__dict__.iteritems():
        try:
            e = v()
            status = e.status;
            message = str(e)
        except:
            continue
        if s:
            print('public static final int %s = %d;'%(k,status))
        else:
            print('case %s: return "%s";'%(k,message))
    if s:
        tdishr_exceptions(False);

def servershr_exceptions(s=True):
    from MDSplus import mdsExceptions as me
    for k,v in me.servershrExceptions.__dict__.iteritems():
        try:
            e = v()
            status = e.status;
            message = str(e)
        except:
            continue
        if s:
            print('public static final int %s = %d;'%(k,status))
        else:
            print('case %s: return "%s";'%(k,message))
    if s:
        servershr_exceptions(False);


def mdsshr_exceptions(s=True):
    from MDSplus import mdsExceptions as me
    for k,v in me.mdsshrExceptions.__dict__.iteritems():
        try:
            e = v()
            status = e.status;
            message = str(e)
        except:
            continue
        if s:
            print('public static final int %s = %d;'%(k,status))
        else:
            print('case %s: return "%s";'%(k,message))
    if s:
        mdsshr_exceptions(False);


def mdsdcl_exceptions(s=True):
    from MDSplus import mdsExceptions as me
    for k,v in me.mdsdclExceptions.__dict__.iteritems():
        try:
            e = v()
            status = e.status;
            message = str(e)
        except:
            continue
        if s:
            print('public static final int %s = %d;'%(k,status))
        else:
            print('case %s: return "%s";'%(k,message))
    if s:
        mdsdcl_exceptions(False);

def mitdevices_exceptions(s=True):
    from MDSplus import mdsExceptions as me
    for k,v in me.mitdevicesExceptions.__dict__.iteritems():
        try:
            e = v()
            status = e.status;
            message = str(e)
        except:
            continue
        if s:
            print('public static final int %s = %d;'%(k,status))
        else:
            print('case %s: return "%s";'%(k,message))
    if s:
        mitdevices_exceptions(False);


def opc(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    i = 0;
    for line in lines:
        if not line.startswith('OPC ('):
            continue
        name = line[5:].split(')',2)[0].split(',',2)[0].strip('\t ')
        print('public static final short Opc%s = %d;'%(name,i))
        i+=1;

def opc_const_deserialize(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    for line in lines:
        if not line.startswith('OPC ('):
            continue
        parse = line[5:].split(')',2)[0].split(',',3)
        fun = parse[1].strip('\t ')
        if not fun.startswith('$') or len(fun)<=1:
            continue
        fun = '_'.join(p[0]+p[1:].lower() for p in fun[1:].split('_'))
        name = parse[0].strip('\t ')
        print('case OPC.Opc%s:return new d%s(b);'%(name,fun))

def opc_const_class(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    for line in lines:
        if not line.startswith('OPC ('):
            continue
        parse = line[5:].split(')',2)[0].split(',',3)
        fun = parse[1].strip('\t ')
        if not fun.startswith('$') or len(fun)<=1:
            continue
        fun = '_'.join(p[0]+p[1:].lower() for p in fun[1:].split('_'))
        name = parse[0].strip('\t ')
        print('public static final class d%s extends CONST{'%(fun,))
        print('public d%s(){super(OPC.Opc%s);}'%(fun,name))
        print('public d%s(final ByteBuffer b){super(b);}}'%(fun,))

def binary(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    go = False
    i = 0
    print('private static final op_rec[] binary    = new op_rec[]{//')
    for line in lines:
        if not go:
            if line.find('binary[]')>=0:
                go = True
            continue
        if line.strip(' \n\t{')=='':
            continue
        args = line.split('}',2)[0].strip('\t ').split(',')
        symbol = ','.join(args[0:-3])
        if symbol=='0':
            symbol='null';
        opcode = args[-3].strip(' ')
        if opcode=='0':
            break
        prec = int(args[-2])
        lorr = int(args[-1])
        print('new op_rec(%s,OPC.%s,(byte)%d,(byte)%d),//%d'%(symbol,opcode,prec,lorr,i))
        i += 1
    print('};')

def unary(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    go = False
    i = 0
    print('private static final op_rec[] unary    = new op_rec[]{//')
    for line in lines:
        if not go:
            if line.find('unary[]')>=0:
                go = True
            continue
        if line.strip(' \n\t{')=='':
            continue
        args = line.split('}',2)[0].strip('\t ').split(',')
        symbol = ','.join(args[0:-3])
        if symbol=='0':
            symbol='null';
        opcode = args[-3].strip(' ')
        if opcode=='0':
            break
        prec = int(args[-2])
        lorr = int(args[-1])
        print('new op_rec(%s,OPC.%s,(byte)%d,(byte)%d),//%d'%(symbol,opcode,prec,lorr,i))
        i += 1
    print('};')

def defcat(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    go = False
    i = 0
    print('private static final def_cat[] tdiDefCat = new def_cat[]{//')
    for line in lines:
        if not go:
            if line.find('TdiREF_CAT[]')>=0:
                go = True
            continue
        if line.strip(' \n\t{')=='':
            continue
        if line.strip(' \n\t{')=='};':
            break
        args = line.split('}',2)[0].strip('{\t ').split(',')
        name = args[0]
        cat = args[1].strip(' ')
        if name=='"P"':
            length = 8;
        else:
            length = int(args[2])
        digits = int(args[3])
        fname = args[4].strip(' ')
        if fname=='0':
            fname='null';
        elif fname=='F_SYM':
            fname='"F"';
        elif fname=='FS_SYM':
            fname='"E"';
        elif fname=='FT_SYM':
            fname='"D"';
        elif fname=='G_SYM':
            fname='"G"';
        elif fname=='D_SYM':
            fname='"V"';
        print('new def_cat(%s,(short)%s,(byte)%d,(byte)%d,%s),//%d'%(name,cat,length,digits,fname,i))
        i += 1
    print('};')

def reffun(filepath):
    def repio(i):
        if(i=='VV'): return 'BU'
        if(i=='XX'): return 'T'
        if(i=='YY'): return 'HC'
        if(i=='SUBSCRIPT'): return 'L'
        if(i=='UNITS'): return 'T'
        return i
    def reptoken(i):
        return i.replace('OK','0')
    with file(filepath) as f:
        lines = f.readlines()
    print('private static final ref_fun[] tdiRefFun = new ref_fun[]{//')
    for line in lines:
        if not line.startswith('OPC'):
            continue
        name,symbol,f1,f2,f3,i1,i2,o1,o2,m1,m2,token = tuple(a.strip('\t ') for a in line.split('(',2)[1].split(')',2)[0].strip('{\t ').split(',',12))
        i1,i2,o1,o2 = map(repio,(i1,i2,o1,o2))
        token = reptoken(token)
        print('new ref_fun("%s","%s",%s,%s,%s,DTYPE.%s,DTYPE.%s,DTYPE.%s,DTYPE.%s,%s,%s,%s),//'%(name,symbol,f1,f2,f3,i1,i2,o1,o2,m1,m2,token))
    print('};')

def constant(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    for line in lines:
        line = line.split('/*',2)[0].strip(' \t\n\r')
        if line.startswith('DATUM('):
            dtype,x,data = line[6:-1].split(', ')
            if dtype=='FLOAT':
                print("case OPC.Opc%s:return new Float32(%sf);"%(x,data[7:]))
            elif dtype=='BU':
                print("case OPC.Opc%s:return new Uint8((byte)%s);"%(x,data))
            elif dtype=='FLOAT_COMPLEX':
                print("case OPC.Opc%s:return new Complex32(0.f, 1.f);"%(x,))
            elif dtype=='MISSING':
                print("case OPC.Opc%s:return Missing.NEW;"%(x,))
            else:
                print("case OPC.Opc%s:return Missing.NEW;//TODO:set correct return value"%(x,))
        if line.startswith('UNITS('):
            dtype,x,data,units = line[6:-1].split(', ')
            if dtype=='FLOAT':
                print('case OPC.Opc%s:return new With_Units(new Float32(%sf),new CString(%s));'%(x,data[7:],units))
            else:
                print("case OPC.Opc%s:return Missing.NEW;//TODO:set correct return value"%(x,))
        if line.startswith('UERR('):
            dtype,x,data,error,units = line[5:-1].split(', ')
            if dtype=='FLOAT':
                print('case OPC.Opc%s:return new With_Units(new With_Error( new Float32(%sf), new Float32(%sf)),new CString(%s));'%(x,data[7:],error[7:],units))
            else:
                print("case OPC.Opc%s:return Missing.NEW;//TODO:set correct return value"%(x,))



#servershr_exceptions()
#treeshr_exceptions()
#mdsdcl_exceptions()
#mitdevices_exceptions()
#tdishr_exceptions()
#mdsshr_exceptions()
#reffun('C:/Git/mdsplus/include/opcbuiltins.h')
#opc('C:/Git/mdsplus/include/opcbuiltins.h')
#opc_const_class('C:/Git/mdsplus/include/opcbuiltins.h')
opc_const_deserialize('C:/Git/mdsplus/include/opcbuiltins.h')
#binary('C:/Git/mdsplus/tdishr/TdiDecompileR.c')
#unary('C:/Git/mdsplus/tdishr/TdiDecompileR.c')
#defcat('C:/Git/mdsplus/tdishr/TdiDefCat.c')
#constant('C:/Git/mdsplus/tdishr/TdiConstant.c')
