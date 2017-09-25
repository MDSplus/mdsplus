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
import MdsPlus.*;
  import java.awt.*;
  public class Class1 {
          public static void main(String[] args) {
          try {
                  float xValue[];
                  int iValue[];
                  MdsPlus mds = new MdsPlus("cmoda", 8000);
                  iValue = new int[0];
                  xValue = new float[0];
                  xValue = mds.Value("[1.0D0, 2.0D0]").Float();
                  System.out.println("The values are "+xValue[0]+" "+xValue[1]);
                  iValue = mds.Value("1+2.5").Int();
                  System.out.println("The value is "+iValue[0]);
                  System.out.println("Here is a string "+mds.Value("'Now is the time'").String());
                  mds.OpenTree("CMOD,XX", 0); /* use a list of shots to prevent opening of whole cmod tree */
                  System.out.println("The time of the last shot was "+mds.Value("TIME_OF_SHOT").String());
                  mds.disconnect(mds);
          } catch (Exception e) {
              System.out.println("The exception was...\n"+e);
          }
          }
  }