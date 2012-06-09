Imports LTR.IO.InDisk.Devio.Server.Providers

Namespace Server.Services

  ''' <summary>
  ''' Class deriving from DevioServiceBase, but without providing a proxy service. Instead,
  ''' it just passes a disk image file name for direct mounting internally in InDisk Virtual
  ''' Disk Driver.
  ''' </summary>
  Public Class DevioNoneService
    Inherits DevioServiceBase

    ''' <summary>
    ''' Name and path of image file mounted by InDisk Virtual Disk Driver.
    ''' </summary>
    Public ReadOnly Imagefile As String

    ''' <summary>
    ''' Creates a DevioServiceBase compatible object, but without providing a proxy service.
    ''' Instead, it just passes a disk image file name for direct mounting internally in InDisk
    ''' Virtual Disk Driver.
    ''' </summary>
    ''' <param name="Imagefile">Name and path of image file mounted by InDisk Virtual Disk Driver.</param>
    Public Sub New(Imagefile As String)
      MyBase.New(New DevioProviderFromStream(New FileStream(Imagefile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite Or FileShare.Delete), ownsStream:=True), OwnsProvider:=True)

      Offset = InDiskAPI.GetOffsetByFileExt(Imagefile)
      Me.Imagefile = Imagefile

    End Sub

    ''' <summary>
    ''' Reads partition table and parses partition entry values into a collection of PARTITION_INFORMATION
    ''' structure objects.
    ''' </summary>
    ''' <returns>Collection of PARTITION_INFORMATION structures objects.</returns>
    Public Overrides Function GetPartitionInformation() As ReadOnlyCollection(Of NativeFileIO.Win32API.PARTITION_INFORMATION)
      Return InDiskAPI.GetPartitionInformation(Imagefile, SectorSize, Offset)
    End Function

    Protected Overrides ReadOnly Property InDiskProxyObjectName As String
      Get
        Return Imagefile
      End Get
    End Property

    Protected Overrides ReadOnly Property InDiskProxyModeFlags As InDiskFlags
      Get
        Return InDiskFlags.TypeFile
      End Get
    End Property

    ''' <summary>
    ''' Dummy implementation that always returns True.
    ''' </summary>
    ''' <returns>Fixed value of True.</returns>
    Public Overrides Function StartServiceThread() As Boolean
      Return True
    End Function

    ''' <summary>
    ''' Dummy implementation that just raises ServiceReady event.
    ''' </summary>
    Public Overrides Sub RunService()
      OnServiceReady()
    End Sub

  End Class

End Namespace
