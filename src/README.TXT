/**************************************************************************
*   pbm2l2030.c   : convert monocromatic PBM file into Lexmark 2030       *
*                   printing escape sequences                             *
*                                                                         *
*                                                                         * 
*   Version       : 1.4 - 16.09.1999                                      *  
*                                                                         * 
*   Copyright (C) : 1999 Thomas Paetzold                                  *
*                                                                         *
*   Contact       : Thomas.Paetzold@FHTW-BERLIN.DE                        *
*                                                                         *
*   Homepage      : http://move.to/$HOME                                  *
*                                                                         *
*  This program is free software; you can redistribute it and/or modify   *
*  it under the terms of the GNU General Public License as published by   *
*  the Free Software Foundation; either version 2 of the License, or      *
*  (at your option) any later version.                                    *
*                                                                         *
*  This program is distributed in the hope that it will be useful,        *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*  GNU General Public License for more details.                           *
*                                                                         *
*  You should have received a copy of the GNU General Public License      *
*  along with this program; if not, write to the Free Software            *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              *
*                                                                         * 
*  Thanks to: Christian Kornblum, the developer of Lexmark 2070 Driver    *
*             Tim Norman, the developer of pbm2ppa for HP-Printers.       * 
*                                                                         * 
*                                                                         *
***************************************************************************/
 
 
What is this program about? 
-------------------------------------------------------------------------- 
This small C program will allow a Linux user to print S/W on a Lexmark 2030 in  
300dpi. The program was tested with SuSE Linux 6.2 and is still trial version!
 
The program reads from STDIN a black and white PBM formatted picture and  
converts it to a typesetting code that the Lexmark Color Jetprinter 2030  
understands and can print.  The output is piped into STDOUT.   
 
It deales as a pbm2ppa translator. Basically, you use ghostscript to render 
PostScript into a bitmapped image in pbm format and then use pbm2ppa to 
translate this into a printer-specific ppa format bitmap ready to be dumped 
to the Lexmark 2030 printer.
For more info about ppa software try out "http://www.httptech.com/ppa/".

Lexmark has not released the interface protocols of their printers so 
these have been reversed-engineered.  It works very well for me but I am 
promising ABSOLUTELY NO GUARANTEE that it will work for you. 

 

How Lexmark did support my project:
--------------------------------------------------------------------------
"Unfortunately the Host-Based printer driver is under License and no information
 for the driver can be distributed.

 Unfortunately due to the age of this printer it is no longer supported via the
 Lexmark Technical Support Center on the web." 



Installation: 
--------------------------------------------------------------------------
(0.)	Install the HP720C with A4 paper and a resolution of 300x300dpi.

	With SuSE, these drivers are in the package "ppa" in the series "ap". 
	With this you can use the SuSE APSFILTER-SETUP to install one of the  
	HP drivers.  The setup therefore is : 
 
	/usr/lib/apsfilter/SETUP 
 
(1.)    Untar the "pbm2l2030_tar.tar.gz" into a build directory as follows: 
 
        gzip -dc pbm2l2030_tar.tar.gz | tar -xof - 
 
(2.)	Compile the code using ´make´. 

(3.)	A ´make install´ will then replace the HP driver located at 
	"/usr/bin/pbm2ppa" with the Lexmark2030 driver (you have to be 'root')
      
(4.)    Delete the useless stuff using ´make clean´.

Now you should be able to print with the "lpr" command. 


If nothing happens list the processes and look for a sleeping pbm2ppa-Process 
which means that the print job did not end correctly. 

