/*
    InDisk Proxy Services.

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

#ifndef _INC_INDPROXY_
#define _INC_INDPROXY_

#if !defined(_WIN32) && !defined(_NTDDK_)
typedef long LONG;
typedef unsigned long ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef unsigned short WCHAR;
#endif

#define INDPROXY_SVC                    L"ImDskSvc"
#define INDPROXY_SVC_PIPE_DOSDEV_NAME   L"\\\\.\\PIPE\\" INDPROXY_SVC
#define INDPROXY_SVC_PIPE_NATIVE_NAME   L"\\Device\\NamedPipe\\" INDPROXY_SVC

#define INDPROXY_FLAG_RO                0x1

typedef enum _INDPROXY_REQ
  {
    INDPROXY_REQ_NULL,
    INDPROXY_REQ_INFO,
    INDPROXY_REQ_READ,
    INDPROXY_REQ_WRITE,
    INDPROXY_REQ_CONNECT,
    INDPROXY_REQ_CLOSE,
  } INDPROXY_REQ;

typedef struct _INDPROXY_CONNECT_REQ
{
  ULONGLONG request_code;
  ULONGLONG flags;
  ULONGLONG length;
} INDPROXY_CONNECT_REQ, *PINDPROXY_CONNECT_REQ;

typedef struct _INDPROXY_CONNECT_RESP
{
  ULONGLONG error_code;
  ULONGLONG object_ptr;
} INDPROXY_CONNECT_RESP, *PINDPROXY_CONNECT_RESP;

typedef struct _INDPROXY_INFO_RESP
{
  ULONGLONG file_size;
  ULONGLONG req_alignment;
  ULONGLONG flags;
} INDPROXY_INFO_RESP, *PINDPROXY_INFO_RESP;

typedef struct _INDPROXY_READ_REQ
{
  ULONGLONG request_code;
  ULONGLONG offset;
  ULONGLONG length;
} INDPROXY_READ_REQ, *PINDPROXY_READ_REQ;

typedef struct _INDPROXY_READ_RESP
{
  ULONGLONG errorno;
  ULONGLONG length;
} INDPROXY_READ_RESP, *PINDPROXY_READ_RESP;

typedef struct _INDPROXY_WRITE_REQ
{
  ULONGLONG request_code;
  ULONGLONG offset;
  ULONGLONG length;
} INDPROXY_WRITE_REQ, *PINDPROXY_WRITE_REQ;

typedef struct _INDPROXY_WRITE_RESP
{
  ULONGLONG errorno;
  ULONGLONG length;
} INDPROXY_WRITE_RESP, *PINDPROXY_WRITE_RESP;

// For shared memory proxy communication only. Offset to data area in
// shared memory.
#define INDPROXY_HEADER_SIZE 4096

#endif // _INC_INDPROXY_
