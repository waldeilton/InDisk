Public Enum INDPROXY_REQ As ULong
  INDPROXY_REQ_NULL
  INDPROXY_REQ_INFO
  INDPROXY_REQ_READ
  INDPROXY_REQ_WRITE
  INDPROXY_REQ_CONNECT
  INDPROXY_REQ_CLOSE
End Enum

Public Enum INDPROXY_FLAGS As ULong
  INDPROXY_FLAG_NONE = 0UL
  INDPROXY_FLAG_RO = 1UL
End Enum

''' <summary>
''' Constants used in connection with InDisk/Devio proxy communication.
''' </summary>
Public Class INDPROXY_CONSTANTS
  Private Sub New()
  End Sub

  ''' <summary>
  ''' Header size when communicating using a shared memory object.
  ''' </summary>
  Public Const INDPROXY_HEADER_SIZE As Integer = 4096

  ''' <summary>
  ''' Default required alignment for I/O operations.
  ''' </summary>
  Public Const REQUIRED_ALIGNMENT As Integer = 512
End Class

<StructLayout(LayoutKind.Sequential)>
Public Structure INDPROXY_CONNECT_REQ
  Public request_code As INDPROXY_REQ
  Public flags As ULong
  Public length As ULong
End Structure

<StructLayout(LayoutKind.Sequential)>
Public Structure INDPROXY_CONNECT_RESP
  Public error_code As ULong
  Public object_ptr As ULong
End Structure

<StructLayout(LayoutKind.Sequential)>
Public Structure INDPROXY_INFO_RESP
  Public file_size As ULong
  Public req_alignment As ULong
  Public flags As INDPROXY_FLAGS
End Structure

<StructLayout(LayoutKind.Sequential)>
Public Structure INDPROXY_READ_REQ
  Public request_code As INDPROXY_REQ
  Public offset As ULong
  Public length As ULong
End Structure

<StructLayout(LayoutKind.Sequential)>
Public Structure INDPROXY_READ_RESP
  Public errorno As ULong
  Public length As ULong
End Structure

<StructLayout(LayoutKind.Sequential)>
Public Structure INDPROXY_WRITE_REQ
  Public request_code As INDPROXY_REQ
  Public offset As ULong
  Public length As ULong
End Structure

<StructLayout(LayoutKind.Sequential)>
Public Structure INDPROXY_WRITE_RESP
  Public errorno As ULong
  Public length As ULong
End Structure

