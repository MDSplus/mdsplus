
Module mdsipshr

    <DllImport("MDSIPSHR.DLL", EntryPoint:="ConnectToMds", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     MdsConnect(<MarshalAs(UnmanagedType.LPStr)> ByVal host As String) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="DisconnectFromMds", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     MdsDisconnect(ByVal socket As Int32) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="MdsOpen", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     MdsOpen(ByVal socket As Int32, <MarshalAs(UnmanagedType.LPStr)> ByVal tree As String, ByVal shot As Int32) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.I1)> ByRef value As Byte) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.I2)> ByRef value As Int16) As Int32
    End Function
    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.I2)> ByRef value As UInt16) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.I4)> ByRef value As Int32) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.I8)> ByRef value As Int64) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.R4)> ByRef value As Single) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.R8)> ByRef value As Double) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, <MarshalAs(UnmanagedType.LPStr)> ByVal value As String) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="SendArg", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     SendArg(ByVal mdsSocket As Int32, ByVal idx As Byte, ByVal dtype As Byte, ByVal nargs As Byte, ByVal nbytes As Int16, _
     ByVal ndims As Byte, ByVal dims() As Int32, ByVal value As IntPtr) As Int32
    End Function

    <DllImport("MDSIPSHR.DLL", EntryPoint:="GetAnswerInfo", SetLastError:=True, _
     CharSet:=CharSet.Unicode, ExactSpelling:=True, _
     CallingConvention:=CallingConvention.Cdecl)> _
     Public Function _
     GetAnsInfo(ByVal mdsSocket As Int32, ByRef dtype As Byte, ByRef len As Int32, ByRef ndims As Int32, ByVal dims() As Int32, _
     ByRef numbytes As Int32, ByRef ptr As IntPtr) As Int32
    End Function
End Module

