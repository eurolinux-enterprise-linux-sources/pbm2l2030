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
 
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pbm.h"
#define PAGE_HEIGHT 3508 
#define PAGE_WIDTH 2480 
#define CARTRIDGE_PENS 52 
#define LEFT_MARGIN 20

/***************************************************************************
 *   Funktion:   lex_move                                                  *
 *   Parameter:  Zeiger auf Ausgabedatei, Pixel vertik. Versch.            *
 *   Aufgabe:    Verschiebt die Druckposition um <Pixel> 600lpi-Punkte     *
 *               nach unten.                                               *
 ***************************************************************************/
void lex_move(FILE *out, long int pixel)
{
  char buffer[] = {0x1b,0x2a,0x03,0x00,0x00};
  int i;
  buffer[4] = (char) pixel;
  buffer[3] = (char) (pixel >> 8);
  for(i=0; i<=4; fprintf(out, "%c", buffer[i++]));
}


/***************************************************************************
 *   Funktion:	lex_init						   *
 *   Parameter: Abstand vom oberen Rand in Punkten     			   *
 *   Aufgabe:   Ausgabe des Druckerheaders			           *
 ***************************************************************************/
void lex_init(FILE *out)
{
   int i;
   char buffer[] = {0x1B,0x2A,0x80,0x1B,0x2A,0x07,
                    0x73,0x30,0x1B,0x2A,0x07,0x63};
   for(i=0; i<=11; fprintf(out, "%c", buffer[i++]));
}

/***************************************************************************
 *   Funktion:	lex_eop  						   *
 *   Parameter: Ausgabedatei, Puffer             			   *
 *   Aufgabe:   Seitenauswurf End OF Page			           *
 ***************************************************************************/
void lex_eop(FILE *out)
{
   int i;
   char buffer[] = {0x1B,0x2A,0x07,0x65};
   for(i=0; i<=12; i++) fprintf(out, "%c", buffer[i]);
}

/***************************************************************************
 *   Funktion:	print pbm						   *
 *   Parameter: Zeiger auf Eingabedatei (PBM) und Ausgabedatei 		   *
 *   Aufgabe:   Lesen des PBM und Druckabwicklung		           * 
 ***************************************************************************/
