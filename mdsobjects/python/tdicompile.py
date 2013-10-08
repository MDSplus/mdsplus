from compound import *
from mdsarray import *
from mdsscalar import *
from tree import *
from ident import *
from treenode import *
from event import *
from builtins import Builtin
from _tdishr import TdiDecompile
import sys,os
optimized=1

class _replacementArgs(object):
    def __init__(self,args):
        self.args=args
        self.idx=1

def tdiCompile(text,replacementArgs=_replacementArgs(())):
    import lex
    if isinstance(replacementArgs,tuple):
        return tdiCompile(text,_replacementArgs(replacementArgs))
    elif not isinstance(replacementArgs,_replacementArgs):
        raise Exception("Second argument to tdiCompile, if suppied, must by a tupple")

### Lexical Tokens
    tokens=['PLUS','MINUS','TIMES','DIVIDE','EQUAL','EQUALS',
            'LPAREN','RPAREN','LBRACE','RBRACE','LBRACKET','RBRACKET','COMMA',
            'BU','B','WU','W','LU','L','QU','Q','FloatNum','T','T2','IDENT','PLACEHOLDER',
            'NAME','ARROW','GREATER','LESS','RAISE','GREATER_EQUAL',
            'LESS_EQUAL','NOT_EQUAL','QUESTION','COLON','LSHIFT','RSHIFT',
            'SEMICOLON','IAND','AND','NOT','PLUSPLUS','MINUSMINUS',
            'SLASHSLASH','IOR','OR','INOT','EQUALSFIRST','TREEPATH','BACKQUOTE',
            ]
### Reserved keywords

    reserved = {'if':'IF','else':'ELSE','public':'IDENTTYPE',
                'private':'IDENTTYPE','fun':'FUN','in':'ARGTYPE',
                'out':'ARGTYPE','inout':'ARGTYPE','optional':'ARGTYPE',
                'as_is':'ARGTYPE','switch':'SWITCH','case':'CASE',
                'for':'FOR','while':'WHILE','break':'BREAK',
                'continue':'CONTINUE','not':'NOT_S','and':'AND_S','or':'OR_S',
                'nor':'NOR_S','mod':'MOD_S','eq':'EQ_S','ne':'NE_S','gt':'GT_S',
                'ge':'GE_S','lt':'LT_S','le':'LE_S','default':'DEFAULT',
                }
    tokens += list(set(reserved.values()))

### ignore comments denoted by /* ..... */  NOTE: Nested comments allowed which required the states trick

    states = (('nestcomment','exclusive'),)

    def t_nestcomment_comment(t):
        r'(.|\n)*?(\*/|/\*)'
        if t.value[-2:]=='/*':
            t.lexer.push_state('nestcomment')
        else:
            t.lexer.pop_state()

    def t_COMMENT(t):
        r'(/\*(.|\n)*?(\*/|/\*))'
        if t.value[-2:]=='/*':
            t.lexer.push_state('nestcomment')
            t.lexer.push_state('nestcomment')

### integer token including hex,binary,octal and decimal
    integer=r'0[Xx][0-9A-Fa-f]+|0[Bb][01]+|0[0-7]+|[1-9]+[0-9]*|0'

    def fix_backquotes(in_str):
        import re
        def replace_backquote_string(match):
            mstr=match.group(0)
            if len(mstr)>4:
                ans = mstr
            elif mstr[1] == '\\':
                ans=mstr
            elif mstr[1] in 'mntr':
                ans=eval("'"+mstr+"'")
            else:
                ans = chr(int(mstr[1:],8))
            return ans
        ans = re.sub(r'\\[0-7]+|\\[\\mntr]',replace_backquote_string,in_str)
        return ans

### string token with double quotes converted to String() instance
    @lex.TOKEN(r'"(?:[^"\\]|\\.)*"')
    def t_T(t):
        t.value=String(fix_backquotes(t.value).replace('\\"','"').replace("\\'","'").replace('\\\\','\\')[1:-1])
        return t

### string token with single quotes converted to String() instance
    @lex.TOKEN(r"'(?:[^'\\]|\\.)*'")
    def t_T2(t):
        t.value=String(fix_backquotes(t.value).replace("\\'","'").replace('\\"','"').replace('\\\\','\\')[1:-1])
        return t

