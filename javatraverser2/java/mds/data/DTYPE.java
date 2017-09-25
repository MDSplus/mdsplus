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
package mds.data;

public final class DTYPE{
    private static final String[] _name          = new String[256];
    private static final String[] _suffix        = new String[256];
    public static final byte      MISSING        = 0;
    public static final byte      BU             = 2;
    public static final byte      WU             = 3;
    public static final byte      LU             = 4;
    public static final byte      QU             = 5;
    public static final byte      B              = 6;
    public static final byte      W              = 7;
    public static final byte      L              = 8;
    public static final byte      Q              = 9;
    public static final byte      F              = 10;             // VMS
    public static final byte      D              = 11;             // VMS
    public static final byte      FC             = 12;             // VMS
    public static final byte      DC             = 13;             // VMS
    public static final byte      T              = 14;
    public static final byte      DSC            = 24;
    public static final byte      OU             = 25;
    public static final byte      O              = 26;
    public static final byte      G              = 27;
    public static final byte      H              = 28;
    public static final byte      GC             = 29;             // IEEE
    public static final byte      HC             = 30;             // 128bit
    public static final byte      POINTER        = 51;
    public static final byte      FS             = 52;             // IEEE
    public static final byte      FT             = 53;             // IEEE
    public static final byte      FSC            = 54;             // IEEE
    public static final byte      FTC            = 55;             // IEEE
    public static final byte      IDENT          = -65;            // 191
    public static final byte      NID            = -64;            // 192
    public static final byte      PATH           = -63;            // 193
    public static final byte      PARAM          = -62;            // 194
    public static final byte      SIGNAL         = -61;            // 195
    public static final byte      DIMENSION      = -60;            // 196
    public static final byte      WINDOW         = -59;            // 197
    public static final byte      SLOPE          = -58;            // 198
    public static final byte      FUNCTION       = -57;            // 199
    public static final byte      CONGLOM        = -56;            // 200
    public static final byte      RANGE          = -55;            // 201
    public static final byte      ACTION         = -54;            // 202
    public static final byte      DISPATCH       = -53;            // 203
    public static final byte      PROGRAM        = -52;            // 204
    public static final byte      ROUTINE        = -51;            // 205
    public static final byte      PROCEDURE      = -50;            // 206
    public static final byte      METHOD         = -49;            // 207
    public static final byte      DEPENDENCY     = -48;            // 208
    public static final byte      CONDITION      = -47;            // 209
    public static final byte      EVENT          = -46;            // 210
    public static final byte      WITH_UNITS     = -45;            // 211
    public static final byte      CALL           = -44;            // 212
    public static final byte      WITH_ERROR     = -43;            // 213
    public static final byte      LIST           = -42;            // 214
    public static final byte      TUPLE          = -41;            // 215
    public static final byte      DICTIONARY     = -40;            // 216
    public static final byte      OPAQUE         = -39;            // 217
    public static final byte      FLOAT          = DTYPE.FS;
    public static final byte      DOUBLE         = DTYPE.FT;
    public static final byte      COMPLEX_FLOAT  = DTYPE.FSC;
    public static final byte      COMPLEX_DOUBLE = DTYPE.FTC;
    static{
        DTYPE._name[DTYPE.B & 0xFF] = "Byte";
        DTYPE._name[DTYPE.W & 0xFF] = "Word";
        DTYPE._name[DTYPE.L & 0xFF] = "Long";
        DTYPE._name[DTYPE.Q & 0xFF] = "Quadword";
        DTYPE._name[DTYPE.O & 0xFF] = "Octaword";
        DTYPE._name[DTYPE.BU & 0xFF] = "Byte_Unsigned";
        DTYPE._name[DTYPE.WU & 0xFF] = "Word_Unsigned";
        DTYPE._name[DTYPE.LU & 0xFF] = "Long_Unsigned";
        DTYPE._name[DTYPE.QU & 0xFF] = "Quadword_Unsigned";
        DTYPE._name[DTYPE.OU & 0xFF] = "Octaword_Unsigned";
        DTYPE._name[DTYPE.D & 0xFF] = "D_Float";
        DTYPE._name[DTYPE.DC & 0xFF] = "D_Complex";
        DTYPE._name[DTYPE.F & 0xFF] = "F_Float";
        DTYPE._name[DTYPE.FC & 0xFF] = "F_Complex";
        DTYPE._name[DTYPE.FS & 0xFF] = "FS_Float";
        DTYPE._name[DTYPE.FSC & 0xFF] = "FSC_Complex";
        DTYPE._name[DTYPE.FT & 0xFF] = "FT_Float";
        DTYPE._name[DTYPE.FTC & 0xFF] = "FT_Complex";
        DTYPE._name[DTYPE.G & 0xFF] = "G_Float";
        DTYPE._name[DTYPE.GC & 0xFF] = "G_Complex";
        DTYPE._name[DTYPE.H & 0xFF] = "H_Float";
        DTYPE._name[DTYPE.HC & 0xFF] = "H_Complex";
        DTYPE._name[DTYPE.T & 0xFF] = "Text";
        DTYPE._name[DTYPE.IDENT & 0xFF] = "Ident";
        DTYPE._name[DTYPE.POINTER & 0xFF] = "Pointer";
        DTYPE._name[DTYPE.EVENT & 0xFF] = "Event";
        DTYPE._name[DTYPE.NID & 0xFF] = "Nid";
        DTYPE._name[DTYPE.PATH & 0xFF] = "Path";
        DTYPE._name[DTYPE.DSC & 0xFF] = "Dsc";
        DTYPE._name[DTYPE.DICTIONARY & 0xFF] = "Dictionary";
        DTYPE._name[DTYPE.TUPLE & 0xFF] = "Tuple";
        DTYPE._name[DTYPE.LIST & 0xFF] = "List";
        DTYPE._name[DTYPE.ACTION & 0xFF] = "Action";
        DTYPE._name[DTYPE.CALL & 0xFF] = "Call";
        DTYPE._name[DTYPE.CONDITION & 0xFF] = "Condition";
        DTYPE._name[DTYPE.CONGLOM & 0xFF] = "Conglom";
        DTYPE._name[DTYPE.DEPENDENCY & 0xFF] = "Dependency";
        DTYPE._name[DTYPE.DIMENSION & 0xFF] = "Dim";
        DTYPE._name[DTYPE.DISPATCH & 0xFF] = "Dispatch";
        DTYPE._name[DTYPE.FUNCTION & 0xFF] = "Function";
        DTYPE._name[DTYPE.METHOD & 0xFF] = "Method";
        DTYPE._name[DTYPE.MISSING & 0xFF] = "$Missing";
        DTYPE._name[DTYPE.OPAQUE & 0xFF] = "Opaque";
        DTYPE._name[DTYPE.PARAM & 0xFF] = "Param";
        DTYPE._name[DTYPE.PROCEDURE & 0xFF] = "Procedure";
        DTYPE._name[DTYPE.PROGRAM & 0xFF] = "Program";
        DTYPE._name[DTYPE.RANGE & 0xFF] = "Range";
        DTYPE._name[DTYPE.ROUTINE & 0xFF] = "Routine";
        DTYPE._name[DTYPE.SIGNAL & 0xFF] = "Signal";
        DTYPE._name[DTYPE.SLOPE & 0xFF] = "Slope";
        DTYPE._name[DTYPE.WINDOW & 0xFF] = "Window";
        DTYPE._name[DTYPE.WITH_ERROR & 0xFF] = "With_Error";
        DTYPE._name[DTYPE.WITH_UNITS & 0xFF] = "With_Units";
    }
    static{
        DTYPE._suffix[DTYPE.B & 0xFF] = "B";
        DTYPE._suffix[DTYPE.W & 0xFF] = "W";
        DTYPE._suffix[DTYPE.L & 0xFF] = "L";
        DTYPE._suffix[DTYPE.Q & 0xFF] = "Q";
        DTYPE._suffix[DTYPE.O & 0xFF] = "O";
        DTYPE._suffix[DTYPE.BU & 0xFF] = "BU";
        DTYPE._suffix[DTYPE.WU & 0xFF] = "WU";
        DTYPE._suffix[DTYPE.LU & 0xFF] = "LU";
        DTYPE._suffix[DTYPE.QU & 0xFF] = "QU";
        DTYPE._suffix[DTYPE.OU & 0xFF] = "OU";
        DTYPE._suffix[DTYPE.D & 0xFF] = "V";
        DTYPE._suffix[DTYPE.DC & 0xFF] = "V";
        DTYPE._suffix[DTYPE.F & 0xFF] = "F";
        DTYPE._suffix[DTYPE.FC & 0xFF] = "F";
        DTYPE._suffix[DTYPE.FS & 0xFF] = "E";
        DTYPE._suffix[DTYPE.FSC & 0xFF] = "E";
        DTYPE._suffix[DTYPE.FT & 0xFF] = "D";
        DTYPE._suffix[DTYPE.FTC & 0xFF] = "D";
        DTYPE._suffix[DTYPE.G & 0xFF] = "G";
        DTYPE._suffix[DTYPE.GC & 0xFF] = "G";
        DTYPE._suffix[DTYPE.H & 0xFF] = "H";
        DTYPE._suffix[DTYPE.HC & 0xFF] = "H";
    }

    public static final String getName(final byte dtype) {
        final int idx = dtype & 0xFF;
        if(idx < DTYPE._name.length) return DTYPE._name[idx];
        return new StringBuffer(9).append("DTYPE_").append(idx).toString();
    }

    public static final String getSuffix(final byte dtype) {
        return DTYPE._suffix[dtype & 0xFF];
    }
}
