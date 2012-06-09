Namespace InDisk.ComInterop

  <Guid("b0bf4b36-0ebe-4ef4-8974-07694a7a3a81")>
  <ClassInterface(ClassInterfaceType.AutoDual)>
  Public Class InDiskCOM

    Public Sub New()

    End Sub

    Public Function GetOffsetByFileExt(ImageFile As String) As LARGE_INTEGER

      Return InDiskAPI.GetOffsetByFileExt(ImageFile)

    End Function

    Public Sub AutoFindOffsetAndSize(Imagefile As String,
                                     <Out()> ByRef Offset As LARGE_INTEGER,
                                     <Out()> ByRef Size As LARGE_INTEGER,
                                     Optional SectorSize As Int32 = 512)

      InDiskAPI.AutoFindOffsetAndSize(Imagefile, CUInt(SectorSize), Offset, Size)

    End Sub

    Public Sub LoadDriver()

      InDiskAPI.LoadDriver()

    End Sub

    Public Sub LoadHelperService()

      InDiskAPI.LoadHelperService()

    End Sub

    Public Sub CreateMountPoint(Directory As String, Target As String)

      InDiskAPI.CreateMountPoint(Directory, Target)

    End Sub

    Public Sub CreateMountPointForDeviceNumber(Directory As String, DeviceNumber As Int32)

      InDiskAPI.CreateMountPoint(Directory, CType(DeviceNumber, UInt32))

    End Sub

    Public Sub RemoveMountPoint(MountPoint As String)

      InDiskAPI.RemoveMountPoint(MountPoint)

    End Sub

    Public Function FindFreeDriveLetter() As String

      Return InDiskAPI.FindFreeDriveLetter()

    End Function

    Public Function GetDeviceList() As Int32()

      Return InDiskAPI.GetDeviceList().ToArray()

    End Function

    Public Sub ExtendDevice(DeviceNumber As Int32, <[In]()> ByRef ExtendSize As LARGE_INTEGER, Optional StatusControl As Int32 = 0)

      InDiskAPI.ExtendDevice(CUInt(DeviceNumber), CLng(ExtendSize), New IntPtr(StatusControl))

    End Sub

    Public Sub CreateDevice(<[In]()> ByRef DiskSize As LARGE_INTEGER,
                            <[In]()> ByRef ImageOffset As LARGE_INTEGER,
                            Optional Filename As String = Nothing,
                            Optional MountPoint As String = Nothing,
                            Optional TracksPerCylinder As Int32 = 0,
                            Optional SectorsPerTrack As Int32 = 0,
                            Optional BytesPerSector As Int32 = 0,
                            Optional Flags As InDiskFlags = 0,
                            Optional NativePath As Boolean = False,
                            Optional StatusControl As Int32 = 0)

      InDiskAPI.CreateDevice(CLng(DiskSize),
                             CUInt(TracksPerCylinder),
                             CUInt(SectorsPerTrack),
                             CUInt(BytesPerSector),
                             CLng(ImageOffset),
                             Flags,
                             Filename,
                             NativePath,
                             MountPoint,
                             CType(StatusControl, IntPtr))

    End Sub

    Public Sub CreateDeviceEx(<[In]()> ByRef DiskSize As LARGE_INTEGER,
                              <[In]()> ByRef ImageOffset As LARGE_INTEGER,
                              Optional Filename As String = Nothing,
                              Optional MountPoint As String = Nothing,
                              Optional TracksPerCylinder As Int32 = 0,
                              Optional SectorsPerTrack As Int32 = 0,
                              Optional BytesPerSector As Int32 = 0,
                              Optional Flags As InDiskFlags = 0,
                              Optional NativePath As Boolean = False,
                              Optional ByRef DeviceNumber As Int32 = -1,
                              Optional StatusControl As Int32 = 0)

      Dim _DeviceNumber = If(DeviceNumber >= 0, CUInt(DeviceNumber), UInt32.MaxValue)

      InDiskAPI.CreateDevice(CLng(DiskSize),
                             CUInt(TracksPerCylinder),
                             CUInt(SectorsPerTrack),
                             CUInt(BytesPerSector),
                             CLng(ImageOffset),
                             Flags,
                             Filename,
                             NativePath,
                             MountPoint,
                             _DeviceNumber,
                             CType(StatusControl, IntPtr))

      DeviceNumber = CInt(_DeviceNumber)

    End Sub

    Public Sub RemoveDeviceByNumber(DeviceNumber As Int32, Optional StatusControl As Int32 = 0)

      InDiskAPI.RemoveDevice(CUInt(DeviceNumber), CType(StatusControl, IntPtr))

    End Sub

    Public Sub RemoveDeviceByMountPoint(MountPoint As String, Optional StatusControl As Int32 = 0)

      InDiskAPI.RemoveDevice(MountPoint, CType(StatusControl, IntPtr))

    End Sub

    Public Sub ForceRemoveDevice(DeviceNumber As Int32)

      InDiskAPI.ForceRemoveDevice(CUInt(DeviceNumber))

    End Sub

    Public Sub QueryDevice(<[In]()> ByRef DeviceNumber As Int32,
                           <Out()> ByRef DiskSize As LARGE_INTEGER,
                           <Out()> ByRef ImageOffset As LARGE_INTEGER,
                           <Out()> Optional ByRef TracksPerCylinder As Int32 = 0,
                           <Out()> Optional ByRef SectorsPerTrack As Int32 = 0,
                           <Out()> Optional ByRef BytesPerSector As Int32 = 0,
                           <Out()> Optional ByRef Flags As InDiskFlags = 0,
                           <Out()> Optional ByRef DriveLetter As String = Nothing,
                           <Out()> Optional ByRef Filename As String = Nothing)

      Dim _DeviceNumber = CUInt(DeviceNumber)
      Dim _DiskSize As Int64
      Dim _TracksPerCylinder As UInt32
      Dim _SectorsPerTrack As UInt32
      Dim _BytesPerSector As UInt32
      Dim _ImageOffset As Int64
      Dim _DriveLetter As Char

      InDiskAPI.QueryDevice(_DeviceNumber,
                            _DiskSize,
                            _TracksPerCylinder,
                            _SectorsPerTrack,
                            _BytesPerSector,
                            _ImageOffset,
                            Flags,
                            _DriveLetter,
                            Filename)

      DeviceNumber = CInt(_DeviceNumber)
      DiskSize = _DiskSize
      TracksPerCylinder = CInt(_TracksPerCylinder)
      SectorsPerTrack = CInt(_SectorsPerTrack)
      BytesPerSector = CInt(_BytesPerSector)
      ImageOffset = _ImageOffset
      DriveLetter = _DriveLetter

    End Sub

    Public Sub ChangeFlagsByDeviceNumber(DeviceNumber As Int32,
                                         FlagsToChange As InDiskFlags,
                                         Flags As InDiskFlags,
                                         Optional StatusControl As Int32 = 0)

      InDiskAPI.ChangeFlags(CUInt(DeviceNumber), FlagsToChange, Flags, CType(StatusControl, IntPtr))

    End Sub

    Public Sub ChangeFlagsByMountPoint(MountPoint As String,
                                       FlagsToChange As InDiskFlags,
                                       Flags As InDiskFlags,
                                       Optional StatusControl As Int32 = 0)

      InDiskAPI.ChangeFlags(MountPoint, FlagsToChange, Flags, CType(StatusControl, IntPtr))

    End Sub

    Public Function IsReadOnly(Flags As InDiskFlags) As Boolean

      Return (Flags And InDiskFlags.ReadOnly) = InDiskFlags.ReadOnly

    End Function

    Public Function IsRemovable(Flags As InDiskFlags) As Boolean

      Return (Flags And InDiskFlags.Removable) = InDiskFlags.Removable

    End Function

    Public Function IsModified(Flags As InDiskFlags) As Boolean

      Return (Flags And InDiskFlags.Modified) = InDiskFlags.Modified

    End Function

    Public Function GetDeviceType(Flags As InDiskFlags) As InDiskFlags

      Return CType(Flags And &HF0UI, InDiskFlags)

    End Function

    Public Function GetDiskType(Flags As InDiskFlags) As InDiskFlags

      Return CType(Flags And &HF00UI, InDiskFlags)

    End Function

    Public Function GetProxyType(Flags As InDiskFlags) As InDiskFlags

      Return CType(Flags And &HF000UI, InDiskFlags)

    End Function

    Public Function OpenDeviceByNumber(DeviceNumber As Int32, AccessMode As FileAccess) As InDiskDevice

      Return New InDiskDevice(CUInt(DeviceNumber), AccessMode)

    End Function

    Public Function OpenDeviceByMountPoint(MountPoint As String, AccessMode As FileAccess) As InDiskDevice

      Return New InDiskDevice(MountPoint, AccessMode)

    End Function

    Public Function OpenControlDevice() As InDiskObject

      Return New InDiskControl

    End Function

  End Class

  <Guid("209d67ea-9afd-436a-a0f1-51c802fe6720")>
  <StructLayout(LayoutKind.Sequential)>
  Public Structure LARGE_INTEGER
    Implements IEquatable(Of LARGE_INTEGER), IEquatable(Of Int64)

    Public LowPart As Int32
    Public HighPart As Int32

    Public Property QuadPart As Int64
      Get
        Dim bytes As New List(Of Byte)(8)
        bytes.AddRange(BitConverter.GetBytes(LowPart))
        bytes.AddRange(BitConverter.GetBytes(HighPart))
        Return BitConverter.ToInt64(bytes.ToArray(), 0)
      End Get
      Set(value As Int64)
        Dim bytes = BitConverter.GetBytes(value)
        LowPart = BitConverter.ToInt32(bytes, 0)
        HighPart = BitConverter.ToInt32(bytes, 4)
      End Set
    End Property

    Public Sub New(value As Int64)
      QuadPart = value
    End Sub

    Public Overrides Function ToString() As String
      Return QuadPart.ToString()
    End Function

    Public Overrides Function GetHashCode() As Integer
      Return QuadPart.GetHashCode()
    End Function

    Shared Widening Operator CType(value As Int64) As LARGE_INTEGER
      Return New LARGE_INTEGER(value)
    End Operator

    Shared Widening Operator CType(value As LARGE_INTEGER) As Int64
      Return value.QuadPart
    End Operator

    Public Overloads Function Equals(other As Int64) As Boolean Implements IEquatable(Of Int64).Equals
      Return QuadPart = other
    End Function

    Public Overloads Function Equals(other As LARGE_INTEGER) As Boolean Implements IEquatable(Of LARGE_INTEGER).Equals
      Return LowPart = other.LowPart AndAlso HighPart = other.HighPart
    End Function
  End Structure

End Namespace