Public Class MDSplus
    Private socket As Int32 = -1


    Public Sub New()
        socket = -1
    End Sub

    Public Sub New(ByVal Container As System.ComponentModel.IContainer)
        socket = -1
    End Sub

    Public Sub New(ByVal host As String)
        Connect(host)
    End Sub

    Public Sub Connect(ByVal host As String)
        socket = MdsConnect(host)
        If (socket = -1) Then
            Throw New System.Exception("Error connecting to host: " + host)
        End If
    End Sub

    Public Sub DisConnect()
        Dim status As Int32 = MdsDisconnect(socket)
    End Sub

    Protected Overrides Sub Finalize()
        DisConnect()
        MyBase.Finalize()
    End Sub

    Public Sub MdsOpen(ByVal tree As String, ByVal shot As Int32)
        Dim status As Int32 = mdsipshr.MdsOpen(socket, tree, shot)
        If ((status And 1) = 0) Then
            Throw New System.Exception("Error opening tree: '" + tree + "' shot: " + shot.ToString + " status: " + status.ToString)
        End If

    End Sub

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Byte) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 6
        Dim nbytes As Short = 1
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int16) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 7
        Dim nbytes As Short = 2
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function
    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As UInt16) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 7
        Dim nbytes As Short = 2
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int32) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 8
        Dim nbytes As Short = 4
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int64) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 9
        Dim nbytes As Short = 8
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Single) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 10
        Dim nbytes As Short = 4
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Double) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 11
        Dim nbytes As Short = 8
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As String) As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 14
        Dim nbytes As Short = value.Length
        Dim ndims As Byte = 0
        Return mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, value)
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Byte()) As Int32
        Dim dims(7) As Int32
        Dim status As Int32
        Dim dtype As Byte = 6
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length)
        Dim nbytes As Short = 1
        Dim ndims As Byte = 1
        dims(0) = value.Length
        Marshal.Copy(value, 0, ptr, value.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int16()) As Int32
        Dim status As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 7
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length * 2)
        Dim nbytes As Short = 2
        Dim ndims As Byte = 1
        dims(0) = value.Length
        Marshal.Copy(value, 0, ptr, value.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function
    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int32()) As Int32
        Dim status As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 8
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length * 4)
        Dim nbytes As Short = 4
        Dim ndims As Byte = 1
        dims(0) = value.Length
        Marshal.Copy(value, 0, ptr, value.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function
    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int32(,,)) As Int32
        Dim status As Int32
        Dim val As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 8
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length * 4)
        Dim nbytes As Short = 4
        Dim ndims As Byte = 3
        Dim i As Integer
        dims(0) = value.GetUpperBound(0) + 1
        dims(1) = value.GetUpperBound(1) + 1
        dims(2) = value.GetUpperBound(2) + 1
        Dim flattened(value.Length) As Int32
        For Each val In value
            flattened(i) = val
            i = i + 1
        Next
        Marshal.Copy(flattened, 0, ptr, flattened.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int32(,)) As Int32
        Dim status As Int32
        Dim val As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 8
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length * 4)
        Dim nbytes As Short = 4
        Dim ndims As Byte = 2
        Dim i As Integer
        dims(0) = value.GetUpperBound(0) + 1
        dims(1) = value.GetUpperBound(1) + 1
        Dim flattened(value.Length) As Int32
        For Each val In value
            flattened(i) = val
            i = i + 1
        Next
        Marshal.Copy(flattened, 0, ptr, flattened.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Int64()) As Int32
        Dim status As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 9
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length * 8)
        Dim nbytes As Short = 8
        Dim ndims As Byte = 1
        dims(0) = value.Length
        Marshal.Copy(value, 0, ptr, value.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Single()) As Int32
        Dim status As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 10
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length * 4)
        Dim nbytes As Short = 4
        Dim ndims As Byte = 1
        dims(0) = value.Length
        Marshal.Copy(value, 0, ptr, value.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Double()) As Int32
        Dim status As Int32
        Dim dims(7) As Int32
        Dim dtype As Byte = 11
        Dim ptr As IntPtr = Marshal.AllocHGlobal(value.Length * 8)
        Dim nbytes As Short = 8
        Dim ndims As Byte = 1
        dims(0) = value.Length
        Marshal.Copy(value, 0, ptr, value.Length)
        status = mdsipshr.SendArg(socket, idx, dtype, numargs, nbytes, ndims, dims, ptr)
        Marshal.FreeHGlobal(ptr)
        Return status
    End Function

    Private Function SendArg(ByVal idx As Byte, ByVal numargs As Byte, ByVal value As Object) As Int32
        Dim status As Int32
        Select Case TypeName(value)
            Case "Byte"
                Dim val As Byte = value
                status = SendArg(idx, numargs, val)
            Case "Short"
                Dim val As Int16 = value
                status = SendArg(idx, numargs, val)
            Case "UShort"
                Dim val As Int16 = value
                status = SendArg(idx, numargs, val)
            Case "Integer"
                Dim val As Int32 = value
                status = SendArg(idx, numargs, val)
            Case "Long"
                Dim val As Int64 = value
                status = SendArg(idx, numargs, val)
            Case "Single"
                Dim val As Single = value
                status = SendArg(idx, numargs, val)
            Case "Double"
                Dim val As Double = value
                status = SendArg(idx, numargs, val)
            Case "String"
                Dim val As String = value
                status = SendArg(idx, numargs, val)
            Case "Byte()"
                Dim val As Byte() = value
                status = SendArg(idx, numargs, val)
            Case "Short()"
                Dim val As Int16() = value
                status = SendArg(idx, numargs, val)
            Case "Integer()"
                Dim val As Int32() = value
                status = SendArg(idx, numargs, val)
            Case "Integer(,)"
                Dim val(,) As Int32 = value
                status = SendArg(idx, numargs, val)
            Case "Integer(,,)"
                Dim val(,,) As Int32 = value
                status = SendArg(idx, numargs, val)
            Case "Long()"
                Dim val As Int64() = value
                status = SendArg(idx, numargs, val)
            Case "Single()"
                Dim val As Single() = value
                status = SendArg(idx, numargs, val)
            Case "Double()"
                Dim val As Double() = value
                status = SendArg(idx, numargs, val)
            Case Else
                Throw New System.Exception("Unsupported argument datatype: " + TypeName(value))
                Return 0
        End Select
        If ((status And 1) = 0) Then
            Throw New System.Exception("SendArg returned bad status: " + status.ToString)
        End If
        Return status
    End Function

    Private Function GetAnswer(ByRef status As Int32) As Object
        Dim dtype As Byte
        Dim len As Int32
        Dim ndims As Int32
        Dim dims(8) As Int32
        Dim numbytes As Int32
        Dim ptr As IntPtr
        status = mdsipshr.GetAnsInfo(socket, dtype, len, ndims, dims, numbytes, ptr)
        If (((status And 1) = 0) Or (dtype = 0)) Then
            Throw New System.Exception(Marshal.PtrToStringAnsi(ptr))
        End If
        Select Case dtype
            Case 2
                If (ndims = 0) Then
                    Return Marshal.ReadByte(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Byte
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 3
                If (ndims = 0) Then
                    Return Marshal.ReadInt16(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Int16
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 4
                If (ndims = 0) Then
                    Return Marshal.ReadInt32(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Int32
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 5
                If (ndims = 0) Then
                    Return Marshal.ReadInt64(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Int64
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 6
                If (ndims = 0) Then
                    Return Marshal.ReadByte(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Byte
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 7
                If (ndims = 0) Then
                    Return Marshal.ReadInt16(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Int16
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 8
                If (ndims = 0) Then
                    Return Marshal.ReadInt32(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Int32
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 9
                If (ndims = 0) Then
                    Return Marshal.ReadInt64(ptr, 0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Int64
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 10
                If (ndims = 0) Then
                    Dim ans(1) As Single
                    Marshal.Copy(ptr, ans, 0, 1)
                    Return ans(0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Single
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 11
                If (ndims = 0) Then
                    Dim ans(1) As Double
                    Marshal.Copy(ptr, ans, 0, 1)
                    Return ans(0)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As Double
                    Marshal.Copy(ptr, ans, 0, dims(0))
                    Return ans
                End If
            Case 14
                If (ndims = 0) Then
                    Return Marshal.PtrToStringAnsi(ptr, numbytes)
                ElseIf (ndims = 1) Then
                    Dim ans(dims(0) - 1) As String
                    Dim i As Int32
                    Dim slen As Int32 = numbytes / dims(0)
                    For i = 0 To dims(0) - 1 Step 1
                        ans(i) = Marshal.PtrToStringAnsi(New IntPtr(ptr.ToInt64 + i * slen), slen).Trim
                    Next
                    Return ans
                End If
            Case Else
                Throw New System.Exception("MDSplus returned unsupported datatype, dtype = " + dtype.ToString)
        End Select
        Return status
    End Function
    Public Function MdsValue(ByVal expression As String, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 1
        SendArg(idx, numargs, expression)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByRef status As Int32) As Object
        Dim istatus As Int32
        Dim idx As Byte = 0
        Dim numargs As Byte = 2
        istatus = SendArg(idx, numargs, expression)
        idx = idx + 1
        istatus = SendArg(idx, numargs, arg1)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 3
        SendArg(idx, numargs, expression)
        idx = idx + 1
        SendArg(idx, numargs, arg1)
        idx = idx + 1
        SendArg(idx, numargs, arg2)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 4
        SendArg(idx, numargs, expression)
        idx = idx + 1
        SendArg(idx, numargs, arg1)
        idx = idx + 1
        SendArg(idx, numargs, arg2)
        idx = idx + 1
        SendArg(idx, numargs, arg3)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 5
        SendArg(idx, numargs, expression)
        idx = idx + 1
        SendArg(idx, numargs, arg1)
        idx = idx + 1
        SendArg(idx, numargs, arg2)
        idx = idx + 1
        SendArg(idx, numargs, arg3)
        idx = idx + 1
        SendArg(idx, numargs, arg4)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByVal arg5 As Object, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 6
        SendArg(idx, numargs, expression)
        idx = idx + 1
        SendArg(idx, numargs, arg1)
        idx = idx + 1
        SendArg(idx, numargs, arg2)
        idx = idx + 1
        SendArg(idx, numargs, arg3)
        idx = idx + 1
        SendArg(idx, numargs, arg4)
        idx = idx + 1
        SendArg(idx, numargs, arg5)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByVal arg5 As Object, _
    ByVal arg6 As Object, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 7
        SendArg(idx, numargs, expression)
        idx = idx + 1
        SendArg(idx, numargs, arg1)
        idx = idx + 1
        SendArg(idx, numargs, arg2)
        idx = idx + 1
        SendArg(idx, numargs, arg3)
        idx = idx + 1
        SendArg(idx, numargs, arg4)
        idx = idx + 1
        SendArg(idx, numargs, arg5)
        idx = idx + 1
        SendArg(idx, numargs, arg6)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByVal arg5 As Object, _
    ByVal arg6 As Object, _
    ByVal arg7 As Object, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 8
        SendArg(idx, numargs, expression)
        idx = idx + 1
        SendArg(idx, numargs, arg1)
        idx = idx + 1
        SendArg(idx, numargs, arg2)
        idx = idx + 1
        SendArg(idx, numargs, arg3)
        idx = idx + 1
        SendArg(idx, numargs, arg4)
        idx = idx + 1
        SendArg(idx, numargs, arg5)
        idx = idx + 1
        SendArg(idx, numargs, arg6)
        idx = idx + 1
        SendArg(idx, numargs, arg7)
        Return GetAnswer(status)
    End Function

    Public Function MdsValue(ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByVal arg5 As Object, _
    ByVal arg6 As Object, _
    ByVal arg7 As Object, _
    ByVal arg8 As Object, _
    ByRef status As Int32) As Object
        Dim idx As Byte = 0
        Dim numargs As Byte = 9
        SendArg(idx, numargs, expression)
        idx = idx + 1
        SendArg(idx, numargs, arg1)
        idx = idx + 1
        SendArg(idx, numargs, arg2)
        idx = idx + 1
        SendArg(idx, numargs, arg3)
        idx = idx + 1
        SendArg(idx, numargs, arg4)
        idx = idx + 1
        SendArg(idx, numargs, arg5)
        idx = idx + 1
        SendArg(idx, numargs, arg6)
        idx = idx + 1
        SendArg(idx, numargs, arg7)
        idx = idx + 1
        SendArg(idx, numargs, arg8)
        Return GetAnswer(status)
    End Function

    Public Function MdsPut(ByVal node As String, _
     ByVal expression As String, _
     ByRef status As Int32) As Int32
        Return MdsValue("TreePut($,$)", node, expression, status)
    End Function

    Public Function MdsPut(ByVal node As String, _
    ByVal expression As String, _
    ByVal arg1 As Object, _
    ByRef status As Int32) As Int32
        Return MdsValue("TreePut($,$,$)", node, expression, arg1, status)
    End Function

    Public Function MdsPut(ByVal node As String, _
    ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByRef status As Int32) As Int32
        Return MdsValue("TreePut($,$,$,$)", node, expression, arg1, arg2, status)
    End Function

    Public Function MdsPut(ByVal node As String, _
    ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByRef status As Int32) As Int32
        Return MdsValue("TreePut($,$,$,$,$)", node, expression, arg1, arg2, arg3, status)
    End Function

    Public Function MdsPut(ByVal node As String, _
    ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByRef status As Int32) As Int32
        Return MdsValue("TreePut($,$,$,$,$,$)", node, expression, arg1, arg2, arg3, arg4, status)
    End Function

    Public Function MdsPut(ByVal node As String, _
    ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByVal arg5 As Object, _
    ByRef status As Int32) As Int32
        Return MdsValue("TreePut($,$,$,$,$,$,$)", node, expression, arg1, arg2, arg3, arg4, arg5, status)
    End Function

    Public Function MdsPut(ByVal node As String, _
    ByVal expression As String, _
    ByVal arg1 As Object, _
    ByVal arg2 As Object, _
    ByVal arg3 As Object, _
    ByVal arg4 As Object, _
    ByVal arg5 As Object, _
    ByVal arg6 As Object, _
    ByRef status As Int32) As Int32
        Return MdsValue("TreePut($,$,$,$,$,$,$,$)", node, expression, arg1, arg2, arg3, arg4, arg5, arg6, status)
    End Function

    Public Sub MdsWfevent(ByVal EventName As String)
        Dim status As Int32
        Dim temp As Int32 = MdsValue("wfevent(""" + EventName + """),1", status)
    End Sub
    Public Sub MdsWfeventWData(ByVal EventName As String, ByVal ans As Byte())
        Dim status As Int32
        ans = MdsValue("wfevent(""" + EventName + """,$)", ans.Length(), status)
     End Sub

    Public Sub MdsSetevent(ByVal EventName As String)
        Dim status As Int32
        Dim temp As Int32 = MdsValue("setevent(""" + EventName + """),1", status)
    End Sub

End Class
