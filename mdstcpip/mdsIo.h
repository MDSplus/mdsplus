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

#ifndef DOXYGEN // hide this part from documentation

#define MDS_IO_OPEN_K   1
#define MDS_IO_CLOSE_K  2
#define MDS_IO_LSEEK_K  3
#define MDS_IO_READ_K   4
#define MDS_IO_WRITE_K  5
#define MDS_IO_LOCK_K   6
#define MDS_IO_EXISTS_K 7
#define MDS_IO_REMOVE_K 8
#define MDS_IO_RENAME_K 9
#define MDS_IO_READ_X_K 10
#define MDS_IO_PARALLEL_K 11

#define MDS_IO_O_CREAT  0x00000040
#define MDS_IO_O_TRUNC  0x00000200
#define MDS_IO_O_EXCL   0x00000080
#define MDS_IO_O_WRONLY 0x00000001
#define MDS_IO_O_RDONLY 0x00004000
#define MDS_IO_O_RDWR   0x00000002

#define MDS_IO_LOCK_RD  0x01
#define MDS_IO_LOCK_WRT 0x02
#define MDS_IO_LOCK_NONE 0x00
#define MDS_IO_LOCK_NOWAIT 0x08
#define MDS_IO_LOCK_MASK 0x03

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_RANDOM
#define O_RANDOM 0
#endif

#endif // DOXYGEN end of hidden code
