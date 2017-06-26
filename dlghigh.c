/*ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออธ
 ณ                                                                          ณ
 ณ            D I A L O G . C - Demonstrate LS_OWNERDRAW Listbox            ณ
 ณ                                                                          ณ
 ิออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
/*ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออธ
 ณ                                                                          ณ
 ณ                  Copyright (c) Bunker Hill Software 1990                 ณ
 ณ                                                                          ณ
 ณ                           Bunker Hill Software                           ณ
 ณ                              P.O. Box 18138                              ณ
 ณ                       Encino, California 91416-8138                      ณ
 ณ                                                                          ณ
 ิออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
/*ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออธ
 ณ                    D I A L O G . C    E D I T    L O G                   ณ
 ณ                                                                          ณ
 ณ Edit        Date        Person  Modification                             ณ
 ิออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออพ
   dlghigh.c   11/07/90    ajd     create basic window, setup dialog, etc...

   FillListBox 11/10/90    ajd     create function to fill the list box.
                                   also create .txt file with news headings
                                   in it. headings came from comtex news
                                   several months previous.

   fonts       11/10/90    ajd     problems getting fonts set properly.
                                   documentation for WinSetPresParm () seems
                                   unclear and not complete.

   fonts       11/12/90    ajd     WinSetPresParam () needs to have the
                                   font passed to it as "8.Courier" with
                                   PP_FONTNAMESIZE. MS docs say this is
                                   to be used for font size, I was assuming
                                   that PP_FONTHANDLE was the font name...

   SubStr      11/13/90    ajd     routine to search for substring withing
                                   a string. takes two pointers to type
                                   char.

   hps         11/14/90    ajd     change WinGetPS () to after WinSetPresParm
                                   so it doesn't get the old font settings.

   SYSCLR      11/15/90    ajd     change the color using SYSCLR_HELPHILITE
                                   so that it will work right for all color
                                   settings.

 *ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
/*ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออธ
 ณ Author:     Alan DuBoff, CIS ID: 76662,660                               ณ
 ณ   Date:     November, 1990                                               ณ
 ิออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
/*ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออธ
 ณ                                                        F U N C T I O N S ณ
 ณ    SYNOPSIS:    allow for multiple color inside a      ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ ณ
 ณ                 listbox. concept could be used for multiple fonts also.  ณ
 ณ DESCRIPTION:    not only have multiple colors, but to have multiple      ณ
 ณ                 colors in a single listbox item. it would be easier      ณ
 ณ                 to use different colors for each item, but I needed      ณ
 ณ                 to have a Keyword highlited.                             ณ
 ณ     RETURNS:    Nothing                                                  ณ
 ณ                                                                          ณ
 ณ    CAUTIONS:    Use Routines at your own risk!                           ณ
 ณ                                                                          ณ
 ณ                                                                          ณ
 ณ    Special Thanks to Guy Scharf who helped me with unclear documentation ณ
 ณ    inside the Microsoft PM Toolkit 1.21 and IBM Toolkit 1.2. Guy Scharf  ณ
 ณ    can be reached on Compu$erve as ID: 76702,557                         ณ
 ิออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dlghigh.h"

MRESULT EXPENTRY ClientWndProc     (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY SelectDlgProc     (HWND, USHORT, MPARAM, MPARAM);
SHORT            FillListBox       (HWND);
SHORT            SubStr            (CHAR *, CHAR *);

CHAR       szClientClass[] = "Dlghigh";
HWND       hwndFrame;
PHWND      hwndClient;
BOOL       fDialogOpen = FALSE;

int main (void)
{
   static ULONG flFrameFlags = FCF_TITLEBAR        | FCF_SYSMENU   |
                               FCF_SIZEBORDER      | FCF_MINMAX    |
                               FCF_TASKLIST        | FCF_ACCELTABLE|
                               FCF_SHELLPOSITION   | FCF_MENU;
   
   HAB         hab; 
   HMQ         hmq;
   QMSG        qmsg;
   BOOL        fSuccess;
               
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);

   WinRegisterClass (
                   hab,
                   szClientClass,
                   ClientWndProc,
                   CS_SIZEREDRAW,
                   0);

   hwndFrame = WinCreateStdWindow (
                   HWND_DESKTOP,
                   WS_VISIBLE,
                   &flFrameFlags,
                   szClientClass,
                   NULL,
                   0,
                   0,
                   ID_DIALOG,
                   &hwndClient);

   fSuccess = WinSetWindowPos (
                   hwndFrame,
                   NULL,
                   0,
                   0,
                   0,
                   0,
                   SWP_MAXIMIZE);

   if (hwndFrame != NULL)
   {
       while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
           WinDispatchMsg (hab, &qmsg);

       WinDestroyWindow (hwndFrame);
       WinDestroyMsgQueue (hmq);
       WinTerminate (hab);
       return 0;
   }
}

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static HWND     hwndMenu;
   HPS             hps;

   switch (msg)
   {
       case WM_CREATE:

           hwndMenu = WinWindowFromID (
                       WinQueryWindow (hwnd, QW_PARENT, FALSE), FID_MENU);

           return 0;

       case WM_COMMAND:
           switch (COMMANDMSG(&msg)->cmd)
           {
               case IDM_OPENDLG:

                   if (!fDialogOpen)
                       WinDlgBox (
                               hwnd,
                               hwnd,
                               SelectDlgProc,
                               0,          
                               IDD_DIALOG,
                               NULL);
                   else
                       WinMessageBox (HWND_DESKTOP, hwnd,
                               "Dialog already open", "Dialog Error", 0,
                               MB_NOICON | MB_OK);


                   return 0;

               case IDM_EXIT:

                   if (MBID_OK == WinMessageBox (
                                   HWND_DESKTOP,
                                   hwndClient,
                                   "Really want to end program?",
                                   szClientClass,
                                   0,
                                   MB_OKCANCEL | MB_ICONQUESTION))
                       WinSendMsg (hwnd, WM_CLOSE, 0L, 0L);

                   return 0;
           }
           break;

       case WM_PAINT:
           hps = WinBeginPaint (hwnd, NULL, NULL);
           GpiErase (hps);
           WinEndPaint (hps);
           return 0;
   }
   return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

SHORT FillListBox (HWND hwnd)
{
   FILE        *ItemsFILE;
   CHAR        szItemText[78];
   SHORT       sDone = 0, sIsEOF;

   WinSendDlgItemMsg (hwnd, IDD_LIST, LM_DELETEALL, NULL, NULL);

   if ((ItemsFILE = fopen ("items.txt", "rb")) == NULL)
   {
       return 1;
   }

   while (!sDone)
   {
       fread (szItemText, 77, 1, ItemsFILE);
       sIsEOF = feof (ItemsFILE);
       if (sIsEOF)
       {
           sDone = TRUE;
           break;
       }
       szItemText [77] = '\0';
       WinSendDlgItemMsg (hwnd, IDD_LIST, LM_INSERTITEM, 0, szItemText);
   }
   fclose (ItemsFILE);
   return 0;
}

MRESULT EXPENTRY SelectDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   HPS             hps;                /* handle to presentation space */
   FONTMETRICS     fm;                 /* fontmetric structure         */
   CHAR            szItemText[80],     /* pointer to text for items    */
                   szTemp[45],         /* asciiz string for message box*/
                   szKeyword[]="SELL"; /* keyword to highlight         */
   OWNERITEM FAR   *poi;               /* Pointer to OWNERITEM struct  */
   RECTL           rcl;                /* Rectangle for WM_DRAWITEM    */
   COLOR           clrFG, clrBG;       /* Foreground/Background colors */
   SHORT           sError, sKeyWordPos;
   MRESULT         mrItem;
   static LONG     lAveChWidth;

   switch (msg)
   {
       case WM_INITDLG:    /* this message is sent after WM_MEASUREITEM
                            */

           sError = FillListBox (hwnd);    /* fill list box            */

           if (sError)                     /* if error, report         */
           {
               WinMessageBox (HWND_DESKTOP, hwnd,
                       "Can't open ITEMS.TXT File", "File I/O", 0,
                       MB_NOICON | MB_OK);
               WinDismissDlg (hwnd, TRUE);
           }
           else
               fDialogOpen = TRUE;

           break;

       case WM_MEASUREITEM:
           /*  this message comes previous to WM_INITDLG and needs
               to have the height of the dialog item you will be
               drawing. note i'm setting the fontsize previous to
               getting the presentation space. this is important
               or the values you will receive will be for the
               previous settings.
            */

           /*  this sets the font to 8 point Courier, you must
               have Courier Fonts on the computer to run this
               example.
            */
           WinSetPresParam (hwnd,
                           PP_FONTNAMESIZE,
                           24L,
                           (PVOID) "8.Courier");

           hps = WinGetPS (hwnd);          /* get presentation space   */
                                           
                                           /* get fontmetrics info     */
           GpiQueryFontMetrics (hps, (LONG) sizeof (FONTMETRICS), &fm);

                                           /* store width for later    */
           lAveChWidth = fm.lAveCharWidth;

           WinReleasePS (hps);             /* release pres space       */

           /* notice i'm adding 2 to this return value so in this
              example i have one extra row of pels on each side
              of the listbox item. i am centering the item when
              i'm drawing it (DT_VCENTER) so it will be easier
              to read this small font i'm using.
            */

           return ((MRESULT) (fm.lMaxBaselineExt + 2L));

       case WM_DRAWITEM:

           poi = mp2;                  /* pointer to owner item    */

           if (poi->fsState == TRUE)   /* should cell be highlited?*/
           {
               clrFG = SYSCLR_HILITEFOREGROUND;
               clrBG = SYSCLR_HILITEBACKGROUND;
           }
           else
           {
               clrFG = CLR_NEUTRAL;
               clrBG = CLR_BACKGROUND;
           }

           WinSendMsg (poi->hwnd,      /* get item text to write   */
                       LM_QUERYITEMTEXT,
                       (MPARAM) MAKEULONG (poi->idItem, 80),
                       (MPARAM) szItemText);

                                       /*  get coordinates of rect
                                           to write.
                                        */
           rcl.xLeft   = poi->rclItem.xLeft;
           rcl.xRight  = poi->rclItem.xRight;
           rcl.yTop    = poi->rclItem.yTop;
           rcl.yBottom = poi->rclItem.yBottom;

           WinDrawText (poi->hps,
                           -1,         /* null terminated string   */
                           szItemText, /* text to write            */
                           &rcl,       /* area to write text       */
                           clrFG,      /* foreground color         */
                           clrBG,      /* background color         */
                           DT_LEFT | DT_VCENTER | DT_ERASERECT);
                                       /* text drawing flags       */
           
                                       /*  search for keyword in
                                           text to highlite
                                        */
           sKeyWordPos = SubStr (szItemText, szKeyword);

           if (sKeyWordPos >= 0)       /* is keyword found ?       */
           {
               /*  adjust the rectangle for drawing the keyword
                */
               rcl.xLeft += sKeyWordPos * lAveChWidth;
               rcl.xRight = rcl.xLeft + (strlen (szKeyword) * lAveChWidth);

               /*  change foreground color to help highlited color to
                   highlite word found from SubStr ().
                */
               clrFG = SYSCLR_HELPHILITE;

               WinDrawText (poi->hps,
                               strlen (szKeyword), /*  length of text  */
                               szKeyword,          /* text to write    */
                               &rcl,               /* area to write    */
                               clrFG,              /* foreground color */
                               clrBG,              /* background color */
                               DT_LEFT | DT_VCENTER | DT_ERASERECT);
           }                                       /* text drawing flags */

           /*  these values *must* be set to FALSE to tell control
               we don't need this rectangle inverted since we have
               already taken care of it ourself.
            */
           poi->fsState = poi->fsStateOld = FALSE;

           return ((MRESULT) TRUE);

       case WM_CONTROL:
           switch (SHORT1FROMMP (mp1))
           {
               case IDD_LIST:
                   switch (SHORT2FROMMP (mp1))
                   {
                       case LN_ENTER:
                           /*  if we receive a LN_ENTER message
                               query the selection and display
                               selection in WinMessageBox
                            */
                           mrItem = WinSendDlgItemMsg (hwnd,
                                           SHORT1FROMMP (mp1),
                                           LM_QUERYSELECTION,
                                           0L,
                                           0L);
                           /* format the string with selection
                            */
                           sprintf (szTemp, "%s%d",
                                       "Index of selected item: ",
                                       SHORT1FROMMR (mrItem));

                           WinMessageBox (HWND_DESKTOP, hwnd,
                                       szTemp, "Item Selected", 0,
                                       MB_NOICON | MB_OK);

                       return 0;
                   }
                   break;
           }
           break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* this routine searches a string for a substring. it returns
 * the position of the substring in the string to allow the
 * reseting of the rectangle area for drawing. if you didn't
 * use a non-proportional font you could use DT_QUERYEXTENT
 * in a WinDrawText function above and adjust accordingly. I
 * prefer this method as proportional spaced fonts for long
 * strings do not look good to my eye and i wanted to have
 * the time on the items.
 */

SHORT SubStr (CHAR *pchString, CHAR *pchSubString)
{                                                                    
   SHORT   sCheckLen = 0, sLocation = 0,
           sSubStringLen = strlen (pchSubString);

   while (*pchString)
   {
      sCheckLen = 0;
      while (*(pchSubString + sCheckLen) == *(pchString + sCheckLen)
          && *(pchSubString + sCheckLen))
               sCheckLen++;
      if (sCheckLen == sSubStringLen)
               return (sLocation);
      sLocation++;
      pchString++;
   }
   return (-1);
}


