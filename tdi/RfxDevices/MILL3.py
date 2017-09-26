#!/usr/bin/env python
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
from MDSplus import mdsExceptions, Device, Data, version
#from pymodbus.client.sync import ModbusTcpClient
#from pymodbus.exceptions import ModbusException
import struct
import time

class MILL3(Device):
    parts=[
        {'path':':COMMENT', 'type':'text'},
        {'path':':IP', 'type':'text'},
        {'path':':MOXA_COM1', 'type':'text'},
        {'path':':MOXA_COM2', 'type':'text'},
        {'path':':MOXA_COM3', 'type':'text'},
        {'path':':MOXA_COM4', 'type':'text'},
        {'path':':SHORT_TRN_SP', 'type':'numeric'},
        {'path':':LONG_TRN_SP', 'type':'numeric'},
        {'path':':ROTATION_SP', 'type':'numeric'},
        {'path':':SHORT_TRN', 'type':'numeric'},
        {'path':':LONG_TRN', 'type':'numeric'},
        {'path':':ROTATION', 'type':'numeric'},
        {'path':':PRESSURE', 'type':'numeric'}]

    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('A_SERVER','INIT',50,None),Method(None,'INIT',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('A_SERVER','STORE',50,None),Method(None,'STORE',head))",
        'options':('no_write_shot',)})

    ser=None
    c=None


    def sendCmd(self, cmd):
        from serial import SerialReadTimeoutException, SerialBadMessageException
        self.ser.write(cmd)
        time.sleep(0.2)

        remaining = 14

        msg = bytearray()

        while remaining > 0:
            chunk = self.ser.read(remaining)
            if 0 == len(chunk): raise SerialReadTimeoutException
            msg.extend(chunk)
            remaining -= len(chunk)

        if ":" != chr(msg[6]): raise SerialBadMessageException

    #    print(cmd[2:6]==msg[2:6])
        data = struct.unpack_from('>i', version.buffer(msg), 7)

        return data[0]



    def STORE(self):
        import serial
        import modbus_tk
        import modbus_tk.defines as cst
        import modbus_tk.modbus_tcp as modbus_tcp

        class SerialReadTimeoutException(serial.SerialException):
            """Read tmeout give an exception"""

        class SerialBadMessageException(serial.SerialException):
            """Received bad message give an exception"""

        LONG_TRANSLATION_SET_POINT_MAX_VALUE = 1540
        SHORT_TRANSLATION_SET_POINT_MAX_VALUE = 200

        bad = 1

        print('START STORE')

        try:
            ip = str(self.ip.data())
            ip = ip.strip()
            print(' IP: ' + ip)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            longTranslationSetPoint = self.long_trn_sp.data()
            print(' LONG_TRN_SP: ' + str(longTranslationSetPoint))
            if longTranslationSetPoint < 0 or longTranslationSetPoint > LONG_TRANSLATION_SET_POINT_MAX_VALUE:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid LONG_TRN_SP')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid LONG_TRN_SP')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            shortTranslationSetPoint = self.short_trn_sp.data()
            print(' SHORT_TRN_SP: ' + str(shortTranslationSetPoint))
            if shortTranslationSetPoint < 0 or shortTranslationSetPoint > SHORT_TRANSLATION_SET_POINT_MAX_VALUE:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SHORT_TRN_SP')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SHORT_TRN_SP')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom1 = str(self.moxa_com1.data())
            moxaCom1 = moxaCom1.strip()
            print(' MOXA_COM1: ' + moxaCom1)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM1')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom2 = str(self.moxa_com2.data())
            moxaCom2 = moxaCom2.strip()
            print(' MOXA_COM2: ' + moxaCom2)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM2')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom3 = str(self.moxa_com3.data())
            moxaCom3 = moxaCom3.strip()
            print(' MOXA_COM3: ' + moxaCom3)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM3')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom4 = str(self.moxa_com4.data())
            moxaCom4 = moxaCom4.strip()
            print(' MOXA_COM4: ' + moxaCom4)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM4')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        """
        try:
            master = modbus_tcp.TcpMaster(host=ip)
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=1)
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=2)

        except modbus_tk.modbus.ModbusError, e:
            print("Modbus error ", e.get_exception_code()
        except Exception, e2:
            print("Error ", str(e2)
        """


        print('OK')

        try:
            port=moxaCom1

            self.ser=None
            serTimeout=5


            master = modbus_tcp.TcpMaster(host=ip)
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=1)

            #c=ModbusTcpClient(ip)
            #time.sleep(1.0)
            #conn=c.connect()
            time.sleep(1.0)
            #if not conn: raise ModbusException("cannot connect to " + ip)

            #c.write_register(12,1)
            #time.sleep(0.5)

            self.ser = serial.Serial(
            port = port,
            baudrate = 9600,
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            bytesize = serial.EIGHTBITS,
            xonxoff = False,
            rtscts = False,
            dsrdtr = False,
            timeout = serTimeout
            )


            position = self.sendCmd("\x7C\x00\x54\x50\x4F\x53\x00\x00\x00\x00\x00\x01\xC2\x04")
            decimals = self.sendCmd("\x7C\x00\x54\x44\x45\x43\x00\x00\x00\x00\x00\x01\x9C\x04")
            preset = self.sendCmd("\x7C\x00\x54\x52\x45\x46\x00\x00\x00\x00\x00\x01\xAD\x04")
            offset = self.sendCmd("\x7C\x00\x54\x45\x49\x4E\x00\x00\x00\x00\x00\x01\xAC\x04")
            print("raw position: " + str(position))
            print("decimals: " + str(decimals))
            print("preset: " + str(preset))
            print("offset: " + str(offset))
            longPosition=position / (10.0 ** decimals)
            print("long position: " + str(longPosition))
            setattr(self,'long_trn',longPosition)

            port=moxaCom2
            self.ser=None
            serTimeout=5

            self.ser = serial.Serial(
            port = port,
            baudrate = 9600,
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            bytesize = serial.EIGHTBITS,
            xonxoff = False,
            rtscts = False,
            dsrdtr = False,
            timeout = serTimeout
            )

            position = self.sendCmd("\x7C\x00\x54\x50\x4F\x53\x00\x00\x00\x00\x00\x01\xC2\x04")
            decimals = self.sendCmd("\x7C\x00\x54\x44\x45\x43\x00\x00\x00\x00\x00\x01\x9C\x04")
            preset = self.sendCmd("\x7C\x00\x54\x52\x45\x46\x00\x00\x00\x00\x00\x01\xAD\x04")
            offset = self.sendCmd("\x7C\x00\x54\x45\x49\x4E\x00\x00\x00\x00\x00\x01\xAC\x04")
            print("raw position: " + str(position))
            print("decimals: " + str(decimals))
            print("preset: " + str(preset))
            print("offset: " + str(offset))
            shortPosition=position / (10.0 ** decimals)
            print("short position: " + str(shortPosition))
            setattr(self,'short_trn',shortPosition)



            port=moxaCom3
            self.ser=None
            serTimeout=5

            self.ser = serial.Serial(
            port = port,
            baudrate = 9600,
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            bytesize = serial.EIGHTBITS,
            xonxoff = False,
            rtscts = False,
            dsrdtr = False,
            timeout = serTimeout
            )

            position = self.sendCmd("\x7C\x00\x54\x50\x4F\x53\x00\x00\x00\x00\x00\x01\xC2\x04")
            decimals = self.sendCmd("\x7C\x00\x54\x44\x45\x43\x00\x00\x00\x00\x00\x01\x9C\x04")
            preset = self.sendCmd("\x7C\x00\x54\x52\x45\x46\x00\x00\x00\x00\x00\x01\xAD\x04")
            offset = self.sendCmd("\x7C\x00\x54\x45\x49\x4E\x00\x00\x00\x00\x00\x01\xAC\x04")
            print("raw position: " + str(position))
            print("decimals: " + str(decimals))
            print("preset: " + str(preset))
            print("offset: " + str(offset))
            rotation=position / (10.0 ** decimals)
            print("rotation: " + str(rotation))
            setattr(self,'rotation',rotation)


            """
            port=moxaCom4
            self.ser=None
            serTimeout=5

            self.ser = serial.Serial(
            port = port,
            baudrate = 9600,
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            bytesize = serial.EIGHTBITS,
            xonxoff = False,
            rtscts = False,
            dsrdtr = False,
            timeout = 10
            )

            self.ser.write("\x50\x52\x31\x0D")
            time.sleep(0.2)
            ack = self.ser.read(3)
            print(ack.encode("hex"))
            self.ser.write("\x05")
            time.sleep(0.2)

            data = self.ser.readline()
            print(data.encode("hex"))
            if data[0] != '\x30': raise SerialBadMessageException
            data=data[2:13]
            data=data.strip()
            print("pressure: " + data)
            setattr(self,'pressure',float(data))
            """
        except modbus_tk.modbus.ModbusError as e:
            print("Modbus error ", e.get_exception_code())
            bad = 0
        except Exception as e2:
            print("Error ", str(e2))
            bad = 0
#        except ModbusException as e:
#            print(e
#            bad = 0
        except serial.SerialTimeoutException:
            print("Timeout on write operation")
            bad = 0
        except serial.SerialReadTimeoutException:
            print("Timeout on read operation")
            bad = 0
        except serial.SerialBadMessageException:
            print("Bad message")
            bad = 0
        except serial.SerialException:
            print("Could not open port " + port)
            bad = 0
        finally:
            if self.ser is not None: self.ser.close()
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=2)
            #if conn: c.write_register(12,2)
            #time.sleep(1.0)
            #self.c.close()
            #if self.c is not None: self.c.close()
            print("status: " + str(bad))
        return bad


    def INIT(self):
        import serial
        import modbus_tk
        import modbus_tk.defines as cst
        import modbus_tk.modbus_tcp as modbus_tcp

        class SerialReadTimeoutException(serial.SerialException):
            """Read tmeout give an exception"""

        class SerialBadMessageException(serial.SerialException):
            """Received bad message give an exception"""

        LONG_TRANSLATION_SET_POINT_MAX_VALUE = 1540
        SHORT_TRANSLATION_SET_POINT_MAX_VALUE = 200

        bad = 1

        print('START INIT')

        try:
            ip = str(self.ip.data())
            ip = ip.strip()
            print(' IP: ' + ip)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            longTranslationSetPoint = self.long_trn_sp.data()
            print(' LONG_TRN_SP: ' + str(longTranslationSetPoint))
            if longTranslationSetPoint < 0 or longTranslationSetPoint > LONG_TRANSLATION_SET_POINT_MAX_VALUE:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid LONG_TRN_SP')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid LONG_TRN_SP')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            shortTranslationSetPoint = self.short_trn_sp.data()
            print(' SHORT_TRN_SP: ' + str(shortTranslationSetPoint))
            if shortTranslationSetPoint < 0 or shortTranslationSetPoint > SHORT_TRANSLATION_SET_POINT_MAX_VALUE:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SHORT_TRN_SP')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SHORT_TRN_SP')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom1 = str(self.moxa_com1.data())
            moxaCom1 = moxaCom1.strip()
            print(' MOXA_COM1: ' + moxaCom1)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM1')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom2 = str(self.moxa_com2.data())
            moxaCom2 = moxaCom2.strip()
            print(' MOXA_COM2: ' + moxaCom2)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM2')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom3 = str(self.moxa_com3.data())
            moxaCom3 = moxaCom3.strip()
            print(' MOXA_COM3: ' + moxaCom3)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM3')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            moxaCom4 = str(self.moxa_com4.data())
            moxaCom4 = moxaCom4.strip()
            print(' MOXA_COM4: ' + moxaCom4)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid MOXA_COM4')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        """
        try:
            master = modbus_tcp.TcpMaster(host=ip)
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=1)
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=2)

        except modbus_tk.modbus.ModbusError, e:
            print("Modbus error ", e.get_exception_code()
        except Exception, e2:
            print("Error ", str(e2)
        """

        print('OK')

        try:
            port=moxaCom1

            self.ser=None


            master = modbus_tcp.TcpMaster(host=ip)
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=1)

            #c=ModbusTcpClient(ip)
            #time.sleep(1.0)
            #conn=c.connect()
            time.sleep(1.0)
            #if not conn: raise ModbusException("cannot connect to " + ip)

            #c.write_register(12,1)
            #time.sleep(0.5)

            port=moxaCom4
            self.ser=None

            self.ser = serial.Serial(
            port = port,
            baudrate = 9600,
            parity = serial.PARITY_NONE,
            stopbits = serial.STOPBITS_ONE,
            bytesize = serial.EIGHTBITS,
            xonxoff = False,
            rtscts = False,
            dsrdtr = False,
            timeout = 10
            )

            self.ser.write("\x50\x52\x31\x0D")
            time.sleep(0.2)
            ack = self.ser.read(3)
            print(ack.encode("hex"))
            self.ser.write("\x05")
            time.sleep(0.2)

            data = self.ser.readline()
            print(data.encode("hex"))
            if data[0] != '\x30': raise SerialBadMessageException
            data=data[2:13]
            data=data.strip()
            print("pressure: " + data)
            setattr(self,'pressure',float(data))
        except modbus_tk.modbus.ModbusError as e:
            print("Modbus error ", e.get_exception_code())
            bad = 0
        except Exception as e2:
            print("Error ", str(e2))
            bad = 0
#        except ModbusException as e:
#            print(e
#            bad = 0
        except serial.SerialTimeoutException:
            print("Timeout on write operation")
            bad = 0
        except serial.SerialReadTimeoutException:
            print("Timeout on read operation")
            bad = 0
        except serial.SerialBadMessageException:
            print("Bad message")
            bad = 0
        except serial.SerialException:
            print("Could not open port " + port)
            bad = 0
        finally:
            if self.ser is not None: self.ser.close()
            master.execute(1, cst.WRITE_SINGLE_REGISTER, 12, output_value=2)    # da levare con l'inserzione
            #if conn: c.write_register(12,2)
            #time.sleep(1.0)
            #self.c.close()
            #if self.c is not None: self.c.close()
            print("status: " + str(bad))
        return bad
