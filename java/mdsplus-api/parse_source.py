def exceptions(s=True):
    from MDSplus import mdsExceptions as me
    for k, v in me.__dict__.items():
        if k.endswith("Exception"):
            continue
        try:
            e = v()
            status = e.status
            message = str(e)
        except:
            continue
        if s:
            print('public static final int %s = 0%o;' % (k, status))
        else:
            print('case %s: return "%s";' % (k, message))
    if s:
        exceptions(False)


def opc(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    i = 0
    for line in lines:
        if not line.startswith('OPC ('):
            continue
        args = line[5:].split(')', 2)[0].split(',', 3)
        name = args[0].strip('\t ')
        label = args[1].strip('\t ')
        print('Opc%s("%s"), // %d' % (name, label, i))
        i += 1


def opc_const_deserialize(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    for line in lines:
        if not line.startswith('OPC ('):
            continue
        parse = line[5:].split(')', 2)[0].split(',', 3)
        fun = parse[1].strip('\t ')
        if not fun.startswith('$') or len(fun) <= 1:
            continue
        fun = '_'.join(p[0]+p[1:].lower() for p in fun[1:].split('_'))
        name = parse[0].strip('\t ')
        print('case OPC.Opc%s:return new d%s(b);' % (name, fun))


def opc_const_class(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    for line in lines:
        if not line.startswith('OPC ('):
            continue
        parse = line[5:].split(')', 2)[0].split(',', 3)
        fun = parse[1].strip('\t ')
        if not fun.startswith('$') or len(fun) <= 1:
            continue
        fun = '_'.join(p[0]+p[1:].lower() for p in fun[1:].split('_'))
        name = parse[0].strip('\t ')
        print('public static final class d%s extends CONST{' % (fun,))
        print('public d%s(){super(OPC.Opc%s);}' % (fun, name))
        print('public d%s(final ByteBuffer b){super(b);}}' % (fun,))


def binary(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    go = False
    i = 0
    print('private static final op_rec[] binary    = new op_rec[]{//')
    for line in lines:
        if not go:
            if line.find('binary[]') >= 0:
                go = True
            continue
        if line.strip(' \n\t{') == '':
            continue
        args = line.split('}', 2)[0].strip('\t ').split(',')
        symbol = ','.join(args[0:-3])
        if symbol == '0':
            symbol = 'null'
        opcode = args[-3].strip(' ')
        if opcode == '0':
            break
        prec = int(args[-2])
        lorr = int(args[-1])
        print('new op_rec(%s,OPC.%s,(byte)%d,(byte)%d),//%d' %
              (symbol, opcode, prec, lorr, i))
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
            if line.find('unary[]') >= 0:
                go = True
            continue
        if line.strip(' \n\t{') == '':
            continue
        args = line.split('}', 2)[0].strip('\t ').split(',')
        symbol = ','.join(args[0:-3])
        if symbol == '0':
            symbol = 'null'
        opcode = args[-3].strip(' ')
        if opcode == '0':
            break
        prec = int(args[-2])
        lorr = int(args[-1])
        print('new op_rec(%s,OPC.%s,(byte)%d,(byte)%d),//%d' %
              (symbol, opcode, prec, lorr, i))
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
            if line.find('TdiREF_CAT[]') >= 0:
                go = True
            continue
        if line.strip(' \n\t{') == '':
            continue
        if line.strip(' \n\t{') == '};':
            break
        args = line.split('}', 2)[0].strip('{\t ').split(',')
        name = args[0]
        cat = args[1].strip(' ')
        if name == '"P"':
            length = 8
        else:
            length = int(args[2])
        digits = int(args[3])
        fname = args[4].strip(' ')
        if fname == '0':
            fname = 'null'
        elif fname == 'F_SYM':
            fname = '"F"'
        elif fname == 'FS_SYM':
            fname = '"E"'
        elif fname == 'FT_SYM':
            fname = '"D"'
        elif fname == 'G_SYM':
            fname = '"G"'
        elif fname == 'D_SYM':
            fname = '"V"'
        print('new def_cat(%s,(short)%s,(byte)%d,(byte)%d,%s),//%d' %
              (name, cat, length, digits, fname, i))
        i += 1
    print('};')


def reffun(filepath):
    def repio(i):
        if(i == 'VV'):
            return 'BU'
        if(i == 'XX'):
            return 'T'
        if(i == 'YY'):
            return 'HC'
        if(i == 'SUBSCRIPT'):
            return 'L'
        if(i == 'UNITS'):
            return 'T'
        return i

    def reptoken(i):
        return i.replace('OK', '0')
    with file(filepath) as f:
        lines = f.readlines()
    print('private static final ref_fun[] tdiRefFun = new ref_fun[]{//')
    for line in lines:
        if not line.startswith('OPC'):
            continue
        name, symbol, f1, f2, f3, i1, i2, o1, o2, m1, m2, token = tuple(a.strip(
            '\t ') for a in line.split('(', 2)[1].split(')', 2)[0].strip('{\t ').split(',', 12))
        i1, i2, o1, o2 = map(repio, (i1, i2, o1, o2))
        token = reptoken(token)
        print('new ref_fun("%s","%s",%s,%s,%s,DTYPE.%s,DTYPE.%s,DTYPE.%s,DTYPE.%s,%s,%s,%s),//' %
              (name, symbol, f1, f2, f3, i1, i2, o1, o2, m1, m2, token))
    print('};')


def constant(filepath):
    with file(filepath) as f:
        lines = f.readlines()
    for line in lines:
        line = line.split('/*', 2)[0].strip(' \t\n\r')
        if line.startswith('DATUM('):
            dtype, x, data = line[6:-1].split(', ')
            if dtype == 'FLOAT':
                print("case OPC.Opc%s:return new Float32(%sf);" %
                      (x, data[7:]))
            elif dtype == 'BU':
                print("case OPC.Opc%s:return new Uint8((byte)%s);" % (x, data))
            elif dtype == 'FLOAT_COMPLEX':
                print("case OPC.Opc%s:return new Complex32(0.f, 1.f);" % (x,))
            elif dtype == 'MISSING':
                print("case OPC.Opc%s:return Missing.NEW;" % (x,))
            else:
                print(
                    "case OPC.Opc%s:return Missing.NEW;//TODO:set correct return value" % (x,))
        if line.startswith('UNITS('):
            dtype, x, data, units = line[6:-1].split(', ')
            if dtype == 'FLOAT':
                print('case OPC.Opc%s:return new With_Units(new Float32(%sf),new CString(%s));' % (
                    x, data[7:], units))
            else:
                print(
                    "case OPC.Opc%s:return Missing.NEW;//TODO:set correct return value" % (x,))
        if line.startswith('UERR('):
            dtype, x, data, error, units = line[5:-1].split(', ')
            if dtype == 'FLOAT':
                print('case OPC.Opc%s:return new With_Units(new With_Error( new Float32(%sf), new Float32(%sf)),new CString(%s));' % (
                    x, data[7:], error[7:], units))
            else:
                print(
                    "case OPC.Opc%s:return Missing.NEW;//TODO:set correct return value" % (x,))


exceptions()
# reffun('C:/Git/mdsplus/include/opcbuiltins.h')
# opc('C:/Git/mdsplus/include/opcbuiltins.h')
# opc_const_class('C:/Git/mdsplus/include/opcbuiltins.h')
# opc_const_deserialize('C:/Git/mdsplus/include/opcbuiltins.h')
# binary('C:/Git/mdsplus/tdishr/TdiDecompileR.c')
# unary('C:/Git/mdsplus/tdishr/TdiDecompileR.c')
# defcat('C:/Git/mdsplus/tdishr/TdiDefCat.c')
# constant('C:/Git/mdsplus/tdishr/TdiConstant.c')
