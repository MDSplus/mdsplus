#!/usr/bin/python
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

from unittest import TestCase,TestSuite,TextTestRunner
from threading import Lock
from numpy import ndarray, array, int32
from math import log10
import MDSplus as m


class Tests(TestCase):
    inThread = False
    index = 0

    def _doThreeTest(self,tdiexpr,pyexpr,ans,**kwargs):
        """ tests Scalars tdi expression vs. python Expression vs. expected result """
        almost = kwargs.get('almost',False)
        real = kwargs.get('real',True)
        tdians = m.Data.execute(tdiexpr)
        if isinstance(pyexpr,(bool,)):
            pyans = pyexpr
            if isinstance(tdians,(m.Uint8,)) and tdians<2:
                tdians = bool(tdians)
        else:
            pyans = pyexpr.getData()
        if almost:
            E = almost if isinstance(almost,(int,float)) else 7
            if tdians>1 if real else tdians.real>1: E = int(E-log10(float(tdians)))
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

    def _doThreeTestArray(self,tdiexpr,pyexpr,ans,**kwargs):
        """ tests Arrays tdi expression vs. python Expression vs. expected result """
        almost = kwargs.get('almost',False)
        tdians = m.Data.execute(tdiexpr)
        if isinstance(pyexpr,(ndarray,)):
            pyans = pyexpr
            if isinstance(tdians,(m.Uint8,)) and tdians<2:
                tdians = array(tdians,bool)
        else:
            pyans = pyexpr.getData()
        if almost:
            E = -almost if isinstance(almost,(int,float)) else -7
            delta = abs(ans+(ans==0).astype(int)) * (10**E)
        else: delta = None
        self._doUnaryArray(pyans,tdians,delta=delta)
        self._doUnaryArray(pyans,ans,tdiexpr,delta=delta)

    @staticmethod
    def _executeTests(test,a,b,res,**kwargs):
        """ performes tests on operators """
        almost = kwargs.get('almost',False)
        real = kwargs.get('real',True)
        res.reverse()
        test('_a+_b',    a+ b,   res.pop(),**kwargs)
        test('_a-_b',    a- b,   res.pop(),**kwargs)
        test('_a*_b',    a* b,   res.pop(),**kwargs)
        test('_a/_b',    a/ b,   res.pop(),**kwargs)
        test('_a**_b',   a**b,   res.pop(),**kwargs)
        test('_a|_b',    a| b,   res.pop(),**kwargs)
        test('_a&_b',    a& b,   res.pop(),**kwargs)
        test('_a==_b',   a==b,   res.pop())
        test('_a!=_b',   a!=b,   res.pop())
        test('_a<<_b',   a<<b,   res.pop(),**kwargs)
        test('_a>>_b',   a>>b,   res.pop(),**kwargs)
        test('-_a',      -a,     res.pop(),**kwargs)
        test('abs(-_a)', abs(-a),res.pop(),**kwargs)
        test('abs1(-_a)',m.ABS1(-a),res.pop(),**kwargs)
        test('abssq(-_a)',m.ABSSQ(-a),res.pop(),**kwargs)
        rcount = 11
        acount = 4+rcount
        if almost:  # some operations only make sence on floating point
            test('exp(_a)',     m.EXP(a),    res.pop(),**kwargs)
            test('log(_a)',     m.LOG(a),    res.pop(),**kwargs)
            test('sin(_a)',     m.SIN(a),    res.pop(),**kwargs)
            test('cos(_a)',     m.COS(a),    res.pop(),**kwargs)
            if real:  # some operations are only defined for real numbers
                test('tan(_a)',     m.TAN(a),    res.pop(),**kwargs)
                test('asin(_a/10)', m.ASIN(a/10),res.pop(),**kwargs)
                test('acos(_a/10)', m.ACOS(a/10),res.pop(),**kwargs)
                test('atan(_a)',    m.ATAN(a),   res.pop(),**kwargs)
                test('atan2(_a,1)', m.ATAN2(a,1),res.pop(),**kwargs)
                test('log2(_a)',    m.LOG2(a),   res.pop(),**kwargs)
                test('log10(_a)',   m.LOG10(a),  res.pop(),**kwargs)
                test('sind(_a)',    m.SIND(a),   res.pop(),**kwargs)
                test('cosd(_a)',    m.COSD(a),   res.pop(),**kwargs)
                test('tand(_a)',    m.TAND(a),   res.pop(),**kwargs)
                test('anint(_a/3)', m.ANINT(a/3),res.pop(),**kwargs)
            else:  # strip results of real operations
                res = res[:-rcount]
        else:  # strip results of floating operations
            res = res[:-acount]
        if real:  # some binary operations are only defined for real numbers
            test('_a mod _b',   a% b,   res.pop(),**kwargs)
            test('_a>_b',       a> b,   res.pop())
            test('_a>=_b',      a>=b,   res.pop())
            test('_a<_b',       a< b,   res.pop())
            test('_a<=_b',      a<=b,   res.pop())

    def testData(self):
        self.assertEqual(m.Data(2).compare(2),True)
        self.assertEqual(m.Data(2).compare(1),False)

    def testScalars(self):
        def doTest(suffix,cl,scl,ucl,**kw):
            """ test scalar """
            import warnings
            results = [cl(13),cl(7),cl(30),cl(10./3),cl(1000),
                ucl(11),ucl(2),
                False,True,
                cl(80),cl(1),scl(-10),scl(10),scl(10),scl(100),
                cl(22026.4658), cl(2.30258509),
                cl(-0.54402111), cl(-0.83907153), cl(0.64836083),
                cl(1.57079633), cl(0.), cl(1.47112767), cl(1.47112767),
                cl(3.32192809), cl(1.),
                cl(0.17364818), cl(0.98480775), cl(0.17632698),
                cl(3),
                cl(1),True,True,False,False,
                ]
            m.Data.execute('_a=10%s,_b=3%s'%tuple([suffix]*2))
            a,b = cl(10),cl(3)
            with warnings.catch_warnings():
                warnings.simplefilter("ignore")
                self.assertEqual(int(cl(10)),10)
                self.assertAlmostEqual(float(cl(10)),10.)
            warnings.resetwarnings()
            self._doTdiTest('[_a]',m.makeArray(a))
            self._executeTests(self._doThreeTest,a,b,results,**kw)

        doTest('BU',m.Uint8,  m.Int8,   m.Uint8)
        doTest('WU',m.Uint16, m.Int16,  m.Uint16)
        doTest('LU',m.Uint32, m.Int32,  m.Uint32)
        doTest('QU',m.Uint64, m.Int64,  m.Uint64)
        doTest('B', m.Int8,   m.Int8,   m.Uint8)
        doTest('W', m.Int16,  m.Int16,  m.Uint16)
        doTest('',  m.Int32,  m.Int16,  m.Uint32)
        doTest('Q', m.Int64,  m.Int16,  m.Uint64)
        doTest('.', m.Float32,m.Float32,m.Uint32,almost=5)
        doTest('D0',m.Float64,m.Float64,m.Uint64,almost=7)
        doTest('*cmplx(1.,0.)',  m.Complex64, m.Complex64, m.Uint64,almost=5,real=False)
        doTest('*cmplx(1D0,0D0)',m.Complex128,m.Complex128,m.Uint64,almost=7,real=False)

    def testArrays(self):
        def doTest(suffix,cl,scl,ucl,**kw):
            """ test array and signal """
            def results(cl,scl,ucl):
                from numpy import array
                return [
                    cl([13,8,7]),cl([7,0,-3]),cl([30,16,10]),cl([10./3,1,.4]),cl([1000,256,32]),  # +,-,*,/,**
                    ucl([11,4,7]),ucl([2,4,0]), # |,&
                    array([False,True,False]),  # ==
                    array([True,False,True]),   # !=
                    cl([80,64,64]),cl([1,0,0]),scl([-10,-4,-2]),scl([10,4,2]),scl([10,4,2]),scl([100,16,4]),  # <<,>>,-,abs
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
                    array([True,False,False]),  # >
                    array([True,True,False]),   # >=
                    array([False,False,True]),  # <
                    array([False,True,True]),   # <=
                    ]
            """ test array """
            m.Data.execute('_a=[10%s,4%s,2%s],_b=[3%s,4%s,5%s]'%tuple([suffix]*6))
            a,b = cl([10,4,2]),cl([3,4,5])
            self._doThreeTestArray('_a',m.Array(a),m.Data(a))
            self._executeTests(self._doThreeTestArray,a,b,results(cl,scl,ucl),**kw)
            """ test signal """
            Scl  = lambda v: m.Signal(cl(v))
            Sscl = lambda v: m.Signal(scl(v))
            Sucl = lambda v: m.Signal(ucl(v))
            m.Data.execute('_a=Make_Signal(_a,*)')
            a = m.Signal(a)
            self._executeTests(self._doThreeTestArray,a,b,results(Scl,Sscl,Sucl),**kw)

        doTest('BU',m.Uint8Array,  m.Int8Array,   m.Uint8Array)
        doTest('WU',m.Uint16Array, m.Int16Array,  m.Uint16Array)
        doTest('LU',m.Uint32Array, m.Int32Array,  m.Uint32Array)
        doTest('QU',m.Uint64Array, m.Int64Array,  m.Uint64Array)
        doTest('B', m.Int8Array,   m.Int8Array,   m.Uint8Array)
        doTest('W', m.Int16Array,  m.Int16Array,  m.Uint16Array)
        doTest('',  m.Int32Array,  m.Int32Array,  m.Uint32Array)
        doTest('Q', m.Int64Array,  m.Int64Array,  m.Uint64Array)
        doTest('.', m.Float32Array,m.Float32Array,m.Uint32Array,almost=5)
        doTest('D0',m.Float64Array,m.Float64Array,m.Uint64Array,almost=7)
        doTest('*cmplx(1.,0.)',  m.Complex64Array,m.Complex64Array,  m.Uint64Array,almost=5,real=False)
        doTest('*cmplx(1D0,0D0)',m.Complex128Array,m.Complex128Array,m.Uint64Array,almost=7,real=False)

    def vmsSupport(self):
        self.assertEqual(str(m.TdiExecute('1.23F0')),"1.23")
        self.assertEqual(str(m.TdiExecute('1.23V0')),"1.23D0")
        self.assertEqual(str(m.TdiExecute('1.23G0')),"1.23D0")
        self.assertEqual(str(m.TdiExecute('ZERO([33,33,33],1.F0)+1.23F0')),"Set_Range(33,33,33,1.23 /*** etc. ***/)")
        self.assertEqual(str(m.TdiExecute('ZERO([33,33,33],1.V0)+1.23V0')),"Set_Range(33,33,33,1.23D0 /*** etc. ***/)")
        self.assertEqual(str(m.TdiExecute('ZERO([33,33,33],1.G0)+1.23G0')),"Set_Range(33,33,33,1.23D0 /*** etc. ***/)")

    def _doExceptionTest(self,expr,exc):
        try:
           m.Data.execute(expr)
        except Exception as e:
            self.assertEqual(e.__class__,exc)
            return
        self.fail("TDI: '%s' should have signaled %s"%(expr,exc))

    def _doTdiTest(self,expr,res):
        self.assertEqual(m.Data.execute(expr),res)
    def tdiFunctions(self):
        from MDSplus import mdsExceptions as Exc
        """Test Exceptions"""
        self._doExceptionTest('abort()',Exc.TdiABORT)
        self._doExceptionTest('{,}',Exc.TdiSYNTAX)
        self._doExceptionTest('\033[[A',Exc.TdiBOMB)
        self._doExceptionTest('abs()',Exc.TdiMISS_ARG)
        self._doExceptionTest('abs("")',Exc.TdiINVDTYDSC)
        self._doExceptionTest('abs(1,2)',Exc.TdiEXTRA_ARG)
        self._doExceptionTest('"',Exc.TdiUNBALANCE)
        """Test $Missing/NoData/None"""
        self._doTdiTest('',None)
        """ comparison """
        self._doTdiTest('-1O >  1O',False)
        self._doTdiTest('-1O > -2O',True)
        """Test abs"""
        self._doThreeTest('abs(cmplx(3.0,4.0))',m.ABS(m.Complex64(3.+4.j)),m.Float32(5.))
        """Test abs1"""
        self._doThreeTest('abs1(cmplx(3.0,4.0))',m.ABS1(m.Complex64(3.+4.j)),m.Float32(7.))
        """Test abssq"""
        self._doThreeTest('abssq(cmplx(3.0,4.0))',m.ABSSQ(m.Complex64(3.+4.j)),m.Float32(25.))
        """Test accumulate"""
        self._doThreeTestArray('accumulate([1,2,3])',m.ACCUMULATE(m.makeArray([1,2,3])),m.Int32Array([1,3,6]))
        self._doThreeTestArray('accumulate([[1,3,5],[2,4,6]])',m.ACCUMULATE(m.makeArray([[1,3,5],[2,4,6]])),m.Int32Array([[1,4,9], [11,15,21]]))
        self._doThreeTestArray('accumulate([[1,3,5],[2,4,6]],0)',m.ACCUMULATE(m.makeArray([[1,3,5],[2,4,6]]),0),m.Int32Array([[1,4,9],[2,6,12]]))
        #self._doThreeTestArray('accumulate([[1,3,5],[2,4,6]],1)',m.ACCUMULATE([[1,3,5],[2,4,6]],1),m.Int32Array([[1,3,5],[3,7,11]]))  # tdi issue
        self._doUnaryArray(m.Data.execute('accumulate([[1,3,5],[2,4,6]],1)'),m.ACCUMULATE(m.makeArray([[1,3,5],[2,4,6]]),1).getData())
        """Test achar"""
        self._doThreeTest('achar(88)',m.ACHAR(88),m.String('X'))
        """Test ADJUSTL"""
        self._doThreeTest('adjustl(" WORD")',m.ADJUSTL(" WORD"),m.String("WORD "))
        """Test ADJUSTR"""
        self._doThreeTest('adjustr("WORD ")',m.ADJUSTR("WORD "),m.String(" WORD"))
        """Test AIMAG"""
        self._doThreeTest('AIMAG(CMPLX(2.0,3.0))',m.AIMAG(m.CMPLX(2.,3.)),m.Float32(3.0))
        """Test AINT"""
        self._doThreeTest('aint(2.783)',m.AINT(2.783),m.Float32(2.0))
        self._doThreeTest('aint(-2.783)',m.AINT(-2.783),m.Float32(-2.0))
        """Test NE (operates on flattened array, i.e. first 3 values are compared)"""
        A,B = array([1,3,5],int32),array([[0,3,5],[0,0,0],[0,4,8]],int32)
        self._doThreeTestArray('_A=[1,3,5],_B=[[0,3,5],[0,0,0],[0,4,8]],_A ne _B',m.NE(A,B),m.Uint8Array([1,0,0]))
        """Test NE (operates on flattened array, i.e. first 3 values are compared)"""
        self._doThreeTestArray('_A eq _B',m.EQ(A,B),m.Uint8Array([0,1,1]))
        """Test ALL and ANY"""
        self._doThreeTest('all([$TRUE,$FALSE,$TRUE])',m.ALL(array([m.dTRUE(),m.dFALSE(),m.dTRUE()])),m.Uint8(0))
        self._doThreeTest('any([$TRUE,$FALSE,$TRUE])',m.ANY(array([m.dTRUE(),m.dFALSE(),m.dTRUE()])),m.Uint8(1))
        A = 0
        self._doThreeTest('_A=0,all(_A eq _B)',m.ALL(m.EQ(A,B)),False)
        self._doThreeTest('any(_A ne _B)',m.ANY(m.NE(A,B)),True)
        self._doThreeTestArray('all(_A ne _B,0)',m.ALL(m.NE(A,B),0),m.Uint8Array([0,0,0]))
        self._doThreeTestArray('any(_A ne _B,0)',m.ANY(m.NE(A,B),0),m.Uint8Array([1,0,1]))
        self._doThreeTestArray('all(_A eq _B,1)',m.ALL(m.EQ(A,B),1),m.Uint8Array([1,0,0]))
        self._doThreeTestArray('any(_A ne _B,1)',m.ANY(m.NE(A,B),1),m.Uint8Array([0,1,1]))
        """Test allocated"""
        self.assertEqual(m.DEALLOCATE('*')>=2,True)  # deallocates _A and _B and more?
        self.assertEqual(m.ALLOCATED('_xyz'),m.Uint8(0))
        self._doTdiTest('_xyz=0,allocated("_xyz")',m.Uint8(1))
        self.assertEqual(m.ALLOCATED('_xyz'),m.Uint8(1))
        self.assertEqual(m.DEALLOCATE('*'),m.Uint8(1))
        self.assertEqual(m.ALLOCATED('_xyz'),m.Uint8(0))
        """Test AND"""
        A,B = array([0,0,1,1]),array([0,1,0,1])
        self._doThreeTestArray('_A=[0,0,1,1],_B=[0,1,0,1],_A && _B',m.AND(A,B),m.Uint8Array([0,0,0,1]))
        """Test AND_NOT"""
        self._doThreeTestArray('_A AND_NOT _B',m.AND_NOT(A,B),m.Uint8Array([0,0,1,0]))
        """Test ANINT"""
        self._doThreeTest('ANINT(2.783)',m.ANINT(2.783),m.Float32(3.0))
        """Test ARG"""
        self._doTdiTest('execute("abs(arg(cmplx(3.0,4.0)) - .9272952) < .000001")',m.Uint8(1))
        """Test ARGD"""
        self._doTdiTest('execute("abs(argd(cmplx(3.0,4.0)) - 53.1301) < .000001")',m.Uint8(1))
        """Test arg_of"""
        self._doThreeTest('arg_of(pub->foo:B(42,43))',m.ARG_OF(m.Call('pub','foo',42,43).setRType(m.Uint8)),m.Int32(42))
        self._doThreeTest('arg_of(pub->foo:P(42,43),1)',m.ARG_OF(m.Call('pub','foo',42,43).setRType(m.Pointer),1),m.Int32(43))
        self._doThreeTest('arg_of(1+3,1)',m.ARG_OF(m.ADD(1,3),1),m.Int32(3))
        """Test Array"""
        self._doThreeTestArray('array(10)',    m.ARRAY(10),  m.Float32Array([0]*10))
        self._doThreeTestArray('array(10,0)',  m.ARRAY(10,0),m.Int32Array([0]*10))
        self._doThreeTestArray('array(10,0BU)',m.ARRAY(10,m.Uint8(0)),m.Uint8Array([0]*10))
        self._doThreeTestArray('zero(100)',    m.ZERO(100),m.Float32Array([0]*100))

    def tdiPythonInterface(self):
        self._doTdiTest("Py('a=None')",1)
        self._doTdiTest("Py('a=None','a')",None)
        self._doTdiTest("Py('a=123','a')",123)
        self._doTdiTest("Py('import MDSplus;a=MDSplus.Uint8(-1)','a')",m.Uint8(255))
        self._doTdiTest("pyfun('Uint8','MDSplus',-1)",m.Uint8(255))
        self._doTdiTest("pyfun('Uint8',*,-1)",m.Uint8(255))
        self._doTdiTest("pyfun('str',*,123)",m.String("123"))
        self._doTdiTest('_l=list(*,1,2,3)', m.List([1,2,3]))
        self._doTdiTest('_l=list(_l,4,5)', m.List([1,2,3,4,5]))
        self._doTdiTest('apdrm(_l,1,3)',  m.List([1,3,5]))
        self._doTdiTest('_d=dict(*,1,"1",2,"2")', m.Dictionary([1,'1',2,'2']))
        self._doTdiTest('_d=dict(_d,3,"3")',     m.Dictionary([1,'1',2,'2',3,"3"]))
        self._doTdiTest('apdrm(_d,2)',          m.Dictionary([1,'1',3,"3"]))

    def decompile(self):
        self.assertEqual(str(m.Uint8(123)),'123BU')
        self.assertEqual(str(m.Uint16(123)),'123WU')
        self.assertEqual(str(m.Uint32(123)),'123LU')
        self.assertEqual(str(m.Uint64(123)),'123QU')
        self.assertEqual(str(m.Int8(123)),'123B')
        self.assertEqual(str(m.Int16(123)),'123W')
        self.assertEqual(str(m.Int32(123)),'123')
        self.assertEqual(str(m.Int64(123)),'123Q')
        self.assertEqual(str(m.Float32(1.2E-3)),'.0012')
        self.assertEqual(str(m.Float64(1.2E-3)),'.0012D0')
        self.assertEqual(str(m.Signal(m.ZERO(100000,0.).evaluate(),None,0.)),"Build_Signal(Set_Range(100000,0. /*** etc. ***/), *, 0.)")

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['testData','testScalars','testArrays','vmsSupport','tdiFunctions','decompile','tdiPythonInterface']
    @classmethod
    def getTestCases(cls,tests=None):
        if tests is None: tests = cls.getTests()
        return map(cls,tests)

def suite(tests=None):
    return TestSuite(Tests.getTestCases(tests))

def run(tests=None):
    TextTestRunner(verbosity=2).run(suite(tests))

def objgraph():
    import objgraph,gc
    gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    gc.collect()
    objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])

if __name__=='__main__':
    import sys
    if len(sys.argv)==2 and sys.argv[1]=='all':
        run()
    elif len(sys.argv)>1:
        run(sys.argv[1:])
    else: print('Available tests: %s'%(' '.join(Tests.getTests())))
