Namespace InDisk

  ''' <summary>
  ''' InDisk API for sending commands to InDisk Virtual Disk Driver from .NET applications.
  ''' </summary>
  <ComVisible(False)>
  Public Class InDiskAPI

    Private Sub New()

    End Sub

    ''' <summary>
    ''' InDisk API behaviour flags.
    ''' </summary>
    Public Shared Property APIFlags As DLL.InDiskAPIFlags
      Get
        Return DLL.InDiskGetAPIFlags()
      End Get
      Set(value As DLL.InDiskAPIFlags)
        DLL.InDiskSetAPIFlags(value)
      End Set
    End Property

    ''' <summary>
    ''' Checks if filename contains a known extension for which InDisk knows of a constant offset value. That value can be
    ''' later passed as Offset parameter to CreateDevice method.
    ''' </summary>
    ''' <param name="ImageFile">Name of disk image file.</param>
    Public Shared Function GetOffsetByFileExt(ImageFile As String) As Long

      Dim Offset As Long
      If DLL.InDiskGetOffsetByFileExt(ImageFile, Offset) Then
        Return Offset
      Else
        Return 0
      End If

    End Function

    ''' <summary>
    ''' Parses partition table entries from a master boot record and extended partition table record, if any.
    ''' </summary>
    ''' <param name="ImageFile">Name of image file to examine.</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <param name="Offset">Offset in image file where master boot record is located.</param>
    ''' <returns>An array of eight PARTITION_INFORMATION structures</returns>
    Public Shared Function GetPartitionInformation(ImageFile As String, SectorSize As UInt32, Offset As Long) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)

      Dim PartitionInformation(0 To 7) As NativeFileIO.Win32API.PARTITION_INFORMATION

      NativeFileIO.Win32Try(DLL.InDiskGetPartitionInformation(ImageFile, SectorSize, Offset, PartitionInformation))

      Return Array.AsReadOnly(PartitionInformation)

    End Function

    ''' <summary>
    ''' Parses partition table entries from a master boot record and extended partition table record, if any.
    ''' </summary>
    ''' <param name="ImageFile">Disk image to examine.</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <param name="Offset">Offset in image file where master boot record is located.</param>
    ''' <returns>An array of eight PARTITION_INFORMATION structures</returns>
    Public Shared Function GetPartitionInformation(ImageFile As Stream, SectorSize As UInt32, Offset As Long) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)

      Dim StreamReader As DLL.InDiskReadFileManagedProc =
        Function(_Handle As IntPtr,
                 _Buffer As Byte(),
                 _Offset As Int64,
                 _NumberOfBytesToRead As UInt32,
                 ByRef _NumberOfBytesRead As UInt32) As Boolean

          Try
            ImageFile.Position = _Offset
            _NumberOfBytesRead = CUInt(ImageFile.Read(_Buffer, 0, CInt(_NumberOfBytesToRead)))
            Return True

          Catch
            Return False

          End Try

        End Function

      Dim PartitionInformation(0 To 7) As NativeFileIO.Win32API.PARTITION_INFORMATION

      NativeFileIO.Win32Try(DLL.InDiskGetPartitionInfoIndirect(Nothing, StreamReader, SectorSize, Offset, PartitionInformation))

      Return Array.AsReadOnly(PartitionInformation)

    End Function

    ''' <summary>
    ''' Parses partition table entries from a master boot record and extended partition table record, if any.
    ''' </summary>
    ''' <param name="Handle">Value to pass as first parameter to ReadFileProc.</param>
    ''' <param name="ReadFileProc">Reference to method that reads disk image.</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <param name="Offset">Offset in image file where master boot record is located.</param>
    ''' <returns>An array of eight PARTITION_INFORMATION structures</returns>
    Public Shared Function GetPartitionInformation(Handle As IntPtr, ReadFileProc As DLL.InDiskReadFileManagedProc, SectorSize As UInt32, Offset As Long) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)

      Dim PartitionInformation(0 To 7) As NativeFileIO.Win32API.PARTITION_INFORMATION

      NativeFileIO.Win32Try(DLL.InDiskGetPartitionInfoIndirect(Handle, ReadFileProc, SectorSize, Offset, PartitionInformation))

      Return Array.AsReadOnly(PartitionInformation)

    End Function

    ''' <summary>
    ''' Parses partition table entries from a master boot record and extended partition table record, if any.
    ''' </summary>
    ''' <param name="Handle">Value to pass as first parameter to ReadFileProc.</param>
    ''' <param name="ReadFileProc">Reference to method that reads disk image.</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <returns>An array of eight PARTITION_INFORMATION structures</returns>
    Public Shared Function GetPartitionInformation(Handle As IntPtr, ReadFileProc As DLL.InDiskReadFileManagedProc, SectorSize As UInt32) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)

      Return GetPartitionInformation(Handle, ReadFileProc, SectorSize, 0)

    End Function

    ''' <summary>
    ''' Parses partition table entries from a master boot record and extended partition table record, if any.
    ''' </summary>
    ''' <param name="Handle">Value to pass as first parameter to ReadFileProc.</param>
    ''' <param name="ReadFileProc">Reference to method that reads disk image.</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <param name="Offset">Offset in image file where master boot record is located.</param>
    ''' <returns>An array of eight PARTITION_INFORMATION structures</returns>
    Public Shared Function GetPartitionInformation(Handle As IntPtr, ReadFileProc As DLL.InDiskReadFileUnmanagedProc, SectorSize As UInt32, Offset As Long) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)

      Dim PartitionInformation(0 To 7) As NativeFileIO.Win32API.PARTITION_INFORMATION

      NativeFileIO.Win32Try(DLL.InDiskGetPartitionInfoIndirect(Handle, ReadFileProc, SectorSize, Offset, PartitionInformation))

      Return Array.AsReadOnly(PartitionInformation)

    End Function

    ''' <summary>
    ''' Parses partition table entries from a master boot record and extended partition table record, if any.
    ''' </summary>
    ''' <param name="Handle">Value to pass as first parameter to ReadFileProc.</param>
    ''' <param name="ReadFileProc">Reference to method that reads disk image.</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <returns>An array of eight PARTITION_INFORMATION structures</returns>
    Public Shared Function GetPartitionInformation(Handle As IntPtr, ReadFileProc As DLL.InDiskReadFileUnmanagedProc, SectorSize As UInt32) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)

      Return GetPartitionInformation(Handle, ReadFileProc, SectorSize, 0)

    End Function

    ''' <summary>
    ''' Parses partition table entries from a master boot record and extended partition table record, if any.
    ''' </summary>
    ''' <param name="ImageFile">Name of image file to examine</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <returns>An array of eight PARTITION_INFORMATION structures</returns>
    Public Shared Function GetPartitionInformation(ImageFile As String, SectorSize As UInt32) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)

      Return GetPartitionInformation(ImageFile, SectorSize, 0)

    End Function

    Public Shared Function FilterDefinedPartitions(PartitionList As IEnumerable(Of NativeFileIO.Win32API.PARTITION_INFORMATION)) As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)
      Dim DefinedPartitions As New List(Of NativeFileIO.Win32API.PARTITION_INFORMATION)(7)
      For Each PartitionInfo In PartitionList
        If PartitionInfo.StartingOffset <> 0 AndAlso
          PartitionInfo.PartitionLength <> 0 AndAlso
          Not PartitionInfo.IsContainerPartition Then

          DefinedPartitions.Add(PartitionInfo)
        End If
      Next
      Return DefinedPartitions.AsReadOnly()
    End Function

    ''' <summary>
    ''' Combines GetOffsetByFileExt() and GetPartitionInformation() so that both format-specific offset and 
    ''' offset to first partition is combined into resulting Offset. If a partition was found, size of it is
    ''' also returned in the Size parameter.
    ''' </summary>
    ''' <param name="Imagefile">Name of image file to examine</param>
    ''' <param name="SectorSize">Sector size for translating sector values to absolute byte positions. This
    ''' parameter is in most cases 512.</param>
    ''' <param name="Offset">Absolute offset in image file where volume data begins</param>
    ''' <param name="Size">Size of partition if a partition table was found, otherwise zero</param>
    ''' <remarks></remarks>
    Public Shared Sub AutoFindOffsetAndSize(Imagefile As String,
                                            SectorSize As UInt32,
                                            <Out()> ByRef Offset As Long,
                                            <Out()> ByRef Size As Long)

      Offset = 0
      Size = 0

      Try
        Offset = InDiskAPI.GetOffsetByFileExt(Imagefile)

        Dim PartitionList = InDiskAPI.FilterDefinedPartitions(InDiskAPI.GetPartitionInformation(Imagefile, SectorSize, Offset))
        If PartitionList Is Nothing OrElse PartitionList.Count = 0 Then
          Exit Try
        End If
        If PartitionList(0).StartingOffset > 0 AndAlso
              PartitionList(0).PartitionLength > 0 AndAlso
              Not PartitionList(0).IsContainerPartition Then

          Offset += PartitionList(0).StartingOffset
          Size = PartitionList(0).PartitionLength
        End If

      Catch

      End Try

    End Sub

    ''' <summary>
    ''' Loads InDisk Virtual Disk Driver into Windows kernel. This driver is needed to create InDisk virtual disks. For
    ''' this method to be called successfully, driver needs to be installed and caller needs permission to load kernel mode
    ''' drivers.
    ''' </summary>
    Public Shared Sub LoadDriver()

      NativeFileIO.Win32Try(DLL.InDiskStartService("InDisk"))

    End Sub

    ''' <summary>
    ''' Starts InDisk Virtual Disk Driver Helper Service. This service is needed to create proxy type InDisk virtual disks
    ''' where the I/O proxy application is called through TCP/IP or a serial communications port. For
    ''' this method to be called successfully, service needs to be installed and caller needs permission to start services.
    ''' </summary>
    ''' <remarks></remarks>
    Public Shared Sub LoadHelperService()

      NativeFileIO.Win32Try(DLL.InDiskStartService("ImDskSvc"))

    End Sub

    ''' <summary>
    ''' An easy way to turn an empty NTFS directory to a reparsepoint that redirects
    ''' requests to a mounted device. Acts quite like mount points or symbolic links
    ''' in *nix. If MountPoint specifies a character followed by a colon, a drive
    ''' letter is instead created to point to Target.
    ''' </summary>
    ''' <param name="Directory">Path to empty directory on an NTFS volume, or a drive letter
    ''' followed by a colon.</param>
    ''' <param name="Target">Target path in native format, for example \Device\InDisk0</param>
    Public Shared Sub CreateMountPoint(Directory As String, Target As String)

      NativeFileIO.Win32Try(DLL.InDiskCreateMountPoint(Directory, Target))

    End Sub

    ''' <summary>
    ''' An easy way to turn an empty NTFS directory to a reparsepoint that redirects
    ''' requests to an InDisk device. Acts quite like mount points or symbolic links
    ''' in *nix. If MountPoint specifies a character followed by a colon, a drive
    ''' letter is instead created to point to Target.
    ''' </summary>
    ''' <param name="Directory">Path to empty directory on an NTFS volume, or a drive letter
    ''' followed by a colon.</param>
    ''' <param name="DeviceNumber">Device number of an existing InDisk virtual disk</param>
    Public Shared Sub CreateMountPoint(Directory As String, DeviceNumber As UInt32)

      NativeFileIO.Win32Try(DLL.InDiskCreateMountPoint(Directory, "\Device\InDisk" & DeviceNumber))

    End Sub

    ''' <summary>
    ''' Restores a reparsepoint to be an ordinary empty directory, or removes a drive
    ''' letter mount point.
    ''' </summary>
    ''' <param name="MountPoint">Path to a reparse point on an NTFS volume, or a drive
    ''' letter followed by a colon to remove a drive letter mount point.</param>
    Public Shared Sub RemoveMountPoint(MountPoint As String)

      NativeFileIO.Win32Try(DLL.InDiskRemoveMountPoint(MountPoint))

    End Sub

    ''' <summary>
    ''' Returns first free drive letter available.
    ''' </summary>
    Public Shared Function FindFreeDriveLetter() As Char

      Return DLL.InDiskFindFreeDriveLetter()

    End Function

    ''' <summary>
    ''' Retrieves a list of virtual disks on this system. Each element in returned list holds a device number of a loaded
    ''' InDisk virtual disk.
    ''' </summary>
    Public Shared Function GetDeviceList() As List(Of Int32)

      Dim List As New List(Of Int32)
      Dim NativeDeviceList = DLL.InDiskGetDeviceList()
      Dim NumberValue As UInt64 = 1
      Dim Number As Int32 = 0
      Do
        If (NativeDeviceList And NumberValue) <> 0 Then
          List.Add(Number)
        End If
        Number += 1
        If Number > 63 Then
          Exit Do
        End If
        NumberValue += NumberValue
      Loop

      Return List

    End Function

    ''' <summary>
    ''' Extends size of an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number of InDisk virtual disk to extend.</param>
    ''' <param name="ExtendSize">Size to add.</param>
    ''' <param name="StatusControl">Optional handle to control that can display status messages during operation.</param>
    Public Shared Sub ExtendDevice(DeviceNumber As UInt32, ExtendSize As Int64, StatusControl As IntPtr)

      NativeFileIO.Win32Try(DLL.InDiskExtendDevice(StatusControl, DeviceNumber, ExtendSize))

    End Sub

    ''' <summary>
    ''' Extends size of an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number of InDisk virtual disk to extend.</param>
    ''' <param name="ExtendSize">Size to add.</param>
    Public Shared Sub ExtendDevice(DeviceNumber As UInt32, ExtendSize As Int64)

      ExtendDevice(DeviceNumber, ExtendSize, Nothing)

    End Sub

    ''' <summary>
    ''' Creates a new InDisk virtual disk.
    ''' </summary>
    ''' <param name="DiskSize">Size of virtual disk. If this parameter is zero, current size of disk image file will
    ''' automatically be used as virtual disk size.</param>
    ''' <param name="TracksPerCylinder">Number of tracks per cylinder for virtual disk geometry. This parameter can be zero
    '''  in which case most reasonable value will be automatically used by the driver.</param>
    ''' <param name="SectorsPerTrack">Number of sectors per track for virtual disk geometry. This parameter can be zero
    '''  in which case most reasonable value will be automatically used by the driver.</param>
    ''' <param name="BytesPerSector">Number of bytes per sector for virtual disk geometry. This parameter can be zero
    '''  in which case most reasonable value will be automatically used by the driver.</param>
    ''' <param name="ImageOffset">A skip offset if virtual disk data does not begin immediately at start of disk image file.
    ''' Frequently used with image formats like Nero NRG which start with a file header not used by InDisk or Windows
    ''' filesystem drivers.</param>
    ''' <param name="Flags">Flags specifying properties for virtual disk. See comments for each flag value.</param>
    ''' <param name="Filename">Name of disk image file to use or create. If disk image file already exists the DiskSize
    ''' parameter can be zero in which case current disk image file size will be used as virtual disk size. If Filename
    ''' paramter is Nothing/null disk will be created in virtual memory and not backed by a physical disk image file.</param>
    ''' <param name="NativePath">Specifies whether Filename parameter specifies a path in Windows native path format, the
    ''' path format used by drivers in Windows NT kernels, for example \Device\Harddisk0\Partition1\imagefile.img. If this
    ''' parameter is False path in FIlename parameter will be interpreted as an ordinary user application path.</param>
    ''' <param name="MountPoint">Mount point in the form of a drive letter and colon to create for newly created virtual
    ''' disk. If this parameter is Nothing/null the virtual disk will be created without a drive letter.</param>
    ''' <param name="StatusControl">Optional handle to control that can display status messages during operation.</param>
    Public Shared Sub CreateDevice(DiskSize As Int64,
                                   TracksPerCylinder As UInt32,
                                   SectorsPerTrack As UInt32,
                                   BytesPerSector As UInt32,
                                   ImageOffset As Int64,
                                   Flags As InDiskFlags,
                                   Filename As String,
                                   NativePath As Boolean,
                                   MountPoint As String,
                                   StatusControl As IntPtr)

      Dim DiskGeometry As New NativeFileIO.Win32API.DISK_GEOMETRY With {
        .Cylinders = DiskSize,
        .TracksPerCylinder = TracksPerCylinder,
        .SectorsPerTrack = SectorsPerTrack,
        .BytesPerSector = BytesPerSector
      }

      NativeFileIO.Win32Try(DLL.InDiskCreateDevice(StatusControl,
                                                   DiskGeometry,
                                                   ImageOffset,
                                                   Flags,
                                                   Filename,
                                                   NativePath,
                                                   MountPoint))

    End Sub

    ''' <summary>
    ''' Creates a new InDisk virtual disk.
    ''' </summary>
    ''' <param name="DiskSize">Size of virtual disk. If this parameter is zero, current size of disk image file will
    ''' automatically be used as virtual disk size.</param>
    ''' <param name="TracksPerCylinder">Number of tracks per cylinder for virtual disk geometry. This parameter can be zero
    '''  in which case most reasonable value will be automatically used by the driver.</param>
    ''' <param name="SectorsPerTrack">Number of sectors per track for virtual disk geometry. This parameter can be zero
    '''  in which case most reasonable value will be automatically used by the driver.</param>
    ''' <param name="BytesPerSector">Number of bytes per sector for virtual disk geometry. This parameter can be zero
    '''  in which case most reasonable value will be automatically used by the driver.</param>
    ''' <param name="ImageOffset">A skip offset if virtual disk data does not begin immediately at start of disk image file.
    ''' Frequently used with image formats like Nero NRG which start with a file header not used by InDisk or Windows
    ''' filesystem drivers.</param>
    ''' <param name="Flags">Flags specifying properties for virtual disk. See comments for each flag value.</param>
    ''' <param name="Filename">Name of disk image file to use or create. If disk image file already exists the DiskSize
    ''' parameter can be zero in which case current disk image file size will be used as virtual disk size. If Filename
    ''' paramter is Nothing/null disk will be created in virtual memory and not backed by a physical disk image file.</param>
    ''' <param name="NativePath">Specifies whether Filename parameter specifies a path in Windows native path format, the
    ''' path format used by drivers in Windows NT kernels, for example \Device\Harddisk0\Partition1\imagefile.img. If this
    ''' parameter is False path in FIlename parameter will be interpreted as an ordinary user application path.</param>
    ''' <param name="MountPoint">Mount point in the form of a drive letter and colon to create for newly created virtual
    ''' disk. If this parameter is Nothing/null the virtual disk will be created without a drive letter.</param>
    ''' <param name="DeviceNumber">In: Device number for device to create. Device number must not be in use by an existing
    ''' virtual disk. For automatic allocation of device number, pass UInt32.MaxValue.
    '''
    ''' Out: Device number for created device.</param>
    ''' <param name="StatusControl">Optional handle to control that can display status messages during operation.</param>
    Public Shared Sub CreateDevice(DiskSize As Int64,
                                   TracksPerCylinder As UInt32,
                                   SectorsPerTrack As UInt32,
                                   BytesPerSector As UInt32,
                                   ImageOffset As Int64,
                                   Flags As InDiskFlags,
                                   Filename As String,
                                   NativePath As Boolean,
                                   MountPoint As String,
                                   ByRef DeviceNumber As UInt32,
                                   StatusControl As IntPtr)

      Dim DiskGeometry As New NativeFileIO.Win32API.DISK_GEOMETRY With {
        .Cylinders = DiskSize,
        .TracksPerCylinder = TracksPerCylinder,
        .SectorsPerTrack = SectorsPerTrack,
        .BytesPerSector = BytesPerSector
      }

      NativeFileIO.Win32Try(DLL.InDiskCreateDeviceEx(StatusControl,
                                                     DeviceNumber,
                                                     DiskGeometry,
                                                     ImageOffset,
                                                     Flags,
                                                     Filename,
                                                     NativePath,
                                                     MountPoint))

    End Sub

    ''' <summary>
    ''' Removes an existing InDisk virtual disk from system.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number to remove.</param>
    Public Shared Sub RemoveDevice(DeviceNumber As UInt32)

      RemoveDevice(DeviceNumber, Nothing)

    End Sub

    ''' <summary>
    ''' Removes an existing InDisk virtual disk from system.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number to remove.</param>
    ''' <param name="StatusControl">Optional handle to control that can display status messages during operation.</param>
    Public Shared Sub RemoveDevice(DeviceNumber As UInt32, StatusControl As IntPtr)

      NativeFileIO.Win32Try(DLL.InDiskRemoveDevice(StatusControl, DeviceNumber, Nothing))

    End Sub

    ''' <summary>
    ''' Removes an existing InDisk virtual disk from system.
    ''' </summary>
    ''' <param name="MountPoint">Mount point of virtual disk to remove.</param>
    Public Shared Sub RemoveDevice(MountPoint As String)

      RemoveDevice(MountPoint, Nothing)

    End Sub

    ''' <summary>
    ''' Removes an existing InDisk virtual disk from system.
    ''' </summary>
    ''' <param name="MountPoint">Mount point of virtual disk to remove.</param>
    ''' <param name="StatusControl">Optional handle to control that can display status messages during operation.</param>
    Public Shared Sub RemoveDevice(MountPoint As String, StatusControl As IntPtr)

      If String.IsNullOrEmpty(MountPoint) Then
        Throw New ArgumentNullException("MountPoint")
      End If
      NativeFileIO.Win32Try(DLL.InDiskRemoveDevice(StatusControl, 0, MountPoint))

    End Sub

    ''' <summary>
    ''' Forcefully removes an existing InDisk virtual disk from system even if it is use by other applications.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number to remove.</param>
    Public Shared Sub ForceRemoveDevice(DeviceNumber As UInt32)

      NativeFileIO.Win32Try(DLL.InDiskForceRemoveDevice(IntPtr.Zero, DeviceNumber))

    End Sub

    ''' <summary>
    ''' Retrieves properties for an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number of InDisk virtual disk to retrieve properties for.</param>
    ''' <param name="DiskSize">Size of virtual disk.</param>
    ''' <param name="TracksPerCylinder">Number of tracks per cylinder for virtual disk geometry.</param>
    ''' <param name="SectorsPerTrack">Number of sectors per track for virtual disk geometry.</param>
    ''' <param name="BytesPerSector">Number of bytes per sector for virtual disk geometry.</param>
    ''' <param name="ImageOffset">A skip offset if virtual disk data does not begin immediately at start of disk image file.
    ''' Frequently used with image formats like Nero NRG which start with a file header not used by InDisk or Windows
    ''' filesystem drivers.</param>
    ''' <param name="Flags">Flags specifying properties for virtual disk. See comments for each flag value.</param>
    ''' <param name="DriveLetter">Drive letter if specified when virtual disk was created. If virtual disk was created
    ''' without a drive letter this parameter will be set to an empty Char value.</param>
    ''' <param name="Filename">Name of disk image file holding storage for file type virtual disk or used to create a
    ''' virtual memory type virtual disk.</param>
    Public Shared Sub QueryDevice(DeviceNumber As UInt32,
                                  ByRef DiskSize As Int64,
                                  ByRef TracksPerCylinder As UInt32,
                                  ByRef SectorsPerTrack As UInt32,
                                  ByRef BytesPerSector As UInt32,
                                  ByRef ImageOffset As Int64,
                                  ByRef Flags As InDiskFlags,
                                  ByRef DriveLetter As Char,
                                  ByRef Filename As String)

      Dim CreateDataBuffer As Byte() = Nothing
      Array.Resize(CreateDataBuffer, 1096)

      NativeFileIO.Win32Try(DLL.InDiskQueryDevice(DeviceNumber, CreateDataBuffer, CreateDataBuffer.Length))

      Dim CreateDataReader As New BinaryReader(New MemoryStream(CreateDataBuffer), Encoding.Unicode)
      DeviceNumber = CreateDataReader.ReadUInt32()
      Dim Dummy = CreateDataReader.ReadUInt32()
      DiskSize = CreateDataReader.ReadInt64()
      Dim MediaType = CreateDataReader.ReadInt32()
      TracksPerCylinder = CreateDataReader.ReadUInt32()
      SectorsPerTrack = CreateDataReader.ReadUInt32()
      BytesPerSector = CreateDataReader.ReadUInt32()
      ImageOffset = CreateDataReader.ReadInt64()
      Flags = CType(CreateDataReader.ReadUInt32(), InDiskFlags)
      DriveLetter = CreateDataReader.ReadChar()
      Dim FilenameLength = CreateDataReader.ReadUInt16()
      If FilenameLength = 0 Then
        Filename = Nothing
      Else
        Filename = Encoding.Unicode.GetString(CreateDataReader.ReadBytes(FilenameLength))
      End If

    End Sub

    ''' <summary>
    ''' Retrieves properties for an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number of InDisk virtual disk to retrieve properties for.</param>
    Public Shared Function QueryDevice(DeviceNumber As UInt32) As DLL.InDiskCreateData

      Dim CreateDataBuffer As New DLL.InDiskCreateData
      NativeFileIO.Win32Try(DLL.InDiskQueryDevice(DeviceNumber, CreateDataBuffer, Marshal.SizeOf(CreateDataBuffer.GetType())))
      Return CreateDataBuffer

    End Function

    ''' <summary>
    ''' Modifies properties for an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number of InDisk virtual disk to modify properties for.</param>
    ''' <param name="FlagsToChange">Flags for which to change values for.</param>
    ''' <param name="Flags">New flag values.</param>
    Public Shared Sub ChangeFlags(DeviceNumber As UInt32,
                                  FlagsToChange As InDiskFlags,
                                  Flags As InDiskFlags)

      ChangeFlags(DeviceNumber, FlagsToChange, Flags, Nothing)

    End Sub

    ''' <summary>
    ''' Modifies properties for an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="DeviceNumber">Device number of InDisk virtual disk to modify properties for.</param>
    ''' <param name="FlagsToChange">Flags for which to change values for.</param>
    ''' <param name="Flags">New flag values.</param>
    ''' <param name="StatusControl">Optional handle to control that can display status messages during operation.</param>
    Public Shared Sub ChangeFlags(DeviceNumber As UInt32,
                                  FlagsToChange As InDiskFlags,
                                  Flags As InDiskFlags,
                                  StatusControl As IntPtr)

      NativeFileIO.Win32Try(DLL.InDiskChangeFlags(StatusControl,
                                                  DeviceNumber,
                                                  Nothing,
                                                  FlagsToChange,
                                                  Flags))

    End Sub

    ''' <summary>
    ''' Modifies properties for an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="MountPoint">Mount point of InDisk virtual disk to modify properties for.</param>
    ''' <param name="FlagsToChange">Flags for which to change values for.</param>
    ''' <param name="Flags">New flag values.</param>
    Public Shared Sub ChangeFlags(MountPoint As String,
                                  FlagsToChange As InDiskFlags,
                                  Flags As InDiskFlags)

      ChangeFlags(MountPoint, FlagsToChange, Flags, Nothing)

    End Sub

    ''' <summary>
    ''' Modifies properties for an existing InDisk virtual disk.
    ''' </summary>
    ''' <param name="MountPoint">Mount point of InDisk virtual disk to modify properties for.</param>
    ''' <param name="FlagsToChange">Flags for which to change values for.</param>
    ''' <param name="Flags">New flag values.</param>
    ''' <param name="StatusControl">Optional handle to control that can display status messages during operation.</param>
    Public Shared Sub ChangeFlags(MountPoint As String,
                                  FlagsToChange As InDiskFlags,
                                  Flags As InDiskFlags,
                                  StatusControl As IntPtr)

      NativeFileIO.Win32Try(DLL.InDiskChangeFlags(StatusControl,
                                                  0,
                                                  MountPoint,
                                                  FlagsToChange,
                                                  Flags))

    End Sub

    ''' <summary>
    ''' Checks if Flags specifies a read only virtual disk.
    ''' </summary>
    ''' <param name="Flags">Flag field to check.</param>
    Public Shared Function IsReadOnly(Flags As InDiskFlags) As Boolean

      Return (Flags And InDiskFlags.ReadOnly) = InDiskFlags.ReadOnly

    End Function

    ''' <summary>
    ''' Checks if Flags specifies a removable virtual disk.
    ''' </summary>
    ''' <param name="Flags">Flag field to check.</param>
    Public Shared Function IsRemovable(Flags As InDiskFlags) As Boolean

      Return (Flags And InDiskFlags.Removable) = InDiskFlags.Removable

    End Function

    ''' <summary>
    ''' Checks if Flags specifies a modified virtual disk.
    ''' </summary>
    ''' <param name="Flags">Flag field to check.</param>
    Public Shared Function IsModified(Flags As InDiskFlags) As Boolean

      Return (Flags And InDiskFlags.Modified) = InDiskFlags.Modified

    End Function

    ''' <summary>
    ''' Gets device type bits from a Flag field.
    ''' </summary>
    ''' <param name="Flags">Flag field to check.</param>
    Public Shared Function GetDeviceType(Flags As InDiskFlags) As InDiskFlags

      Return CType(Flags And &HF0UI, InDiskFlags)

    End Function

    ''' <summary>
    ''' Gets disk type bits from a Flag field.
    ''' </summary>
    ''' <param name="Flags">Flag field to check.</param>
    Public Shared Function GetDiskType(Flags As InDiskFlags) As InDiskFlags

      Return CType(Flags And &HF00UI, InDiskFlags)

    End Function

    ''' <summary>
    ''' Gets proxy type bits from a Flag field.
    ''' </summary>
    ''' <param name="Flags">Flag field to check.</param>
    Public Shared Function GetProxyType(Flags As InDiskFlags) As InDiskFlags

      Return CType(Flags And &HF000UI, InDiskFlags)

    End Function

  End Class

End Namespace
