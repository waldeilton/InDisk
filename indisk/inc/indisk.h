/*
    InDisk Virtual Disk Driver for Windows NT/2000/XP.

    Copyright (C) 2005-2007 Olof Lagerkvist.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _INC_INDISK_
#define _INC_INDISK_

#ifndef __T
#if defined(_NTDDK_) || defined(UNICODE) || defined(_UNICODE)
#define __T(x)  L ## x
#else
#define __T(x)  x
#endif
#endif

#ifndef _T
#define _T(x)   __T(x)
#endif

#define INDISK_VERSION                 0x0157
#define INDISK_DRIVER_VERSION          0x0103

#ifndef ZERO_STRUCT
#define ZERO_STRUCT { 0 }
#endif

///
/// The base names for the device objects created in \Device
///
#define INDISK_DEVICE_DIR_NAME         _T("\\Device")
#define INDISK_DEVICE_BASE_NAME        INDISK_DEVICE_DIR_NAME  _T("\\InDisk")
#define INDISK_CTL_DEVICE_NAME         INDISK_DEVICE_BASE_NAME _T("Ctl")

///
/// The symlinks to the device objects created in \DosDevices
///
#define INDISK_CTL_DOSDEV              _T("InDiskCtl")
#define INDISK_CTL_DOSDEV_NAME         _T("\\\\.\\")        INDISK_CTL_DOSDEV
#define INDISK_CTL_SYMLINK_NAME        _T("\\DosDevices\\") INDISK_CTL_DOSDEV

///
/// The driver name and image path
///
#define INDISK_DRIVER_NAME             _T("InDisk")
#define INDISK_DRIVER_PATH             _T("system32\\drivers\\indisk.sys")

///
/// Registry settings. It is possible to specify devices to be mounted
/// automatically when the driver loads.
///
#define INDISK_CFG_PARAMETER_KEY                  _T("\\Parameters")
#define INDISK_CFG_MAX_DEVICES_VALUE              _T("MaxDevices")
#define INDISK_CFG_LOAD_DEVICES_VALUE             _T("LoadDevices")
#define INDISK_CFG_DISALLOWED_DRIVE_LETTERS_VALUE _T("DisallowedDriveLetters")
#define INDISK_CFG_IMAGE_FILE_PREFIX              _T("FileName")
#define INDISK_CFG_SIZE_PREFIX                    _T("Size")
#define INDISK_CFG_FLAGS_PREFIX                   _T("Flags")
#define INDISK_CFG_DRIVE_LETTER_PREFIX            _T("DriveLetter")

///
/// Base value for the IOCTL's.
///
#define FILE_DEVICE_INDISK             0x8372

#define IOCTL_INDISK_QUERY_VERSION     ((ULONG) CTL_CODE(FILE_DEVICE_INDISK, 0x800, METHOD_BUFFERED, 0))
#define IOCTL_INDISK_CREATE_DEVICE     ((ULONG) CTL_CODE(FILE_DEVICE_INDISK, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))
#define IOCTL_INDISK_QUERY_DEVICE      ((ULONG) CTL_CODE(FILE_DEVICE_INDISK, 0x802, METHOD_BUFFERED, 0))
#define IOCTL_INDISK_QUERY_DRIVER      ((ULONG) CTL_CODE(FILE_DEVICE_INDISK, 0x803, METHOD_BUFFERED, 0))
#define IOCTL_INDISK_REFERENCE_HANDLE  ((ULONG) CTL_CODE(FILE_DEVICE_INDISK, 0x804, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))
#define IOCTL_INDISK_SET_DEVICE_FLAGS  ((ULONG) CTL_CODE(FILE_DEVICE_INDISK, 0x805, METHOD_BUFFERED, 0))
#define IOCTL_INDISK_REMOVE_DEVICE     ((ULONG) CTL_CODE(FILE_DEVICE_INDISK, 0x806, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))

///
/// Bit constants for the Flags field in INDISK_CREATE_DATA
///

/// Read-only device
#define INDISK_OPTION_RO                0x00000001

/// Check if flags specifies read-only
#define INDISK_READONLY(x)              ((ULONG)(x) & 0x00000001)

/// Removable, hot-plug, device
#define INDISK_OPTION_REMOVABLE         0x00000002

/// Check if flags specifies removable
#define INDISK_REMOVABLE(x)             ((ULONG)(x) & 0x00000002)

/// Device type is virtual harddisk partition
#define INDISK_DEVICE_TYPE_HD           0x00000010
/// Device type is virtual floppy drive
#define INDISK_DEVICE_TYPE_FD           0x00000020
/// Device type is virtual CD/DVD-ROM drive
#define INDISK_DEVICE_TYPE_CD           0x00000030

/// Extracts the INDISK_DEVICE_TYPE_xxx from flags
#define INDISK_DEVICE_TYPE(x)           ((ULONG)(x) & 0x000000F0)

/// Virtual disk is backed by image file
#define INDISK_TYPE_FILE                0x00000100
/// Virtual disk is backed by virtual memory
#define INDISK_TYPE_VM                  0x00000200
/// Virtual disk is backed by proxy connection
#define INDISK_TYPE_PROXY               0x00000300

/// Extracts the INDISK_TYPE_xxx from flags
#define INDISK_TYPE(x)                  ((ULONG)(x) & 0x00000F00)

/// Proxy connection is direct-type
#define INDISK_PROXY_TYPE_DIRECT        0x00000000
/// Proxy connection is over serial line
#define INDISK_PROXY_TYPE_COMM          0x00001000
/// Proxy connection is over TCP/IP
#define INDISK_PROXY_TYPE_TCP           0x00002000
/// Proxy connection uses shared memory
#define INDISK_PROXY_TYPE_SHM           0x00003000

/// Extracts the INDISK_PROXY_TYPE_xxx from flags
#define INDISK_PROXY_TYPE(x)            ((ULONG)(x) & 0x0000F000)

/// Extracts the INDISK_PROXY_TYPE_xxx from flags
#define INDISK_IMAGE_MODIFIED           0x00010000

/// Specify as device number to automatically select first free.
#define INDISK_AUTO_DEVICE_NUMBER       ((ULONG)-1)

/**
   Structure used by the IOCTL_INDISK_CREATE_DEVICE and
   IOCTL_INDISK_QUERY_DEVICE calls and by the InDiskQueryDevice() function.
*/
typedef struct _INDISK_CREATE_DATA
{
  /// On create this can be set to INDISK_AUTO_DEVICE_NUMBER
  ULONG           DeviceNumber;
  /// Total size in bytes (in the Cylinders field) and virtual geometry.
  DISK_GEOMETRY   DiskGeometry;
  /// The byte offset in the image file where the virtual disk begins.
  LARGE_INTEGER   ImageOffset;
  /// Creation flags. Type of device and type of connection.
  ULONG           Flags;
  /// Driveletter (if used, otherwise zero).
  WCHAR           DriveLetter;
  /// Length in bytes of the FileName member.
  USHORT          FileNameLength;
  /// Dynamically-sized member that specifies the image file name.
  WCHAR           FileName[1];
} INDISK_CREATE_DATA, *PINDISK_CREATE_DATA;

