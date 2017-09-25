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
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import jtraverser.jTraverserFacade;

public class jTraverser{
    static Pattern pattern = Pattern.compile("(.*)[\\\\/]([^\\\\/]+)_((model|[0-9]+))\\.tree");

    @SuppressWarnings("unused")
    public static void main(final String args[]) {
        if(args.length >= 4) new jTraverserFacade(args[0], args[1], args[2], args[3], null);
        else if(args.length == 3) new jTraverserFacade(args[0], args[1], args[2], null, null);
        else if(args.length == 2) new jTraverserFacade(args[0], args[1], null, null, null);
        else if(args.length == 1){
            final Matcher matcher = jTraverser.pattern.matcher(args[0]);
            if(matcher.find()) new jTraverserFacade(null, matcher.group(2), matcher.group(3), null, matcher.group(1));
            else new jTraverserFacade(args[0], null, null, null, null);
        }else new jTraverserFacade(null, null, null, null, null);
    }
}
