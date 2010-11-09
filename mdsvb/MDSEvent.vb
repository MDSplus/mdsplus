Imports System
Imports System.Threading
Imports System.Runtime.InteropServices
'
' Example usage:
'
' Constructor (event-name, callback-address)
'Private ev = New MDSEvent.MDSEvent("TEST_EVENT", AddressOf EvCallback)

' Declaration of type for callback (not needed I think)
' Delegate Sub EventCallback(ByVal eventName As String)
'
' Sample Callback routine (Runs in event class's thread !)
'
'Private Sub EvCallback(ByVal event_name As String)
'   Dim buf = ev.getData()
'   Dim Converter As New System.Text.ASCIIEncoding()
'   ShotStr = Converter.GetString(buf)
'   count = 0
'End Sub
'
'Sample callback that runs in the application's thread
'
'Private Sub EvCallback2(ByVal event_name As String)
'   If Me.CountLabel.InvokeRequired Then
'       Dim d As New EventCallback(AddressOf EvCallback)
'       Me.Invoke(d, New Object() {[event_name]})
'   Else
'       Dim buf = ev.getData()
'       Dim Converter As New System.Text.ASCIIEncoding()
'       ShotLabel.Text = Converter.GetString(buf)'
'       count = 0
'       CountLabel.Text = count.ToString()
'   End If
'End Sub

Public Class MDSEvent
    <DllImport("MDSSHR.DLL", EntryPoint:="MDSWfevent", SetLastError:=True, _
      CharSet:=CharSet.Unicode, ExactSpelling:=True, _
      CallingConvention:=CallingConvention.Cdecl)> _
    Shared Function _
      MDSWfevent(<MarshalAs(UnmanagedType.LPStr)> ByVal evName As String, _
                 ByVal buflen As Int32, ByVal buf As IntPtr, _
                 ByRef datlen As Int32) As Int32
    End Function

    Private thread As System.Threading.Thread
    Private eventname As String
    Private timeout As Integer
    Private raw As IntPtr
    Private retlen As Int32

    Delegate Sub EventCallback(ByVal eventName As String)
    Private callback As EventCallback
    Private active As Boolean

    Sub cancel()
        active = False
    End Sub

    Sub Run()
        Dim status As Int32

        While active
            Try
                status = MDSWfevent(eventname, 4096, raw, retlen)
                If active Then
                    callback.Invoke(eventname)
                End If
            Catch ex As Exception
            End Try
        End While
    End Sub

    Function getData() As Byte()
        Dim ans(retlen - 1) As Byte
        Marshal.Copy(raw, ans, 0, retlen)
        Return ans
    End Function

    Sub New(ByVal eventname As String, ByVal callback As EventCallback, Optional ByVal timeout As Integer = 0)
        Me.thread = New Thread(AddressOf Run)
        Me.eventname = eventname
        Me.timeout = timeout
        Me.callback = callback
        Me.active = True
        Me.raw = Marshal.AllocHGlobal(4096)
        Me.thread.Start()
    End Sub
End Class