int print_pbm (FILE *in, FILE *out)
{
  char line[1024];
  pbm_stat pbm;
  int done_page, cur_height = 0, page_height = 0, numpages = 0;
  char lex_binhd[] = {0x1b,0x2a,0x04,0x00,0x00,0x00,0x01,0x01,0x01,
                      0x07,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x32,0x33,0x34,0x35};
  long bytesize, stdbytesize; 
  register int i=0;
  register int cur_byte = 0;
  register int cur_pos = 0;
  char *buffer; 
  int breite, stdbreite;
  int leftmargin;
  register int redleft;
  register int redright;
  int bstart, empty_lines;


  while(make_pbm_stat(&pbm, in))
  {

    /* Begrenzung auf maximale Breite */
    if (pbm.width <= PAGE_WIDTH) stdbreite = pbm.width; 
    else stdbreite = PAGE_WIDTH;

    /* Berechnung der Puffergroesse */
    stdbytesize = 26 + 7 * stdbreite; 

    /* Speicher reservieren und leeren */
    buffer = (char *) malloc(stdbytesize - 26);
    for (i=0; i<=stdbytesize - 26; buffer[i++]=0);

    /*  Seite Laden und Drucker initialisieren... */
    lex_init(out);


    /*  Vorschub  ... */
   lex_move(out, 52);   
    


    /* Die Seite kann gedruckt werden... */
    done_page=0;
    page_height=0;
    cur_height=0;
    empty_lines=0;
    while(!done_page)
    {
      /* Setzen der Standardbreite vor Reduktion */
      breite = stdbreite;
      leftmargin = LEFT_MARGIN;
      bytesize = stdbytesize;
      
        /* Zeile einlesen, passenden Pen einstellen */
        pbm_readline(&pbm, line);
        cur_pos = 7 - (cur_height % 8);
        cur_byte = cur_height / 8;

 
        /* Bits binär verschieben --> vgl. Protocol .. :-(( */
        for(i=0; (i <= breite); i++)
        {                              
          buffer[(i * 7) + cur_byte] 
          |= (0 < (line[i/8] & (0x80 >> (i % 8)))) << cur_pos;      
 
        }


        /* Genug im Buffer, also drucken */
        if ((cur_height++>=CARTRIDGE_PENS) || (page_height >= pbm.height))
	{


	  /* Reduktion der Nullbytes zur Druckoptimierung */
           
	  redleft = 0; redright = 0; bstart = 0;

          /* Linke Reduktion von 0x0 */ 

          while ((buffer[redleft] == 0) && (redleft < bytesize - 26))
	    redleft++;

	  /* Rechte Reduktion von 0x0 */ 
          while ((buffer[bytesize - 27 - redright] == 0) 
		 && (redright < bytesize - 26))
	    redright++;

          /* Neuberechnung der Breite- und Marginwerte */
	  breite -= redleft / 7 + redright / 7;
	  leftmargin += redleft / 7;
	  bstart = redleft - (redleft % 7);
               if (bstart < 0) bstart = 0; 

	  
	  /* Berechnen der Puffergröße für Drucker */
	  bytesize = 26 + 7 * breite; 


	  /* Einbauen der Puffergröße in Header */  
	  lex_binhd[4] = (char) bytesize;
	  lex_binhd[3] = (char) (bytesize >> 8);
	  

	  /* Einbauen der Zeilenlänge in Pixeln (Anzahl der Pakete)*/
	  lex_binhd[12] = (char) breite;
	  lex_binhd[11] = (char) (breite >> 8);

	  /* Einbauen der Startkoordinaten */
	  lex_binhd[14] = (char) leftmargin;
	  lex_binhd[13] = (char) (leftmargin >> 8);

	  /* Einbauen der Endkoordinaten */
	  lex_binhd[16] = (char) (leftmargin+breite-1);
	  lex_binhd[15] = (char) ((leftmargin+breite-1) >> 8);

      
	  if (breite <= 0) { /* Leere Zeile, also nicht drucken */
	    empty_lines++;
	  }
	  else { /* Zeile mit Inhalt, also drucken */
	    if (empty_lines) { /* Leerzeilen als ganzes (kein Stottern) */
	      lex_move(out, empty_lines * 104);
	      empty_lines = 0;
	    }
	    for(i=0; i<26; i++) fprintf(out, "%c", lex_binhd[i]);
	    for(i=0; i<(bytesize-26);i++) fprintf(out, "%c", buffer[i+bstart]);
	    lex_move(out, 52);
	  }
          for (i=0; i<=(stdbytesize - 26); buffer[i++]=0);
          cur_height = 0; 
        }
        
        /* Seitenende durch pbm oder physikalisch */ 
        if ((page_height++>=PAGE_HEIGHT)||
            (page_height >= pbm.height)) done_page = 1; 
    }

    /* Seite auswerfen */
    lex_eop(out);

    /* eat any remaining whitespace so process will not hang */
    if(pbm.version==P1)
      fgets (line, 1024, in);

    /* Freigabe des dynamisch reservierten Speichers */
    numpages++;
    free(buffer);
  }

  if (numpages == 0)
  {
    hilfe("No pages printed! Try /usr/bin/pbm2ppa < input > output to check...");
    return 1;
  }

  return 0;
}


/***************************************************************************
 *   Funktion:	hilfe							   *
 *   Parameter: Fehlertext						   *
 *   Aufgabe:   Ausgabe von fatalen Fehlern mit genereller Hilfe           *
 ***************************************************************************/
hilfe(char *fehler)
{
	fprintf(stderr, "Lexmark2030: %s \n", fehler);

}


/***************************************************************************
 *   Funktion:	main							   *
 *   Parameter: Komandozeile						   *
 *   Aufgabe:   Steuerung der Abläufe				           *
 ***************************************************************************/
int main(char argv[], int argc)
{
   return print_pbm(stdin, stdout);
}


