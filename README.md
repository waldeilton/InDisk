	  InDisk
	===========
	== Indirect Disk
	==
	== by Rick C. Hodgin
	==    rick.c.hodgin@gmail.com
	==
	== Original idea  :  May 22, 2012
	== Original GitHub:  May 30, 2012
	===========


July 6, 2012 RCH
======
I have been developing client / server protocols for
communicating between local and remote.  Code is 100%
functional for transferring messages back and forth.
40% functional in terms of fully supporting all necessary
options in protocol form to support a robust, real-time,
peer-to-peer / client-server model of data maintenance.

Have not yet begun working on ImDisk support as I am
able to handle everything in simulation right now.


June 8, 2012 RCH
======
I found free software for a Windows NT/2K/XP/Vista/7 RAM disk.

The imdisk_source directory will serve as a reference.  The
actual source for this project will be maintained in the InDisk
directory, and will be compilable with Visual Studio 2008 or
later.

See InDisk on YouTube:
http://www.youtube.com/watch?v=EM56J1QRS-I


May 30, 2012 RCH
======
An access layer (pseudo-ram disk), allowing reads/writes to come from alternate sources, such as a SQL server, local or remote hard drive, the Internet or the cloud, providing files which appear to come from a solid
disk location, when in fact they are either mirrored (conveyed as is
from a hard disk source, from a literal disk file) or assembled (by
performing a query on a remote data source, and assembling it into a
properly formatted file to the calling app).

Designed originally to facilitiate the needs of Visual FoxPro tables,
which use their .DBF contents in physical disk files to describe both
the table structure and data, be sent to a remote SQL database server
without having to change the application at all.

Basically, this virtualizes the hard disk into a layer which can have
its content directed or re-directed to any source.

By intercepting read/write/seek commands, and then coordinating those
with a remote data source (such as a remote SQL database server),
InDisk (the pseudo-ram disk) can present to the calling application
(a FoxPro app) a physical disk file for access, when in reality its data
source comes from the location InDisk has been directed to guide it,
such as a real hard disk file, or a remote SQL database server table
which is interpreted on-the-fly and arranged into a DBF-looking file,
or both, or something else, so the original application sees only a
usable disk file for reading, writing, seeking, locking, unlocking, etc.,
to create an environment where legacy applications can publish their
data in near real-time to alternate, modern, queryable / publishable
sources, and without any changes being made to the legacy binary code
or software libraries.

In addition, the ability to journal changes (what did Susie change today?
Look at the change log recorded by InDisk, when InDisk is set to record
those changes), and to then create an original reference file which is
what the file looked like at a given point and time, along with the full
recorded journal, allowing a recreation of exactly what the data source
looked like at any point and time.

It is my desire that this project, released under the GPLv3, be created
for Windows and Linux operating systems.

Free Software projects benefit from the community participating, and as
such this project is made public for the purposes of drawing in developers
from all over the world.

May The Lord bless this project, as I give my time and talents to Him.

Best regards,
Rick C. Hodgin

