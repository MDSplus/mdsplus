#!/usr/bin/python
import ctypes as ct
import numpy as np
import time

try:    tobytes = np.ndarray.tobytes
except: tobytes = np.ndarray.tostring
def buf2ptr(a): return ct.c_void_p(a.__array_interface__['data'][0])
debug=0
reg10to125=np.array([[0,20],[1,228],[2,162],[3,21],[4,146],[5,237],[6,45],[7,42],[8,0],[9,192],[10,8],[11,66],[16,0],[17,128],[18,0],[19,44],[20,62],[21,255],[22,223],[23,31],[24,63],[25,224],[31,0],[32,0],[33,3],[34,0],[35,0],[36,3],[40,32],[41,2],[42,37],[43,0],[44,0],[45,4],[46,0],[47,0],[48,4],[55,0],[131,31],[132,2],[138,15],[139,255],[142,0],[143,0],[136,64]],np.uint8)

class sis830xException(Exception):
    _status_codes = {
         0:'Stat830xSuccess',
         1:'Stat830xInvalidDeviceIndex',
         2:'Stat830xNullArgument',
         3:'Stat830xAlreadyOpen',
         4:'Stat830xNotOpen',
         5:'Stat830xIoctlError',
         6:'Stat830xReadCallError',
         7:'Stat830xReadLenError',
         8:'Stat830xWriteCallError',
         9:'Stat830xWriteLenError',
        10:'Stat830xSpiBusy',
        11:'Stat830xFlashBusy',
        12:'Stat830xInvalidPath',
        13:'Stat830xFileTooLarge',
        14:'Stat830xMemAlloc',
        15:'Stat830xNotSupported',
        16:'Stat830xVerifyError',
        17:'Stat830xI2cBusy',
        18:'Stat830xI2cNack',
        19:'Stat830xSynthNoLock',
        20:'Stat830xFileNotSupported',
        21:'Stat830xFlashEraseError',
        22:'Stat830xClkMuxCalibError',
        23:'Stat830xClkMuxParaBwSelErr',
        24:'Stat830xClkMuxParaN1HsErr',
        25:'Stat830xClkMuxParaNc1LsErr',
        26:'Stat830xClkMuxParaNc2LsErr',
        27:'Stat830xClkMuxParaN2HsErr',
        28:'Stat830xClkMuxParaN2LsErr',
        29:'Stat830xClkMuxParaN31Err',
        30:'Stat830xClkMuxParaClkInFreqErr',
    }
    @staticmethod
    def isException(status): return not status==0
    @classmethod
    def getMsg(cls,status):
        if status in cls._status_codes:
            return cls._status_codes[status]
        return 'unknown exception: %d'%status
    @classmethod
    def checkStatus(cls,status):
        if status==0: return
        raise cls(status)
    def __init__(self,status):
        self.status = int(status)
        super(sis830xException,self).__init__(self.message)
    @property
    def message(self): return self.getMsg(self.status)


class _Register(property):
    class RegisterList(object):
        def check_index(self,idx):
            if idx<self.prop.array[0]: raise IndexError("UnderRange: Index must be in range [%d..%d]."%self.prop.array)
            if idx>self.prop.array[1]: raise IndexError( "OverRange: Index must be in range [%d..%d]."%self.prop.array)
            return idx-self.prop.array[0]
        def get(self,idx):
            if not self.prop.r: raise AttributeError
            idx = self.check_index(idx)
            return self.parent.ReadRegister(self.prop.addr+idx)
        def set(self,idx,value):
            if not self.prop.w: raise AttributeError
            idx=self.check_index(idx)
            self.parent.WriteRegister(self.prop.addr+idx,value)
        def __repr__(self):
            if  self.prop.name is None:
                start = self.prop.addr
                end   = start+self.prop.len
                return 'RegisterList[0x%04x..0x%04x]'%(start,end)
            return '%s[%d..%d]'%(self.prop.name,self.prop.array[0],self.prop.array[1])
        def __getitem__(self,idx): return self.get(idx)
        def __setitem__(self,idx,value): self.set(idx,value)
        def __len__(self): return self.prop.len
        @property
        def __doc__(self): return self.prop.__doc__
        def __init__(self, parent, prop):
            self.parent = parent
            self.prop   = prop
    def __get__(self, inst, cls):
        if inst is None: return self
        if self.array is None:
            if not self.r:
                super(_Register,self).__get__(inst, cls)
            return inst.ReadRegister(self.addr)
        else:
            return _Register.RegisterList(inst,self)
    @property
    def len(self):
        if self.array is None: return None
        return self.array[1]-self.array[0]+1
    def __set__(self, inst, value):
        if not self.w:
            super(_Register,self).__set__(inst,value)
        if self.array is None:
            return inst.WriteRegister(self.addr,value)
        values = enumerate(value)
        if len(value)==self.len:
            for idx,val in values:
                self.parent.WriteRegister(self.prop.addr+idx,val)
        raise AttributeError("length of array must match size of register array: %d"%(self.len,))
    @property
    def __doc__(self): return '%s=0x%04x, %s\n%s'%(self.name,self.addr,self.man,self.doc)
    def __init__(self, addr, name, man, r, w, doc, array=None):
        self.addr = addr
        self.name = name
        self.man  = man
        self.r    = r
        self.w    = w
        self.doc  = doc
        self.array= (0,array-1) if isinstance(array,int) else array

class _JKFlag(property):
    def __get__(self, inst, cls):
        return (self.register.__get__(inst, cls)&self.enable)>0
    def __set__(self, inst, value):
        self.register.__set__(inst,self.enable if value else self.disable)
    def __init__(self, register, enable, disable, doc=None):
        if doc is not None: self.__doc__ = doc
        self.register = register
        self.enable   = 1<<enable
        self.disable  = 1<<disable

class _SCFlag(property):
    def __get__(self, inst, cls):
        return (self.status.__get__(inst, cls)&self.value)>0
    def __set__(self, inst, value):
        if value: self.clear.__set__(inst, self.value)
    def __init__(self, status, clear, bit, doc=None):
        if doc is not None: self.__doc__ = doc
        self.status = status
        self.clear  = clear
        self.value  = 1<<bit

class _dummy_fun(object):
    def __init__(self,name):
        print('!!! SIMULATION ONLY. REAL DRIVER NOT LOADED !!!')
        self.name = name
    def __call__(self,*args):
        print('%s(%s)'%(self.name,','.join([str(arg) for arg in args])))
        return 0
class _dummy_lib(object):
    reg = {}
    def __init__(self,name):
        self.name = name
    def __getattr__(self,name):
        return _dummy_fun(name)


