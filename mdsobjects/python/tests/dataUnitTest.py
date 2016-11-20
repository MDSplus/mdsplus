from unittest import TestCase,TestSuite
from mdsdata import Data,makeData

class dataTests(TestCase):
    def _doThreeTest(self,tdiexpr,pyexpr,ans,almost=False):
        """ tests Scalars tdi expression vs. python Expression vs. expected result """
        from mdsscalar import Uint8
        tdians = Data.execute(tdiexpr)
        if isinstance(pyexpr,(bool,)):
            pyans = pyexpr
            if isinstance(tdians,(Uint8,)) and tdians<2:
                tdians = bool(tdians)
        else:
            pyans = pyexpr.getData()
        if almost:
            E = almost if isinstance(almost,(int,float)) else 5
            self.assertAlmostEqual(pyans,tdians,E)
            self.assertAlmostEqual(pyans,ans,E,msg='%s: %s,%s'%(tdiexpr,pyans,ans))
        else:
            self.assertEqual(pyans,tdians)
            self.assertEqual(pyans,ans,msg='%s: %s,%s'%(tdiexpr,pyans,ans))

    def _doUnaryArray(self,ans1,ans2,msg=None,delta=None):
        """ compares arrays """
        if msg is not None:
            msg = 'Unexpected TDI result for: %s'%msg
        if delta is not None:
            self.assertEqual((abs(ans1-ans2)<delta).all(),True,msg=msg)
        else:
            self.assertEqual((ans1==ans2).all(),True,msg=msg)

    def _doThreeTestArray(self,tdiexpr,pyexpr,ans,almost=False):
        """ tests Arrays tdi expression vs. python Expression vs. expected result """
        from mdsscalar import Uint8
        from numpy import ndarray, array
        tdians = Data.execute(tdiexpr)
        if isinstance(pyexpr,(ndarray,)):
            pyans = pyexpr
            if isinstance(tdians,(Uint8,)) and tdians<2:
                tdians = array(tdians,bool)
        else:
            pyans = pyexpr.data()
        if almost:
            E = -almost if isinstance(almost,(int,float)) else -7
            delta = abs(ans+(ans==0).astype(int)) * (10**E)
        else: delta = None
        self._doUnaryArray(pyans,tdians,delta=delta)
        self._doUnaryArray(pyans,ans,tdiexpr,delta=delta)

    def _doExceptionTest(self,expr,exc):
        try:
            Data.execute(expr)
            self.fail("'%s' should have signaled an exception"%expr)
        except Exception as e:
            self.assertEqual(e.__class__,exc)


    def operatorsAndFunction(self):
        def executeTests(test,a,b,res,almost=False,real=True):
            """ performes tests on operators """
            import tdibuiltins as c
            res.reverse()
            test('_a+_b',    a+ b,   res.pop(),almost=almost)
            test('_a-_b',    a- b,   res.pop(),almost=almost)
            test('_a*_b',    a* b,   res.pop(),almost=almost)
            test('_a/_b',    a/ b,   res.pop(),almost=almost)
            #test('_a**_b',   a**b,   res.pop(),almost=almost)
            #test('_a|_b',    a| b,   res.pop())
            #test('_a&_b',    a& b,   res.pop())
            #test('_a==_b',   a==b,   res.pop())
            #test('_a!=_b',   a!=b,   res.pop())
            #test('_a<<_b',   a<<b,   res.pop())
            #test('_a>>_b',   a>>b,   res.pop())
            #test('-_a',      -a,     res.pop(),almost=almost)
            #test('abs(-_a)', abs(-a),res.pop(),almost=almost)
            #test('abs1(-_a)',c.ABS1(-a),res.pop(),almost=almost)
            #test('abssq(-_a)',c.ABSSQ(-a),res.pop(),almost=almost)
            rcount = 11
            acount = 4+rcount
            if almost:  # some operations only make sence on floating point
                test('exp(_a)',     c.EXP(a),    res.pop(),almost=almost)
                test('log(_a)',     c.LOG(a),    res.pop(),almost=almost)
                test('sin(_a)',     c.SIN(a),    res.pop(),almost=almost)
                test('cos(_a)',     c.COS(a),    res.pop(),almost=almost)
                if real:  # some operations are only defined for real numbers
                    test('tan(_a)',     c.TAN(a),    res.pop(),almost=almost)
                    test('asin(_a/10)', c.ASIN(a/10),res.pop(),almost=almost)
                    test('acos(_a/10)', c.ACOS(a/10),res.pop(),almost=almost)
                    test('atan(_a)',    c.ATAN(a),   res.pop(),almost=almost)
                    test('atan2(_a,1)', c.ATAN2(a,1),res.pop(),almost=almost)
                    test('log2(_a)',    c.LOG2(a),   res.pop(),almost=almost)
                    test('log10(_a)',   c.LOG10(a),  res.pop(),almost=almost)
                    test('sind(_a)',    c.SIND(a),   res.pop(),almost=almost)
                    test('cosd(_a)',    c.COSD(a),   res.pop(),almost=almost)
                    test('tand(_a)',    c.TAND(a),   res.pop(),almost=almost)
                    test('anint(_a/3)', c.ANINT(a/3),res.pop(),almost=almost)
                else:  # strip results of real operations
                    res = res[:-rcount]
            else:  # strip results of floating operations
                res = res[:-acount]
            if real:  # some binary operations are only defined for real numbers
                pass
                #test('_a mod _b',   a% b,   res.pop(),almost=almost)
                #test('_a>_b',       a> b,   res.pop())
                #test('_a>=_b',      a>=b,   res.pop())
                #test('_a<_b',       a< b,   res.pop())
                #test('_a<=_b',      a<=b,   res.pop())

        def testScalars():
            """ test scalars """
            def doTest(suffix,cl,scl,ucl,**kw):
                """ test scalar """
                import warnings
                from mdsarray import makeArray
                results = [cl(13),cl(7),cl(30),cl(10./3),#cl(1000),
                    #ucl(11),ucl(2),
                    #False,True,
                    #cl(80),cl(1),scl(-10),scl(10),scl(10),scl(100),
                    cl(22026.4658), cl(2.30258509),
                    cl(-0.54402111), cl(-0.83907153), cl(0.64836083),
                    cl(1.57079633), cl(0.), cl(1.47112767), cl(1.47112767),
                    cl(3.32192809), cl(1.),
                    cl(0.17364818), cl(0.98480775), cl(0.17632698),
                    cl(3),
                    cl(1),True,True,False,False,
                    ]
                Data.execute('_a=10%s,_b=3%s'%tuple([suffix]*2))
                a,b = cl(10),cl(3)
                with warnings.catch_warnings():
                    warnings.simplefilter("ignore")
                    if kw.get('real',True):
                        self.assertEqual(int(cl(10)),10)
                        self.assertAlmostEqual(float(cl(10)),10.)
                warnings.resetwarnings()
                self.assertEqual(Data.execute('[_a]'),makeArray(a))
                executeTests(self._doThreeTest,a,b,results,**kw)

            import mdsscalar as s
            doTest('BU',s.Uint8,  s.Int8,   s.Uint8)
            doTest('WU',s.Uint16, s.Int16,  s.Uint16)
            doTest('LU',s.Uint32, s.Int32,  s.Uint32)
            doTest('QU',s.Uint64, s.Int64,  s.Uint64)
            doTest('B', s.Int8,   s.Int8,   s.Uint8)
            doTest('W', s.Int16,  s.Int16,  s.Uint16)
            doTest('',  s.Int32,  s.Int16,  s.Uint32)
            doTest('Q', s.Int64,  s.Int16,  s.Uint64)
            doTest('.', s.Float32,s.Float32,s.Uint32,almost=5)
            doTest('D0',s.Float64,s.Float64,s.Uint64,almost=4)
            doTest('*cmplx(1.,0.)',  s.Complex64, s.Complex64, s.Uint64,almost=5,real=False)
            doTest('*cmplx(1D0,0D0)',s.Complex128,s.Complex128,s.Uint64,almost=4,real=False)

        def testArrays():
            """ test arrays and signals """
            def doTest(suffix,cl,scl,ucl,**kw):
                """ test array and signal """
                from mdsarray import makeArray
                from compound import Signal
                def results(cl,scl,ucl):
                    #from numpy import array
                    return [
                        cl([13,8,7]),cl([7,0,-3]),cl([30,16,10]),cl([10./3,1,.4]),#cl([1000,256,32]),  # +,-,*,/,**
                        #cl([11,4,7]),ucl([2,4,0]), # |,&
                        #array([False,True,False]),  # ==
                        #array([True,False,True]),   # !=
                        #cl([80,64,64]),cl([1,0,0]),scl([-10,-4,-2]),scl([10,4,2]),scl([10,4,2]),scl([100,16,4]),  # <<,>>,-,abs
                        cl([ 22026.4658,  54.5981500,  7.38905610]),  # exp
                        cl([ 2.30258509,  1.38629436,  0.69314718]),  # log
                        cl([-0.54402111, -0.7568025 ,  0.90929743]),  # sin
                        cl([-0.83907153, -0.65364362, -0.41614684]),  # cos
                        cl([ 0.64836083,  1.15782128, -2.18503986]),  # tan
                        cl([ 1.57079633,  0.41151685,  0.20135792]),  # asin
                        cl([ 0.        ,  1.15927948,  1.36943841]),  # acos
                        cl([ 1.47112767,  1.32581766,  1.10714872]),  # atan
                        cl([ 1.47112767,  1.32581766,  1.10714872]),  # atan2
                        cl([ 3.32192809,  2.        ,  1.        ]),  # log2
                        cl([ 1.        ,  0.60205999,  0.30103   ]),  # log10
                        cl([ 0.17364818,  0.06975647,  0.0348995 ]),  # sind
                        cl([ 0.98480775,  0.99756405,  0.99939083]),  # cosd
                        cl([ 0.17632698,  0.06992681,  0.03492077]),  # tand
                        cl([3,1,1]),  # anint
                        cl([1,0,2]),  # %
                        #array([True,False,False]),  # >
                        #array([True,True,False]),   # >=
                        #array([False,False,True]),  # <
                        #array([False,True,True]),   # <=
                        ]
                """ test array """
                Data.execute('_a=[10%s,4%s,2%s],_b=[3%s,4%s,5%s]'%tuple([suffix]*6))
                a,b = cl([10,4,2]),cl([3,4,5])
                self._doThreeTestArray('_a',makeArray(a),makeData(a))
                executeTests(self._doThreeTestArray,a,b,results(cl,scl,ucl),**kw)
                """ test signal """
                Scl  = lambda v: Signal(cl(v))
                Sscl = lambda v: Signal(scl(v))
                Sucl = lambda v: Signal(ucl(v))
                Data.execute('_a=Make_Signal(_a,*)')
                a = Signal(a)
                executeTests(self._doThreeTestArray,a,b,results(Scl,Sscl,Sucl),**kw)

            import mdsarray as a
            doTest('BU',a.Uint8Array,  a.Int8Array,   a.Uint8Array)
            doTest('WU',a.Uint16Array, a.Int16Array,  a.Uint16Array)
            doTest('LU',a.Uint32Array, a.Int32Array,  a.Uint32Array)
            doTest('QU',a.Uint64Array, a.Int64Array,  a.Uint64Array)
            doTest('B', a.Int8Array,   a.Int8Array,   a.Uint8Array)
            doTest('W', a.Int16Array,  a.Int16Array,  a.Uint16Array)
            doTest('',  a.Int32Array,  a.Int32Array,  a.Uint32Array)
            doTest('Q', a.Int64Array,  a.Int64Array,  a.Uint64Array)
            doTest('.', a.Float32Array,a.Float32Array,a.Uint32Array,almost=5)
            doTest('D0',a.Float64Array,a.Float64Array,a.Uint64Array,almost=4)
            doTest('*cmplx(1.,0.)',  a.Complex64Array,a.Complex64Array,  a.Uint64Array,almost=5,real=False)
            doTest('*cmplx(1D0,0D0)',a.Complex128Array,a.Complex128Array,a.Uint64Array,almost=4,real=False)

        testScalars()
        #testArrays()

    def tdiFunctions(self):
        import mdsExceptions as Exc
        import mdsscalar as s
        import mdsarray as a
        import tdibuiltins as c
        import compound as cc
        c.dTRUE = c.__dict__['$TRUE']
        c.dFALSE = c.__dict__['$FALSE']
        """Test abort"""
        self._doExceptionTest('abort()',Exc.TdiABORT)
        """Test syntax"""
        self._doExceptionTest('\033[[A',Exc.TdiBOMB)
        self.assertEqual(Data.execute(''),None)
        """Test abs"""
        self._doThreeTest('abs(cmplx(3.0,4.0))',c.ABS(s.Complex64(3.+4.j)),s.Float32(5.))
        """Test abs1"""
        self._doThreeTest('abs1(cmplx(3.0,4.0))',c.ABS1(s.Complex64(3.+4.j)),s.Float32(7.))
        """Test abssq"""
        self._doThreeTest('abssq(cmplx(3.0,4.0))',c.ABSSQ(s.Complex64(3.+4.j)),s.Float32(25.))
        """Test accumulate"""
        self._doThreeTestArray('accumulate([1,2,3])',c.ACCUMULATE(a.makeArray([1,2,3])),a.Int32Array([1,3,6]))
        self._doThreeTestArray('accumulate([[1,3,5],[2,4,6]])',c.ACCUMULATE(a.makeArray([[1,3,5],[2,4,6]])),a.Int32Array([[1,4,9], [11,15,21]]))
        self._doThreeTestArray('accumulate([[1,3,5],[2,4,6]],0)',c.ACCUMULATE(a.makeArray([[1,3,5],[2,4,6]]),0),a.Int32Array([[1,4,9],[2,6,12]]))
        #self._doThreeTestArray('accumulate([[1,3,5],[2,4,6]],1)',c.ACCUMULATE([[1,3,5],[2,4,6]],1),a.Int32Array([[1,3,5],[3,7,11]]))  # tdi issue
        self._doUnaryArray(Data.execute('accumulate([[1,3,5],[2,4,6]],1)'),c.ACCUMULATE(a.makeArray([[1,3,5],[2,4,6]]),1).getData())
        """Test achar"""
        self._doThreeTest('achar(88)',c.ACHAR(88),s.String('X'))
        """Test ADJUSTL"""
        self._doThreeTest('adjustl(" WORD")',c.ADJUSTL(" WORD"),s.String("WORD "))
        """Test ADJUSTR"""
        self._doThreeTest('adjustr("WORD ")',c.ADJUSTR("WORD "),s.String(" WORD"))
        """Test AIMAG"""
        self._doThreeTest('AIMAG(CMPLX(2.0,3.0))',c.AIMAG(c.CMPLX(2.,3.)),s.Float32(3.0))
        """Test AINT"""
        self._doThreeTest('aint(2.783)',c.AINT(2.783),s.Float32(2.0))
        self._doThreeTest('aint(-2.783)',c.AINT(-2.783),s.Float32(-2.0))
        """Test NE (operates on flattened array, i.e. first 3 values are compared)"""
        A,B = a.makeArray([1,3,5]),a.makeArray([[0,3,5],[0,0,0],[0,4,8]])
        self._doThreeTestArray('_A=[1,3,5],_B=[[0,3,5],[0,0,0],[0,4,8]],_A ne _B',c.NE(A,B),a.Uint8Array([1,0,0]))
        """Test NE (operates on flattened array, i.e. first 3 values are compared)"""
        self._doThreeTestArray('_A eq _B',c.EQ(A,B),a.Uint8Array([0,1,1]))
        """Test ALL and ANY"""
        self._doThreeTest('all([$TRUE,$FALSE,$TRUE])',c.ALL(a.makeArray([1,0,1])),s.Uint8(0))
        self._doThreeTest('any([$TRUE,$FALSE,$TRUE])',c.ANY(a.makeArray([1,0,1])),s.Uint8(1))
        A = 0
        self._doThreeTest('_A=0,all(_A eq _B)',c.ALL(c.EQ(A,B)),False)
        self._doThreeTest('any(_A ne _B)',c.ANY(c.NE(A,B)),True)
        self._doThreeTestArray('all(_A ne _B,0)',c.ALL(c.NE(A,B),0),a.Uint8Array([0,0,0]))
        self._doThreeTestArray('any(_A ne _B,0)',c.ANY(c.NE(A,B),0),a.Uint8Array([1,0,1]))
        self._doThreeTestArray('all(_A eq _B,1)',c.ALL(c.EQ(A,B),1),a.Uint8Array([1,0,0]))
        self._doThreeTestArray('any(_A ne _B,1)',c.ANY(c.NE(A,B),1),a.Uint8Array([0,1,1]))
        """Test allocated"""
        self.assertEqual(c.DEALLOCATE('*')>=2,True)  # deallocates _A and _B and more?
        self.assertEqual(c.ALLOCATED('_xyz'),s.Uint8(0))
        self.assertEqual(Data.execute('_xyz=0,allocated("_xyz")'),s.Uint8(1))
        self.assertEqual(c.ALLOCATED('_xyz'),s.Uint8(1))
        self.assertEqual(c.DEALLOCATE('*'),s.Uint8(1))
        self.assertEqual(c.ALLOCATED('_xyz'),s.Uint8(0))
        """Test AND"""
        A,B=a.makeArray([0,0,1,1]),a.makeArray([0,1,0,1])
        self._doThreeTestArray('_A=[0,0,1,1],_B=[0,1,0,1],_A && _B',c.AND(A,B),a.Uint8Array([0,0,0,1]))
        """Test AND_NOT"""
        self._doThreeTestArray('_A AND_NOT _B',c.AND_NOT(A,B),a.Uint8Array([0,0,1,0]))
        """Test ANINT"""
        self._doThreeTest('ANINT(2.783)',c.ANINT(2.783),s.Float32(3.0))
        """Test ARG"""
        self.assertEqual(Data.execute('execute("abs(arg(cmplx(3.0,4.0)) - .9272952) < .000001")'),s.Uint8(1))
        """Test ARGD"""
        self.assertEqual(Data.execute('execute("abs(argd(cmplx(3.0,4.0)) - 53.1301) < .000001")'),s.Uint8(1))
        """Test arg_of"""
        self._doThreeTest('arg_of(pub->foo(42,43))',c.ARG_OF(cc.Call('pub','foo',42,43)),s.Int32(42))
        self._doThreeTest('arg_of(pub->foo(42,43),1)',c.ARG_OF(cc.Call('pub','foo',42,43),1),s.Int32(43))
        self._doThreeTest('arg_of(1+3,1)',c.ARG_OF(c.ADD(1,3),1),s.Int32(3))
        """Test Array"""
        self._doThreeTestArray('array(10)',    c.ARRAY(10),  a.Float32Array([0]*10))
        self._doThreeTestArray('array(10,0)',  c.ARRAY(10,0),a.Int32Array([0]*10))
        self._doThreeTestArray('array(10,0BU)',c.ARRAY(10,s.Uint8(0)),a.Uint8Array([0]*10))
        self._doThreeTestArray('zero(100)',    c.ZERO(100),a.Float32Array([0]*100))

    def tdiPythonInterface(self):
        from mdsdata import Data
        from mdsscalar import Uint8,String
        #self.assertEqual(Data.execute("Py('a=None')"),1)
        self.assertEqual(Data.execute("Py('a=None','a')"),None)
        self.assertEqual(Data.execute("Py('a=123','a')"),123)
        self.assertEqual(Data.execute("Py('import MDSplus;a=MDSplus.Uint8(-1)','a')"),Uint8(255))
        self.assertEqual(Data.execute("pyfun('Uint8','MDSplus',-1)"),Uint8(255))
        self.assertEqual(Data.execute("pyfun('Uint8',*,-1)"),Uint8(255))
        self.assertEqual(Data.execute("pyfun('str',*,123)"),String(123))
        from apd import List
        self.assertEqual(Data.execute('_l=list(1,2,3)'), List([1,2,3]))
        self.assertEqual(Data.execute('apdadd(_l,4,5)'), List([1,2,3,4,5]))
        self.assertEqual(Data.execute('apdrm(_l,1,3)'),  List([1,3,5]))
        from apd import Dictionary
        self.assertEqual(Data.execute('_d=dict(1,"1",2,"2")'), Dictionary([1,'1',2,'2']))
        self.assertEqual(Data.execute('apdadd(_d,3,"3")'),     Dictionary([1,'1',2,'2',3,"3"]))
        self.assertEqual(Data.execute('apdrm(_d,2)'),          Dictionary([1,'1',3,"3"]))

    def decompile(self):
        import mdsscalar as s
        self.assertEqual(str(s.Uint8(123)),'123BU')
        self.assertEqual(str(s.Uint16(123)),'123WU')
        self.assertEqual(str(s.Uint32(123)),'123LU')
        self.assertEqual(str(s.Uint64(123)),'123QU')
        self.assertEqual(str(s.Int8(123)),'123B')
        self.assertEqual(str(s.Int16(123)),'123W')
        self.assertEqual(str(s.Int32(123)),'123')
        self.assertEqual(str(s.Int64(123)),'123Q')
        #self.assertEqual(str(s.Float32(1.2E-3)),'.0012')
        #self.assertEqual(str(s.Float64(1.2E-3)),'.0012D0')

    def runTest(self):
        self.operatorsAndFunction()
        self.tdiFunctions()
        self.decompile()
        self.tdiPythonInterface()


def suite():
    tests = ['operatorsAndFunction','tdiFunctions','decompile','tdiPythonInterface']
    return TestSuite(map(dataTests,tests))

if __name__=='__main__':
    from unittest import TextTestRunner
    TextTestRunner().run(suite())