### unsigned byte token converted to Uint8() instance
    @lex.TOKEN(r'(?i)(byte_unsigned|unsigned_byte)\((?P<number1>('+integer+r'))\)|(?P<number2>('+integer+r'))(bu|ub)')
    def t_BU(t):        
        t.value=Uint8(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### unsigned word converted to Uint16() instance
    @lex.TOKEN(r'(?i)(word_unsigned|unsigned_word)\((?P<number1>('+integer+r'))\)|(?P<number2>('+integer+r'))(wu|uw)')
    def t_WU(t):
        t.value=Uint16(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### signed word converted to Int16() instance
    @lex.TOKEN(r'(?i)word\((?P<number1>('+integer+r'))\)|(?P<number2>('+integer+r'))w')
    def t_W(t):
        t.value=Int16(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### unsigned quadword converted to Uint64() instance
    @lex.TOKEN(r'(?i)(quadword_unsigned|unsigned_quadword)\((?P<number1>('+integer+r'))\)|(?P<number2>('+integer+r'))(uq|qu)')
    def t_QU(t):
        t.value=Uint64(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### unsigned int converted to Uint32() instance
    @lex.TOKEN(r'(?i)(long_unsigned|unsigned_long)\((?P<number1>('+integer+r'))\)|(?P<number2>('+integer+r'))(lu|ul|u)')
    def t_LU(t):
        t.value=Uint32(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### signed quadword converted to Int64() instance
    @lex.TOKEN(r'(?i)quadword\((?P<number1>('+integer+r'))\)|(?P<number2>('+integer+r'))q')
    def t_Q(t):
        t.value=Int64(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### Float instance converted to either Float32() or Float64() instance
    @lex.TOKEN(r'(?i)([0-9]+\.(?!\.)[0-9]*|[0-9]*\.[0-9]+|[0-9]+)(?P<exp>([dgef]))[-+]?[0-9]+|[0-9]+\.(?!\.)[0-9]*|[0-9]*\.[0-9]+')
    def t_FloatNum(t):
        exp=t.lexer.lexmatch.group('exp')
        if exp is not None:
            exp=exp.lower()
        val=t.value.lower().replace('d','e').replace('g','e').replace('f','e')
        if exp is None or exp == 'e' or exp == 'f':
            t.value=Float32(val)
        else:
            t.value=Float64(val)
            if 'inf' in repr(t.value.data()):
                t.value=Float32(val)
        return t

### signed byte converted to Int8() instance
    @lex.TOKEN(r'(?i)byte\((?P<number1>('+integer+'))\)|(?P<number2>('+integer+'))b')
    def t_B(t):
        t.value=Int8(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### signed int converted to Int32() instances. NOTE must be end of the scalar tokens to work for some reason.
    @lex.TOKEN(r'(?i)long\((?P<number1>('+integer+'))\)|(?P<number2>('+integer+'))l?')
    def t_L(t):
        t.value=Int32(int(t.lexer.lexmatch.group('number1') or t.lexer.lexmatch.group('number2'),0))
        return t

### Ident or builtin constant converted to either Ident() instance or a Builtin() instance for constants such as $PI
    @lex.TOKEN(r'(?i)(\$([a-z]+[a-z0-9_\$]*)|([0-9]+[a-z_\$]+[a-z0-9_\$]*))|(_[a-z0-9_\$]*)')
    def t_IDENT(t):
        if t.value.lower()=="$roprand":
            import numpy as np
            t.value=np.frombuffer(np.getbuffer(np.int32(2147483647)),dtype=np.float32)[0]
        else:
            try:
                t.value=Builtin(t.value,())
            except Exception:
                t.value=Ident(t.value)
        return t

### Placeholders
    @lex.TOKEN(r'\$[1-9]*[0-9]*')
    def t_PLACEHOLDER(t):
        if len(t.value)==1:
            idx=replacementArgs.idx
        else:
            idx=int(t.value[1:])
        if idx <= len(replacementArgs.args):
            t.value=makeData(replacementArgs.args[idx-1])
        else:
            raise Exception('%TDI-E-TdiMISS_ARG, Missing argument is required for function')
        replacementArgs.idx=idx+1
        return t

### Tree path \[treename::]tagname[.|:]node[.|:]node...
    pname=r'[a-z][a-z0-9$_]*'
    @lex.TOKEN(r'(?i)(((\\('+pname+r'::)?|[\.:])?'+pname+r')|(\.-(\.?-)*))([\.:]'+pname+r')*')
    def t_TREEPATH(t):
        if t.value.lower() in reserved:
            t.type=reserved[t.value.lower()]
        else:
            import re
            original_value=t.value
            if re.match(r'[\s]*(\(|->)',t.lexer.lexdata[t.lexer.lexpos:]) is not None:
                skip=t.value.find(':')
                if skip == 0:
                    t.lexer.lexpos=t.lexer.lexpos-len(t.value)+1
                    t.type='COLON'
                    t.value=':'
                else:
                    if skip > -1:
                        t.lexer.lexpos=t.lexer.lexpos-len(t.value)+skip
                        t.value=t.value[0:skip]
                    t.type='NAME'
            else:
                try:
                    t.value=Tree().getNode(t.value)
                except:
                    if t.value[0] in '.:':
                        t.value='\\'+Tree().tree+'::TOP'+t.value
                    elif t.value[0] == '\\':
                        if t.value.find('::') == -1:
                            t.value='\\'+Tree().tree+'::'+t.value[1:]
                    else:
                        t.value='\\'+Tree().tree+'::TOP:'+t.value
                    t.value=TreePath(t.value.upper())
                t.value.original_value=original_value
        return t

### Various operators
    t_PLUS    = r'\+'
    t_MINUS   = r'-'
    t_TIMES   = r'\*'
    t_DIVIDE  = r'/'
    t_EQUALS  = r'=='
    t_EQUAL   = r'='
    t_LPAREN  = r'\('
    t_RPAREN  = r'\)'
    t_LBRACE  = r'{'
    t_RBRACE  = r'}'
    t_LBRACKET = r'\['
    t_RBRACKET = r'\]'
    t_COMMA   = r','
    t_ARROW   = r'->'
    t_GREATER = r'>'
    t_GREATER_EQUAL = r'>='
    t_LESS    = r'<'
    t_LESS_EQUAL = r'<='
    t_NOT_EQUAL = r'!=|<>'
    t_RAISE   = r'\^|\*\*'
    t_QUESTION = r'\?'
    t_LSHIFT = r'<<'
    t_RSHIFT = r'>>'
    t_SEMICOLON = r';'
    t_IAND = r'&'
    t_AND = r'&&'
    t_NOT = r'!'
    t_PLUSPLUS = r'\+\+'
    t_MINUSMINUS = r'--'
    t_SLASHSLASH = r'//'
    t_IOR = r'\|'
    t_OR = r'\|\|'
    t_INOT = r'~'
    t_EQUALSFIRST = r'\+=|-=|\*=|/=|\^=|\*\*=|<==|>==|>>=|<<=|&=|&&=|!==|\|=|\|\|=|//='
    t_BACKQUOTE = r'`'


    def t_COLON(t):
        r'\.\.|:'
        t.value=':'
        return t
        

### Name token which begins with an alpha followed by zero or more of aphanumeric or underscore
### or a reserved word token such as if, while, switch, for ...
    def t_NAME(t):
        r'(?i)\b[a-z]+[a-z0-9_]*\b'
        t.type = reserved.get(t.value.lower(),'NAME')
        return t


# Define a rule so we can track line numbers
    def t_newline(t):
        r'\n+'
        t.lexer.lineno += len(t.value)


# Error handling rule
    def t_ANY_error(t):
        print( "Illegal character '%s'(%d) at line %d around '%s'" % (t.value[0],ord(t.value[0]),t.lexer.lineno,t.lexer.lexdata[t.lexer.lexpos-10:t.lexer.lexpos+10]))
#        t.lexer.skip(1)

# A string containing ignored characters (spaces and tabs)
    t_ANY_ignore  = ' \t\r\0'


# Build the lexer

    lex.lex(debug=0,optimize=optimized,lextab='tdilextab')

    precedence = (
        ('right','EQUAL'),
        ('right','COMMA'),
        ('left','COLON'),
        ('left','QUESTION'),
        ('left','OR','AND','OR_S','AND_S'),
        ('left','GREATER','GREATER_EQUAL','LESS','LESS_EQUAL','EQUALS','NOT_EQUAL','GT_S','GE_S','LT_S','LE_S','EQ_S','NE_S'),
        ('left','SLASHSLASH'),
        ('left','PLUS','MINUS','IOR','IAND'),
        ('left','TIMES','DIVIDE'),
        ('left','RAISE','MOD_S'),
        ('right','RSHIFT','LSHIFT','UNOP'),
        ('left','LBRACKET','LPAREN','IDENTTYPE'),
        )

    def p_compilation(t):
        """compilation : statements\n| operand\n | operand SEMICOLON
        """
        t[0]=t[1]
        if isinstance(t[0],Builtin) and len(t[0].args)==2 and isinstance(t[0].args[0],String) and isinstance(t[0].args[1],String):
            t[0]=String(str(t[0].args[0])+str(t[0].args[1]))

### operands can be arguments to operators
    def p_operand(t):
        """operand : scalar\n| operation\n| parenthisized_operand\n| ident\n| vector\n| TREEPATH"""
        t[0]=t[1]

### Subscripting (i.e. _a[32])
    def p_subscript(t):
        """operation : operand vector"""
        if len(t) == 2:
            t[0]=t[1]
        else:
            args=[t[1],]
            if isinstance(t[2],Builtin):
                for arg in t[2].args:
                    args.append(arg)
            else:
                for arg in t[2]:
                    args.append(arg)
            t[0]=Builtin('subscript',tuple(args))

### parenthisized operands such as (1+2) for specifying things like (1+2)*10
    def p_parenthisized_operand(t):
        'parenthisized_operand : LPAREN operand RPAREN'
        t[0]=t[2]

### Basic scalars supported by MDSplus
    def p_scalar(t):
        'scalar : BU \n| B \n| WU \n| W \n| LU \n| L \n| QU \n| Q \n| FloatNum \n| T \n| T2 \n| missing'
        t[0]=t[1]

### Ken variable (i.e. _gub or public _gub)
    def p_ident(t):
        """ident : IDENT\n| PLACEHOLDER\n| IDENTTYPE IDENT"""
        if len(t) == 2:
            t[0]=t[1]
        else:
            t[0]=Builtin(t[1],(str(t[2]),))

### Missing value specified by asterisk
    def p_missing(t):
        'missing : TIMES'
        t[0]=makeData(None)

### Range constructor (a : b [:c])
    def p_range(t):
        """range : range COLON operand\n| operand COLON operand"""
        if isinstance(t[1],list):
            t[1].append(t[3])
            t[0]=t[1]
        else:
            t[0]=[t[1],t[3]]

    def p_op_range(t):
        """operation : range"""
        t[0]=Range(tuple(t[1]))

### Loop control operations (i.e. break, continue)
    def p_loop_control(t):
        'operation : BREAK\n| CONTINUE'
        t[0]=Builtin(t[1],tuple())

### Unary arithmetic operations such as ~a, -a
    def p_unaryop(t):
        """operation : NOT operand %prec UNOP\n| INOT operand %prec UNOP\n| MINUS operand %prec UNOP\n| PLUS operand %prec UNOP
        | NOT_S operand %prec UNOP"""
        ops = {'!':'NOT','~':'INOT','-':'UNARY_MINUS','not':'NOT','+':'UNARY_PLUS'}
        if t[1]=='-' and isinstance(t[2],Scalar):
            t[0]=makeData(-t[2].data())
        elif t[1]=='+' and isinstance(t[2],Scalar):
            t[0]=t[2]
        else:
            t[0]=Builtin(ops[t[1].lower()],(t[2],))

### Binary arithmetic operations such as a+b a>=b a^b a&&b
    def p_binop(t):
        """operation : operand PLUS operand
        | operand MINUS operand\n| operand TIMES operand\n| operand DIVIDE operand
        | operand RAISE operand\n| operand RSHIFT operand\n| operand LSHIFT operand
        | operand LESS operand\n| operand GREATER operand\n| operand LESS_EQUAL operand
        | operand GREATER_EQUAL operand\n| operand EQUALS operand \n| operand IAND operand
        | operand AND operand \n| operand OR operand \n| operand NOT_EQUAL operand
        | operand IOR operand\n| operand AND_S operand \n| operand OR_S operand\n| operand NOR_S operand
        | operand MOD_S operand
        | MOD_S LPAREN operand COMMA operand RPAREN
        | operand GT_S operand\n| operand GE_S operand\n| operand LT_S operand\n| operand LE_S operand
        | operand EQ_S operand\n| operand NE_S operand
        """
        ops = {'+':'add','-':'subtract','*':'multiply','/':'divide','<':'lt',
               '>':'gt','^':'power','**':'power','<=':'le','>=':'ge','==':'eq',
               '>>':'shift_right','<<':'shift_left','&':'iand','&&':'and','!=':'NE','<>':'NE',
               '|':'ior','||':'or','and':'and','or':'or','nor':'nor','mod':'MOD',
               'gt':'gt','ge':'ge','lt':'lt','le':'le','eq':'eq','ne':'ne'}
        if len(t)==4:
            t[0]=Builtin(ops[t[2].lower()],(t[1],t[3]))
        else:
            t[0]=Builtin(ops[t[1].lower()],(t[3],t[5]))

### Concatenation operator a // b [// c]
### Jump through hoops to emulate weird tdi behavior which concatenates string types at compile time except for the
### caveat that if the number of concatenation arguments is even and all strings concatenate the first n-1 and
### then make a concat function that concats the first n-1 with the nth, other wise concat them all. If any of the
### items being concatenated is not a string then don't concat anything at run time.
    class Concat(list):
        def get(self):
            compile_time_concat=True
            for arg in self:
                if not isinstance(arg,(str,String)):
                    compile_time_concat=False
                    break
            if compile_time_concat:
                c=list()
                c.append(self[0])
                if len(self) % 2 == 0:
                    for arg in self[1:-1]:
                        c[-1]=str(c[-1])+str(arg)
                    c.append(self[-1])
                else:
                    for arg in self[1:]:
                        c[-1]=String(str(c[-1])+str(arg))
                if len(c)>1:
                    return Builtin('concat',tuple(c))
                else:
                    return c[0]
            else:
                return Builtin('concat',tuple(self))

    def p_concat(t):
        'concat : operand SLASHSLASH operand\n| concat SLASHSLASH operand\n operation : concat'
        if len(t)==4:
            if isinstance(t[1],Concat):
                t[1].append(t[3])
                t[0]=t[1]
            else:
                t[0]=Concat([t[1],t[3]])
        else:
            t[0]=t[1].get()
            if isinstance(t[0],String):
                t.type='scalar'

### Conditional operation (i.e. a ? b : c)
    def p_conditional(t):
        'operation : operand QUESTION operand COLON operand'
        t[0]=Builtin('conditional',(t[3],t[5],t[1]))

### Ident increment/decrement (i.e. _i++, _i--, ++_i, --_i)
    def p_inc_dec(t):
        """operation : ident PLUSPLUS\n| ident MINUSMINUS\n| PLUSPLUS ident\n| MINUSMINUS ident"""
        op={'++':'_inc','--':'_dec'}
        if isinstance(t[1],str):
            t[0]=Builtin('pre'+op[t[1]],(t[2],))
        else:
            t[0]=Builtin('post'+op[t[2]],(t[1],))

### Ken variable assignment (i.e. _i=1)
    def p_assignment(t):
        'operation : operand EQUAL operand %prec EQUAL'
        t[0]=Builtin('EQUALS',(t[1],t[3]))

### Argument list for function calls (i.e. ([a[,b[,c]]])  )
    def p_arglist(t):
        """arglist : LPAREN args RPAREN\n args :\n| args operand\n| args COMMA\n| args ARGTYPE LPAREN operand RPAREN"""
        if len(t)==4:
            t[0]=t[2]
        elif len(t)==1:
            t[0]=list()
        else:
            if len(t)==6:
                t[2]=Builtin(t[2],(t[4],))
            if isinstance(t[2],str):
                if len(t[1])==0:
                    t[1].append(None)
                t[1].append(None)
            else:
                if len(t[1]) > 0 and (t[1][-1] is None or isinstance(t[1][-1],EmptyData)):
                    t[1][-1]=t[2]
                else:
                    t[1].append(t[2])
            t[0]=t[1]

### Function call (i.e. gub(1,2,,3)) also handles build_xxx() and make_xxx() operations
    def p_function(t):
        """operation : NAME arglist\n| EQ_S arglist\n| NE_S arglist\n| LE_S arglist
        | LT_S arglist\n| GT_S arglist\n| GE_S arglist"""

        def doBuild(name,args):
            def build_with_units(args):
                args[0].units=args[1]
                return args[0]

            def build_with_error(args):
                args[0].error=args[1]
                return args[0]

            def build_param(args):
                try:
                    args[0].help=args[1]
                    args[0].validation=args[2]
                except:
                    pass
                return args[0]

            def build_slope(args):
                new_args=list()
                if len(args)>1:
                    new_args.append(args[1])
                else:
                    new_args.append(None)
                if len(args)>2:
                    new_args.append(args[2])
                else:
                    new_args.append(None)
                new_args.append(args[0])
                return Range(tuple(new_args))

            def buildPath(args):
                if isinstance(args[0],(str,String)):
                    name=str(args[0])
                    if len(name) > 1 and name[0:2]=='\\\\':
                        name=name[1:]
                    ans = TreePath(name)
                else:
                    ans = Builtin('build_path',args)
                return ans
            
            def buildCall(args):
                ans=Call(args[1:])
                ans.retType=args[0]
                return ans

### retain original node specifiers when building a using function
            def buildUsing(args_in):
                def restoreTreePaths(arg):
                    if isinstance(arg,Compound):
                        args=list()
                        for a in arg.args:
                            args.append(restoreTreePaths(a))
                        arg.args=tuple(args)
                        ans = arg
                    elif isinstance(arg,(TreePath,TreeNode)) and hasattr(arg,'original_value'):
                        ans = TreePath(arg.original_value)
                    else:
                        ans = arg
                    return ans

                args=list()
                for arg in args_in:
                    args.append(restoreTreePaths(arg))
                ans = Builtin('using',tuple(args))
                return ans

            known_builds={'BUILD_ACTION':Action,
                          #BUILD_CONDITION':Condition,
                          'BUILD_CONGLOM':Conglom,
                          'BUILD_DEPENDENCY':Dependency,
                          'BUILD_DIM':Dimension,
                          'BUILD_DISPATCH':Dispatch,
                          'BUILD_EVENT':Event,
                          'BUILD_FUNCTION':Builtin,
                          'BUILD_METHOD':Method,
                          'BUILD_PARAM':build_param,
                          'BUILD_PROCEDURE':Procedure,
                          'BUILD_PROGRAM':Program,
                          'BUILD_RANGE':Range,
                          'BUILD_ROUTINE':Routine,
                          'BUILD_SIGNAL':Signal,
                          'BUILD_SLOPE':build_slope,
                          'BUILD_WINDOW':Window,
                          'BUILD_WITH_UNITS':build_with_units,
                          'BUILD_CALL':buildCall,
                          'BUILD_WITH_ERROR':build_with_error,
                          'BUILD_OPAQUE':Opaque,
                          'BUILD_PATH':buildPath,
                          'USING':buildUsing,}
            return known_builds[name.upper()](args)

        def doMake(name,args):
            for arg in args:
                if not isinstance(arg,(Array,Scalar,EmptyData)) and arg is not None:
                    raise Exception('use make opcode')
            name=name.upper().replace('MAKE_','BUILD_')
            if 'BUILD_' in name:
                return doBuild(name,tuple(args))
            else:
                raise Exception("not a make_ call")

        try:
            t[0]=doBuild(t[1],tuple(t[2]))
        except Exception:
            try:
                t[0]=doMake(t[1],tuple(t[2]))
            except Exception:
                try:
                    numbers=['byte','byte_unsigned','unsigned_byte','word','word_unsigned','unsigned_word',
                             'long','long_unsigned','unsigned_long','quadword','quadword_unsigned','unsigned_quadword',
                             'float','double','f_float','g_float','d_float','fs_float','ft_float']
                    if t[1].lower() in numbers and (isinstance(t[2][0],Scalar) or isinstance(t[2][0],Array)):
                        t[0]=Data.evaluate(Builtin(t[1],tuple(t[2])))
                    else:
                        t[0]=Builtin(t[1],tuple(t[2]))
                except Exception:
                    t[0]=Builtin('ext_function',tuple([None,t[1]]+t[2]))

### call library (i.e. library->gub(a,b,c))
    def p_rettype(t):
        'rettype : COLON NAME'
        rettypes={'bu':2,'wu':3,'lu':4,'qu':5,'b':6,'w':7,'l':8,'q':9,'f':10,'d':11,'fc':12,'dc':13,'t':14,
                  'dsc':24,'p':51,'f':52,'fs':52,'ft':53,'fsc':54,'ftc':55}
        if t[2].lower() in rettypes:
            t[0]=rettypes[t[2].lower()]

    def p_call(t):
        """operation : NAME ARROW NAME arglist\n| NAME ARROW NAME rettype arglist"""
        if len(t)==5:
            t[0]=Call(tuple([t[1],t[3]]+t[4]))
        else:
            t[0]=Call(tuple([t[1],t[3]]+t[5]),opcode=t[4])

### Loop and fun statements found inside braces and sometimes in parens
    def p_optional_semicolon(t):
        """optional_semicolon : SEMICOLON\n| empty"""
        pass

    class CommaList(list):
        def get(self):
            return Builtin('comma',tuple(self))

    def p_statement(t):
        """statement : operand SEMICOLON\n| comma_list SEMICOLON\n| comma_list\n| operand\n| SEMICOLON
        """
        if isinstance(t[1],str):
            pass
        elif isinstance(t[1],CommaList):
            t[0]=t[1].get()
        else:
            t[0]=t[1]

    def p_statements(t):
        """statements : statement\n| statements statement\n| statements braced_statements"""
        if len(t)==2:
            t[0]=Builtin('statement',(t[1],))
        else:
            if t[2] is None:
                t[0]=t[1]
            elif len(t[1].args) < 250:
                t[1].args=tuple(list(t[1].args)+[t[2]])
                t[0]=t[1]
            else:
                t[0]=Builtin('statement',(t[1],t[2]))

    def p_braced_statements(t):
        """braced_statements : LBRACE statements RBRACE optional_semicolon\n | LBRACE RBRACE optional_semicolon"""
        if len(t)==5:
            if len(t[2].args)==1:
                t[0]=t[2].args[0]
            else:
                t[0]=t[2]
        else:
            pass

### paren statement list as in if_error(_a,(_a=1;_b++),42)
    def p_statement_list(t):
        'operation : LPAREN statements RPAREN'
        if len(t[2].args)==1:
            t[0]=t[2].args[0]
        else:
            t[0]=t[2]

### comma operand list as in _a=1,_b=2,3
    def p_comma_list(t):
        """comma_list : COMMA\n| operand COMMA\n| comma_list COMMA\n| comma_list operand"""
        if isinstance(t[1],CommaList):
            if isinstance(t[2],str):
                if t[1].lastNone:
                    t[1].append(None)
            else:
                t[1].append(t[2])
                t[1].lastNone=False
            t[0]=t[1]
        else:
            t[0]=CommaList()
            if len(t)==2:
                t[0].append(None)
                t[0].lastNone=True
            else:
                t[0].append(t[1])
                t[0].lastNone=False

### comma operation as in (_a=1,_b=2,3)
    def p_comma_list_operation(t):
        'operation : LPAREN comma_list RPAREN'
        t[0]=t[2].get()

    def p_empty(t):
        'empty :'
        pass

### For statement (i.e. for (_x=1;_x<10;_x++){statements...} or for (...) statement
    def p_optional_comma_list(t):
        """optional_operand : comma_list\n| operand\n| empty"""
        if isinstance(t[1],CommaList):
            t[0]=t[1].get()
        else:
            t[0]=t[1]

    def p_for(t):
        """operation : FOR LPAREN optional_operand SEMICOLON operand SEMICOLON optional_operand RPAREN braced_statements
        | FOR LPAREN optional_operand SEMICOLON operand SEMICOLON optional_operand RPAREN statement"""
        t[0]=Builtin('for',(t[3],t[5],t[7],t[9]))

### If statement (i.e. if (_x<10) {_x=42;} else {_x=43;})
    def p_if_begin(t):
        """if_begin : IF LPAREN operand RPAREN"""
        t[0]=t[3]

    def p_ifelse_body(t):
        """ifelse_body : braced_statements\n| statement"""
        t[0]=t[1]

    def p_if(t):
        """operation : if_begin ifelse_body\n| if_begin ifelse_body ELSE ifelse_body"""
        args=[t[1],t[2]]
        if len(t) > 3:
            args.append(t[4])
        t[0]=Builtin('if',tuple(args))

### While statement (i.e. while(expression){statements;} )
    def p_while(t):
        """operation : WHILE LPAREN operand RPAREN braced_statements
        | WHILE LPAREN operand RPAREN statement"""
        t[0]=Builtin('while',(t[3],t[5]))

### FUN definition (i.e. public fun gub(args){statements} )
    def p_fun_arg(t):
        """fun_arg : ARGTYPE IDENT\n| ARGTYPE ARGTYPE IDENT\n| IDENT\n| ARGTYPE LPAREN IDENT RPAREN\n| ARGTYPE ARGTYPE LPAREN IDENT RPAREN"""
        if len(t) == 2:
            t[0]=t[1]
        elif len(t) == 3:
            t[0]=Builtin(t[1],(str(t[2]),))
        elif len(t) == 4:
            t[0]=Builtin(t[1],(Builtin(t[2],(str(t[3]),)),))
        elif len(t) == 5:
            t[0]=Builtin(t[1],(t[3],))
        else:
            t[0]=Builtin(t[1],(Builtin(t[2],(t[4],)),))

    def p_fun_args(t):
        """fun_args : LPAREN\n| fun_args fun_arg\n| fun_args COMMA\n| fun_args RPAREN"""
        if len(t)==2:
            t[0]=list()
        elif isinstance(t[2],str):
            t[0]=t[1]
        else:
            t[1].append(t[2])
            t[0]=t[1]
                              
    def p_fun(t):
        """operation : IDENTTYPE FUN NAME fun_args braced_statements
        | FUN IDENTTYPE NAME fun_args braced_statements
        | FUN NAME fun_args braced_statements"""
        args=list()
        if len(t) == 6:
            if t[1].lower() == 'fun':
                itype=t[2]
            else:
                itype=t[1]
            args.append(Builtin(itype,(t[3],)))
            args.append(t[5])
            for arg in t[4]:
                args.append(arg)
        else:
            args.append(t[2])
            args.append(t[4])
            for arg in t[3]:
                args.append(arg)
        t[0]=Builtin('fun',tuple(args))

### Vector/Array declarations (i.e. [_a,_b,_c] or [1,2,3,])
    def p_vector(t):
        """vector_part : LBRACKET operand
        | LBRACKET
        | vector_part COMMA operand
        vector : vector_part RBRACKET"""
        if isinstance(t[1],str):
            if len(t)==2:
                t[0]=Builtin('vector',tuple())
                t[0].isarray=True
            else:
                t[0]=Builtin('vector',(t[2],))
                t[0].isarray = isinstance(t[2],Scalar) or isinstance(t[2],Array)
        elif t[2] == ',':
            args=list(t[1].args)
            if len(args) > 250:
                args=[Builtin('vector',tuple(args)),t[3]]
            else:
                args.append(t[3])
            t[1].args=tuple(args)
            t[0]=t[1]
            t[0].isarray = t[1].isarray and (isinstance(t[3],Scalar) or isinstance(t[3],Array))
        else:
            if t[1].isarray:
                t[0]=Data.evaluate(t[1])
            else:
                t[0]=Builtin('vector',t[1].args)

### Switch statement (i.e. switch(_a) {case(42) {statements} case(43) {statements}} )
    def p_case(t):
        """case : CASE LPAREN operand RPAREN braced_statements\n| CASE LPAREN operand RPAREN statement
        | CASE LPAREN operand RPAREN\n| CASE DEFAULT braced_statements
        | CASE DEFAULT statement\n| statement"""
        if len(t)==4:
            t[0]=Builtin('default',(t[3],))
        elif len(t)==5:
            t[0]=Builtin('case',(None,None))
            t[0].args=(t[3],)
            t[0].doAppendCase=True
        elif len(t)==6:
            t[0]=Builtin('case',(t[3],t[5]))
        else:
            t[0]=t[1]

    def p_cases(t):
        """cases : case\n| cases case"""

        def findCaseWithNoStatements(case,parent=None,argidx=0):
            ans=None
            if isinstance(case,Builtin):
                if case.name=='CASE' and len(case.args)==1:
                    ans = {'case':case,'parent':parent,'argidx':argidx}
                else:
                    for idx in range(len(case.args)):
                        ans = findCaseWithNoStatements(case.args[idx],case,idx)
                        if ans is not None:
                            break
            return ans

        def appendCase(cases,case):
            c=findCaseWithNoStatements(cases)
            if c is not None:
                appendTo=c
            else:
                appendTo={'case':cases,'parent':None,'argidx':0}
            if len(appendTo['case'].args) < 250:
                appendTo['case'].args=tuple(list(appendTo['case'].args)+[case,])
                return cases
            else:
                statement = Builtin('statement',(appendTo['case'],case))
                if appendTo['parent']==None:
                    return statement
                else:
                    args=list(appendTo['parent'].args)
                    args[appendTo['idx']]=statement
                    appendTo['parent'].args=tuple(args)
                    return cases

        if len(t)==3:
            t[1]=appendCase(t[1],t[2])
        t[0]=t[1]

    def p_switch(t):
        """operation : SWITCH LPAREN operand RPAREN LBRACE cases RBRACE"""
        t[0]=Builtin('switch',(t[3],t[6]))

### "Equals First" statement (i.e. _gub+=42)
    def p_operand_equal_first(t):
        """operation : ident EQUALSFIRST operand"""
        ops = {'+':'add','-':'subtract','*':'multiply','/':'divide','<':'lt',
                  '>':'gt','^':'power','**':'power','<=':'le','>=':'ge','==':'eq',
                  '>>':'shift_right','<<':'shift_left','&':'iand','&&':'and','!=':'NE',
                  '|':'ior','||':'or','//':'concat'}
        items=ops.items()
        ef_dict=dict()
        for itm in items:
            ef_dict.setdefault(itm[0]+'=',itm[1])
        t[0]=Builtin('equals_first',(Builtin(ef_dict[t[2]],(t[1],t[3])),))

### BACKQUOTED expression (i.e. `getnci(gub,"RECORD")
    def p_operand_backquote(t):
        """operand : BACKQUOTE operand"""
        t[0]=Data.evaluate(t[2])

### Handle syntax errors
    def p_error(t):
        if t is not None:
            print("Syntax error at '%s' in line %d: %s" % (t.value,t.lexer.lineno,t.lexer.lexdata[t.lexer.lexpos-10:t.lexer.lexpos+10]))
        else:
            print("Syntax error")

    import yacc
    yacc.yacc(write_tables=optimized,debug=0,optimize=optimized,tabmodule='tdiparsetab')
    return yacc.parse(text)

def compileFile(filename):
    return tdiCompile('\n'.join(open(filename,'r').readlines()).replace('\r\n','\n'))
