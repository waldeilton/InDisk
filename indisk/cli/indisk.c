/*
    Control program for the InDisk Virtual Disk Driver for Windows NT/2000/XP.

    Copyright (C) 2004-2011 Olof Lagerkvist.

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

#include <windows.h>
#include <winioctl.h>
#include <shellapi.h>
#include <dbt.h>

#include <stdio.h>
#include <stdlib.h>

#include "..\inc\ntumapi.h"
#include "..\inc\indisk.h"
#include "..\inc\indproxy.h"

enum
  {
    INDISK_CLI_ERROR_DEVICE_NOT_FOUND = 1,
    INDISK_CLI_ERROR_DEVICE_INACCESSIBLE = 2,
    INDISK_CLI_ERROR_CREATE_DEVICE = 3,
    INDISK_CLI_ERROR_DRIVER_NOT_INSTALLED = 4,
    INDISK_CLI_ERROR_DRIVER_WRONG_VERSION = 5,
    INDISK_CLI_ERROR_DRIVER_INACCESSIBLE = 6,
    INDISK_CLI_ERROR_SERVICE_INACCESSIBLE = 7,
    INDISK_CLI_ERROR_FORMAT = 8,
    INDISK_CLI_ERROR_BAD_MOUNT_POINT = 9,
    INDISK_CLI_ERROR_BAD_SYNTAX = 10,
    INDISK_CLI_ERROR_NOT_ENOUGH_MEMORY = 11,
    INDISK_CLI_ERROR_PARTITION_NOT_FOUND = 12,
    INDISK_CLI_ERROR_WRONG_SYNTAX = 13,
    INDISK_CLI_ERROR_FATAL = -1
  };

//#define DbgOemPrintF(x) InDiskOemPrintF x
#define DbgOemPrintF(x)

/// Macros for "human readable" file sizes.
#define _1KB  (1ui64<<10)
#define _1MB  (1ui64<<20)
#define _1GB  (1ui64<<30)
#define _1TB  (1ui64<<40)

#define _B(n)  ((double)(n))
#define _KB(n) ((double)(n)/_1KB)
#define _MB(n) ((double)(n)/_1MB)
#define _GB(n) ((double)(n)/_1GB)
#define _TB(n) ((double)(n)/_1TB)

#define _h(n) ((n)>=_1TB ? _TB(n) : (n)>=_1GB ? _GB(n) :	\
	       (n)>=_1MB ? _MB(n) : (n)>=_1KB ? _KB(n) : (n))
#define _p(n) ((n)>=_1TB ? "TB" : (n)>=_1GB ? "GB" :			\
	       (n)>=_1MB ? "MB" : (n)>=_1KB ? "KB": (n)==1 ? "byte" : "bytes")

void __declspec(noreturn)
InDiskSyntaxHelp()
{
  int rc = fputs
    ("Control program for the InDisk Virtual Disk Driver.\r\n"
     "For copyrights and credits, type indisk --version\r\n"
     "\n"
     "Syntax:\r\n"
     "indisk -a -t type -m mountpoint [-n] [-o opt1[,opt2 ...]] [-f|-F file]\r\n"
     "       [-s size] [-b offset] [-v partition] [-S sectorsize] [-u unit]\r\n"
     "       [-x sectors/track] [-y tracks/cylinder] [-p \"format-parameters\"]\r\n"
     "indisk -d|-D [-u unit | -m mountpoint]\r\n"
     "indisk -R -u unit\r\n"
     "indisk -l [-u unit | -m mountpoint]\r\n"
     "indisk -e [-s size] [-o opt1[,opt2 ...]] [-u unit | -m mountpoint]\r\n"
     "\n"
     "-a      Attach a virtual disk. This will configure and attach a virtual disk\r\n"
     "        with the parameters specified and attach it to the system.\r\n"
     "\n"
     "-d      Detach a virtual disk from the system and release all resources.\r\n"
     "        Use -D to force removal even if the device is in use.\r\n"
     "\n"
     "-R      Emergency removal of hung virtual disks. Should only be used as a last\r\n"
     "        resort when a virtual disk has some kind of problem that makes it\r\n"
     "        impossible to detach it in a safe way. This could happen for example\r\n"
     "        for proxy-type virtual disks sometimes when proxy communication fails.\r\n"
     "        Note that this does not attempt to dismount filesystem or lock the\r\n"
     "        volume in any way so there is a potential risk of data loss. Use with\r\n"
     "        caution!\r\n"
     "\n"
     "-e      Edit an existing virtual disk.\r\n"
     "\n"
     "        Along with the -s parameter extends the size of an existing virtual\r\n"
     "        disk. Note that even if the disk can be extended successfully, the\r\n"
     "        existing filesystem on it can only be extended to fill the new size\r\n"
     "        without re-formatting if you are running Windows 2000 or later and the\r\n"
     "        current filesystem is NTFS.\r\n"
     "\n"
     "        Along with the -o parameter changes media characteristics for an\r\n"
     "        existing virtual disk. Options that can be changed on existing virtual\r\n"
     "        disks are those specifying wether or not the media of the virtual disk\r\n"
     "        should be writable and/or removable.\r\n"
     "\n"
     "-t type\r\n"
     "        Select the backingstore for the virtual disk.\r\n"
     "\n"
     "vm      Storage for this type of virtual disk is allocated from virtual memory\r\n"
     "        in the system process. If a file is specified with -f that file is\r\n"
     "        is loaded into the memory allocated for the disk image.\r\n"
     "\n"
     "file    A file specified with -f file becomes the backingstore for this\r\n"
     "        virtual disk.\r\n"
     "\n"
     "proxy   The actual backingstore for this type of virtual disk is controlled by\r\n"
     "        an InDisk storage server accessed by the driver on this machine by\r\n"
     "        sending storage I/O request through a named pipe specified with -f.\r\n"
     "\n"
     "-f file or -F file\r\n"
     "        Filename to use as backingstore for the file type virtual disk, to\r\n"
     "        initialize a vm type virtual disk or name of a named pipe for I/O\r\n"
     "        client/server communication for proxy type virtual disks. For proxy\r\n"
     "        type virtual disks \"file\" may be a COM port or a remote server\r\n"
     "        address if the -o options includes \"ip\" or \"comm\".\r\n"
     "\n"
     "        Instead of using -f to specify 'DOS-style' paths, such as\r\n"
     "        C:\\dir\\image.bin or \\\\server\\share\\image.bin, you can use -F to\r\n"
     "        specify 'NT-style' native paths, such as\r\n"
     "        \\Device\\Harddisk0\\Partition1\\image.bin. This makes it possible to\r\n"
     "        specify files on disks or communication devices that currently have no\r\n"
     "        drive letters assigned.\r\n"
     "\n"
     "-l      List configured devices. If given with -u or -m, display details about\r\n"
     "        that particular device.\r\n"
     "\n"
     "-n      When printing InDisk device names, print only the unit number without\r\n"
     "        the \\Device\\InDisk prefix.\r\n"
     "\n"
     "-s size\r\n"
     "        Size of the virtual disk. Size is number of bytes unless suffixed with\r\n"
     "        a b, k, m, g, t, K, M, G or T which denotes number of 512-byte blocks,\r\n"
     "        thousand bytes, million bytes, billion bytes, trillion bytes,\r\n"
     "        kilobytes, megabytes, gigabytes and terabytes respectively. The suffix\r\n"
     "        can also be % to indicate percentage of free physical memory which\r\n"
     "        could be useful when creating vm type virtual disks. It is optional to\r\n"
     "        specify a size unless the file to use for a file type virtual disk does\r\n"
     "        not already exist or when a vm type virtual disk is created without\r\n"
     "        specifying an initialization image file using the -f or -F. If size is\r\n"
     "        specified when creating a file type virtual disk, the size of the file\r\n"
     "        used as backingstore for the virtual disk is adjusted to the new size\r\n"
     "        specified with this size option.\r\n"
     "\n"
     "        The size can be a negative value to indicate the size of free physical\r\n"
     "        memory minus this size. If you e.g. type -400M the size of the virtual\r\n"
     "        disk will be the amount of free physical memory minus 400 MB.\r\n"
     "\n"
     "-b offset\r\n"
     "        Specifies an offset in an image file where the virtual disk begins. All\r\n"
     "        offsets of I/O operations on the virtual disk will be relative to this\r\n"
     "        offset. This parameter is particularily useful when mounting a specific\r\n"
     "        partition in an image file that contains an image of a complete hard\r\n"
     "        disk, not just one partition. This parameter has no effect when\r\n"
     "        creating a blank vm type virtual disk. When creating a vm type virtual\r\n"
     "        disk with a pre-load image file specified with -f or -F paramters, the\r\n"
     "        -b parameter specifies an offset in the image file where the image to\r\n"
     "        be loaded into the vm type virtual disk begins.\r\n"
     "\n"
     "        Specify auto as offset to automatically select offset for a few known\r\n"
     "        non-raw disk image file formats. Currently auto-selection is supported\r\n"
     "        for Nero .nrg and Microsoft .sdi image files.\r\n"
     "\n"
     "-v partition\r\n"
     "        Specifies which partition to mount when mounting a raw hard disk image\r\n"
     "        file containing a master boot record and partitions.\r\n"
     "\n"
     "        Specify number 1-4 to mount a partition from the primary partition\r\n"
     "        table and 5-8 to mount a partition from an extended partition table.\r\n"
     "\n"
     "-S sectorsize\r\n"
     "        Sectorsize to use for the virtual disk device. Default value is 512\r\n"
     "        bytes except for CD-ROM/DVD-ROM style devices where 2048 bytes is used\r\n"
     "        by default.\r\n"
     "\n"
     "-x sectors/track\r\n"
     "        See the description of the -y option below.\r\n"
     "\n"
     "-y tracks/cylinder\r\n"
     "        The -x and -y options can be used to specify a synthetic geometry.\r\n"
     "        This is useful for constructing bootable images for later download to\r\n"
     "        physical devices. Default values depends on the device-type specified\r\n"
     "        with the -o option. If the 'fd' option is specified the default values\r\n"
     "        are based on the virtual disk size, e.g. a 1440K image gets 2\r\n"
     "        tracks/cylinder and 18 sectors/track.\r\n"
     "\n"
     "-p \"format-parameters\"\r\n"
     "        If -p is specified the 'format' command is invoked to create a\r\n"
     "        filesystem when the new virtual disk has been created.\r\n"
     "        \"format-parameters\" must be a parameter string enclosed within\r\n"
     "        double-quotes. The string is added to the command line that starts\r\n"
     "        'format'. You usually specify something like \"/fs:ntfs /q /y\", that\r\n"
     "        is, create an NTFS filesystem with quick formatting and without user\r\n"
     "        interaction.\r\n"
     "\n"
     "-o option\r\n"
     "        Set or reset options.\r\n"
     "\n"
     "ro      Creates a read-only virtual disk. For vm type virtual disks, this\r\n"
     "        option can only be used if the -f option is also specified.\r\n"
     "\n"
     "rw      Specifies that the virtual disk should be read/writable. This is the\r\n"
     "        default setting. It can be used with the -e parameter to set an\r\n"
     "        existing read-only virtual disk writable.\r\n"
     "\n"
     "rem     Specifies that the device should be created with removable media\r\n"
     "        characteristics. This changes the device properties returned by the\r\n"
     "        driver to the system. For example, this changes how some filesystems\r\n"
     "        cache write operations.\r\n"
     "\n"
     "fix     Specifies that the media characteristics of the virtual disk should be\r\n"
     "        fixed media, as opposed to removable media specified with the rem\r\n"
     "        option. Fixed media is the default setting. The fix option can be used\r\n"
     "        with the -e parameter to set an existing removable virtual disk as\r\n"
     "        fixed.\r\n"
     "\n"
     "saved   Clears the 'image modified' flag from an existing virtual disk. This\r\n"
     "        flag is set by the driver when an image is modified and is displayed\r\n"
     "        in the -l output for a virtual disk. The 'saved' option is only valid\r\n"
     "        with the -e parameter.\r\n"
     "\n"
     "        Note that virtual floppy or CD/DVD-ROM drives are always read-only and\r\n"
     "        removable devices and that cannot be changed.\r\n"
     "\n"
     "cd      Creates a virtual CD-ROM/DVD-ROM. This is the default if the file\r\n"
     "        name specified with the -f option ends with either .iso, .nrg or .bin\r\n"
     "        extensions.\r\n"
     "\n"
     "fd      Creates a virtual floppy disk. This is the default if the size of the\r\n"
     "        virtual disk is any of 160K, 180K, 320K, 360K, 640K, 720K, 820K, 1200K,\r\n"
     "        1440K, 1680K, 1722K, 2880K, 123264K or 234752K.\r\n"
     "\n"
     "hd      Creates a virtual fixed disk partition. This is the default unless\r\n"
     "        file extension or size match the criterias for defaulting to the cd or\r\n"
     "        fd options.\r\n"
     "\n"
     "ip      Can only be used with proxy-type virtual disks. With this option, the\r\n"
     "        user-mode service component is initialized to connect to an InDisk\r\n"
     "        storage server using TCP/IP. With this option, the -f switch specifies\r\n"
     "        the remote host optionally followed by a colon and a port number to\r\n"
     "        connect to.\r\n"
     "\n"
     "comm    Can only be used with proxy-type virtual disks. With this option, the\r\n"
     "        user-mode service component is initialized to connect to an InDisk\r\n"
     "        storage server through a COM port. With this option, the -f switch\r\n"
     "        specifies the COM port to connect to, optionally followed by a colon,\r\n"
     "        a space, and then a device settings string with the same syntax as the\r\n"
     "        MODE command.\r\n"
     "\n"
     "shm     Can only be used with proxy-type virtual disks. With this option, the\r\n"
     "        driver communicates with a storage server on the same computer using\r\n"
     "        shared memory block to transfer I/O data.\r\n"
     "\n"
     "-u unit\r\n"
     "        Along with -a, request a specific unit number for the InDisk device\r\n"
     "        instead of automatic allocation. Along with -d or -l specifies the\r\n"
     "        unit number of the virtual disk to remove or query.\r\n"
     "\n"
     "-m mountpoint\r\n"
     "        Specifies a drive letter or mount point for the new virtual disk, the\r\n"
     "        virtual disk to query or the virtual disk to remove. When creating a\r\n"
     "        new virtual disk you can specify #: as mountpoint in which case the\r\n"
     "        first unused drive letter is automatically used.\r\n",
     stderr);

  if (rc > 0)
    exit(INDISK_CLI_ERROR_WRONG_SYNTAX);
  else
    exit(INDISK_CLI_ERROR_FATAL);
}

BOOL
InDiskOemPrintF(FILE *Stream, LPCSTR Message, ...)
{
  va_list param_list;
  LPSTR lpBuf = NULL;

  va_start(param_list, Message);

  if (!FormatMessageA(78 |
		      FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_STRING, Message, 0, 0,
		      (LPSTR) &lpBuf, 0, &param_list))
    return FALSE;

  CharToOemA(lpBuf, lpBuf);
  fprintf(Stream, "%s\n", lpBuf);
  LocalFree(lpBuf);
  return TRUE;
}

void
PrintLastError(LPCWSTR Prefix)
{
  LPSTR MsgBuf;

  if (!FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK |
		      FORMAT_MESSAGE_ALLOCATE_BUFFER |
		      FORMAT_MESSAGE_FROM_SYSTEM |
		      FORMAT_MESSAGE_IGNORE_INSERTS,
		      NULL, GetLastError(), 0, (LPSTR) &MsgBuf, 0, NULL))
    MsgBuf = NULL;

  InDiskOemPrintF(stderr, "%1!ws! %2", Prefix, MsgBuf);

  if (MsgBuf != NULL)
    LocalFree(MsgBuf);
}

BOOL
InDiskCliCheckDriverVersion(HANDLE Device)
{
  DWORD VersionCheck;
  DWORD BytesReturned;

  if (!DeviceIoControl(Device,
                       IOCTL_INDISK_QUERY_VERSION,
                       NULL, 0,
		       &VersionCheck, sizeof VersionCheck,
                       &BytesReturned, NULL))
    switch (GetLastError())
      {
      case ERROR_INVALID_FUNCTION:
      case ERROR_NOT_SUPPORTED:
	fputs("Error: Not an InDisk device.\r\n", stderr);
	return FALSE;

      default:
	PrintLastError(L"Error opening device:");
	return FALSE;
      }

  if (BytesReturned < sizeof VersionCheck)
    {
      fprintf(stderr,
	      "Wrong version of InDisk Virtual Disk Driver.\n"
	      "No current driver version information, expected: %u.%u.\n"
	      "Please reinstall InDisk and reboot if this issue persists.\n",
	      HIBYTE(INDISK_DRIVER_VERSION), LOBYTE(INDISK_DRIVER_VERSION),
	      BytesReturned);
      return FALSE;
    }

  if (VersionCheck != INDISK_DRIVER_VERSION)
    {
      fprintf(stderr,
	      "Wrong version of InDisk Virtual Disk Driver.\n"
	      "Expected: %u.%u Installed: %u.%u\n"
	      "Please re-install InDisk and reboot if this issue persists.\n",
	      HIBYTE(INDISK_DRIVER_VERSION), LOBYTE(INDISK_DRIVER_VERSION),
	      HIBYTE(VersionCheck), LOBYTE(VersionCheck));
      return FALSE;
    }

  return TRUE;
}

int
InDiskCliCreateDevice(LPDWORD DeviceNumber,
		      PDISK_GEOMETRY DiskGeometry,
		      PLARGE_INTEGER ImageOffset,
		      DWORD Flags,
		      LPCWSTR FileName,
		      BOOL NativePath,
		      LPWSTR MountPoint)
{
  PINDISK_CREATE_DATA create_data;
  HANDLE driver;
  UNICODE_STRING file_name;
  DWORD dw;

  RtlInitUnicodeString(&file_name, INDISK_CTL_DEVICE_NAME);

  for (;;)
    {
      driver = InDiskOpenDeviceByName(&file_name,
				      GENERIC_READ | GENERIC_WRITE);

      if (driver != INVALID_HANDLE_VALUE)
	break;

      if (GetLastError() != ERROR_FILE_NOT_FOUND)
	{
	  PrintLastError(L"Error controlling the InDisk Virtual Disk Driver:");
	  return INDISK_CLI_ERROR_DRIVER_INACCESSIBLE;
	}

      if (!InDiskStartService(INDISK_DRIVER_NAME))
	switch (GetLastError())
	  {
	  case ERROR_SERVICE_DOES_NOT_EXIST:
	    fputs("The InDisk Virtual Disk Driver is not installed. "
		  "Please re-install InDisk.\r\n", stderr);
	    return INDISK_CLI_ERROR_DRIVER_NOT_INSTALLED;

	  case ERROR_PATH_NOT_FOUND:
	  case ERROR_FILE_NOT_FOUND:
	    fputs("Cannot load indisk.sys. "
		  "Please re-install InDisk.\r\n", stderr);
	    return INDISK_CLI_ERROR_DRIVER_NOT_INSTALLED;

	  case ERROR_SERVICE_DISABLED:
	    fputs("The InDisk Virtual Disk Driver is disabled.\r\n", stderr);
	    return INDISK_CLI_ERROR_DRIVER_NOT_INSTALLED;

	  default:
	    PrintLastError(L"Error loading InDisk Virtual Disk Driver:");
	    return INDISK_CLI_ERROR_DRIVER_NOT_INSTALLED;
	  }

      Sleep(0);
      puts("The InDisk Virtual Disk Driver was loaded into the kernel.");
    }

  if (!InDiskCliCheckDriverVersion(driver))
    {
      CloseHandle(driver);
      return INDISK_CLI_ERROR_DRIVER_WRONG_VERSION;
    }

  // Proxy reconnection types requires the user mode service.
  if ((INDISK_TYPE(Flags) == INDISK_TYPE_PROXY) &
      ((INDISK_PROXY_TYPE(Flags) == INDISK_PROXY_TYPE_TCP) |
       (INDISK_PROXY_TYPE(Flags) == INDISK_PROXY_TYPE_COMM)))
    {
      if (!WaitNamedPipe(INDPROXY_SVC_PIPE_DOSDEV_NAME, 0))
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
	  if (InDiskStartService(INDPROXY_SVC))
	    {
	      while (!WaitNamedPipe(INDPROXY_SVC_PIPE_DOSDEV_NAME, 0))
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		  Sleep(200);
		else
		  break;

	      puts
		("The InDisk Virtual Disk Driver Helper Service was started.");
	    }
	  else
	    {
	      switch (GetLastError())
		{
		case ERROR_SERVICE_DOES_NOT_EXIST:
		  fputs("The InDisk Virtual Disk Driver Helper Service is not "
			"installed.\r\n"
			"Please re-install InDisk.\r\n", stderr);
		  break;

		case ERROR_PATH_NOT_FOUND:
		case ERROR_FILE_NOT_FOUND:
		  fputs("Cannot start InDisk Virtual Disk Driver Helper "
			"Service.\r\n"
			"Please re-install InDisk.\r\n", stderr);
		  break;

		case ERROR_SERVICE_DISABLED:
		  fputs("The InDisk Virtual Disk Driver Helper Service is "
			"disabled.\r\n", stderr);
		  break;

		default:
		  PrintLastError
		    (L"Error starting InDisk Virtual Disk Driver Helper "
		     L"Service:");
		}

	      CloseHandle(driver);
	      return INDISK_CLI_ERROR_SERVICE_INACCESSIBLE;
	    }
    }

  if (FileName == NULL)
    RtlInitUnicodeString(&file_name, NULL);
  else if (NativePath)
    {
      if (!RtlCreateUnicodeString(&file_name, FileName))
	{
	  CloseHandle(driver);
	  fputs("Memory allocation error.\r\n", stderr);
	  return INDISK_CLI_ERROR_FATAL;
	}
    }
  else if ((INDISK_TYPE(Flags) == INDISK_TYPE_PROXY) &
	   (INDISK_PROXY_TYPE(Flags) == INDISK_PROXY_TYPE_SHM))
    {
      LPWSTR namespace_prefix;
      LPWSTR prefixed_name;
      HANDLE h = CreateFile(L"\\\\?\\Global", 0, FILE_SHARE_READ, NULL,
			    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if ((h == INVALID_HANDLE_VALUE) &
	  (GetLastError() == ERROR_FILE_NOT_FOUND))
	namespace_prefix = L"\\BaseNamedObjects\\";
      else
	namespace_prefix = L"\\BaseNamedObjects\\Global\\";

      if (h != INVALID_HANDLE_VALUE)
	CloseHandle(h);

      prefixed_name = (LPWSTR)
	_alloca(((wcslen(namespace_prefix) + wcslen(FileName)) << 1) + 1);

      if (prefixed_name == NULL)
	{
	  CloseHandle(driver);
	  fputs("Memory allocation error.\r\n", stderr);
	  return INDISK_CLI_ERROR_FATAL;
	}

      wcscpy(prefixed_name, namespace_prefix);
      wcscat(prefixed_name, FileName);

      if (!RtlCreateUnicodeString(&file_name, prefixed_name))
	{
	  CloseHandle(driver);
	  fputs("Memory allocation error.\r\n", stderr);
	  return INDISK_CLI_ERROR_FATAL;
	}
    }
  else
    {
      if (!RtlDosPathNameToNtPathName_U(FileName, &file_name, NULL, NULL))
	{
	  CloseHandle(driver);
	  fputs("Memory allocation error.\r\n", stderr);
	  return INDISK_CLI_ERROR_FATAL;
	}
    }

  create_data = _alloca(sizeof(INDISK_CREATE_DATA) + file_name.Length);
  if (create_data == NULL)
    {
      perror("Memory allocation error");
      CloseHandle(driver);
      RtlFreeUnicodeString(&file_name);
      return INDISK_CLI_ERROR_FATAL;
    }

  ZeroMemory(create_data, sizeof(INDISK_CREATE_DATA) + file_name.Length);

  puts("Creating device...");

  // Check if mount point is a drive letter or junction point
  if (MountPoint != NULL)
    if ((wcslen(MountPoint) == 2) ? MountPoint[1] == ':' : 
	(wcslen(MountPoint) == 3) ? wcscmp(MountPoint + 1, L":\\") == 0 :
	FALSE)
      create_data->DriveLetter = MountPoint[0];

  create_data->DeviceNumber   = *DeviceNumber;
  create_data->DiskGeometry   = *DiskGeometry;
  create_data->ImageOffset    = *ImageOffset;
  create_data->Flags          = Flags;
  create_data->FileNameLength = file_name.Length;

  if (file_name.Length != 0)
    {
      memcpy(&create_data->FileName, file_name.Buffer, file_name.Length);
      RtlFreeUnicodeString(&file_name);
    }

  if (!DeviceIoControl(driver,
                       IOCTL_INDISK_CREATE_DEVICE,
                       create_data,
		       sizeof(INDISK_CREATE_DATA) +
		       create_data->FileNameLength,
                       create_data,
		       sizeof(INDISK_CREATE_DATA) +
		       create_data->FileNameLength,
                       &dw,
		       NULL))
    {
      PrintLastError(L"Error creating virtual disk:");
      CloseHandle(driver);
      return INDISK_CLI_ERROR_CREATE_DEVICE;
    }

  CloseHandle(driver);

  *DeviceNumber = create_data->DeviceNumber;

  if (MountPoint != NULL)
    {
      WCHAR device_path[MAX_PATH];

      // Build device path, e.g. \Device\InDisk2
      _snwprintf(device_path, sizeof(device_path) / sizeof(*device_path) - 1,
		 INDISK_DEVICE_BASE_NAME L"%u", create_data->DeviceNumber);
      device_path[sizeof(device_path)/sizeof(*device_path) - 1] = 0;

      if (create_data->DriveLetter == 0)
	{
	  if (!InDiskCreateMountPoint(MountPoint, device_path))
	    {
	      switch (GetLastError())
		{
		case ERROR_INVALID_REPARSE_DATA:
		  InDiskOemPrintF(stderr,
				  "Invalid mount point path: '%1!ws!'\n",
				  MountPoint);
		  break;

		case ERROR_INVALID_PARAMETER:
		  fputs("This version of Windows only supports drive letters "
			"as mount points.\r\n"
			"Windows 2000 or higher is required to support "
			"subdirectory mount points.\r\n",
			stderr);
		  break;

		case ERROR_INVALID_FUNCTION:
		case ERROR_NOT_A_REPARSE_POINT:
		  fputs("Mount points are only supported on NTFS volumes.\r\n",
			stderr);
		  break;

		case ERROR_DIRECTORY:
		case ERROR_DIR_NOT_EMPTY:
		  fputs("Mount points can only created on empty "
			"directories.\r\n", stderr);
		  break;

		default:
		  PrintLastError(L"Error creating mount point:");
		}

	      fputs
		("Warning: The device is created without a mount point.\r\n",
		 stderr);

	      MountPoint[0] = 0;
	    }
	}
      else
	if (!DefineDosDevice(DDD_RAW_TARGET_PATH, MountPoint, device_path))
	  PrintLastError(L"Error creating mount point:");
    }

  return 0;
}

int
InDiskCliFormatDisk(LPWSTR MountPoint, LPWSTR FormatOptions)
{
  WCHAR format_cmd_prefix[] = L"format.com ";

  LPWSTR format_cmd = (LPWSTR) _alloca(sizeof(format_cmd_prefix) +
				       ((wcslen(MountPoint) + 1) << 1) +
				       (wcslen(FormatOptions) << 1));

  STARTUPINFO startup_info = { sizeof(startup_info) };
  PROCESS_INFORMATION process_info;

  puts("Formatting disk...");

  wcscpy(format_cmd, format_cmd_prefix);
  wcscat(format_cmd, MountPoint);
  wcscat(format_cmd, L" ");
  wcscat(format_cmd, FormatOptions);

  if (!CreateProcess(NULL, format_cmd, NULL, NULL, TRUE, 0, NULL, NULL,
		     &startup_info, &process_info))
    {
      PrintLastError(L"Cannot format drive:");
      return INDISK_CLI_ERROR_FORMAT;
    }

  CloseHandle(process_info.hThread);
  WaitForSingleObject(process_info.hProcess, INFINITE);
  CloseHandle(process_info.hProcess);

  return 0;
}

int
InDiskCliRemoveDevice(DWORD DeviceNumber,
		      LPCWSTR MountPoint,
		      BOOL ForceDismount,
		      BOOL EmergencyRemove)
{
  WCHAR drive_letter_mount_point[] = L" :";
  DWORD dw;

  if (EmergencyRemove)
    {
      puts("Emergency removal...");

      if (!InDiskForceRemoveDevice(NULL, DeviceNumber))
	{
	  PrintLastError(MountPoint == NULL ? L"Error" : MountPoint);
	  return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	}
    }
  else
    {
      PINDISK_CREATE_DATA create_data = (PINDISK_CREATE_DATA)
	_alloca(sizeof(INDISK_CREATE_DATA) + (MAX_PATH << 2));
      HANDLE device;

      if (MountPoint == NULL)
	{
	  device = InDiskOpenDeviceByNumber(DeviceNumber,
					    GENERIC_READ | GENERIC_WRITE);

	  if (device == INVALID_HANDLE_VALUE)
	    device = InDiskOpenDeviceByNumber(DeviceNumber,
					      GENERIC_READ);

	  if (device == INVALID_HANDLE_VALUE)
	    device = InDiskOpenDeviceByNumber(DeviceNumber,
					      FILE_READ_ATTRIBUTES);
	}
      else if ((wcslen(MountPoint) == 2) ? MountPoint[1] == ':' : 
	       (wcslen(MountPoint) == 3) ? wcscmp(MountPoint + 1, L":\\") == 0
	       : FALSE)
	{
	  WCHAR drive_letter_path[] = L"\\\\.\\ :";
	  drive_letter_path[4] = MountPoint[0];

	  // Notify processes that this device is about to be removed.
	  if ((MountPoint[0] >= L'A') & (MountPoint[0] <= L'Z'))
	    {
	      DEV_BROADCAST_VOLUME dev_broadcast_volume = {
		sizeof(DEV_BROADCAST_VOLUME),
		DBT_DEVTYP_VOLUME
	      };
	      DWORD_PTR dwp;

	      puts("Notifying applications...");

	      dev_broadcast_volume.dbcv_unitmask = 1 << (MountPoint[0] - L'A');

	      DbgOemPrintF((stdout, "Sending DBT_DEVICEQUERYREMOVE...\n"));

	      SendMessageTimeout(HWND_BROADCAST,
				 WM_DEVICECHANGE,
				 DBT_DEVICEQUERYREMOVE,
				 (LPARAM)&dev_broadcast_volume,
				 SMTO_BLOCK | SMTO_ABORTIFHUNG,
				 4000,
				 &dwp);

	      DbgOemPrintF((stdout, "Sending DBT_DEVICEREMOVEPENDING...\n"));

	      SendMessageTimeout(HWND_BROADCAST,
				 WM_DEVICECHANGE,
				 DBT_DEVICEREMOVEPENDING,
				 (LPARAM)&dev_broadcast_volume,
				 SMTO_BLOCK | SMTO_ABORTIFHUNG,
				 4000,
				 &dwp);
	    }

	  DbgOemPrintF((stdout, "Opening %1!ws!...\n", MountPoint));

	  device = CreateFile(drive_letter_path,
			      GENERIC_READ | GENERIC_WRITE,
			      FILE_SHARE_READ | FILE_SHARE_WRITE,
			      NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING,
			      NULL);

	  if (device == INVALID_HANDLE_VALUE)
	    device = CreateFile(drive_letter_path,
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING,
				NULL);

	  if (device == INVALID_HANDLE_VALUE)
	    device = CreateFile(drive_letter_path,
				FILE_READ_ATTRIBUTES,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING,
				NULL);
	}
      else
	{
	  device = InDiskOpenDeviceByMountPoint(MountPoint,
						GENERIC_READ | GENERIC_WRITE);

	  if (device == INVALID_HANDLE_VALUE)
	    device = InDiskOpenDeviceByMountPoint(MountPoint,
						  GENERIC_READ);

	  if (device == INVALID_HANDLE_VALUE)
	    device = InDiskOpenDeviceByMountPoint(MountPoint,
						  FILE_READ_ATTRIBUTES);

	  if (device == INVALID_HANDLE_VALUE)
	    switch (GetLastError())
	      {
	      case ERROR_INVALID_PARAMETER:
		fputs("This version of Windows only supports drive letters as "
		      "mount points.\r\n"
		      "Windows 2000 or higher is required to support "
		      "subdirectory mount points.\r\n",
		      stderr);
		return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	      case ERROR_INVALID_FUNCTION:
		fputs("Mount points are only supported on NTFS volumes.\r\n",
		      stderr);
		return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	      case ERROR_NOT_A_REPARSE_POINT:
	      case ERROR_DIRECTORY:
	      case ERROR_DIR_NOT_EMPTY:
		InDiskOemPrintF(stderr, "Not a mount point: '%1!ws!'\n",
				MountPoint);
		return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	      default:
		PrintLastError(MountPoint);
		return INDISK_CLI_ERROR_BAD_MOUNT_POINT;
	      }
	}

      if (device == INVALID_HANDLE_VALUE)
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
	  {
	    fputs("No such device.\r\n", stderr);
	    return INDISK_CLI_ERROR_DEVICE_NOT_FOUND;
	  }
	else
	  {
	    PrintLastError(L"Error opening device:");
	    return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	  }

      if (!InDiskCliCheckDriverVersion(device))
	{
	  CloseHandle(device);
	  return INDISK_CLI_ERROR_DRIVER_WRONG_VERSION;
	}

      if (!DeviceIoControl(device,
			   IOCTL_INDISK_QUERY_DEVICE,
			   NULL,
			   0,
			   create_data,
			   sizeof(INDISK_CREATE_DATA) + (MAX_PATH << 2),
			   &dw, NULL))
	{
	  PrintLastError(MountPoint);
	  InDiskOemPrintF(stderr,
			  "%1!ws!: Is that drive really an InDisk drive?",
			  MountPoint);
	  return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	}

      if (dw < sizeof(INDISK_CREATE_DATA) - sizeof(*create_data->FileName))
	{
	  InDiskOemPrintF(stderr,
			  "%1!ws!: Is that drive really an InDisk drive?",
			  MountPoint);
	  return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	}

      if ((MountPoint == NULL) & (create_data->DriveLetter != 0))
	{
	  drive_letter_mount_point[0] = create_data->DriveLetter;
	  MountPoint = drive_letter_mount_point;
	}

      puts("Flushing file buffers...");

      FlushFileBuffers(device);

      puts("Locking volume...");

      if (!DeviceIoControl(device,
			   FSCTL_LOCK_VOLUME,
			   NULL,
			   0,
			   NULL,
			   0,
			   &dw,
			   NULL))
	if (ForceDismount)
	  {
	    puts("Failed, forcing dismount...");
	
	    DeviceIoControl(device,
			    FSCTL_DISMOUNT_VOLUME,
			    NULL,
			    0,
			    NULL,
			    0,
			    &dw,
			    NULL);
	
	    DeviceIoControl(device,
			    FSCTL_LOCK_VOLUME,
			    NULL,
			    0,
			    NULL,
			    0,
			    &dw,
			    NULL);
	  }
	else
	  {
	    PrintLastError(MountPoint == NULL ? L"Error" : MountPoint);
	    return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	  }
      else
	{
	  puts("Dismounting filesystem...");

	  if (!DeviceIoControl(device,
			       FSCTL_DISMOUNT_VOLUME,
			       NULL,
			       0,
			       NULL,
			       0,
			       &dw,
			       NULL))
	    {
	      PrintLastError(MountPoint == NULL ? L"Error" : MountPoint);
	      return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	    }
	}

      puts("Removing device...");

      if (!DeviceIoControl(device,
			   IOCTL_STORAGE_EJECT_MEDIA,
			   NULL,
			   0,
			   NULL,
			   0,
			   &dw,
			   NULL))
	if (ForceDismount ? !InDiskForceRemoveDevice(device, 0) : FALSE)
	  {
	    PrintLastError(MountPoint == NULL ? L"Error" : MountPoint);
	    return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	  }

      DeviceIoControl(device,
		      FSCTL_UNLOCK_VOLUME,
		      NULL,
		      0,
		      NULL,
		      0,
		      &dw,
		      NULL);

      CloseHandle(device);
    }

  if (MountPoint != NULL)
    {
      puts("Removing mountpoint...");

      if ((wcslen(MountPoint) == 2) ? MountPoint[1] == ':' : 
	  (wcslen(MountPoint) == 3) ? wcscmp(MountPoint + 1, L":\\") == 0 :
	  FALSE)
	{
	  DWORD_PTR dwp;

	  WCHAR drive_mount_point[3] = L" :";
	  WCHAR reg_key[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints\\ ";
	  WCHAR reg_key2[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2\\ ";

	  drive_mount_point[0] = MountPoint[0];

	  if (!DefineDosDevice(DDD_REMOVE_DEFINITION, drive_mount_point, NULL))
	    PrintLastError(MountPoint);

	  reg_key[63] = MountPoint[0];
	  reg_key2[64] = MountPoint[0];

	  RegDeleteKey(HKEY_CURRENT_USER, reg_key);
	  RegDeleteKey(HKEY_CURRENT_USER, reg_key2);

	  DbgOemPrintF((stdout, "Sending DBT_DEVNODES_CHANGED...\n"));

	  SendMessageTimeout(HWND_BROADCAST,
			     WM_DEVICECHANGE,
			     DBT_DEVNODES_CHANGED,
			     0,
			     SMTO_BLOCK | SMTO_ABORTIFHUNG,
			     4000,
			     &dwp);
	}
      else
	{
	  if (!InDiskRemoveMountPoint(MountPoint))
	    switch (GetLastError())
	      {
	      case ERROR_INVALID_PARAMETER:
		fputs("This version of Windows only supports drive letters as "
		      "mount points.\r\n"
		      "Windows 2000 or higher is required to support "
		      "subdirectory mount points.\r\n",
		      stderr);
		break;

	      case ERROR_INVALID_FUNCTION:
		fputs("Mount points are only supported on empty directories "
		      "on NTFS volumes.\r\n",
		      stderr);
		break;

	      case ERROR_NOT_A_REPARSE_POINT:
	      case ERROR_DIRECTORY:
	      case ERROR_DIR_NOT_EMPTY:
		InDiskOemPrintF(stderr,
				"Not a mount point: '%1!ws!'\n", MountPoint);
		break;

	      default:
		PrintLastError(MountPoint);
	      }
	}
    }

  puts("Done.");

  return 0;
}

int
InDiskCliQueryStatusDriver(BOOL NumericPrint)
{
  DWORDLONG device_list = InDiskGetDeviceList();
  DWORD counter;

  if (device_list == 0)
    switch (GetLastError())
      {
      case NO_ERROR:
	puts("No virtual disks.");
	return 0;

      case ERROR_FILE_NOT_FOUND:
	puts("The InDisk Virtual Disk Driver is not loaded.");
	return 0;

      default:
	PrintLastError(L"Cannot control the InDisk Virtual Disk Driver:");
	return -1;
      }

  for (counter = 0; device_list != 0; device_list >>= 1, counter++)
    if (device_list & 1)
      printf("%s%u\n", NumericPrint ? "" : "\\Device\\InDisk", counter);

  return 0;
}

int
InDiskCliQueryStatusDevice(DWORD DeviceNumber, LPWSTR MountPoint)
{
  HANDLE device;
  DWORD dw;
  PINDISK_CREATE_DATA create_data = (PINDISK_CREATE_DATA)
    _alloca(sizeof(INDISK_CREATE_DATA) + (MAX_PATH << 2));
  char message_buffer[MAX_PATH];

  if (create_data == NULL)
    {
      perror("Memory allocation error");
      return INDISK_CLI_ERROR_FATAL;
    }

  if (MountPoint == NULL)
    {
      device = InDiskOpenDeviceByNumber(DeviceNumber,
					FILE_READ_ATTRIBUTES);
    }
  else if ((wcslen(MountPoint) == 2) ? MountPoint[1] == ':' : 
	   (wcslen(MountPoint) == 3) ? wcscmp(MountPoint + 1, L":\\") == 0 :
	   FALSE)
    {
      WCHAR drive_letter_path[] = L"\\\\.\\ :";

      drive_letter_path[4] = MountPoint[0];

      DbgOemPrintF((stdout, "Opening %1!ws!...\n", MountPoint));

      device = CreateFile(drive_letter_path,
			  FILE_READ_ATTRIBUTES,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    }
  else
    {
      device = InDiskOpenDeviceByMountPoint(MountPoint,
					    FILE_READ_ATTRIBUTES);
      if (device == INVALID_HANDLE_VALUE)
	{
	  switch (GetLastError())
	    {
	    case ERROR_INVALID_PARAMETER:
	      fputs("This version of Windows only supports drive letters as "
		    "mount points.\r\n"
		    "Windows 2000 or higher is required to support "
		    "subdirectory mount points.\r\n",
		    stderr);
	      break;

	    case ERROR_INVALID_FUNCTION:
	      fputs("Mount points are only supported on empty directories on "
		    "NTFS volumes.\r\n",
		    stderr);
	      break;

	    case ERROR_NOT_A_REPARSE_POINT:
	    case ERROR_DIRECTORY:
	    case ERROR_DIR_NOT_EMPTY:
	      InDiskOemPrintF(stderr,
			      "Not a mount point: '%1!ws!'\n", MountPoint);
	      break;

	    default:
	      PrintLastError(MountPoint);
	    }

	  return INDISK_CLI_ERROR_BAD_MOUNT_POINT;
	}
    }

  if (device == INVALID_HANDLE_VALUE)
    if (GetLastError() == ERROR_FILE_NOT_FOUND)
      {
	fputs("No such device.\r\n", stderr);
	return INDISK_CLI_ERROR_DEVICE_NOT_FOUND;
      }
    else
      {
	PrintLastError(L"Error opening device:");
	return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
      }

  if (!InDiskCliCheckDriverVersion(device))
    {
      CloseHandle(device);
      return INDISK_CLI_ERROR_DRIVER_WRONG_VERSION;
    }

  if (!DeviceIoControl(device,
                       IOCTL_INDISK_QUERY_DEVICE,
                       NULL,
                       0,
                       create_data,
                       sizeof(INDISK_CREATE_DATA) + (MAX_PATH << 2),
                       &dw, NULL))
    {
      PrintLastError(MountPoint);
      InDiskOemPrintF(stderr,
		      "%1!ws!: Is that drive really an InDisk drive?",
		      MountPoint);
      CloseHandle(device);
      return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
    }

  if (dw < sizeof(INDISK_CREATE_DATA) - sizeof(*create_data->FileName))
    {
      InDiskOemPrintF(stderr,
		      "%1!ws!: Is that drive really an InDisk drive?",
		      MountPoint);
      CloseHandle(device);
      return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
    }

  CloseHandle(device);

  _snprintf(message_buffer, sizeof message_buffer,
	    "%wc%ws%s%.*ws\nSize: %I64u bytes (%.4g %s)%s%s%s%s%s.",
	    create_data->DriveLetter == 0 ?
	    L' ' : create_data->DriveLetter,
	    (MountPoint == NULL) | (create_data->DriveLetter != 0) ?
	    L":" : MountPoint,
	    create_data->FileNameLength > 0 ?
	    " = " : "",
	    (int)(create_data->FileNameLength /
		  sizeof(*create_data->FileName)),
	    create_data->FileName,
	    create_data->DiskGeometry.Cylinders.QuadPart,
	    _h(create_data->DiskGeometry.Cylinders.QuadPart),
	    _p(create_data->DiskGeometry.Cylinders.QuadPart),
	    INDISK_READONLY(create_data->Flags) ?
	    ", ReadOnly" : "",
	    INDISK_REMOVABLE(create_data->Flags) ?
	    ", Removable" : "",
	    INDISK_TYPE(create_data->Flags) == INDISK_TYPE_VM ?
	    ", Virtual Memory Disk" :
	    INDISK_TYPE(create_data->Flags) == INDISK_TYPE_PROXY ?
	    ", Proxy Virtual Disk" : ", File Type Virtual Disk",
	    INDISK_DEVICE_TYPE(create_data->Flags) ==
	    INDISK_DEVICE_TYPE_CD ? ", CD-ROM" :
	    INDISK_DEVICE_TYPE(create_data->Flags) ==
	    INDISK_DEVICE_TYPE_FD ? ", Floppy" : ", HDD",
	    create_data->Flags & INDISK_IMAGE_MODIFIED ? ", Modified" : "");

  message_buffer[sizeof(message_buffer)-1] = 0;

  CharToOemA(message_buffer, message_buffer);
  puts(message_buffer);

  return 0;
}

int
InDiskCliChangeFlags(DWORD DeviceNumber, LPCWSTR MountPoint,
		     DWORD FlagsToChange, DWORD Flags)
{
  HANDLE device;
  DWORD dw;
  INDISK_SET_DEVICE_FLAGS device_flags;

  if (MountPoint == NULL)
    {
      device = InDiskOpenDeviceByNumber(DeviceNumber,
					GENERIC_READ | GENERIC_WRITE);
      if (device == INVALID_HANDLE_VALUE)
	device = InDiskOpenDeviceByNumber(DeviceNumber,
					  GENERIC_READ);
    }
  else if ((wcslen(MountPoint) == 2) ? MountPoint[1] == ':' : 
	   (wcslen(MountPoint) == 3) ? wcscmp(MountPoint + 1, L":\\") == 0 :
	   FALSE)
    {
      WCHAR drive_letter_path[] = L"\\\\.\\ :";

      drive_letter_path[4] = MountPoint[0];

      DbgOemPrintF((stdout, "Opening %1!ws!...\n", MountPoint));

      device = CreateFile(drive_letter_path,
			  GENERIC_READ | GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);

      if (device == INVALID_HANDLE_VALUE)
	device = CreateFile(drive_letter_path,
			    GENERIC_READ,
			    FILE_SHARE_READ | FILE_SHARE_WRITE,
			    NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    }
  else
    {
      device = InDiskOpenDeviceByMountPoint(MountPoint,
					    GENERIC_READ | GENERIC_WRITE);
      if (device == INVALID_HANDLE_VALUE)
	device = InDiskOpenDeviceByMountPoint(MountPoint,
					      GENERIC_READ);

      if (device == INVALID_HANDLE_VALUE)
	switch (GetLastError())
	  {
	  case ERROR_INVALID_PARAMETER:
	    fputs("This version of Windows only supports drive letters as "
		  "mount points.\r\n"
		  "Windows 2000 or higher is required to support "
		  "subdirectory mount points.\r\n",
		  stderr);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	  case ERROR_INVALID_FUNCTION:
	    fputs("Mount points are only supported on NTFS volumes.\r\n",
		  stderr);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	  case ERROR_NOT_A_REPARSE_POINT:
	  case ERROR_DIRECTORY:
	  case ERROR_DIR_NOT_EMPTY:
	    InDiskOemPrintF(stderr, "Not a mount point: '%1!ws!'\n",
			    MountPoint);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	  default:
	    PrintLastError(MountPoint);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;
	}
    }

  if (device == INVALID_HANDLE_VALUE)
    if (GetLastError() == ERROR_FILE_NOT_FOUND)
      {
	fputs("No such device.\r\n", stderr);
	return INDISK_CLI_ERROR_DEVICE_NOT_FOUND;
      }
    else
      {
	PrintLastError(L"Error opening device:");
	return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
      }

  if (!InDiskCliCheckDriverVersion(device))
    {
      CloseHandle(device);
      return INDISK_CLI_ERROR_DRIVER_WRONG_VERSION;
    }

  if (FlagsToChange & (INDISK_OPTION_RO | INDISK_OPTION_REMOVABLE))
    {
      puts("Flushing file buffers...");

      FlushFileBuffers(device);

      puts("Locking volume...");

      if (!DeviceIoControl(device,
			   FSCTL_LOCK_VOLUME,
			   NULL,
			   0,
			   NULL,
			   0,
			   &dw,
			   NULL))
	{
	  PrintLastError(MountPoint == NULL ? L"Error" : MountPoint);
	  CloseHandle(device);
	  return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	}

      puts("Dismounting filesystem...");

      if (!DeviceIoControl(device,
			   FSCTL_DISMOUNT_VOLUME,
			   NULL,
			   0,
			   NULL,
			   0,
			   &dw,
			   NULL))
	{
	  PrintLastError(MountPoint == NULL ? L"Error" : MountPoint);
	  CloseHandle(device);
	  return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
	}
    }

  puts("Setting new flags...");

  device_flags.FlagsToChange = FlagsToChange;
  device_flags.FlagValues = Flags;
  if (!DeviceIoControl(device,
                       IOCTL_INDISK_SET_DEVICE_FLAGS,
                       &device_flags,
                       sizeof(device_flags),
                       &device_flags,
                       sizeof(device_flags),
                       &dw, NULL))
    PrintLastError(MountPoint);

  if (device_flags.FlagsToChange != 0)
    {
      CloseHandle(device);
      InDiskOemPrintF(stderr,
		      "%1!ws!: Not all new options were successfully changed.",
		      MountPoint);
      return INDISK_CLI_ERROR_CREATE_DEVICE;
    }
  else
    {
      CloseHandle(device);
      puts("Done.");
      return 0;
    }
}

int
InDiskCliExtendDevice(DWORD DeviceNumber, LPCWSTR MountPoint,
		      LARGE_INTEGER ExtendSize)
{
  HANDLE device;
  DWORD dw;
  DISK_GROW_PARTITION grow_partition = { 0 };
  GET_LENGTH_INFORMATION length_information;
  DISK_GEOMETRY disk_geometry;
  LONGLONG new_filesystem_size;

  if (MountPoint == NULL)
    {
      device = InDiskOpenDeviceByNumber(DeviceNumber,
					GENERIC_READ | GENERIC_WRITE);
      if (device == INVALID_HANDLE_VALUE)
	device = InDiskOpenDeviceByNumber(DeviceNumber,
					  GENERIC_READ);
    }
  else if ((wcslen(MountPoint) == 2) ? MountPoint[1] == ':' : 
	   (wcslen(MountPoint) == 3) ? wcscmp(MountPoint + 1, L":\\") == 0 :
	   FALSE)
    {
      WCHAR drive_letter_path[] = L"\\\\.\\ :";

      drive_letter_path[4] = MountPoint[0];

      DbgOemPrintF((stdout, "Opening %1!ws!...\n", MountPoint));

      device = CreateFile(drive_letter_path,
			  GENERIC_READ | GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);

      if (device == INVALID_HANDLE_VALUE)
	device = CreateFile(drive_letter_path,
			    GENERIC_READ,
			    FILE_SHARE_READ | FILE_SHARE_WRITE,
			    NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    }
  else
    {
      device = InDiskOpenDeviceByMountPoint(MountPoint,
					    GENERIC_READ | GENERIC_WRITE);
      if (device == INVALID_HANDLE_VALUE)
	device = InDiskOpenDeviceByMountPoint(MountPoint,
					      GENERIC_READ);

      if (device == INVALID_HANDLE_VALUE)
	switch (GetLastError())
	  {
	  case ERROR_INVALID_PARAMETER:
	    fputs("This version of Windows only supports drive letters as "
		  "mount points.\r\n"
		  "Windows 2000 or higher is required to support "
		  "subdirectory mount points.\r\n",
		  stderr);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	  case ERROR_INVALID_FUNCTION:
	    fputs("Mount points are only supported on NTFS volumes.\r\n",
		  stderr);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	  case ERROR_NOT_A_REPARSE_POINT:
	  case ERROR_DIRECTORY:
	  case ERROR_DIR_NOT_EMPTY:
	    InDiskOemPrintF(stderr, "Not a mount point: '%1!ws!'\n",
			    MountPoint);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;

	  default:
	    PrintLastError(MountPoint);
	    return INDISK_CLI_ERROR_BAD_MOUNT_POINT;
	}
    }

  if (device == INVALID_HANDLE_VALUE)
    if (GetLastError() == ERROR_FILE_NOT_FOUND)
      {
	fputs("No such device.\r\n", stderr);
	return INDISK_CLI_ERROR_DEVICE_NOT_FOUND;
      }
    else
      {
	PrintLastError(L"Error opening device:");
	return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
      }

  puts("Extending disk size...");

  grow_partition.PartitionNumber = 1;
  grow_partition.BytesToGrow = ExtendSize;
  if (!DeviceIoControl(device,
                       IOCTL_DISK_GROW_PARTITION,
                       &grow_partition,
                       sizeof(grow_partition),
                       NULL,
                       0,
                       &dw, NULL))
    {
      PrintLastError(MountPoint);
      CloseHandle(device);
      return INDISK_CLI_ERROR_CREATE_DEVICE;
    }

  puts("Extending filesystem size...");

  if (!DeviceIoControl(device,
                       IOCTL_DISK_GET_LENGTH_INFO,
                       NULL,
                       0,
                       &length_information,
                       sizeof(length_information),
                       &dw, NULL))
    {
      PrintLastError(MountPoint);
      InDiskOemPrintF(stderr,
		      "%1!ws!: Is that drive really an InDisk drive?",
		      MountPoint);
      CloseHandle(device);
      return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
    }

  if (!DeviceIoControl(device,
                       IOCTL_DISK_GET_DRIVE_GEOMETRY,
                       NULL,
                       0,
                       &disk_geometry,
                       sizeof(disk_geometry),
                       &dw, NULL))
    {
      PrintLastError(MountPoint);
      InDiskOemPrintF(stderr,
		      "%1!ws!: Is that drive really an InDisk drive?",
		      MountPoint);
      CloseHandle(device);
      return INDISK_CLI_ERROR_DEVICE_INACCESSIBLE;
    }

  new_filesystem_size =
    length_information.Length.QuadPart /
    disk_geometry.BytesPerSector;

  if (!DeviceIoControl(device,
                       FSCTL_EXTEND_VOLUME,
                       &new_filesystem_size,
                       sizeof(new_filesystem_size),
                       NULL,
                       0,
                       &dw, NULL))
    {
      PrintLastError(MountPoint);
      puts("The disk size was extended successfully, but it was not possible to extend the\r\n"
	   "current filesystem on it. You will have to reformat the disk to use the full\r\n"
	   "disk size.");
    }

  CloseHandle(device);

  printf("New size: %.4g %s\n",
	 _h(length_information.Length.QuadPart),
	 _p(length_information.Length.QuadPart));

  return 0;
}

int
__cdecl
wmain(int argc, LPWSTR argv[])
{
  enum
    {
      OP_MODE_NONE,
      OP_MODE_CREATE,
      OP_MODE_REMOVE,
      OP_MODE_QUERY,
      OP_MODE_EDIT
    } op_mode = OP_MODE_NONE;
  DWORD flags = 0;
  BOOL native_path = FALSE;
  BOOL numeric_print = FALSE;
  BOOL force_dismount = FALSE;
  BOOL emergency_remove = FALSE;
  LPWSTR file_name = NULL;
  LPWSTR mount_point = NULL;
  LPWSTR format_options = NULL;
  DWORD device_number = INDISK_AUTO_DEVICE_NUMBER;
  DISK_GEOMETRY disk_geometry = { 0 };
  LARGE_INTEGER image_offset = { 0 };
  BOOL auto_find_offset = FALSE;
  BYTE auto_find_partition_entry = 0;
  DWORD flags_to_change = 0;
  int ret = 0;

  if (argc == 2)
    if (wcscmp(argv[1], L"--version") == 0)
      {
	printf
	  ("Control program for the InDisk Virtual Disk Driver for Windows NT/2000/XP.\n"
	   "Version %i.%i.%i - (Compiled " __DATE__ ")\n"
	   "\n"
	   "Copyright (C) 2004-2010 Olof Lagerkvist.\r\n"
"\n"
	   "http://www.ltr-data.se     olof@ltr-data.se\r\n"
	   "\n"
	   "Permission is hereby granted, free of charge, to any person\r\n"
	   "obtaining a copy of this software and associated documentation\r\n"
	   "files (the \"Software\"), to deal in the Software without\r\n"
	   "restriction, including without limitation the rights to use,\r\n"
	   "copy, modify, merge, publish, distribute, sublicense, and/or\r\n"
	   "sell copies of the Software, and to permit persons to whom the\r\n"
	   "Software is furnished to do so, subject to the following\r\n"
	   "conditions:\r\n"
	   "\r\n"
	   "The above copyright notice and this permission notice shall be\r\n"
	   "included in all copies or substantial portions of the Software.\r\n"
	   "\r\n"
	   "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\r\n"
	   "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES\r\n"
	   "OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND\r\n"
	   "NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT\r\n"
	   "HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,\r\n"
	   "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING\r\n"
	   "FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR\r\n"
	   "OTHER DEALINGS IN THE SOFTWARE.\r\n"
	   "\r\n"
	   "This program contains some GNU GPL licensed code:\r\n"
	   "- Parts related to floppy emulation based on VFD by Ken Kato.\r\n"
	   "  http://chitchat.at.infoseek.co.jp/vmware/vfd.html\r\n"
	   "Copyright (C) Free Software Foundation, Inc.\r\n"
	   "Read gpl.txt for the full GNU GPL license.\r\n"
	   "\r\n"
	   "This program may contain BSD licensed code:\r\n"
	   "- Some code ported to NT from the FreeBSD md driver by Olof Lagerkvist.\r\n"
	   "  http://www.ltr-data.se\r\n"
	   "Copyright (C) The FreeBSD Project.\r\n"
	   "Copyright (C) The Regents of the University of California.\r\n",
	   (INDISK_VERSION & 0xFF00) >> 8,
	   (INDISK_VERSION & 0xF0) >> 4,
	   INDISK_VERSION & 0xF);

	return 0;
      }

  while (argc-- > 1)
    {
      argv++;

      if (wcslen(argv[0]) == 2 ? argv[0][0] == L'-' : FALSE)
	switch (argv[0][1])
	  {
	  case L'a':
	    if (op_mode != OP_MODE_NONE)
	      InDiskSyntaxHelp();

	    op_mode = OP_MODE_CREATE;
	    break;

	  case L'd':
	  case L'D':
	  case L'R':
	    if (op_mode != OP_MODE_NONE)
	      InDiskSyntaxHelp();

	    op_mode = OP_MODE_REMOVE;

	    if (argv[0][1] == L'D')
	      force_dismount = TRUE;

	    if (argv[0][1] == L'R')
	      {
		force_dismount = TRUE;
		emergency_remove = TRUE;
	      }

	    break;

	  case L'l':
	    if (op_mode != OP_MODE_NONE)
	      InDiskSyntaxHelp();

	    op_mode = OP_MODE_QUERY;
	    break;

	  case L'e':
	    if (op_mode != OP_MODE_NONE)
	      InDiskSyntaxHelp();

	    op_mode = OP_MODE_EDIT;
	    break;

	  case L't':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(INDISK_TYPE(flags) != 0))
	      InDiskSyntaxHelp();

	    if (wcscmp(argv[1], L"file") == 0)
	      flags |= INDISK_TYPE_FILE;
	    else if (wcscmp(argv[1], L"vm") == 0)
	      flags |= INDISK_TYPE_VM;
	    else if (wcscmp(argv[1], L"proxy") == 0)
	      flags |= INDISK_TYPE_PROXY;
	    else
	      InDiskSyntaxHelp();

	    argc--;
	    argv++;
	    break;

	  case L'n':
	    numeric_print = TRUE;
	    break;

	  case L'o':
	    if (((op_mode != OP_MODE_CREATE) & (op_mode != OP_MODE_EDIT)) |
		(argc < 2))
	      InDiskSyntaxHelp();

	    {
	      LPWSTR opt;

	      for (opt = wcstok(argv[1], L",");
		   opt != NULL;
		   opt = wcstok(NULL, L","))
		if (wcscmp(opt, L"ro") == 0)
		  {
		    if (INDISK_READONLY(flags_to_change))
		      InDiskSyntaxHelp();

		    flags_to_change |= INDISK_OPTION_RO;
		    flags |= INDISK_OPTION_RO;
		  }
		else if (wcscmp(opt, L"rw") == 0)
		  {
		    if (INDISK_READONLY(flags_to_change))
		      InDiskSyntaxHelp();

		    flags_to_change |= INDISK_OPTION_RO;
		    flags &= ~INDISK_OPTION_RO;
		  }
		else if (wcscmp(opt, L"rem") == 0)
		  {
		    if (INDISK_REMOVABLE(flags_to_change))
		      InDiskSyntaxHelp();

		    flags_to_change |= INDISK_OPTION_REMOVABLE;
		    flags |= INDISK_OPTION_REMOVABLE;
		  }
		else if (wcscmp(opt, L"fix") == 0)
		  {
		    if (INDISK_REMOVABLE(flags_to_change))
		      InDiskSyntaxHelp();

		    flags_to_change |= INDISK_OPTION_REMOVABLE;
		    flags &= ~INDISK_OPTION_REMOVABLE;
		  }
		else if (wcscmp(opt, L"saved") == 0)
		  {
		    if (op_mode != OP_MODE_EDIT)
		      InDiskSyntaxHelp();

		    flags_to_change |= INDISK_IMAGE_MODIFIED;
		    flags &= ~INDISK_IMAGE_MODIFIED;
		  }
	      // None of the other options are valid with the -e parameter.
		else if (op_mode != OP_MODE_CREATE)
		  InDiskSyntaxHelp();
		else if (wcscmp(opt, L"ip") == 0)
		  {
		    if ((INDISK_TYPE(flags) != INDISK_TYPE_PROXY) |
			(INDISK_PROXY_TYPE(flags) != INDISK_PROXY_TYPE_DIRECT))
		      InDiskSyntaxHelp();

		    native_path = TRUE;
		    flags |= INDISK_PROXY_TYPE_TCP;
		  }
		else if (wcscmp(opt, L"comm") == 0)
		  {
		    if ((INDISK_TYPE(flags) != INDISK_TYPE_PROXY) |
			(INDISK_PROXY_TYPE(flags) != INDISK_PROXY_TYPE_DIRECT))
		      InDiskSyntaxHelp();

		    native_path = TRUE;
		    flags |= INDISK_PROXY_TYPE_COMM;
		  }
		else if (wcscmp(opt, L"shm") == 0)
		  {
		    if ((INDISK_TYPE(flags) != INDISK_TYPE_PROXY) |
			(INDISK_PROXY_TYPE(flags) != INDISK_PROXY_TYPE_DIRECT))
		      InDiskSyntaxHelp();

		    flags |= INDISK_PROXY_TYPE_SHM;
		  }
		else if (INDISK_DEVICE_TYPE(flags) != 0)
		  InDiskSyntaxHelp();
		else if (wcscmp(opt, L"hd") == 0)
		  flags |= INDISK_DEVICE_TYPE_HD;
		else if (wcscmp(opt, L"fd") == 0)
		  flags |= INDISK_DEVICE_TYPE_FD;
		else if (wcscmp(opt, L"cd") == 0)
		  flags |= INDISK_DEVICE_TYPE_CD;
		else
		  InDiskSyntaxHelp();
	    }

	    argc--;
	    argv++;
	    break;

	  case L'f':
	  case L'F':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(file_name != NULL))
	      InDiskSyntaxHelp();

	    if (argv[0][1] == L'F')
	      native_path = TRUE;

	    file_name = argv[1];

	    argc--;
	    argv++;
	    break;

	  case L's':
	    if (((op_mode != OP_MODE_CREATE) & (op_mode != OP_MODE_EDIT)) |
		(argc < 2) |
		(disk_geometry.Cylinders.QuadPart != 0))
	      InDiskSyntaxHelp();

	    {
              WCHAR suffix = 0;

	      swscanf(argv[1], L"%I64i%c",
		      &disk_geometry.Cylinders, &suffix);

              switch (suffix)
                {
		case 0:
		  break;
		case '%':
		  if ((disk_geometry.Cylinders.QuadPart <= 0) |
		      (disk_geometry.Cylinders.QuadPart >= 100))
		    InDiskSyntaxHelp();

		  {
		    MEMORYSTATUS memstat;
		    GlobalMemoryStatus(&memstat);
		    disk_geometry.Cylinders.QuadPart =
		      disk_geometry.Cylinders.QuadPart *
		      memstat.dwAvailPhys / 100;
		  }

		  break;
                case 'T':
                  disk_geometry.Cylinders.QuadPart <<= 10;
                case 'G':
                  disk_geometry.Cylinders.QuadPart <<= 10;
                case 'M':
                  disk_geometry.Cylinders.QuadPart <<= 10;
                case 'K':
                  disk_geometry.Cylinders.QuadPart <<= 10;
                  break;
                case 'b':
                  disk_geometry.Cylinders.QuadPart <<= 9;
                  break;
                case 't':
                  disk_geometry.Cylinders.QuadPart *= 1000;
                case 'g':
                  disk_geometry.Cylinders.QuadPart *= 1000;
                case 'm':
                  disk_geometry.Cylinders.QuadPart *= 1000;
                case 'k':
                  disk_geometry.Cylinders.QuadPart *= 1000;
                  break;
                default:
                  fprintf(stderr, "InDisk: Unsupported size suffix: '%wc'\n",
                          suffix);
                  return INDISK_CLI_ERROR_BAD_SYNTAX;
                }

	      if (disk_geometry.Cylinders.QuadPart < 0)
		{
		  MEMORYSTATUS memstat;
		  GlobalMemoryStatus(&memstat);
		  disk_geometry.Cylinders.QuadPart =
		    memstat.dwAvailPhys +
		    disk_geometry.Cylinders.QuadPart;

		  if (disk_geometry.Cylinders.QuadPart < 0)
		    {
		      fprintf(stderr,
			      "InDisk: Not enough memory, there is currently "
			      "%.4g %s free physical memory.\n",
			      _h(memstat.dwAvailPhys),
			      _p(memstat.dwAvailPhys));

		      return INDISK_CLI_ERROR_NOT_ENOUGH_MEMORY;
		    }
		}
	    }

	    argc--;
	    argv++;
	    break;

	  case L'S':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(disk_geometry.BytesPerSector != 0))
	      InDiskSyntaxHelp();

	    if (!iswdigit(argv[1][0]))
	      InDiskSyntaxHelp();

	    disk_geometry.BytesPerSector = wcstoul(argv[1], NULL, 0);

	    argc--;
	    argv++;
	    break;
	    
	  case L'x':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(disk_geometry.SectorsPerTrack != 0))
	      InDiskSyntaxHelp();

	    if (!iswdigit(argv[1][0]))
	      InDiskSyntaxHelp();

	    disk_geometry.SectorsPerTrack = wcstoul(argv[1], NULL, 0);

	    argc--;
	    argv++;
	    break;
	    
	  case L'y':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(disk_geometry.TracksPerCylinder != 0))
	      InDiskSyntaxHelp();

	    if (!iswdigit(argv[1][0]))
	      InDiskSyntaxHelp();

	    disk_geometry.TracksPerCylinder = wcstoul(argv[1], NULL, 0);

	    argc--;
	    argv++;
	    break;

	  case L'v':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(auto_find_partition_entry != 0))
	      InDiskSyntaxHelp();

	    if ((argv[1][0] < L'1') | (argv[1][0] > L'8'))
	      InDiskSyntaxHelp();

	    if (argv[1][1] != 0)
	      InDiskSyntaxHelp();

	    auto_find_partition_entry = (BYTE)(argv[1][0] - L'0');
	    argc--;
	    argv++;
	    break;

	  case L'b':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(image_offset.QuadPart != 0) |
		(auto_find_offset != FALSE))
	      InDiskSyntaxHelp();

	    if (wcscmp(argv[1], L"auto") == 0)
	      auto_find_offset = TRUE;
	    else
	      {
		WCHAR suffix = 0;

		swscanf(argv[1], L"%I64u%c",
			&image_offset, &suffix);

		switch (suffix)
		  {
		  case 0:
		    break;
		  case 'T':
		    image_offset.QuadPart <<= 10;
		  case 'G':
		    image_offset.QuadPart <<= 10;
		  case 'M':
		    image_offset.QuadPart <<= 10;
		  case 'K':
		    image_offset.QuadPart <<= 10;
		    break;
		  case 'b':
		    image_offset.QuadPart <<= 9;
		    break;
		  case 't':
		    image_offset.QuadPart *= 1000;
		  case 'g':
		    image_offset.QuadPart *= 1000;
		  case 'm':
		    image_offset.QuadPart *= 1000;
		  case 'k':
		    image_offset.QuadPart *= 1000;
		  default:
		    fprintf(stderr, "InDisk: Unsupported size suffix: '%wc'\n",
			    suffix);
		    return INDISK_CLI_ERROR_BAD_SYNTAX;
		  }
	      }

	    argc--;
	    argv++;
	    break;

	  case L'p':
	    if ((op_mode != OP_MODE_CREATE) |
		(argc < 2) |
		(format_options != NULL))
	      InDiskSyntaxHelp();

	    format_options = argv[1];

	    argc--;
	    argv++;
	    break;

	  case L'u':
	    if ((argc < 2) |
		((mount_point != NULL) & (op_mode != OP_MODE_CREATE)) |
		(device_number != INDISK_AUTO_DEVICE_NUMBER))
	      InDiskSyntaxHelp();

	    if (!iswdigit(argv[1][0]))
	      InDiskSyntaxHelp();

	    device_number = wcstoul(argv[1], NULL, 0);

	    argc--;
	    argv++;
	    break;
	    
	  case L'm':
	    if ((argc < 2) |
		(mount_point != NULL) |
		((device_number != INDISK_AUTO_DEVICE_NUMBER) &
		 (op_mode != OP_MODE_CREATE)))
	      InDiskSyntaxHelp();

	    mount_point = argv[1];

	    if ((op_mode == OP_MODE_CREATE) &
		(wcscmp(mount_point, L"#:") == 0))
	      mount_point[0] = InDiskFindFreeDriveLetter();

	    argc--;
	    argv++;
	    break;

	  default:
	    InDiskSyntaxHelp();
	  }
      else
	InDiskSyntaxHelp();
    }

  switch (op_mode)
    {
    case OP_MODE_CREATE:
      {
	if (auto_find_offset)
	  if (file_name == NULL)
	    InDiskSyntaxHelp();
	  else
	    InDiskGetOffsetByFileExt(file_name, &image_offset);

	if (auto_find_partition_entry != 0)
	  {
	    PARTITION_INFORMATION partition_information[8];
	    PPARTITION_INFORMATION part_rec =
	      partition_information +
	      auto_find_partition_entry - 1;

	    if (!InDiskGetPartitionInformation(file_name,
					       disk_geometry.BytesPerSector,
					       &image_offset,
					       partition_information))
	      {
		fputs("Error: Partition table not found.\r\n", stderr);
		return INDISK_CLI_ERROR_PARTITION_NOT_FOUND;
	      }

	    if ((part_rec->StartingOffset.QuadPart == 0) |
		(part_rec->PartitionLength.QuadPart == 0))
	      {
		fprintf(stderr,
			"Error: Partition %i is not defined.\n",
			(int)auto_find_partition_entry);
		return INDISK_CLI_ERROR_PARTITION_NOT_FOUND;
	      }

	    image_offset.QuadPart += part_rec->StartingOffset.QuadPart;
	    disk_geometry.Cylinders = part_rec->PartitionLength;
	  }
	else if (auto_find_offset)
	  {
	    PARTITION_INFORMATION partition_information[8];
	    if (InDiskGetPartitionInformation(file_name,
					      disk_geometry.BytesPerSector,
					      &image_offset,
					      partition_information))
	      {
		PPARTITION_INFORMATION part_rec;
		for (part_rec = partition_information;
		     part_rec < partition_information + 8;
		     part_rec++)
		  if ((part_rec->StartingOffset.QuadPart != 0) &
		      (part_rec->PartitionLength.QuadPart != 0) &
		      !IsContainerPartition(part_rec->PartitionType))
		    {
		      image_offset.QuadPart +=
			part_rec->StartingOffset.QuadPart;
		      disk_geometry.Cylinders = part_rec->PartitionLength;
		      break;
		    }
	      }
	  }

	ret = InDiskCliCreateDevice(&device_number, &disk_geometry,
				    &image_offset, flags, file_name,
				    native_path, mount_point);

	if (ret != 0)
	  return ret;

	if (numeric_print)
	  printf("%u\n", device_number);
	else
	  InDiskOemPrintF(stdout,
			  "Created device %1!u!: %2!ws! -> %3!ws!",
			  device_number,
			  mount_point == NULL ? L"No mountpoint" : mount_point,
			  file_name == NULL ? L"VM image" : file_name);

	if (format_options != NULL)
	  if (mount_point != NULL)
	    InDiskCliFormatDisk(mount_point, format_options);
	  else
	    fputs("Formatting is not supported without a mount point.\r\n"
		  "The virtual disk has been created without formatting.\r\n",
		  stderr);

	// Notify processes that new device has arrived.
	if (mount_point != NULL)
	  if ((wcslen(mount_point) == 2) ? mount_point[1] == ':' : 
	      (wcslen(mount_point) == 3) ?
	      wcscmp(mount_point + 1, L":\\") == 0 : FALSE)
	    {
	      DWORD_PTR dwp;
	      DEV_BROADCAST_VOLUME dev_broadcast_volume = {
		sizeof(DEV_BROADCAST_VOLUME),
		DBT_DEVTYP_VOLUME
	      };

	      puts("Notifying applications...");

	      dev_broadcast_volume.dbcv_unitmask =
		1 << (mount_point[0] - L'A');

	      SendMessageTimeout(HWND_BROADCAST,
				 WM_DEVICECHANGE,
				 DBT_DEVICEARRIVAL,
				 (LPARAM)&dev_broadcast_volume,
				 SMTO_BLOCK | SMTO_ABORTIFHUNG,
				 4000,
				 &dwp);

	      dev_broadcast_volume.dbcv_flags = DBTF_MEDIA;

	      SendMessageTimeout(HWND_BROADCAST,
				 WM_DEVICECHANGE,
				 DBT_DEVICEARRIVAL,
				 (LPARAM)&dev_broadcast_volume,
				 SMTO_BLOCK | SMTO_ABORTIFHUNG,
				 4000,
				 &dwp);

	      SendMessageTimeout(HWND_BROADCAST,
				 WM_DEVICECHANGE,
				 DBT_DEVNODES_CHANGED,
				 (LPARAM)0,
				 SMTO_BLOCK | SMTO_ABORTIFHUNG,
				 4000,
				 &dwp);

	      /*
	      PostMessage(HWND_BROADCAST,
			  WM_DEVICECHANGE,
			  DBT_DEVICEARRIVAL,
			  (LPARAM)&dev_broadcast_volume);

	      dev_broadcast_volume.dbcv_flags = DBTF_MEDIA;

	      PostMessage(HWND_BROADCAST,
			  WM_DEVICECHANGE,
			  DBT_DEVICEARRIVAL,
			  (LPARAM)&dev_broadcast_volume);
	      */
	    }

	puts("Done.");

	return 0;
      }

    case OP_MODE_REMOVE:
      if ((device_number == INDISK_AUTO_DEVICE_NUMBER) &
	  ((mount_point == NULL) |
	   emergency_remove))
	InDiskSyntaxHelp();
      
      return InDiskCliRemoveDevice(device_number, mount_point, force_dismount,
				   emergency_remove);

    case OP_MODE_QUERY:
      if ((device_number == INDISK_AUTO_DEVICE_NUMBER) &
	  (mount_point == NULL))
	return !InDiskCliQueryStatusDriver(numeric_print);
      
      return InDiskCliQueryStatusDevice(device_number, mount_point);

    case OP_MODE_EDIT:
      if ((device_number == INDISK_AUTO_DEVICE_NUMBER) &
	  (mount_point == NULL))
	InDiskSyntaxHelp();

      if (flags_to_change != 0)
	ret = InDiskCliChangeFlags(device_number, mount_point, flags_to_change,
				   flags);

      if (disk_geometry.Cylinders.QuadPart > 0)
	ret = InDiskCliExtendDevice(device_number, mount_point,
				    disk_geometry.Cylinders);

      return ret;
    }

  InDiskSyntaxHelp();
}

#ifndef _WIN64
__declspec(noreturn)
void
__cdecl
wmainCRTStartup()
{
  int argc = 0;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);

  if (argv == NULL)
    {
      MessageBoxA(NULL,
		  "This program requires Windows NT/2000/XP.",
		  "InDisk Virtual Disk Driver",
		  MB_ICONSTOP);

      ExitProcess((UINT)-1);
    }

  exit(wmain(argc, argv));
}
#endif