class _module(object):
    _up = None
    def __getattr__(self,name):
        if isinstance(self._up,_module):
            return self._up.__getattr__(name)
        return self._up.__getattribute__(name)
    def __getattribute__(self,name):
        v = super(_module,self).__getattribute__(name)
        if name.startswith('__') and name.endswith('__'): return v
        if isinstance(v,type) and issubclass(v,(_module,)):
            return v(self)
        return v
    def __init__(self, up):
        self._up = up
        def __setattr__(self,name,value):
            self._up.__setattr__(name,value)
        self.__setattr__ = __setattr__

class _register(_module):
    @property
    def __doc__(self): return self.reg.__doc__


class sis8300ku(object):
    class _DEVICE(ct.Structure):
        _fields_ = [('open',  ct.c_uint32),
                    ('fp',    ct.c_uint32),
                    ('type',  ct.c_uint32),
                    ('ident', ct.c_uint32),
                    ('serial',ct.c_uint32)
                   ]
    def __getattribute__(self,name):
        v = super(sis8300ku,self).__getattribute__(name)
        if name.startswith('__') and name.endswith('__'): return v
        if isinstance(v,type) and issubclass(v,(_module,)):
            return v(self)
        return v
    debug = False
    numchan = 10
    @property
    def byref(self): return ct.byref(self.dev)
    tot_blocks  = 0x2000000     # 2GB in 512bit /block
    _bytes_per_block   = 64
    _bytes_per_sample  = 2
    _samples_per_block = _bytes_per_block//_bytes_per_sample
    tot_bytes   = tot_blocks*_bytes_per_block
    tot_samples = tot_blocks*_samples_per_block
    path = None
    @property
    def max_blocks(self): return self.tot_blocks//len(self.sample.chanlist)
    @property
    def max_samples(self):return self.max_blocks*self._samples_per_block
    def check(self,status): return sis830xException.checkStatus(status)
    def __init__(self,path='/dev/sis8300-0'):
        try:    self.lib = ct.CDLL('libSIS830x.so') # TODO: cleanup
        except: self.lib = _dummy_lib('libSIS830x.so')
        self.dev  = sis8300ku._DEVICE()
        self.path = path
        self.open()
        def __setattr__(self,name,value):
            if name in self.__class__.__dict__:
                super(sis83xx,self).__setattr__(name,value)
            raise AttributeError
        self.__setattr__ = __setattr__
    def __del__(self): self.close()
    @property
    def is_open(self): return not self.dev.open==0
    def close(self):
        if self.is_open:
            self.user.user_led = False
            self.check(self.lib.sis830x_CloseDevice(self.byref))
    def open(self):
        self.check(self.lib.sis830x_OpenDeviceOnPath(ct.c_char_p(self.path),self.byref))
        self.user.user_led = True
    def WriteRegister(self,offset,value):
        if isinstance(self.lib,_dummy_lib): # TODO: cleanup
            if debug>0: print('wrote %04x: %08x'%(offset,value))
            self.lib.reg[offset] = value
            return
        self.check(self.lib.sis830x_WriteRegister(self.byref,ct.c_uint32(offset),ct.c_uint32(value)))

    def ReadRegister(self,offset):
        if isinstance(self.lib,_dummy_lib): # TODO: cleanup
            if offset == 0x0010:
                return int(time.time()%10)
            value = self.lib.reg.get(offset,0)
            if debug>0: print('read  %04x: %08x'%(offset,value))
            return value
        value=ct.c_uint32()
        self.check(self.lib.sis830x_ReadRegister(self.byref,ct.c_uint32(offset),ct.byref(value)))
        return int(value.value)

    def readMemory(self,address,buf):
        try: self.check(self.lib.sis830x_ReadMemory(self.byref,ct.c_uint32(address),buf,ct.c_int32(ct.sizeof(buf))))
        except sis830xException as e:
            if not e.status == 7:
                raise
    """
    manual: SIS8300KU-M-x005-1-V105.pdf
    _reg_identifier                               = _Register?(addr,  name,                                       manual,   read, write,doc)
    """
    class identifier_version(_register):
        reg = _Register(0x0000,'SIS8300_IDENTIFIER_VERSION_REG',          '7.7.1',   True, False,'This register holds the module identifier (SIS8303) and the firmware version and revision.')
    class serial_number(_register):
        reg = _Register(0x0001,'SIS8300_SERIAL_NUMBER_REG',               '7.7.2',   True, False,'This register holds the Serial Number of the module.')
    class user(_register):
        reg = _Register(0x0004,'SIS8300_USER_CONTROL_STATUS_REG',         '7.7.3',   True, True, 'The control register is implemented as a selective J/K register, a specific function is enabled by writing a 1 into the set/enable bit, the function is disabled by writing a 1 into the clear/disable bit (which location is 16-bit higher in the register). An undefined toggle status will result from setting both the enable and disable bits for a specific function at the same time. The only function at this point in time is user LED on/off.')
        led_test = _JKFlag(reg, 1, 17, "Controls LED-Test")
        user_led = _JKFlag(reg, 0, 16, "Controls User-LED")
    class firmware_options(_register):
        reg = _Register(0x0005,'SIS8300_FIRMWARE_OPTIONS_REG',            '7.7.4',   True, False,'This register holds the information of the Xilinx firmware option features.')
    class adc_temp_i2c(_register):
        reg = _Register(0x0006,'SIS8300_ADC_TEMP_I2C_REG',                '7.7.5',   True, True, '')
    class pcie(_register):
        status = _Register(0x0007,'SIS8300KU_PCIE_STATUS_REG',            '7.7.6',   True, False,'This register holds the information of the PCI Express Link.')
    class acquisition(_register):
        reg = _Register(0x0010,'SIS8300_ACQUISITION_CONTROL_STATUS_REG',  '7.7.7',   True, True, '')
        def start(self):  self.reg = 1<<0
        def arm(self):    self.reg = 1<<1
        def reset(self):  self.reg = 1<<2
        @property
        def is_armed(self):   return self.reg&(1<<1)>0
        @property
        def is_enabled(self): return self.reg&(1<<0)>0
        @property
        def is_busy(self):    return self.reg&(1<<4)>0

        def wait4done(self,timeout):
            i = 0
            if debug>0: print('waiting to be done')
            while self.is_busy:
                if i>=timeout: return False
                time.sleep(.2)
                i+=1
            return True

    class sample(_module):
        control = _Register(0x0011,'SIS8300_SAMPLE_CONTROL_REG',              '7.7.8',   True, True, 'ADC channels can be disabled from storing data to memory by setting the corresponding disable bit in this register.')
        _mask_channels = 0b001111111111
        _mask_external = 0b100000000000
        _mask_internal = 0b010000000000
        __chanlist = None
        @property
        def _chanlist(self):
            return set(np.nonzero(np.frombuffer(bin((~self.control)&self._mask_channels)[2:],np.uint8))[0])
        @property
        def chanlist(self):
            if self.__chanlist is None:
                self.__chanlist = self._chanlist
            return self.__chanlist
        @chanlist.setter
        def chanlist(self,chanlist):
            if chanlist is None: chanlist = set(range(self.numchan))
            else:                chanlist = set(chanlist)
            bits = (1<<12)-1 # convert include list in exclude list
            for i in chanlist:
                if i<0 or i>10: raise Exception('Channel out of range [0..9]')
                bits&= ~(1<<i)
            self.__chanlist = None
            self.control = bits
            self.__chanlist = chanlist
        class start_block(_module):
            reg = _Register(0x0120,'SIS8300_SAMPLE_START_ADDRESS_CH#_REG',    '7.7.31',  True, True, 'The write function to these registers defines the memory start block address.',(1,10))
            def get(self,i):     return self.reg[i+1]>>1
            def set(self,i,val): self.reg[i+1] = int(val)<<1
            def update(self):
                actchan = self._up.chanlist
                max_blocks = self.max_blocks
                for i,ch in enumerate(actchan):
                    reg = i*max_blocks
                    self.set(ch,reg)
                    if debug>0: print("set %d:0x%08x"%(ch,reg*self._bytes_per_block))
        def start_address(self,chan):
            max_blocks = self.max_blocks
            for i,ch in enumerate(self.chanlist):
                if ch==chan:
                    return max_blocks*i*self._bytes_per_block
            return -1
        _length = _Register(0x012A,'SIS8300_SAMPLE_LENGTH_REG',               '7.7.32',  True, True, 'This register defines the number of sample blocks of each ADC channel.')
        @property
        def length(self): return (self._length>>1)*self._samples_per_block
        @length.setter # 5.5 DDR4 Memory buffer Address structure: into next 512 block count -((-a)//b) is like ceil
        def length(self,value): self._length = (-((-value)//self._samples_per_block))<<1
        def setup(self,length,chanlist):
            self.length   = length
            if debug>0: print("length = %d (%d) -> %d"%(length,self._length,self.length))
            self.chanlist = chanlist
            self.acquisition.reset()
            self.start_block.update()

    class mlvds_io(_register):
        reg = _Register(0x0012,'SIS8300_MLVDS_IO_CONTROL_REG',            '7.7.9',   True, True, '')
        def setup(self,issingle=False,isfalling=False,ismaster=True):
            if issingle:
                bits = 0
            else:
                bits = 1<<8  # use MLVDS Input 0 as trigger
                if ismaster:
                    bits|= 1<<24 # enable MLVDS Output 0
                if isfalling: bits|= 1<<0 # Input0 Falling Edge
            self.reg = bits
    class rj45_io(_register):
        reg = _Register(0x0013,'SIS8300KU_RJ45_IO_CONTROL_REG',           '7.7.10',  True, True, '')
        def setup(self,issingle=False,isfalling=False):
            if issingle:
                bits = 1<<9
                if isfalling:  bits|= 1<<13 # Input1 Falling Edge
            else:
                # bits = 1<<24 # Input1 on MLVDS Out 0, even if not armed
                bits= 1<<25 # Input1 on MLVDS Out 0 if armed
            self.reg = bits
    class sfpx_link(_register):
        reg = _Register(0x0014,'SIS8300KU_SFP#_LINK_CONTROL_STATUS_REG',  '7.7.11',  True, True, 'Control for link on sfp 1 and 2',(1,2))
    class portx_link(_register):
        reg = _Register(0x0016,'SIS8300KU_PORT##_LINK_CONTROL_STATUS_REG','7.7.13',  True, True, 'Control for link on port 12, 13, 14, and 15',(12,15))
    class dac_trigger(_register):
        reg = _Register(0x0020,'SIS8300_DAC_TRIGGER_CONTROL_REG',         '7.7.15',  True, True, '')
    class clock_distribution_mux(_register):
        reg = _Register(0x0040,'SIS8300_CLOCK_DISTRIBUTION_MUX_REG',      '7.7.16',  True, True, 'The  SIS8300-KU  has 5 IDT  ICS853S057  clock multiplexer  chips, which are labelled A to E in the clock distribution scheme  in section  2.8. The multiplexer  control register holds the two select bits for the 5 multiplexer chips as shown in the table below.')
        ab_rtm_clk2 = 0<<0 | 0<<2 # set MUX A/B to RTM_CLK2
        ab_amc_clkb = 1<<0 | 1<<2 # set MUX A/B to TCLKB
        ab_amc_clka = 2<<0 | 2<<2 # set MUX A/B to TCLKA
        ab_onboard  = 3<<0 | 3<<2 # set MUX A/B to ADCLK946 250MHz
        c_rj45 = 0<<4             # set MUX  C  to EXTCLKB
        c_sma  = 1<<4             # set MUX  C  to EXTCLKA
        c_muxa = 2<<4;c_int=c_muxa# set MUX  C  to MUX  A
        c_muxb = 3<<4             # set MUX  C  to MUX  B
        de_int = 0<<8 | 0<<10     # set MUX D/E to MUX A/B
        de_mul = 1<<8 | 1<<10     # set MUX D/E to SI5326 aka MUL
        de_rj45= 2<<8 | 2<<10     # set MUX D/E to EXTCLKB
        de_sma = 3<<8 | 3<<10     # set MUX D/E to EXTCLKA
        #  config  =     INT    |   MUL  | ADC
        bypass_rj45=      0     |    0   | de_rj45
        bypass_sma =      0     |    0   | de_sma
        bypass_int = ab_onboard |    0   | de_int
        bypass_clka= ab_amc_clka|    0   | de_int
        bypass_clkb= ab_amc_clkb|    0   | de_int
        si5326_rj45=      0     | c_rj45 | de_mul
        si5326_sma =      0     | c_sma  | de_mul
        si5326_int = ab_onboard | c_int  | de_mul
        si5326_clka= ab_amc_clka| c_int  | de_mul
        si5326_clkb= ab_amc_clkb| c_int  | de_mul
    mux = clock_distribution_mux
    class ad9510(_register):
        reg = _Register(0x0041,'SIS8300_AD9510_SPI_REG',                  '7.7.17',  True, True, 'The  parameters of the Clock Distribution IC AD9510  chips  can be configured with  the SPI (serial Peripheral Interface).')
        @property
        def is_busy(self): return (self.reg&(1<<31))>0
    class clock_multiplier(_register):
        reg = _Register(0x0042,'SIS8300_CLOCK_MULTIPLIER_SPI_REG',        '7.7.18',  True, True, 'Several parameters of the  Clock Multiplier    SI5326    chip can be configured with the SPI (serial Peripheral Interface).')
        @property
        def is_busy(self): return (self.reg&(1<<31))>0
    class mgtclk_synth_i2c(_register):
        reg = _Register(0x0043,'SIS8300_MGTCLK_SYNTH_I2C_REG',            '7.7.19',  True, True, 'Several parameters of the  Clock  Synthesizer  SI5338A  chip can be configured with the  I\xc2\xb2C Interface.')
    class spi_flash(_register):
        reg = _Register(0x0044,'SIS8300_SPI_FLASH_REG',                   '7.7.20',  True, True, '')
    class dac_control(_register):
        reg = _Register(0x0045,'SIS8300_DAC_CONTROL_REG',                 '7.7.21',  True, True, '')
    class dac_data(_register):
        reg = _Register(0x0046,'SIS8300_DAC_DATA_REG',                    '7.7.22',  True, True, '')
    class rtm_i2c_bus(_register):
        reg = _Register(0x0047,'SIS8300_RTM_I2C_BUS_REG',                 '7.7.23',  True, True, 'Rear Transition Modules (uRTMs) like the DWC8VM1 or DWC8300 have components that are configured and/or read out by an I\xc2\xb2C interface over the Zone 3 connector.')
    class adc_serial_interface(_register):
        reg = _Register(0x0048,'SIS8300_ADC_SERIAL_INTERFACE_REG',        '7.7.24',  True, True, 'Several parameters of the ADC AD9268 chip  can be configured with the SPI (serial Peripheral Interface).')
    class adc_input_tap_delay(_register):
        reg = _Register(0x0049,'SIS8300_ADC_INPUT_TAP_DELAY',             '7.7.25',  True, True, 'The ADC input tap delay register is used to adjust the FPGA data strobe timing.')
    class wr_dac(_register):
        reg = _Register(0x004A,'SIS8300_WR_DAC_SPI_REG',                  '7.7.26',  True, True, 'Depended on assembly option (White Rabbit option) two  additional clock sources  (WR_CLK and  WR_20M_VCXO)  are provided to the FPGA.  These clocks are destined for a White Rabbit IP  Core and for this it is required to adjust  the frequency  in a narrow range.  The clock adjustment are controlled by two DAC  AD5662  chips  which can control  by the  interface register. Please refer to the documentation of the DAC AD5662 for details.')
    class dac_trigger_preclk(_register):
        reg = _Register(0x004E,'SIS8300_DAC_TRIGGER_PRECLK_REG',          '7.7.27',  True, True, "The DAC uses the  same clock source like the ADC 1. With the DAC CLK prescaler  it is possible to define a different clock period based on the ADC clock.  The  prescaler  are set up by the divider Bits: 1 _ _ \xef\x80\xab \xef\x80\xbd divider CLK SOURCE CLK DAC (A divider value of one generates a twice period time of the source) The 'ADC trigger select Bits' defines the internal trigger source: 0h = ADC_1, ..., 9h = ADC_10 Bit  write  read 31  DAC 2 CLK divider Bit 7  DAC 2 CLK divider Bit 7 ...  ...  ...")
    class dac_data_endp(_register):
        reg = _Register(0x004F,'SIS8300_DAC_DATA_ENDP_REG',               '7.7.28',  True, True, "This register defines the last element in the DAC RAM and mark the value at the RAM read logic 'wrap around' to the first RAM element (if the 'Wrap  select'  Bit set at the DAC control register).")
    class ublaze(_register):
        reg = _Register(0x00FE,'SIS8300_UBLAZE_CONTROL_STATUS_REG',       '7.7.29',  True, True, 'A write from PCIe side to this register is always possible.')
    class master_reset(_register):
        reg = _Register(0x00FF,'_MASTER_RESET',                           '7.6',     False,True, 'Bit 0 = 1:   Master Reset (reset all registers)')
    class trigger(_module):
        setup     = _Register(0x0100,'SIS8300_TRIGGER_SETUP_CH#_REG',           '7.7.30.1',True, True, 'These read/write registers hold the 8-bit wide trigger pulse length (in sample clocks), the Peaking and Gap Time of the trapezoidal FIR filter.',(1,10))
        threshold = _Register(0x0110,'SIS8300_TRIGGER_THRESHOLD_CH#_REG',       '7.7.30.2',True, True, 'These read/write registers hold the threshold values for the 10 ADC channels.',(1,10))
    class pretrigger_delay(_register):
        reg = _Register(0x012B,'SIS8300_PRETRIGGER_DELAY_REG',            '7.7.33',  True, True, 'This register defines the number of pre trigger delay samples for all channels.')
    class rtm_lvds_io(_register):
        reg = _Register(0x012F,'SIS8300_RTM_LVDS_IO_CONTROL_REG',         '7.7.34',  True, True, '')
    class dma(_module):
        class read(_register):
            _dst_adr_lo32 = _Register(0x0200,'DMA_READ_DST_ADR_LO32',                   '7.7.35',  True, True, 'This register holds the lower 32bits of the destination address (byte address !) in system memory into which the card will transfer data.')
            _dst_adr_hi32 = _Register(0x0201,'DMA_READ_DST_ADR_HI32',                   '7.7.36',  True, True, 'This register holds the upper 32bits of the destination address (byte address !) in system memory into which the card will transfer data.')
            @property
            def dst_adr(self):
                lohi = np.array([self._dst_adr_lo32,self._dst_adr_hi32],np.int32)
                return np.frombuffer(tobytes(lohi),np.int64).tolist()[0]
            @dst_adr.setter
            def dst_adr(self,value):
                lohi = np.frombuffer(tobytes(np.int64(value)),np.int32)
                self._dst_adr_lo32,self._dst_adr_hi32 = lohi.tolist()
            _src_adr_lo32 = _Register(0x0202,'DMA_READ_SRC_ADR_LO32',                   '7.7.37',  True, True, 'This register holds the 32bit source (byte) address in the card\xe2\x80\x99s address space which is used to select the data source which is read from.')
            src_adr = _src_adr_lo32
            len = _Register(0x0203,'DMA_READ_LEN',                            '7.7.38',  True, True, 'This register holds the amount of data (bytes !)  which is going to be transferred.')
            ctrl= _Register(0x0204,'DMA_READ_CTRL',                           '7.7.39',  True, True, 'This register starts the Read DMA process and allows to poll the transfer status.')
            byteswap = _Register(0x0205,'DMA_READ_BYTESWAP',                       '7.7.40',  True, True, 'This register allows swapping each byte in a sample for optimizing data handling on big/little endian machines.')
        class write(_module):
            _dst_adr_lo32 = _Register(0x0210,'DMA_WRITE_DST_ADR_LO32',                  '7.7.41',  True, True, 'This register holds the lower 32bits of the destination address (byte address !)  in system memory from which the card will transfer data.')
            _dst_adr_hi32 = _Register(0x0211,'DMA_WRITE_DST_ADR_HI32',                  '7.7.42',  True, True, 'This register holds the upper 32bits of the destination address (byte address !)  in system memory from which the card will transfer data.')
            @property
            def dst_adr(self):
                lohi = np.array([self._dst_adr_lo32,self._dst_adr_hi32],np.int32)
                return np.frombuffer(tobytes(lohi),np.int64).tolist()[0]
            @dst_adr.setter
            def dst_adr(self,value):
                lohi = np.frombuffer(tobytes(np.int64(value)),np.int32)
                self._dst_adr_lo32,self._dst_adr_hi32 = lohi.tolist()
            _src_adr_lo32 = _Register(0x0212,'DMA_WRITE_SRC_ADR_LO32',                  '7.7.43',  True, True, 'This register holds the 32bit destination (byte) address in the cards address space which is used to select the data source which is written to.')
            src_adr = _src_adr_lo32
            len = _Register(0x0213,'DMA_WRITE_LEN',                           '7.7.44',  True, True, 'This register holds the amount of data (bytes) which is going to be transferred.')
            ctrl= _Register(0x0214,'DMA_WRITE_CTRL',                          '7.7.45',  True, True, 'This register starts the Write DMA process and allows to poll the transfer status.')
        _pc2card_max_nof_outstanding_requests = _Register(0x0215,'DMA_PC2CARD_MAX_NOF_OUTSTANDING_REQUESTS','7.7.46',  True, False, 'This register defines the maximal number of outstanding requests during a Write DMA process. The default value is 16 (0x10).')
        @property
        def pc2card_max_nof_outstanding_requests(self): return self._pc2card_max_nof_outstanding_requests&0b111111
    class daq_dma_chain(_register):
        reg = _Register(0x0216,'DAQ_DMA_CHAIN',                           '7.7.47',  True, True, 'This register allows the chaining of the DAQ Done Signal into the DMA Start Signal.')
    class irq(_module):
        _enable = _Register(0x0220,'IRQ_ENABLE',                              '7.7.48',  True, True, 'This register enables each interrupt source for interrupt generation. The register is implemented as a J-K register.')
        _status = _Register(0x0221,'IRQ_STATUS',                              '7.7.49',  True, False,'This register lists the latched interrupt bits for which an interrupt has been generated.')
        _clear  = _Register(0x0222,'IRQ_CLEAR',                               '7.7.50',  False,True, 'This register clears any handled interrupts and allows the logic to generate new interrupts.')
        _refresh= _Register(0x0223,'IRQ_REFRESH',                             '7.7.51',  False,True ,'This register refreshes the interrupt logic. This might be needed in the case an interrupt happens while the software interrupt service routine was still handling the previous interrupt.')
        user          = _JKFlag(_enable, 15, 31, "User IRQ enabled status")
        daq_done      = _JKFlag(_enable, 14, 30, "DAQ Done IRQ enabled status")
        dma_write_done= _JKFlag(_enable,  1, 17, "Write DMA Done IRQ enabled status")
        dma_read_done = _JKFlag(_enable,  0, 16, "Read DMA Done IRQ enabled status")
        is_user          = _SCFlag(_status, _clear, 15, "Checks or clears User IRQ")
        is_daq_done      = _SCFlag(_status, _clear, 14, "Checks or clears DAQ Done IRQ")
        is_dma_write_done= _SCFlag(_status, _clear,  1, "Checks or clears Write DMA Done IRQ")
        is_dma_read_done = _SCFlag(_status, _clear,  0, "Checks or clears Read DMA Done IRQ")
        def refresh(self): self._refresh=1
    class memory_test(_register):
        reg = _Register(0x0230,'_MEMORY_TEST',                            '7.6',     True, True, 'MEMORY test Mode register')
    class ram_fifo_debug(_register):
        reg = _Register(0x0231,'_RAM_FIFO_DEBUG',                         '7.6',     True, True, 'RAM FIFO debug register')
    class external(_register):
        reg = _Register(0x0400,'_EXTERNAL_REG',                           '7.6',     True, True, 'Mapped out of register bank to top level. May be used for user defined register.',0x1000-0x400)
    def reset(self): self.master_reset.reg = 1;self.user.user_led=True

    class adc(_module):
        def setup(self):
            self.check(self.lib.sis830x_ADC_AD9268_SPI_Setup(self.byref))

    """ CLOCK CONTROL """
    class clock(_module):
        @property
        def mux(self): return self._up.mux.reg
        @mux.setter
        def mux(self,value): self._up.mux.reg = value
        def divider(self,div):
            if div<0 or div>32: raise Exception("divider out of range [1..32]")
            if div==1:
                value = 0x8000
            else:
                value = ((div-2)//2)<<4|((div-1)//2)
            if debug>0: print("divider value 0x%04x"%value)
            values=np.array(([value]*7)+[0xC000,0x001D],np.uint32)
            values_ptr = values.ctypes.data_as(ct.POINTER(ct.c_uint32))
            self.check(self.lib.sis830x_AD9510_SPI_Setup(self.byref, values_ptr, ct.c_int32(1)))
        divider = property(None,divider)
    class si5326(_module):
        lfin = (   2e3, 710e6)
        lf3  = (   2e3,   2e6)
        lfosc= (4850e6,5670e6)
        lfout= (   2e3,1475e6)
        @classmethod
        def get_params(cls,fin,fout):
            """ TOOL to calculate si5326 params for a better result use DSPLLsim"""
            if fin<cls.lfin[0] or fin>cls.lfin[1]:
                raise Exception('fin out of range: %3.f < fin < %3.f'%cls.lfin)
            if fout<cls.lfout[0] or fout>cls.lfout[1]:
                raise Exception('fout out of range: %3.f < fout < %3.f'%cls.lfout)
            N3 = int(fin/2e6)
            f3 = fin/N3
            N2 = int(5e9/f3)
            if (N2%2)>0:
                N2*=2;N3*=2;f3/=2
            for N2_hs in range(32,512):
                if N2%N2_hs==0: break
            N2_ls=N2//N2_hs
            if N2_ls%2>1:
                N2_ls*=1;N3*=2;f3/=2
            fosc = f3*N2_ls*N2_hs
            N1 = int(fosc/fout)
            for N1_hs in range(4,12):
                if N1%N1_hs==0: break
            N1_ls=N1//N1_hs
            FOUT = fin/N3*N2_hs*N2_ls/N1_hs/N1_ls
            if f3<cls.lf3[0] or f3>cls.lf3[1]:
                raise Exception('f3 out of range: %3.f < f3 < %3.f'%cls.lf3)
            if fosc<cls.lfosc[0] or fosc>cls.lfosc[1]:
                raise Exception('fosc out of range: %3.f < fosc < %3.f'%cls.lfosc)
            print([N1_hs,N1_ls,N2_hs,N2_ls,N3],fin,f3,fosc,FOUT)
            return [N1_hs,N1_ls,N2_hs,N2_ls,N3]
        @property
        def LOL(self): return bool(self.ReadRegister(130)&1)
        @property
        def LOS(self): return bool(self.ReadRegister(129)&2)
        def reset(self):
            self.WriteRegister(136,1<<7)
            time.sleep(0.01)
        def ical(self):
            self.WriteRegister(136,1<<6)
            time.sleep(.1)
            while self.LOL:
                time.sleep(.1)
        def WriteRegister(self,addr,data):
            self.check(self.lib.sis830x_si5326_WriteRegister(self.byref, ct.c_uint32(addr), ct.c_uint32(data)))
        def ReadRegister(self,addr):
            data = ct.c_uint32(0)
            self.check(self.lib.sis830x_si5326_ReadRegister(self.byref, ct.c_uint32(addr), ct.byref(data)))
            return data.value&0xff
        def SetParameterExternalClk(self,n1_hs, nc1_ls, nc2_ls, n2_hs, n2_ls, n31, bw_sel):
            """
            n1_hs:  N1  high speed divider which drives NCn_LS (n = 1 to 2) low-speed divider. 000:4,111:11
            nc1_ls: NC1 low-speed divider, which drives CKOUT1 output. 0:1,1:2,3:4,...
            nc2_ls: NC2 low-speed divider, which drives CKOUT2 output. 0:1,1:2,3:4,...
            n2_hs:  N2  high speed divider, which drives N2LS          32:2:512
            n31:    input divider for CKIN1
            """
            n1_hs, nc1_ls, nc2_ls, n2_hs, n2_ls, n31, bw_sel = map(ct.c_uint32,[n1_hs, nc1_ls, nc2_ls, n2_hs, n2_ls, n31, bw_sel])
            self.check(self.lib.sis830x_si5326_SetParameterExternalClk(self.byref, n1_hs, nc1_ls, nc2_ls, n2_hs, n2_ls, n31, bw_sel))
        @staticmethod
        def mult(N1_H,N1_L,N2_H,N2_L,N3):
            return 1./N3*(N2_H*N2_L)/(N1_H*N1_L)
        def load(self,reg):
            self.reset()
            for r,v in reg:
                if debug>0: print("%d: 0x%02x"%(r,v))
                self.WriteRegister(r,v)
            time.sleep(1)
            self.ical()
            time.sleep(1)
            self.test()
        def setup(self,N1_H,N1_L,N2_H,N2_L,N3,BW=10):
            mult = self.mult(N1_H,N1_L,N2_H,N2_L,N3)
            if mult == 1.:
                if debug>0: print("using bypass method")
                self.BypassExternalClk()
                return mult
            if debug>0: print("using clock mupliplier %f"%mult)
            self.SetParameterExternalClk(N1_H,N1_L,4,N2_H,N2_L,N3,BW)
            return mult
        def BypassExternalClk(self):
            self.check(self.lib.sis830x_si5326_BypassExternalClk(self.byref))
        def BypassInternalRefOsc(self):
            self.check(self.lib.sis830x_si5326_BypassInternalRefOsc(self.byref))
        def GetStatusExternalClk(self):
            data = ct.c_uint32(0)
            self.check(self.lib.sis830x_si5326_GetStatusExternalClk(self.byref, ct.byref(data)))
            return data.value
        def test(self):
            status = self.GetStatusExternalClk()
            if (status&1==0): print("CKIN1 is not the active input clock")
            if (status&2 >0): print("Internal loss-of-signal alarm on CKIN1 input")


    def setup(self,pretrigger=0,samples=1000000,chanlist=None,mux=mux.bypass_int,divider=25,si5326=None,issingle=False,isfalling=False,ismaster=True):
        self.reset()
        if chanlist is None: chanlist = set(range(sis8300ku.numchan))
        self.rj45_io.setup(issingle,isfalling)
        self.mlvds_io.setup(issingle,isfalling,ismaster)
        if si5326 is None:
            self.si5326.BypassExternalClk()
            mult = 1
        else:
            mult = self.si5326.setup(*si5326)
        self.clock.mux     = mux
        self.clock.divider = divider
        time.sleep(0.001)
        self.pretrigger_delay.reg = pretrigger
        self.sample.setup(samples,chanlist)
        self.adc.setup()
        time.sleep(0.01)
        if debug>0: print("Multiplier is set to %f/%d = "(mult,divider,mult/divider))

    def reader(self,channel,num_samples=max_samples,max_chunk=1<<17):
        """Initializes a generator of data chunks for the requested channel.
        The reader will read a total of num_samples samples.
        The maximum number of samples per chunk is given by max_chunk (mult of 32)
        """
        num_samples = min(num_samples,self.max_samples)  # should not read more than possible
        address = self.sample.start_address(channel)                   # byte address of first block
        if debug>0:
            end_address = self.sample.start_block.get(channel)*self._bytes_per_block
            print("get %d:0x%08x - 0x%08x : %d"%(channel,address,end_address,(end_address-address)//2))
        buf_len = -((-max_chunk)//self._bytes_per_block)*self._bytes_per_block  # need to read full blocks
        buf = (ct.c_uint16*buf_len)()
        for offset in range(0,num_samples-buf_len,buf_len):
            try:    self.readMemory(address+offset*2,buf)
            except: pass
            yield offset,(np.array(buf)-32768).astype(np.int16)
        offset += buf_len*2
        left = num_samples-offset
        if left<=0: return
        self.readMemory(address+offset*2,buf)
        yield offset,(np.array(buf)[:left]-32768).astype(np.int16)

""" MDSPLUS  STUFF """

try:
 import MDSplus as m

 class mdsrecord(object):
    """ A class for general interaction with MDSplus nodes
    obj._trigger = mdsrecord('trigger',float)
    obj._trigger = 5   <=>   obj.trigger.record = 5
    a = obj._trigger   <=>   a = float(obj.trigger.record.data())
    """
    def __get__(self,inst,cls):
        data = inst.__getattr__(self._name).record.data()
        if self._fun is None: return data
        return self._fun(data)
    def __set__(self,inst,value):
        inst.__getattr__(self._name).record=value
    def __init__(self,name,fun=None):
        self._name = name
        self._fun  = fun

 class SIS8300KU(m.Device):
    _device_class = sis8300ku
    parts = [
      {'path': ':ACTIONSERVER', 'type': 'TEXT',    'options':('no_write_shot','write_once')},
      {'path': ':ACTIONSERVER:INIT',          'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(node.DISPATCH,node.TASK)'},
      {'path': ':ACTIONSERVER:INIT:DISPATCH', 'type': 'DISPATCH','options':('no_write_shot','write_once'), 'valueExpr':'Dispatch(head.actionserver,"INIT",31)'},
      {'path': ':ACTIONSERVER:INIT:TASK',     'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"init",head)'},
      {'path': ':ACTIONSERVER:ARM',          'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(node.DISPATCH,node.TASK)'},
      {'path': ':ACTIONSERVER:ARM:DISPATCH', 'type': 'DISPATCH','options':('no_write_shot','write_once'), 'valueExpr':'Dispatch(head.actionserver,"INIT",head.actionserver_init)'},
      {'path': ':ACTIONSERVER:ARM:TASK',     'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"arm",head)'},
      {'path': ':ACTIONSERVER:SOFT_TRIGGER',          'type': 'ACTION',  'options':('no_write_shot','write_once','disabled'), 'valueExpr':'Action(node.DISPATCH,node.TASK)'},
      {'path': ':ACTIONSERVER:SOFT_TRIGGER:DISPATCH', 'type': 'DISPATCH','options':('no_write_shot','write_once'), 'valueExpr':'Dispatch(head.actionserver,"PULSE",1)'},
      {'path': ':ACTIONSERVER:SOFT_TRIGGER:TASK',     'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"soft_trigger",head)'},
      {'path': ':ACTIONSERVER:STORE',          'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(node.DISPATCH,node.TASK)'},
      {'path': ':ACTIONSERVER:STORE:DISPATCH', 'type': 'DISPATCH','options':('no_write_shot','write_once'), 'valueExpr':'Dispatch(head.actionserver,"STORE",31)'},
      {'path': ':ACTIONSERVER:STORE:TASK',     'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"store",head)'},
      {'path': ':ACTIONSERVER:DEINIT',          'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(node.DISPATCH,node.TASK)'},
      {'path': ':ACTIONSERVER:DEINIT:DISPATCH', 'type': 'DISPATCH','options':('no_write_shot','write_once'), 'valueExpr':'Dispatch(head.actionserver,"DEINIT",31)'},
      {'path': ':ACTIONSERVER:DEINIT:TASK',     'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"deinit",head)'},
      {'path': ':COMMENT',        'type': 'text'},
      {'path': ':DEVICE_PATH',    'type': 'text',     'value': '/dev/sis8300-0',   'options': ('no_write_shot',)},
      {'path': ':TRIGGER',        'type': 'numeric',  'valueExpr':'Int64(0)',      'options': ('no_write_shot',), 'help':'offset of trigger to reference, Input1'},
      {'path': ':TRIGGER:PRE',    'type': 'numeric',  'value': 0,             'options': ('no_write_shot',)},
      {'path': ':TRIGGER:POST',   'type': 'numeric',  'value': 1000000,       'options': ('no_write_shot',)},
      {'path': ':CLOCK',          'type': 'numeric',  'valueExpr':'DIVIDE(LONG(MULTIPLY(node.IN,node.SI5326.MULTI)),node.DIVIDER)', 'options': ('no_write_shot',)},
      {'path': ':CLOCK:IN',       'type': 'numeric',  'value': 250000000, 'options': ('no_write_shot',)},
      {'path': ':CLOCK:DIVIDER',  'type': 'numeric',  'value': 25,        'options': ('no_write_shot',)},
      {'path': ':CLOCK:MUX',      'type': 'numeric',  'valueExpr':'node.SI5326_INT',      'options': ('no_write_shot',),             'help':'MUX value chose from members'},
      {'path': ':CLOCK:MUX:BYPASS_INT', 'type': 'numeric', 'value':sis8300ku.mux.bypass_int, 'options': ('no_write_shot','write_once'), 'help':'internal 250MHz clock'},
      {'path': ':CLOCK:MUX:BYPASS_RJ45','type': 'numeric', 'value':sis8300ku.mux.bypass_rj45,'options': ('no_write_shot','write_once'), 'help':'front panel rj45 connector'},
      {'path': ':CLOCK:MUX:BYPASS_SMA', 'type': 'numeric', 'value':sis8300ku.mux.bypass_sma, 'options': ('no_write_shot','write_once'), 'help':'front panel sma connector'},
      {'path': ':CLOCK:MUX:BYPASS_CLKA','type': 'numeric', 'value':sis8300ku.mux.bypass_clka,'options': ('no_write_shot','write_once'), 'help':'backplane TCLKA'},
      {'path': ':CLOCK:MUX:BYPASS_CLKB','type': 'numeric', 'value':sis8300ku.mux.bypass_clkb,'options': ('no_write_shot','write_once'), 'help':'backplane TCLKB'},
      {'path': ':CLOCK:MUX:SI5326_INT', 'type': 'numeric', 'value':sis8300ku.mux.si5326_int, 'options': ('no_write_shot','write_once'), 'help':'internal 250MHz clock'},
      {'path': ':CLOCK:MUX:SI5326_RJ45','type': 'numeric', 'value':sis8300ku.mux.si5326_rj45,'options': ('no_write_shot','write_once'), 'help':'front panel rj45 connector'},
      {'path': ':CLOCK:MUX:SI5326_SMA', 'type': 'numeric', 'value':sis8300ku.mux.si5326_sma, 'options': ('no_write_shot','write_once'), 'help':'front panel sma connector'},
      {'path': ':CLOCK:MUX:SI5326_CLKA','type': 'numeric', 'value':sis8300ku.mux.si5326_clka,'options': ('no_write_shot','write_once'), 'help':'backplane TCLKA'},
      {'path': ':CLOCK:MUX:SI5326_CLKB','type': 'numeric', 'value':sis8300ku.mux.si5326_clkb,'options': ('no_write_shot','write_once'), 'help':'backplane TCLKB'},
      {'path': ':CLOCK:SI5326',         'type': 'numeric', 'valueExpr':'tdi("[11,  2, 5,  550, 125]")', 'options': ('no_write_shot',), 'help':'N1_HS,N1_LS,N2_HS,N2_LS,N3,BW_SEL'},
      {'path': ':CLOCK:SI5326:MULTI',   'type': 'numeric', 'valueExpr':'TdiCompile("IF_ERROR(1./$1[4]*$1[3]*$1[2]/$1[1]/$1[0],1)",node.parent)', 'options': ('no_write_shot','write_once'), 'help':'Return the Multiplier for the si5326'},
    ]
    # [ 5,104, 5,  520, 125] # -> 250. ->  10.
    # [ 5, 34, 8,  340, 125] # -> 250. ->  32.
    # [11, 10,11,  250, 125] # -> 250. ->  50.
    # [ 9,  6, 5,  540, 125] # -> 250. -> 100.
    # [ 4, 10,10,  250, 125] # -> 250. -> 125.
    # [11,  2, 5,  550, 125] # -> 250. -> 250.
    # [ 7, 80, 4,  700,   5] # ->  10. ->  10.
    # [ 5, 34, 8,  340,   5] # ->  10. ->  32.
    # [ 7, 16,10,  280,   5] # ->  10. ->  50.
    # [ 4, 14,10,  280,   5] # ->  10. -> 100.
    # [11,  4, 5,  555,   5] # ->  10. -> 125.
    # [ 5,  4,10,  250,   5] # ->  10. -> 250.
    for i in range(_device_class.numchan):
            parts.append({'path': ':CHANNEL%d'%i,        'type': 'SIGNAL',                         'options': ('no_write_model', 'write_once',)})
            parts.append({'path': ':CHANNEL%d:BITOFF'%i, 'type': 'NUMERIC', 'valueExpr':'Int32(0)','options': ('no_write_shot',), 'help':'bit offset'})
            parts.append({'path': ':CHANNEL%d:SLOPE'%i,  'type': 'NUMERIC', 'value':1./32767,      'options': ('no_write_shot',)})

    _device_path    = mdsrecord('device_path',str)
    _clock          = mdsrecord('clock',int)
    _clock_in       = mdsrecord('clock_in',int)
    _clock_divider  = mdsrecord('clock_divider',int)
    _clock_mux      = mdsrecord('clock_mux',int)
    _trigger        = mdsrecord('trigger',int)
    _trigger_pre    = mdsrecord('trigger_pre',int)
    _trigger_post   = mdsrecord('trigger_post',int)
    _devices = {}
    @property
    def numchan(self): return self._device_class.numchan
    @property
    def device(self):
        path = self._device_path
        if not path in self._devices:
            self._devices[path] = self._device_class(path)
        device = self._devices[path]
        if not device.is_open: device.open()
        return device
    def close_device(self):
        del(self._devices[self._device_path])
    def init(self,issingle=False,isfalling=False):
        issingle,isfalling = bool(issingle),bool(isfalling)
        dev = self.device
        pre = self._trigger_pre
        divider = self._clock_divider
        chanlist = [i for i in range(self.numchan) if self.__getattr__('channel%d'%(i)).on]
        ismaster = self._device_path.endswith("-0")
        try: reg = self.clock_si5326.record.data().tolist()
        except m.TreeNODATA: reg = None
        dev.setup(pre, pre+self._trigger_post, chanlist, self._clock_mux, divider, reg, issingle, isfalling, ismaster)
    def soft_trigger(self):
        self.device.acquisition.start()
    def arm(self):
        dev = self.device
        dev.acquisition.arm()
        i = 0
        while not dev.acquisition.is_enabled:
            if i >= 100: return False
            time.sleep(.1)
            i+=1
        return True

    def store(self):
        dev = self.device
        dt = 1000000000//self._clock
        if dev.acquisition.is_armed:
            raise m.DevNOT_TRIGGERED
        if not dev.acquisition.wait4done(self._trigger_post*dt):
            raise Exception('Timeout')
        trg= self._trigger
        dim = m.Dimension(m.Window(None,None,self.trigger),m.Range(None,None,m.DIVIDE(m.Int64(1e9),self.clock)))
        for ch in range(self.numchan):
            node = self.__getattr__('channel%d'%ch)
            if not node.on: continue
            node.setSegmentScale(m.MULTIPLY(m.SUBTRACT(m.dVALUE(),node.BITOFF),node.SLOPE))
            pre = self._trigger_pre
            for idx0,buf in self.device.reader(ch,pre+self._trigger_post):
                idx0-= pre
                idx1 = idx0+len(buf)-1
                dim[0][0],dim[0][1] = m.Int32(idx0),m.Int32(idx1)
                if debug>2: print(idx0*dt+trg,idx1*dt+trg,dim,len(dim),buf.shape)
                node.makeSegment(m.Int64(idx0*dt+trg),m.Int64(idx1*dt+trg),dim,buf)

    def deinit(self):
        self.device.acquisition.reset()


 def acq_test(devid=0,len=1e6,exttrg=False,extclk=False):
    offsets = [
        [-109,-109,-83,-118,8,-35,-8,-130,-43,45],
    ]
    from LocalDevices.sis83xx import SIS8300KU as DEV
    m.setenv('test_path','/tmp')
    with m.Tree('test',-1,'NEW') as t:
        dev = DEV.Add(t,'SIS8300_%d'%devid)
        dev.DEVICE_PATH.record = '/dev/sis8300-%i'%devid
        t.write()
        for ch in range(10):
            dev.getNode("CHANNEL%d:BITOFF"%ch).record = m.Int32(offsets[0][ch])
            dev.TRIGGER.PRE.record  = int(len*.1)
            dev.TRIGGER.POST.record = int(len*.9)
        if extclk:
            dev.CLOCK.IN.record     = m.Int32(1e7)
            dev.CLOCK.MUX.record    = dev.CLOCK.MUX.SI5326_RJ45
            dev.CLOCK.DIVIDER.record= m.Int8(25)
            dev.CLOCK.SI5326.record = m.Int32Array([5,4,10,250,5])
    t.readonly()
    t.createPulse(1)
    """ shot """
    t = m.Tree('test',1)
    dev = t.getNode('SIS8300_%d'%devid)
    dev.init()
    print('init done')
    dev.arm()    # ready for trigger/start stream (thread)
    print('arm done')
    time.sleep(1)
    device = dev.device
    if exttrg:
        while device.acquisition.is_armed:
            time.sleep(.1)
        print('triggered')
    else:
        dev.soft_trigger()
        print('soft_trigger done')
    dev.store()  # read data/wait for stream results, store devices param, e.g. callibration
    print('store done')
    dev.deinit() # disarm
    dev.device.close() # disarm
    return t
except:
 print("MDSplus not found.")
 def acq_test(*a,**kw): pass

""" TESTS """

def led_test():
    dev = sis8300ku('/dev/sis8300-0')
    dev.user.led_test = True
    time.sleep(5)
    dev.user.led_test = False

if __name__=='__main__':
    import sys
    if len(sys.argv)>1:
        acq_test(int(sys.argv[1]))
    else:
        acq_test()
