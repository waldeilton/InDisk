Namespace InDisk

  ''' <summary>
  ''' Represents InDisk Virtual Disk Driver control device object.
  ''' </summary>
  <ComVisible(False)>
  Public Class InDiskControl
    Inherits InDiskObject

    ''' <summary>
    ''' Creates a new instance and opens InDisk Virtual Disk Driver control device object.
    ''' </summary>
    Public Sub New()
      MyBase.New("\\?\InDiskCtl", AccessMode:=0)

    End Sub

  End Class

End Namespace