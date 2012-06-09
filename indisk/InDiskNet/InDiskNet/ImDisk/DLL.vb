Namespace InDisk

  <ComVisible(False)>
  Public Class DLL

    Private Sub New()

    End Sub

    ''' <summary>
    ''' InDisk API behaviour flags.
    ''' </summary>
    <Flags()>
    Public Enum InDiskAPIFlags As UInt64

      ''' <summary>
      ''' If set, no broadcast window messages are sent on creation and removal of drive letters.
      ''' </summary>
      NoBroadcastNotify = &H1

      ''' <summary>
      ''' If set, RemoveDevice() will automatically force a dismount of filesystem invalidating
      ''' any open handles.
      ''' </summary>
      ForceDismount = &H2

    End Enum

    Public Declare Unicode Function InDiskGetAPIFlags _
      Lib "indisk.cpl" (
      ) As InDiskAPIFlags

    Public Declare Unicode Function InDiskSetAPIFlags _
      Lib "indisk.cpl" (
        Flags As InDiskAPIFlags
      ) As InDiskAPIFlags

    Public Declare Unicode Function InDiskCheckDriverVersion _
      Lib "indisk.cpl" (
        Handle As SafeFileHandle
      ) As Boolean

    Public Declare Unicode Function InDiskStartService _
      Lib "indisk.cpl" (
        <MarshalAs(UnmanagedType.LPWStr), [In]()> ServiceName As String
      ) As Boolean

    Public Declare Unicode Function InDiskGetOffsetByFileExt _
      Lib "indisk.cpl" (
        <MarshalAs(UnmanagedType.LPWStr), [In]()> ImageFileName As String,
        ByRef Offset As Int64
      ) As Boolean

    Public Declare Unicode Function InDiskGetPartitionInformation _
      Lib "indisk.cpl" (
        <MarshalAs(UnmanagedType.LPWStr), [In]()> ImageFileName As String,
        SectorSize As UInt32,
        <[In]()> ByRef Offset As Int64,
        PartitionInformation As IntPtr
      ) As Boolean

    Public Declare Unicode Function InDiskGetPartitionInformation _
      Lib "indisk.cpl" (
        <MarshalAs(UnmanagedType.LPWStr), [In]()> ImageFileName As String,
        SectorSize As UInt32,
        <[In]()> ByRef Offset As Int64,
        <MarshalAs(UnmanagedType.LPArray), Out()> PartitionInformation As NativeFileIO.Win32API.PARTITION_INFORMATION()
      ) As Boolean

    Public Delegate Function InDiskReadFileManagedProc _
      (
        Handle As IntPtr,
        <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=3), Out()> Buffer As Byte(),
        Offset As Int64,
        NumberOfBytes As UInt32,
        <Out()> ByRef NumberOfBytesRead As UInt32
      ) As Boolean

    Public Delegate Function InDiskReadFileUnmanagedProc _
      (
        Handle As IntPtr,
        Buffer As IntPtr,
        Offset As Int64,
        NumberOfBytes As UInt32,
        <Out()> ByRef NumberOfBytesRead As UInt32
      ) As Boolean

    Public Declare Unicode Function InDiskReadFileHandle _
      Lib "indisk.cpl" (
        Handle As IntPtr,
        <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=3), Out()> Buffer As Byte(),
        Offset As Int64,
        NumberOfBytes As UInt32,
        <Out()> ByRef NumberOfBytesRead As UInt32
      ) As Boolean

    Public Declare Unicode Function InDiskGetPartitionInfoIndirect _
      Lib "indisk.cpl" (
        Handle As IntPtr,
        <MarshalAs(UnmanagedType.FunctionPtr)> ReadFileProc As InDiskReadFileManagedProc,
        SectorSize As UInt32,
        <[In]()> ByRef Offset As Int64,
        PartitionInformation As IntPtr
      ) As Boolean

    Public Declare Unicode Function InDiskGetPartitionInfoIndirect _
      Lib "indisk.cpl" (
        Handle As IntPtr,
        <MarshalAs(UnmanagedType.FunctionPtr)> ReadFileProc As InDiskReadFileManagedProc,
        SectorSize As UInt32,
        <[In]()> ByRef Offset As Int64,
        <MarshalAs(UnmanagedType.LPArray), Out()> PartitionInformation As NativeFileIO.Win32API.PARTITION_INFORMATION()
      ) As Boolean

    Public Declare Unicode Function InDiskGetPartitionInfoIndirect _
      Lib "indisk.cpl" (
        Handle As IntPtr,
        <MarshalAs(UnmanagedType.FunctionPtr)> ReadFileProc As InDiskReadFileUnmanagedProc,
        SectorSize As UInt32,
        <[In]()> ByRef Offset As Int64,
        PartitionInformation As IntPtr
      ) As Boolean

    Public Declare Unicode Function InDiskGetPartitionInfoIndirect _
      Lib "indisk.cpl" (
        Handle As IntPtr,
        <MarshalAs(UnmanagedType.FunctionPtr)> ReadFileProc As InDiskReadFileUnmanagedProc,
        SectorSize As UInt32,
        <[In]()> ByRef Offset As Int64,
        <MarshalAs(UnmanagedType.LPArray), Out()> PartitionInformation As NativeFileIO.Win32API.PARTITION_INFORMATION()
      ) As Boolean

    Public Declare Unicode Function InDiskCreateMountPoint _
      Lib "indisk.cpl" (
        <MarshalAs(UnmanagedType.LPWStr), [In]()> Directory As String,
        <MarshalAs(UnmanagedType.LPWStr), [In]()> Target As String
      ) As Boolean

    Public Declare Unicode Function InDiskRemoveMountPoint _
      Lib "indisk.cpl" (
        <MarshalAs(UnmanagedType.LPWStr), [In]()> MountPoint As String
      ) As Boolean

    Public Declare Unicode Function InDiskOpenDeviceByNumber _
      Lib "indisk.cpl" (
        DeviceNumber As UInt32,
        AccessMode As UInt32
      ) As SafeFileHandle

    Public Declare Unicode Function InDiskOpenDeviceByMountPoint _
      Lib "indisk.cpl" (
        <MarshalAs(UnmanagedType.LPWStr), [In]()> MountPoint As String,
        AccessMode As UInt32
      ) As SafeFileHandle

    Public Declare Unicode Function InDiskGetVolumeSize _
      Lib "indisk.cpl" (
        Handle As SafeFileHandle,
        ByRef Size As Int64
      ) As Boolean

    Public Declare Unicode Function InDiskSaveImageFile _
      Lib "indisk.cpl" (
        DeviceHandle As SafeFileHandle,
        FileHandle As SafeFileHandle,
        BufferSize As UInt32,
        <MarshalAs(UnmanagedType.Bool)> ByRef CancelFlag As Boolean
      ) As Boolean

    Public Declare Unicode Function InDiskSaveImageFile _
      Lib "indisk.cpl" (
        DeviceHandle As SafeFileHandle,
        FileHandle As SafeFileHandle,
        BufferSize As UInt32,
        CancelFlag As IntPtr
      ) As Boolean

    Public Declare Unicode Function InDiskExtendDevice _
      Lib "indisk.cpl" (
        hWndStatusText As IntPtr,
        DeviceNumber As UInt32,
        ByRef ExtendSize As Int64
      ) As Boolean

    Public Declare Unicode Function InDiskCreateDevice _
      Lib "indisk.cpl" (
        hWndStatusText As IntPtr,
        ByRef DiskGeometry As NativeFileIO.Win32API.DISK_GEOMETRY,
        ByRef ImageOffset As Int64,
        Flags As UInt32,
        <MarshalAs(UnmanagedType.LPWStr), [In]()> Filename As String,
        <MarshalAs(UnmanagedType.Bool)> NativePath As Boolean,
        <MarshalAs(UnmanagedType.LPWStr), [In]()> MountPoint As String
      ) As Boolean

    Public Declare Unicode Function InDiskCreateDeviceEx _
      Lib "indisk.cpl" (
        hWndStatusText As IntPtr,
        ByRef DeviceNumber As UInt32,
        ByRef DiskGeometry As NativeFileIO.Win32API.DISK_GEOMETRY,
        ByRef ImageOffset As Int64,
        Flags As UInt32,
        <MarshalAs(UnmanagedType.LPWStr), [In]()> Filename As String,
        <MarshalAs(UnmanagedType.Bool)> NativePath As Boolean,
        <MarshalAs(UnmanagedType.LPWStr), [In]()> MountPoint As String
      ) As Boolean

    Public Declare Unicode Function InDiskRemoveDevice _
      Lib "indisk.cpl" (
        hWndStatusText As IntPtr,
        DeviceNumber As UInt32,
        <MarshalAs(UnmanagedType.LPWStr), [In]()> MountPoint As String
      ) As Boolean

    Public Declare Unicode Function InDiskForceRemoveDevice _
      Lib "indisk.cpl" (
        DeviceHandle As IntPtr,
        DeviceNumber As UInt32
      ) As Boolean

    Public Declare Unicode Function InDiskForceRemoveDevice _
      Lib "indisk.cpl" (
        DeviceHandle As SafeFileHandle,
        DeviceNumber As UInt32
      ) As Boolean

    Public Declare Unicode Function InDiskChangeFlags _
      Lib "indisk.cpl" (
        hWndStatusText As IntPtr,
        DeviceNumber As UInt32,
        <MarshalAs(UnmanagedType.LPWStr), [In]()> MountPoint As String,
        FlagsToChange As UInt32,
        Flags As UInt32
      ) As Boolean

    Public Declare Unicode Function InDiskQueryDevice _
      Lib "indisk.cpl" (
        DeviceNumber As UInt32,
        <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=2), Out()> CreateData As Byte(),
        CreateDataSize As Int32
      ) As Boolean

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Unicode)>
    <ComVisible(False)>
    Public Structure InDiskCreateData
      Public DeviceNumber As Int32
      Private _Dummy As Int32
      Public DiskSize As Int64
      Public MediaType As Int32
      Public TracksPerCylinder As UInt32
      Public SectorsPerTrack As UInt32
      Public BytesPerSector As UInt32
      Public ImageOffset As Int64
      Public Flags As InDiskFlags
      Public DriveLetter As Char
      Private _FilenameLength As UInt16

      <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=16384)>
      Private _Filename As String

      Public Property Filename As String
        Get
          If _Filename IsNot Nothing AndAlso _Filename.Length > _FilenameLength \ 2 Then
            _Filename = _Filename.Remove(_FilenameLength \ 2)
          End If
          Return _Filename
        End Get
        Set(value As String)
          If value Is Nothing Then
            _Filename = Nothing
            _FilenameLength = 0
            Return
          End If
          _Filename = value
          _FilenameLength = CUShort(_Filename.Length * 2)
        End Set
      End Property
    End Structure

    Public Declare Unicode Function InDiskQueryDevice _
      Lib "indisk.cpl" (
        DeviceNumber As UInt32,
        <Out()> ByRef CreateData As InDiskCreateData,
        CreateDataSize As Int32
      ) As Boolean

    Public Declare Unicode Function InDiskFindFreeDriveLetter _
      Lib "indisk.cpl" (
      ) As Char

    Public Declare Unicode Function InDiskGetDeviceList _
      Lib "indisk.cpl" (
      ) As UInt64

    Public Declare Unicode Sub InDiskSaveImageFileInteractive _
      Lib "indisk.cpl" (
        DeviceHandle As SafeFileHandle,
        WindowHandle As IntPtr,
        BufferSize As UInt32,
        <MarshalAs(UnmanagedType.Bool)> IsCdRomType As Boolean
      )

  End Class

End Namespace