typedef struct _INDISK_SET_DEVICE_FLAGS
{
  ULONG FlagsToChange;
  ULONG FlagValues;
} INDISK_SET_DEVICE_FLAGS, *PINDISK_SET_DEVICE_FLAGS;

#define INDISK_API_NO_BROADCAST_NOTIFY  0x00000001
#define INDISK_API_FORCE_DISMOUNT       0x00000002

#ifdef WINAPI

#ifdef __cplusplus
extern "C" {
#endif

/**
   Get behaviour flags for API.
*/
ULONGLONG
WINAPI
InDiskGetAPIFlags();

/**
   Set behaviour flags for API. Returns previously defined flag field.

   Flags        New flags value to set.
*/
ULONGLONG
WINAPI
InDiskSetAPIFlags(ULONGLONG Flags);

/**
   An interactive rundll32.exe-compatible function to show the Add New Virtual
   Disk dialog box with a file name already filled in. It is used by the
   Windows Explorer context menues.

   hWnd         Specifies a window that will be the owner window of any
                MessageBox:es or similar.

   hInst        Ignored.

   lpszCmdLine  An ANSI string specifying the image file to mount.

   nCmdShow     Ignored.
*/
void
WINAPI
RunDLL_MountFile(HWND hWnd,
		 HINSTANCE hInst,
		 LPSTR lpszCmdLine,
		 int nCmdShow);

/**
   An interactive rundll32.exe-compatible function to remove an existing InDisk
   virtual disk. If the filesystem on the device cannot be locked and
   dismounted a MessageBox() is displayed that asks the user if dismount should
   be forced.

   hWnd         Specifies a window that will be the owner window of any
                MessageBox:es or similar.

   hInst        Ignored.

   lpszCmdLine  An ANSI string specifying the the virtual disk to remove. This
                can be on the form "F:" or "F:\" (without the quotes).

   nCmdShow     Ignored.
*/
void
WINAPI
RunDLL_RemoveDevice(HWND hWnd,
		    HINSTANCE hInst,
		    LPSTR lpszCmdLine,
		    int nCmdShow);

/**
   An interactive rundll32.exe-compatible function to save a virtual or
   physical drive as an image file. If the filesystem on the device cannot be
   locked and dismounted a MessageBox() is displayed that asks the user if the
   image saving should continue anyway.

   hWnd         Specifies a window that will be the owner window of any
                MessageBox:es or similar.

   hInst        Ignored.

   lpszCmdLine  An ANSI string specifying the the disk to save. This can be on
   the form "F:" or "F:\" (without the quotes).

   nCmdShow     Ignored.
*/
void
WINAPI
RunDLL_SaveImageFile(HWND hWnd,
		     HINSTANCE hInst,
	             LPSTR lpszCmdLine,
		     int nCmdShow);

/**
   This function displays a MessageBox() dialog with a
   FormatMessage()-formatted message.
   
   hWndParent   Parent window for the MessageBox() call.

   uStyle       Style for the MessageBox() call.

   lpTitle      Window title for the MessageBox() call.

   lpMessage    Format string to be used in call to FormatMessage() followed
                 by field parameters.
*/
BOOL
CDECL
MsgBoxPrintF(IN HWND hWndParent OPTIONAL,
	     IN UINT uStyle,
	     IN LPCWSTR lpTitle,
	     IN LPCWSTR lpMessage, ...);

/**
   This function is a quick perror()-style way of displaying an error message
   for the last failed Windows API call.

   hWndParent   Parent window for the MessageBox() call.

   Prefix       Text to print before the error message string.
*/
VOID
WINAPI
MsgBoxLastError(IN HWND hWndParent OPTIONAL,
		IN LPCWSTR Prefix);

/**
   Used to get a string describing a partition type.

   PartitionType  Partition type from partition table.

   Name           Pointer to memory that receives a string describing the
                  partition type.

   NameSize       Size of memory area pointed to by the Name parameter.
*/
VOID
WINAPI
InDiskGetPartitionTypeName(IN BYTE PartitionType,
			   OUT LPWSTR Name,
			   IN DWORD NameSize);

/**
   Returns the offset in bytes to actual disk image data for some known
   "non-raw" image file formats with headers. Returns TRUE if file extension
   is recognized and the known offset has been stored in the variable pointed
   to by the Offset parameter. Otherwise the function returns FALSE and the
   value pointed to by the Offset parameter is not changed.

   ImageFile    Name of raw disk image file. This does not need to be a valid
                path or filename, just the extension is used by this function.

   Offset       Returned offset in bytes if function returns TRUE.
*/
BOOL
WINAPI
InDiskGetOffsetByFileExt(IN LPCWSTR ImageFile,
			 OUT PLARGE_INTEGER Offset);

/**
   Attempts to find partition information from a partition table for a raw
   disk image file. If no master boot record is found this function returns
   FALSE. Returns TRUE if a master boot record with a partition table is found
   and values stored in the structures pointed to by the PartitionInformation
   parameter. Otherwise the function returns FALSE.

   ImageFile    Name of raw disk image file to examine.

   SectorSize   Optional sector size used on disk if different from default
                512 bytes.

   Offset       Optional offset in bytes to master boot record within file for
                use with "non-raw" image files with headers before the actual
		disk image data.

   PartitionInformation
                Pointer to an array of eight PARTITION_INFORMATION structures
		which will receive information from four recognized primary
		partition entries followed by four recognized extended entries.
*/
BOOL
WINAPI
InDiskGetPartitionInformation(IN LPCWSTR ImageFile,
			      IN DWORD SectorSize OPTIONAL,
			      IN PLARGE_INTEGER Offset OPTIONAL,
			      OUT PPARTITION_INFORMATION PartitionInformation);


/**
   Prototype for raw disk reader function used with
   InDiskGetPartitionInfoIndirect().

   Handle                Value that was passed as first parameter to
                         InDiskGetPartitionInfoIndirect().

   Buffer                Buffer where read data is to be stored.

   Offset                Disk offset where read operation starts.

   NumberOfBytesToRead   Number of bytes to read from disk.

   NumberOfBytesRead     Pointer to DWORD size variable where function stores
                         number of bytes actually read into Buffer. This value
                         can be equal to or less than NumberOfBytesToRead
			 parameter.
*/
typedef BOOL (WINAPI *InDiskReadFileProc)(IN HANDLE Handle,
					  IN OUT LPVOID Buffer,
					  IN LARGE_INTEGER Offset,
					  IN DWORD NumberOfBytesToRead,
					  OUT LPDWORD NumberOfBytesRead);

/**
   A device read function with InDiskReadFileProc, which means that it can be
   used when calling InDiskGetPartitionInfoIndirect() function.

   Handle       Operating system file handle representing a file or device
                opened for reading.

   Buffer       Buffer where read data is to be stored.

   Offset       Disk offset where read operation starts.

   NumberOfBytesToRead
                Number of bytes to read from disk.

   NumberOfBytesRead
                Pointer to DWORD size variable where function stores number of
		bytes actually read into Buffer. This value can be equal to or
		less than NumberOfBytesToRead parameter.
*/
BOOL
WINAPI
InDiskReadFileHandle(IN HANDLE Handle,
		     IN OUT LPVOID Buffer,
		     IN LARGE_INTEGER Offset,
		     IN DWORD NumberOfBytesToRead,
		     OUT LPDWORD NumberOfBytesRead);

/**
   Attempts to find partition information from a partition table for a disk
   image through a supplied device reader function.

   If no master boot record is found this function returns FALSE. Returns TRUE
   if a master boot record with a partition table is found and values stored in
   the structures pointed to by the PartitionInformation parameter. Otherwise
   the function returns FALSE.

   Handle       Value that is passed as first parameter to ReadFileProc.

   ReadFileProc Procedure of type InDiskReadFileProc that is called to read raw
                disk image.

   SectorSize   Optional sector size used on disk if different from default
                512 bytes.

   Offset       Optional offset in bytes to master boot record within file for
                use with "non-raw" image files with headers before the actual
		disk image data.

   PartitionInformation
                Pointer to an array of eight PARTITION_INFORMATION structures
		which will receive information from four recognized primary
		partition entries followed by four recognized extended entries.
*/
BOOL
WINAPI
InDiskGetPartitionInfoIndirect(IN HANDLE Handle,
			       IN InDiskReadFileProc ReadFileProc,
			       IN DWORD SectorSize OPTIONAL,
			       IN PLARGE_INTEGER Offset OPTIONAL,
			       OUT PPARTITION_INFORMATION PartitionInfo);

/**
   Starts a Win32 service or loads a kernel module or driver.

   ServiceName  Key name of the service or driver.
*/
BOOL
WINAPI
InDiskStartService(IN LPWSTR ServiceName);

/**
   An easy way to turn an empty NTFS directory to a reparsepoint that redirects
   requests to a mounted device. Acts quite like mount points or symbolic links
   in *nix. If MountPoint specifies a character followed by a colon, a drive
   letter is instead created to point to Target.

   MountPoint   Path to empty directory on an NTFS volume, or a drive letter
                followed by a colon.

   Target       Target device path on kernel object namespace form, e.g.
                \Device\InDisk2 or similar.
*/
BOOL
WINAPI
InDiskCreateMountPoint(IN LPCWSTR MountPoint,
		       IN LPCWSTR Target);

/**
   Restores a reparsepoint to be an ordinary empty directory, or removes a
   drive letter mount point.

   MountPoint   Path to a reparse point on an NTFS volume, or a drive letter
                followed by a colon to remove a drive letter mount point.
*/
BOOL
WINAPI
InDiskRemoveMountPoint(IN LPCWSTR MountPoint);

/**
   Opens a device object in the kernel object namespace.

   FileName     Full kernel object namespace path to the object to open, e.g.
                \Device\InDisk2 or similar.

   AccessMode   Access mode to request.
*/
HANDLE
WINAPI
InDiskOpenDeviceByName(IN PUNICODE_STRING FileName,
		       IN DWORD AccessMode);

/**
   Opens an InDisk device by the device number.

   FileName     Native path to InDisk device, such as "\Device\InDisk2".

   AccessMode   Access mode to request.
*/
HANDLE
WINAPI
InDiskOpenDeviceByNumber(IN DWORD DeviceNumber,
			 IN DWORD AccessMode);

/**
   Opens the device a junction/mount-point type reparse point is pointing to.

   MountPoint   Path to the reparse point on an NTFS volume.

   AccessMode   Access mode to request to the target device.
*/
HANDLE
WINAPI
InDiskOpenDeviceByMountPoint(IN LPCWSTR MountPoint,
			     IN DWORD AccessMode);

/**
   Check that the user-mode library and kernel-mode driver version matches for
   an open InDisk created device object.

   DeviceHandle Handle to an open InDisk virtual disk or control device.
*/
BOOL
WINAPI
InDiskCheckDriverVersion(IN HANDLE DeviceHandle);

/**
   Retrieves the version numbers of the user-mode API library and the kernel-
   mode driver.
*/
BOOL
WINAPI
InDiskGetVersion(OUT PULONG LibraryVersion OPTIONAL,
		 OUT PULONG DriverVersion OPTIONAL);

/**
   Returns the first free drive letter in the range D-Z.
*/
WCHAR
WINAPI
InDiskFindFreeDriveLetter();

/**
   Returns a bit-field representing InDisk devices. Bit 0 represents device 0,
   bit 1 represents device 1 and so on. A bit is 1 if the device exists or 0 if
   the device number is free.
*/
ULONGLONG
WINAPI
InDiskGetDeviceList();

/**
   This function sends an IOCTL_INDISK_QUERY_DEVICE control code to an existing
   device and returns information about the device in an INDISK_CREATE_DATA
   structue.

   DeviceNumber    Number of the InDisk device to query.

   CreateData      Pointer to a sufficiently large INDISK_CREATE_DATA
                   structure to receive all data including the image file name
		   where applicable.

   CreateDataSize  The size in bytes of the memory the CreateData parameter
                   points to. The function call will fail if the memory is not
		   large enough to hold the entire INDISK_CREATE_DATA
		   structure.
*/
BOOL
WINAPI
InDiskQueryDevice(IN DWORD DeviceNumber,
		  OUT PINDISK_CREATE_DATA CreateData,
		  IN ULONG CreateDataSize);

/**
   This function creates a new InDisk virtual disk device.

   hWndStatusText  A handle to a window that can display status message text.
                   The function will send WM_SETTEXT messages to this window.
		   If this parameter is NULL no WM_SETTEXT messages are sent
		   and the function acts non-interactive.

   DiskGeometry    The virtual geometry of the new virtual disk. Note that the
                   Cylinders member does not specify the number of Cylinders
		   but the total size in bytes of the new virtual disk. The
		   actual number of cylinders are then automatically
		   calculated and rounded down if necessary.

		   The Cylinders member can be zero if the device is backed by
		   an image file or a proxy device, but not if it is virtual
		   memory only device.

		   All or some of the other members of this structure can be
		   zero in which case they are automatically filled in with
		   most reasonable values by the driver.

   Flags           Bitwise or-ed combination of one of the INDISK_TYPE_xxx
                   flags, one of the INDISK_DEVICE_TYPE_xxx flags and any
		   number of INDISK_OPTION_xxx flags. The flags can often be
		   left zero and left to the driver to automatically select.
		   For example, if a virtual disk size is specified to 1440 KB
		   and an image file name is not specified, the driver
		   automatically selects INDISK_TYPE_VM|INDISK_DEVICE_TYPE_FD
		   for this parameter.

   FileName        Name of disk image file. In case INDISK_TYPE_VM is
                   specified in the Flags parameter, this file will be loaded
		   into the virtual memory-backed disk when created.

   NativePath      Set to TRUE if the FileName parameter specifies an NT
                   native path, such as \??\C:\imagefile.img or FALSE if it
		   specifies a Win32/DOS-style path such as C:\imagefile.img.

   MountPoint      Drive letter to assign to the new virtual device. It can be
                   specified on the form F: or F:\.
*/
BOOL
WINAPI
InDiskCreateDevice(IN HWND hWndStatusText OPTIONAL,
		   IN OUT PDISK_GEOMETRY DiskGeometry OPTIONAL,
		   IN PLARGE_INTEGER ImageOffset OPTIONAL,
		   IN DWORD Flags OPTIONAL,
		   IN LPCWSTR FileName OPTIONAL,
		   IN BOOL NativePath,
		   IN LPWSTR MountPoint OPTIONAL);

/**
   This function creates a new InDisk virtual disk device.

   hWndStatusText  A handle to a window that can display status message text.
                   The function will send WM_SETTEXT messages to this window.
		   If this parameter is NULL no WM_SETTEXT messages are sent
		   and the function acts non-interactive.

   DeviceNumber    In: Device number for device to create. Device number must
                   not be in use by an existing virtual disk. For automatic
                   allocation of device number, use INDISK_AUTO_DEVICE_NUMBER
                   constant or specify a NULL pointer.

                   Out: If DeviceNumber parameter is not NULL, device number
		   for created device is returned in DWORD variable pointed to.

   DiskGeometry    The virtual geometry of the new virtual disk. Note that the
                   Cylinders member does not specify the number of Cylinders
		   but the total size in bytes of the new virtual disk. The
		   actual number of cylinders are then automatically
		   calculated and rounded down if necessary.

		   The Cylinders member can be zero if the device is backed by
		   an image file or a proxy device, but not if it is virtual
		   memory only device.

		   All or some of the other members of this structure can be
		   zero in which case they are automatically filled in with
		   most reasonable values by the driver.

   Flags           Bitwise or-ed combination of one of the INDISK_TYPE_xxx
                   flags, one of the INDISK_DEVICE_TYPE_xxx flags and any
		   number of INDISK_OPTION_xxx flags. The flags can often be
		   left zero and left to the driver to automatically select.
		   For example, if a virtual disk size is specified to 1440 KB
		   and an image file name is not specified, the driver
		   automatically selects INDISK_TYPE_VM|INDISK_DEVICE_TYPE_FD
		   for this parameter.

   FileName        Name of disk image file. In case INDISK_TYPE_VM is
                   specified in the Flags parameter, this file will be loaded
		   into the virtual memory-backed disk when created.

   NativePath      Set to TRUE if the FileName parameter specifies an NT
                   native path, such as \??\C:\imagefile.img or FALSE if it
		   specifies a Win32/DOS-style path such as C:\imagefile.img.

   MountPoint      Drive letter to assign to the new virtual device. It can be
                   specified on the form F: or F:\.
*/
BOOL
WINAPI
InDiskCreateDeviceEx(IN HWND hWndStatusText OPTIONAL,
		     IN OUT LPDWORD DeviceNumber OPTIONAL,
		     IN OUT PDISK_GEOMETRY DiskGeometry OPTIONAL,
		     IN PLARGE_INTEGER ImageOffset OPTIONAL,
		     IN DWORD Flags OPTIONAL,
		     IN LPCWSTR FileName OPTIONAL,
		     IN BOOL NativePath,
		     IN LPWSTR MountPoint OPTIONAL);

/**
   This function removes (unmounts) an existing InDisk virtual disk device.

   hWndStatusText  A handle to a window that can display status message text.
                   The function will send WM_SETTEXT messages to this window.
		   If this parameter is NULL no WM_SETTEXT messages are sent
		   and the function acts non-interactive.

   DeviceNumber    Number of the InDisk device to remove. This parameter is
                   only used if MountPoint parameter is null.

   MountPoint      Drive letter of the device to remove. It can be specified
                   on the form F: or F:\.
*/
BOOL
WINAPI
InDiskRemoveDevice(IN HWND hWndStatusText OPTIONAL,
		   IN DWORD DeviceNumber OPTIONAL,
		   IN LPCWSTR MountPoint OPTIONAL);

/**
   This function forcefully removes (unmounts) an existing InDisk virtual disk
   device. Any unsaved data will be lost.

   Device          Handle to open device. If not NULL, it is used to query
                   device number to find out which device to remove. If this
		   parameter is NULL the DeviceNumber parameter is used
		   instead.

   DeviceNumber    Number of the InDisk device to remove. This parameter is
                   only used if Device parameter is NULL.
*/
BOOL
WINAPI
InDiskForceRemoveDevice(IN HANDLE Device OPTIONAL,
			IN DWORD DeviceNumber OPTIONAL);

/**
   This function changes the device characteristics of an existing InDisk
   virtual disk device.

   hWndStatusText  A handle to a window that can display status message text.
                   The function will send WM_SETTEXT messages to this window.
		   If this parameter is NULL no WM_SETTEXT messages are sent
		   and the function acts non-interactive.

   DeviceNumber    Number of the InDisk device to change. This parameter is
                   only used if MountPoint parameter is null.

   MountPoint      Drive letter of the device to change. It can be specified
                   on the form F: or F:\.

   FlagsToChange   A bit-field specifying which flags to edit. The flags are
                   the same as the option flags in the Flags parameter used
		   when a new virtual disk is created. Only flags set in this
		   parameter are changed to the corresponding flag value in the
		   Flags parameter.

   Flags           New values for the flags specified by the FlagsToChange
                   parameter.
*/
BOOL
WINAPI
InDiskChangeFlags(HWND hWndStatusText OPTIONAL,
		  DWORD DeviceNumber OPTIONAL,
		  LPCWSTR MountPoint OPTIONAL,
		  DWORD FlagsToChange,
		  DWORD Flags);

/**
   This function extends the size of an existing InDisk virtual disk device.

   hWndStatusText  A handle to a window that can display status message text.
                   The function will send WM_SETTEXT messages to this window.
		   If this parameter is NULL no WM_SETTEXT messages are sent
		   and the function acts non-interactive.

   DeviceNumber    Number of the InDisk device to extend.

   ExtendSize      A pointer to a LARGE_INTEGER structure that specifies the
                   number of bytes to extend the device.
*/
BOOL
WINAPI
InDiskExtendDevice(IN HWND hWndStatusText OPTIONAL,
		   IN DWORD DeviceNumber,
		   IN CONST PLARGE_INTEGER ExtendSize);

/**
   This function saves the contents of a device to an image file.

   DeviceHandle    Handle to a device for which the contents are to be saved to
                   an image file.

		   The handle must be opened for reading, may be
		   opened for sequential scan and/or without intermediate
		   buffering but cannot be opened for overlapped operation.
		   Please note that a call to this function will turn on
		   FSCTL_ALLOW_EXTENDED_DASD_IO on for this handle.

   FileHandle      Handle to an image file opened for writing. The handle
                   can be opened for operation without intermediate buffering
		   but performance is usually better if the handle is opened
		   with intermediate buffering. The handle cannot be opened for
		   overlapped operation.

   BufferSize      I/O buffer size to use when reading source disk. This
                   parameter is optional, if it is zero the buffer size to use
                   will automatically choosen.

   CancelFlag      Optional pointer to a BOOL value. If this BOOL value is set
                   to TRUE during the function call the operation is cancelled,
		   the function returns FALSE and GetLastError() will return
		   ERROR_CANCELLED. If this parameter is non-null the function
		   will also dispatch window messages for the current thread
		   between each I/O operation.
*/
BOOL
WINAPI
InDiskSaveImageFile(IN HANDLE DeviceHandle,
		    IN HANDLE FileHandle,
		    IN DWORD BufferSize OPTIONAL,
		    IN LPBOOL CancelFlag OPTIONAL);

/**
   This function gets the size of a disk volume.

   Handle          Handle to a disk volume device.

   Size            Pointer to a 64 bit variable that upon successful completion
                   receives disk volume size as a signed integer.
*/
BOOL
WINAPI
InDiskGetVolumeSize(IN HANDLE Handle,
		    OUT PLONGLONG Size);

/**
   This function builds a Master Boot Record, MBR, in memory. The MBR will
   contain a default Initial Program Loader, IPL, which could be used to boot
   an operating system partition when the MBR is written to a disk.

   DiskGeometry    Pointer to a DISK_GEOMETRY or DISK_GEOMETRY_EX structure
                   that contains information about logical geometry of the
		   disk.

		   This function only uses the BytesPerSector, SectorsPerTrack
		   and TracksPerCylinder members.

		   This parameter can be NULL if NumberOfParts parameter is
		   zero.

   PartitionInfo   Pointer to an array of up to four PARTITION_INFORMATION
                   structures containing information about partitions to store
		   in MBR partition table.

		   This function only uses the StartingOffset, PartitionLength,
		   BootIndicator and PartitionType members.

		   This parameter can be NULL if NumberOfParts parameter is
		   zero.

   NumberOfParts   Number of PARTITION_INFORMATION structures in array that
                   PartitionInfo parameter points to.

		   If this parameter is zero, DiskGeometry and PartitionInfo
		   parameters are ignored and can be NULL. In that case MBR
		   will contain an empty partition table when this function
		   returns.

   MBR             Pointer to memory buffer of at least 512 bytes where MBR
                   will be built.

   MBRSize         Size of buffer pointed to by MBR parameter. This parameter
                   must be at least 512.
*/
BOOL
WINAPI
InDiskBuildMBR(IN PDISK_GEOMETRY DiskGeometry OPTIONAL,
	       IN PPARTITION_INFORMATION PartitionInfo OPTIONAL,
	       IN BYTE NumberOfParts OPTIONAL,
	       IN OUT LPBYTE MBR,
	       IN DWORD_PTR MBRSize);

/**
   This function converts a CHS disk address to LBA format.

   DiskGeometry    Pointer to a DISK_GEOMETRY or DISK_GEOMETRY_EX structure
                   that contains information about logical geometry of the
		   disk. This function only uses the SectorsPerTrack and
		   TracksPerCylinder members.

   CHS             Pointer to CHS disk address in three-byte partition table
                   style format.
*/
DWORD
WINAPI
InDiskConvertCHSToLBA(IN PDISK_GEOMETRY DiskGeometry,
		      IN LPBYTE CHS);

/**
   This function converts an LBA disk address to three-byte partition style CHS
   format. The three bytes are returned in the three lower bytes of a DWORD.

   DiskGeometry    Pointer to a DISK_GEOMETRY or DISK_GEOMETRY_EX structure
                   that contains information about logical geometry of the
		   disk. This function only uses the SectorsPerTrack and
		   TracksPerCylinder members.

   LBA             LBA disk address.
*/
DWORD
WINAPI
InDiskConvertLBAToCHS(IN PDISK_GEOMETRY DiskGeometry,
		      IN DWORD LBA);

/**
   This function adjusts size of a saved image file. If file size is less than
   requested disk size, the size will be left unchanged with return value FALSE
   and GetLastError() will return ERROR_DISK_OPERATION_FAILED.

   FileHandle      Handle to file where disk image has been saved.

   FileSize        Size of original disk which image file should be adjusted
                   to.
*/
BOOL
WINAPI
InDiskAdjustImageFileSize(IN HANDLE FileHandle,
			  IN PLARGE_INTEGER FileSize);

/**
   This function converts a native NT-style path to a Win32 DOS-style path. The
   path string is converted in-place and the start address is adjusted to skip
   over native directories such as \??\. Because of this, the Path parameter is
   a pointer to a pointer to a string so that the pointer can be adjusted to
   the new start address.

   Path            Pointer to pointer to Path string in native NT-style format.
                   Upon return the pointed address will contain the start
		   address of the Win32 DOS-style path within the original
		   buffer.
*/
VOID
WINAPI
InDiskNativePathToWin32(IN OUT LPWSTR *Path);

/**
   This function saves the contents of a device to an image file. This is a
   user-interactive function that displays dialog boxes where user can select
   image file and other options.

   DeviceHandle    Handle to a device for which the contents are to be saved to
                   an image file.

		   The handle must be opened for reading, may be
		   opened for sequential scan and/or without intermediate
		   buffering but cannot be opened for overlapped operation.
		   Please note that a call to this function will turn on
		   FSCTL_ALLOW_EXTENDED_DASD_IO on for this handle.

   WindowHandle    Handle to existing window that will be parent to dialog
                   boxes etc.

   BufferSize      I/O buffer size to use when reading source disk. This
                   parameter is optional, if it is zero the buffer size to use
                   will automatically choosen.

   IsCdRomType     If this parameter is TRUE and the source device type cannot
                   be automatically determined this function will ask user for
		   a .iso suffixed image file name.
*/
VOID
WINAPI
InDiskSaveImageFileInteractive(IN HANDLE DeviceHandle,
			       IN HWND WindowHandle OPTIONAL,
			       IN DWORD BufferSize OPTIONAL,
			       IN BOOL IsCdRomType OPTIONAL);

#ifdef __cplusplus
}
#endif

#endif

#endif // _INC_INDISK_