/////////////////////////////////////////////////////////////////////////////
// Name:        patched_dirctrlg.cpp
// Purpose:     PatchedGenericDirCtrl
// Author:      Harm van der Heijden, Robert Roebling, Julian Smart
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id: dirctrlg.cpp,v 1.48.2.1 2002/09/27 21:40:37 SN Exp $
// Copyright:   (c) Harm van der Heijden, Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "patched_dirctrlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_DIRDLG

#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/button.h"
#include "wx/layout.h"
#include "wx/msgdlg.h"
#include "wx/textctrl.h"
#include "wx/textdlg.h"
#include "wx/filefn.h"
#include "wx/cmndata.h"
#include "wx/gdicmn.h"
#include "wx/intl.h"
#include "wx/imaglist.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include "wx/tokenzr.h"
#include "wx/dir.h"
#include "wx/settings.h"

#if wxUSE_STATLINE
#include "wx/statline.h"
#endif

#include "patched_dirctrlg.h"

#if defined(__WXMAC__)
#include  "wx/mac/private.h"    // includes mac headers
#endif

#ifdef __WXMSW__
#include <windows.h>

// FIXME - Mingw32 1.0 has both _getdrive() and _chdrive(). For now, let's assume
//         older releases don't, but it should be verified and the checks modified
//         accordingly.
#if !defined(__WXWINE__) && (!defined(__GNUWIN32__) || \
    (defined(__MINGW32_MAJOR_VERSION) && __MINGW32_MAJOR_VERSION >= 1))
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>
#endif

#endif

#ifdef __WXPM__

#define INCL_BASE
#include <os2.h>
#ifndef __EMX__
#include <direct.h>
#endif
#include <stdlib.h>
#include <ctype.h>

#endif // __WXPM__

#if defined(__WXMAC__)
#  include "MoreFilesExtras.h"
#endif

#ifdef __BORLANDC__
#include "dos.h"
#endif

// If compiled under Windows, this macro can cause problems
#ifdef GetFirstChild
#undef GetFirstChild
#endif

/* Closed folder */
static const char *icon1_xpm[] = {
/* width height ncolors chars_per_pixel */
  "16 16 6 1",
/* colors */
  "   s None  c None",
  ".  c #000000",
  "+  c #c0c0c0",
  "@  c #808080",
  "#  c #ffff00",
  "$  c #ffffff",
/* pixels */
  "                ",
  "   @@@@@        ",
  "  @#+#+#@       ",
  " @#+#+#+#@@@@@@ ",
  " @$$$$$$$$$$$$@.",
  " @$#+#+#+#+#+#@.",
  " @$+#+#+#+#+#+@.",
  " @$#+#+#+#+#+#@.",
  " @$+#+#+#+#+#+@.",
  " @$#+#+#+#+#+#@.",
  " @$+#+#+#+#+#+@.",
  " @$#+#+#+#+#+#@.",
  " @@@@@@@@@@@@@@.",
  "  ..............",
  "                ",
  "                "
};

/* Open folder */
static const char *icon2_xpm[] = {
/* width height ncolors chars_per_pixel */
  "16 16 6 1",
/* colors */
  "   s None  c None",
  ".  c #000000",
  "+  c #c0c0c0",
  "@  c #808080",
  "#  c #ffff00",
  "$  c #ffffff",
/* pixels */
  "                ",
  "   @@@@@        ",
  "  @$$$$$@       ",
  " @$#+#+#$@@@@@@ ",
  " @$+#+#+$$$$$$@.",
  " @$#+#+#+#+#+#@.",
  "@@@@@@@@@@@@@#@.",
  "@$$$$$$$$$$@@+@.",
  "@$#+#+#+#+##.@@.",
  " @$#+#+#+#+#+.@.",
  " @$+#+#+#+#+#.@.",
  "  @$+#+#+#+##@..",
  "  @@@@@@@@@@@@@.",
  "   .............",
  "                ",
  "                "
};

/* File */
static const char *icon3_xpm[] = {
/* width height ncolors chars_per_pixel */
  "16 16 3 1",
/* colors */
  "     s None    c None",
  ".    c #000000",
  "+    c #ffffff",
/* pixels */
  "                ",
  "  ........      ",
  "  .++++++..     ",
  "  .+.+.++.+.    ",
  "  .++++++....   ",
  "  .+.+.+++++.   ",
  "  .+++++++++.   ",
  "  .+.+.+.+.+.   ",
  "  .+++++++++.   ",
  "  .+.+.+.+.+.   ",
  "  .+++++++++.   ",
  "  .+.+.+.+.+.   ",
  "  .+++++++++.   ",
  "  ...........   ",
  "                ",
  "                "
};

/* Computer */
static const char *icon4_xpm[] = {
  "16 16 7 1",
  "     s None    c None",
  ".    c #808080",
  "X    c #c0c0c0",
  "o    c Black",
  "O    c Gray100",
  "+    c #008080",
  "@    c Blue",
  "    ........... ",
  "   .XXXXXXXXXX.o",
  "   .OOOOOOOOO..o",
  "   .OoooooooX..o",
  "   .Oo+...@+X..o",
  "   .Oo+XXX.+X..o",
  "   .Oo+....+X..o",
  "   .Oo++++++X..o",
  "   .OXXXXXXXX.oo",
  "   ..........o.o",
  "   ...........Xo",
  "   .XXXXXXXXXX.o",
  "  .o.o.o.o.o...o",
  " .oXoXoXoXoXo.o ",
  ".XOXXXXXXXXX.o  ",
  "............o   "
};

/* Drive */
static const char *icon5_xpm[] = {
  "16 16 7 1",
  "     s None    c None",
  ".    c #808080",
  "X    c #c0c0c0",
  "o    c Black",
  "O    c Gray100",
  "+    c Green",
  "@    c #008000",
  "                ",
  "                ",
  "                ",
  "                ",
  "  ............. ",
  " .XXXXXXXXXXXX.o",
  ".OOOOOOOOOOOO..o",
  ".XXXXXXXXX+@X..o",
  ".XXXXXXXXXXXX..o",
  ".X..........X..o",
  ".XOOOOOOOOOOX..o",
  "..............o ",
  " ooooooooooooo  ",
  "                ",
  "                ",
  "                "
};

/* CD-ROM */
static const char *icon6_xpm[] = {
  "16 16 10 1",
  "     s None    c None",
  ".    c #808080",
  "X    c #c0c0c0",
  "o    c Yellow",
  "O    c Blue",
  "+    c Black",
  "@    c Gray100",
  "#    c #008080",
  "$    c Green",
  "%    c #008000",
  "        ...     ",
  "      ..XoX..   ",
  "     .O.XoXXX+  ",
  "    ...O.oXXXX+ ",
  "    .O..X.XXXX+ ",
  "   ....X.+..XXX+",
  "   .XXX.+@+.XXX+",
  "   .X@XX.+.X@@X+",
  " .....X...#XX@+ ",
  ".@@@...XXo.O@X+ ",
  ".@XXX..XXoXOO+  ",
  ".@++++..XoX+++  ",
  ".@$%@@XX+++X.+  ",
  ".............+  ",
  " ++++++++++++   ",
  "                "
};

/* Floppy */
static const char *icon7_xpm[] = {
  "16 16 7 1",
  "     s None    c None",
  ".    c #808080",
  "X    c Gray100",
  "o    c #c0c0c0",
  "O    c Black",
  "+    c Cyan",
  "@    c Red",
  "         ......X",
  "        .ooooooO",
  "        .+++++OO",
  "        .++++++O",
  "        .++++++O",
  "        .ooooooO",
  "  .......o....oO",
  " .oooooo.o.O.XoO",
  ".XXXXXXXXOOOOOO ",
  ".ooooooooo@o..O ",
  ".ooo....oooo..O ",
  ".o..OOOO...o..O ",
  ".oooXXXXoooo..O ",
  ".............O  ",
  " OOOOOOOOOOOO   ",
  "                "
};

/* Removeable */
static const char *icon8_xpm[] = {
  "16 16 7 1",
  "     s None    c None",
  ".    c #808080",
  "X    c #c0c0c0",
  "o    c Black",
  "O    c Gray100",
  "+    c Red",
  "@    c #800000",
  "                ",
  "                ",
  "                ",
  "  ............. ",
  " .XXXXXXXXXXXX.o",
  ".OOOOOOOOOOOO..o",
  ".OXXXXXXXXXXX..o",
  ".O+@.oooooo.X..o",
  ".OXXOooooooOX..o",
  ".OXXXOOOOOOXX..o",
  ".OXXXXXXXXXXX..o",
  ".O............o ",
  " ooooooooooooo  ",
  "                ",
  "                ",
  "                "
};


#if defined(__DOS__)

static bool
isDriveAvailable (const wxString & dirName)
{
  // FIXME_MGL - this method leads to hang up under Watcom for some reason
#ifndef __WATCOMC__
  if (dirName.Len () == 3 && dirName[1u] == wxT (':'))
  {
    wxString dirNameLower (dirName.Lower ());
    // VS: always return TRUE for removable media, since Win95 doesn't
    //     like it when MS-DOS app accesses empty floppy drive
    return (dirNameLower[0u] == wxT ('a') ||
            dirNameLower[0u] == wxT ('b') || wxPathExists (dirNameLower));
  }
  else
#endif
    return TRUE;
}

#elif defined(__WINDOWS__) || defined(__WXPM__)

static int
setdrive (int drive)
{
#if defined(__GNUWIN32__) && \
    (defined(__MINGW32_MAJOR_VERSION) && __MINGW32_MAJOR_VERSION >= 1)
  return _chdrive (drive);
#else
  wxChar newdrive[3];

  if (drive < 1 || drive > 31)
    return -1;
  newdrive[0] = (wxChar) (wxT ('A') + drive - 1);
  newdrive[1] = wxT (':');
  newdrive[2] = wxT ('\0');
#if defined(__WXMSW__)
#ifdef __WIN16__
  if (wxSetWorkingDirectory (newdrive))
#else
  if (::SetCurrentDirectory (newdrive))
#endif
#else
  // VA doesn't know what LPSTR is and has its own set
  if (DosSetCurrentDir ((PSZ) newdrive))
#endif
    return 0;
  else
    return -1;
#endif // !GNUWIN32
}

static bool
isDriveAvailable (const wxString & dirName)
{
#ifdef __WIN32__
  UINT errorMode =
    SetErrorMode (SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif
  bool success = TRUE;

  // Check if this is a root directory and if so,
  // whether the drive is avaiable.
  if (dirName.Len () == 3 && dirName[(size_t) 1] == wxT (':'))
  {
    wxString dirNameLower (dirName.Lower ());
#if defined(__WXWINE__) || (defined(__GNUWIN32__) && \
    !(defined(__MINGW32_MAJOR_VERSION) && __MINGW32_MAJOR_VERSION >= 1))
    success = wxPathExists (dirNameLower);
#else
    int currentDrive = _getdrive ();
    int thisDrive = (int) (dirNameLower[(size_t) 0] - 'a' + 1);
    int err = setdrive (thisDrive);
    setdrive (currentDrive);

    if (err == -1)
    {
      success = FALSE;
    }
#endif
  }
#ifdef __WIN32__
  (void) SetErrorMode (errorMode);
#endif

  return success;
}
#endif // __WINDOWS__ || __WXPM__

// Function which is called by quick sort. We want to override the default wxArrayString behaviour,
// and sort regardless of case.
static int
dirCtrlStringCompareFunction (const void *first, const void *second)
{
  wxString *strFirst = (wxString *) first;
  wxString *strSecond = (wxString *) second;

  return strFirst->CmpNoCase (*strSecond);
}

//-----------------------------------------------------------------------------
// PatchedDirItemData
//-----------------------------------------------------------------------------

PatchedDirItemData::PatchedDirItemData (const wxString & path,
                                        const wxString & name, bool isDir)
{
  m_path = path;
  m_name = name;
  /* Insert logic to detect hidden files here
   * In UnixLand we just check whether the first char is a dot
   * For FileNameFromPath read LastDirNameInThisPath ;-) */
  // m_isHidden = (bool)(wxFileNameFromPath(*m_path)[0] == '.');
  m_isHidden = FALSE;
  m_isExpanded = FALSE;
  m_isDir = isDir;
}

PatchedDirItemData::~PatchedDirItemData ()
{
}

void
PatchedDirItemData::SetNewDirName (const wxString & path)
{
  m_path = path;
  m_name = wxFileNameFromPath (path);
}

bool
PatchedDirItemData::HasSubDirs () const
{
  if (m_path.IsEmpty ())
    return FALSE;

  wxDir dir;
  {
    wxLogNull nolog;
    if (!dir.Open (m_path))
      return FALSE;
  }

  return dir.HasSubDirs ();
}

bool
PatchedDirItemData::HasFiles (const wxString & WXUNUSED (spec)) const
{
  if (m_path.IsEmpty ())
    return FALSE;

  wxDir dir;
  {
    wxLogNull nolog;
    if (!dir.Open (m_path))
      return FALSE;
  }

  return dir.HasFiles ();
}

//-----------------------------------------------------------------------------
// PatchedGenericDirCtrl
//-----------------------------------------------------------------------------

     IMPLEMENT_DYNAMIC_CLASS (PatchedGenericDirCtrl, wxControl)
BEGIN_EVENT_TABLE (PatchedGenericDirCtrl, wxControl)
EVT_TREE_ITEM_EXPANDING (-1, PatchedGenericDirCtrl::OnExpandItem)
EVT_TREE_ITEM_COLLAPSED (-1, PatchedGenericDirCtrl::OnCollapseItem)
EVT_TREE_BEGIN_LABEL_EDIT (-1, PatchedGenericDirCtrl::OnBeginEditItem)
EVT_TREE_END_LABEL_EDIT (-1, PatchedGenericDirCtrl::OnEndEditItem)
EVT_SIZE (PatchedGenericDirCtrl::OnSize) END_EVENT_TABLE () 

PatchedGenericDirCtrl::PatchedGenericDirCtrl (void)
{
  Init ();
}

bool
PatchedGenericDirCtrl::Create (wxWindow * parent,
                               const wxWindowID id,
                               const wxString & dir,
                               const wxPoint & pos,
                               const wxSize & size,
                               long style,
                               const wxString & filter,
                               int defaultFilter, const wxString & name)
{
  if (!wxControl::
      Create (parent, id, pos, size, style, wxDefaultValidator, name))
    return FALSE;

  SetBackgroundColour (wxSystemSettings::GetColour (wxSYS_COLOUR_3DFACE));

  Init ();

  long treeStyle = wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT;

  if (style & PATCHED_DIRCTRL_EDIT_LABELS)
    treeStyle |= wxTR_EDIT_LABELS;

  if ((style & PATCHED_DIRCTRL_3D_INTERNAL) == 0)
    treeStyle |= wxNO_BORDER;

  long filterStyle = 0;
  if ((style & PATCHED_DIRCTRL_3D_INTERNAL) == 0)
    filterStyle |= wxNO_BORDER;

  m_treeCtrl = new wxTreeCtrl (this, wxID_TREECTRL, pos, size, treeStyle);

  if (!filter.IsEmpty () && (style & PATCHED_DIRCTRL_SHOW_FILTERS))
    m_filterListCtrl =
      new PatchedDirFilterListCtrl (this, wxID_FILTERLISTCTRL,
                                    wxDefaultPosition, wxDefaultSize,
                                    filterStyle);

  m_defaultPath = dir;
  m_filter = filter;

  SetFilterIndex (defaultFilter);

  if (m_filterListCtrl)
    m_filterListCtrl->FillFilterList (filter, defaultFilter);

  m_imageList = new wxImageList (16, 16, TRUE);
  m_imageList->Add (wxIcon (icon1_xpm));
  m_imageList->Add (wxIcon (icon2_xpm));
  m_imageList->Add (wxIcon (icon3_xpm));
  m_imageList->Add (wxIcon (icon4_xpm));
  m_imageList->Add (wxIcon (icon5_xpm));
  m_imageList->Add (wxIcon (icon6_xpm));
  m_imageList->Add (wxIcon (icon7_xpm));
  m_imageList->Add (wxIcon (icon8_xpm));
  m_treeCtrl->AssignImageList (m_imageList);

  m_showHidden = FALSE;
  PatchedDirItemData *rootData =
    new PatchedDirItemData (wxT (""), wxT (""), TRUE);

  wxString rootName;

#if defined(__WINDOWS__) || defined(__WXPM__) || defined(__DOS__)
  rootName = _("Computer");
#else
  rootName = _("Sections");
#endif

  m_rootId = m_treeCtrl->AddRoot (rootName, 3, -1, rootData);
  m_treeCtrl->SetItemHasChildren (m_rootId);
  ExpandDir (m_rootId);         // automatically expand first level

  // Expand and select the default path
  if (!m_defaultPath.IsEmpty ())
    ExpandPath (m_defaultPath);

  DoResize ();

  return TRUE;
}

PatchedGenericDirCtrl::~PatchedGenericDirCtrl ()
{
}

void
PatchedGenericDirCtrl::Init ()
{
  m_showHidden = FALSE;
  m_imageList = NULL;
  m_currentFilter = 0;
  m_currentFilterStr = wxEmptyString;   // Default: any file
  m_treeCtrl = NULL;
  m_filterListCtrl = NULL;
}

void
PatchedGenericDirCtrl::ShowHidden (bool show)
{
  m_showHidden = show;

  wxString path = GetPath ();
  ReCreateTree ();
  SetPath (path);
}

const wxTreeItemId
PatchedGenericDirCtrl::AddSection (const wxString & path,
                                   const wxString & name, int imageId)
{
  PatchedDirItemData *dir_item = new PatchedDirItemData (path, name, TRUE);

  const wxTreeItemId id = AppendItem (m_rootId, name, imageId, -1, dir_item);

  m_treeCtrl->SetItemHasChildren (id);

  return id;
}

void
PatchedGenericDirCtrl::SetupSections ()
{
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__WXPM__)

#ifdef __WIN32__
  wxChar driveBuffer[256];
  size_t n = (size_t) GetLogicalDriveStrings (255, driveBuffer);
  size_t i = 0;
  while (i < n)
  {
    wxString path, name;
    path.Printf (wxT ("%c:\\"), driveBuffer[i]);
    name.Printf (wxT ("(%c:)"), driveBuffer[i]);

    int imageId = 4;
    int driveType =::GetDriveType (path);
    switch (driveType)
    {
    case DRIVE_REMOVABLE:
      if (path == wxT ("a:\\") || path == wxT ("b:\\"))
        imageId = 6;            // Floppy
      else
        imageId = 7;
      break;
    case DRIVE_FIXED:
      imageId = 4;
      break;
    case DRIVE_REMOTE:
      imageId = 4;
      break;
    case DRIVE_CDROM:
      imageId = 5;
      break;
    default:
      imageId = 4;
      break;
    }

    AddSection (path, name, imageId);

    while (driveBuffer[i] != wxT ('\0'))
      i++;
    i++;
    if (driveBuffer[i] == wxT ('\0'))
      break;
  }
#else // !__WIN32__
  int drive;

  /* If we can switch to the drive, it exists. */
  for (drive = 1; drive <= 26; drive++)
  {
    wxString path, name;
    path.Printf (wxT ("%c:\\"), (char) (drive + 'a' - 1));
    name.Printf (wxT ("(%c:)"), (char) (drive + 'A' - 1));

    if (isDriveAvailable (path))
    {
      AddSection (path, name, (drive <= 2) ? 6 /*floppy */ : 4 /*disk */ );
    }
  }
#endif // __WIN32__/!__WIN32__

#elif defined(__WXMAC__)
  FSSpec volume;
  short index = 1;
  while (1)
  {
    short actualCount = 0;
    if (OnLine (&volume, 1, &actualCount, &index) != noErr
        || actualCount == 0)
      break;

    wxString name = wxMacFSSpec2MacFilename (&volume);
    AddSection (name + wxFILE_SEP_PATH, name, 0);
  }
#elif defined(__UNIX__)
  AddSection (wxT ("/"), wxT ("/"), 3 /*computer icon */ );
#else
#error "Unsupported platform in PatchedGenericDirCtrl!"
#endif
}

void
PatchedGenericDirCtrl::OnBeginEditItem (wxTreeEvent & event)
{
  // don't rename the main entry "Sections"
  if (event.GetItem () == m_rootId)
  {
    event.Veto ();
    return;
  }

  // don't rename the individual sections
  if (m_treeCtrl->GetParent (event.GetItem ()) == m_rootId)
  {
    event.Veto ();
    return;
  }
}

void
PatchedGenericDirCtrl::OnEndEditItem (wxTreeEvent & event)
{
  if ((event.GetLabel ().IsEmpty ()) ||
      (event.GetLabel () == _(".")) ||
      (event.GetLabel () == _("..")) ||
      (event.GetLabel ().First (wxT ("/")) != wxNOT_FOUND))
  {
    wxMessageDialog dialog (this, _("Illegal directory name."), _("Error"),
                            wxOK | wxICON_ERROR);
    dialog.ShowModal ();
    event.Veto ();
    return;
  }

  wxTreeItemId id = event.GetItem ();
  PatchedDirItemData *data =
    (PatchedDirItemData *) m_treeCtrl->GetItemData (id);
  wxASSERT (data);

  wxString new_name (wxPathOnly (data->m_path));
  new_name += wxString (wxFILE_SEP_PATH);
  new_name += event.GetLabel ();

  wxLogNull log;

  if (wxFileExists (new_name))
  {
    wxMessageDialog dialog (this, _("File name exists already."), _("Error"),
                            wxOK | wxICON_ERROR);
    dialog.ShowModal ();
    event.Veto ();
  }

  if (wxRenameFile (data->m_path, new_name))
  {
    data->SetNewDirName (new_name);
  }
  else
  {
    wxMessageDialog dialog (this, _("Operation not permitted."), _("Error"),
                            wxOK | wxICON_ERROR);
    dialog.ShowModal ();
    event.Veto ();
  }
}

void
PatchedGenericDirCtrl::OnExpandItem (wxTreeEvent & event)
{
  wxTreeItemId parentId = event.GetItem ();

  // VS: this is needed because the event handler is called from wxTreeCtrl
  //     ctor when wxTR_HIDE_ROOT was specified
  if (m_rootId == 0)
    m_rootId = m_treeCtrl->GetRootItem ();

  ExpandDir (parentId);
}

void
PatchedGenericDirCtrl::OnCollapseItem (wxTreeEvent & event)
{
  CollapseDir (event.GetItem ());
}

void
PatchedGenericDirCtrl::CollapseDir (const wxTreeItemId & parentId)
{
  wxTreeItemId child;

  PatchedDirItemData *data =
    (PatchedDirItemData *) m_treeCtrl->GetItemData (parentId);
  if (!data)
    return;

  if (!data->m_isExpanded)
    return;

  data->m_isExpanded = FALSE;
  long cookie;
  /* Workaround because DeleteChildren has disapeared (why?) and
   * CollapseAndReset doesn't work as advertised (deletes parent too) */
  child = m_treeCtrl->GetFirstChild (parentId, cookie);
  while (child.IsOk ())
  {
    m_treeCtrl->Delete (child);
    /* Not GetNextChild below, because the cookie mechanism can't
     * handle disappearing children! */
    child = m_treeCtrl->GetFirstChild (parentId, cookie);
  }
}

void
PatchedGenericDirCtrl::ExpandDir (const wxTreeItemId & parentId)
{
  PatchedDirItemData *data =
    (PatchedDirItemData *) m_treeCtrl->GetItemData (parentId);

  if (data->m_isExpanded)
    return;

  data->m_isExpanded = TRUE;

  if (parentId == m_treeCtrl->GetRootItem ())
  {
    SetupSections ();
    return;
  }

  wxASSERT (data);

  wxString search, path, filename;

  wxString dirName (data->m_path);

#if defined(__WINDOWS__) || defined(__DOS__) || defined(__WXPM__)
  // Check if this is a root directory and if so,
  // whether the drive is avaiable.
  if (!isDriveAvailable (dirName))
  {
    data->m_isExpanded = FALSE;
    //wxMessageBox(wxT("Sorry, this drive is not available."));
    return;
  }
#endif

  // This may take a longish time. Go to busy cursor
  wxBusyCursor busy;

#if defined(__WINDOWS__) || defined(__DOS__) || defined(__WXPM__)
  if (dirName.Last () == ':')
    dirName += wxString (wxFILE_SEP_PATH);
#endif

  wxArrayString dirs;
  wxArrayString filenames;

  wxDir d;
  wxString eachFilename;

  wxLogNull log;
  d.Open (dirName);

  if (d.IsOpened ())
  {
    int style = wxDIR_DIRS;
    if (m_showHidden)
      style |= wxDIR_HIDDEN;
    if (d.GetFirst (&eachFilename, wxEmptyString, style))
    {
      do
      {
        if ((eachFilename != wxT (".")) && (eachFilename != wxT ("..")))
        {
          dirs.Add (eachFilename);
        }
      }
      while (d.GetNext (&eachFilename));
    }
  }
  dirs.Sort ((wxArrayString::CompareFunction) dirCtrlStringCompareFunction);

  // Now do the filenames -- but only if we're allowed to
  if ((GetWindowStyle () & PATCHED_DIRCTRL_DIR_ONLY) == 0)
  {
    wxLogNull log;

    d.Open (dirName);

    if (d.IsOpened ())
    {
      if (d.GetFirst (&eachFilename, m_currentFilterStr, wxDIR_FILES))
      {
        do
        {
          if ((eachFilename != wxT (".")) && (eachFilename != wxT ("..")))
          {
            filenames.Add (eachFilename);
          }
        }
        while (d.GetNext (&eachFilename));
      }
    }
    filenames.
      Sort ((wxArrayString::CompareFunction) dirCtrlStringCompareFunction);
  }

  // Add the sorted dirs
  size_t i;
  for (i = 0; i < dirs.Count (); i++)
  {
    wxString eachFilename (dirs[i]);
    path = dirName;
    if (path.Last () != wxFILE_SEP_PATH)
      path += wxString (wxFILE_SEP_PATH);
    path += eachFilename;

    PatchedDirItemData *dir_item =
      new PatchedDirItemData (path, eachFilename, TRUE);
    wxTreeItemId id = AppendItem (parentId, eachFilename, 0, -1, dir_item);
    m_treeCtrl->SetItemImage (id, 1, wxTreeItemIcon_Expanded);

    // Has this got any children? If so, make it expandable.
    // (There are two situations when a dir has children: either it
    // has subdirectories or it contains files that weren't filtered
    // out. The latter only applies to dirctrl with files.)
    if (dir_item->HasSubDirs () ||
        (((GetWindowStyle () & PATCHED_DIRCTRL_DIR_ONLY) == 0) &&
         dir_item->HasFiles (m_currentFilterStr)))
    {
      m_treeCtrl->SetItemHasChildren (id);
    }
  }

  // Add the sorted filenames
  if ((GetWindowStyle () & PATCHED_DIRCTRL_DIR_ONLY) == 0)
  {
    for (i = 0; i < filenames.Count (); i++)
    {
      wxString eachFilename (filenames[i]);
      path = dirName;
      if (path.Last () != wxFILE_SEP_PATH)
        path += wxString (wxFILE_SEP_PATH);
      path += eachFilename;
      //path = dirName + wxString(wxT("/")) + eachFilename;
      PatchedDirItemData *dir_item =
        new PatchedDirItemData (path, eachFilename, FALSE);
      (void) AppendItem (parentId, eachFilename, 2, -1, dir_item);
    }
  }
}

void
PatchedGenericDirCtrl::ReCreateTree ()
{
  CollapseDir (m_treeCtrl->GetRootItem ());
  ExpandDir (m_treeCtrl->GetRootItem ());
}

// Find the child that matches the first part of 'path'.
// E.g. if a child path is "/usr" and 'path' is "/usr/include"
// then the child for /usr is returned.
wxTreeItemId
PatchedGenericDirCtrl::FindChild (wxTreeItemId parentId,
                                  const wxString & path, bool & done)
{
  wxString path2 (path);

  // Make sure all separators are as per the current platform
  path2.Replace (wxT ("\\"), wxString (wxFILE_SEP_PATH));
  path2.Replace (wxT ("/"), wxString (wxFILE_SEP_PATH));

  // Append a separator to foil bogus substring matching
  path2 += wxString (wxFILE_SEP_PATH);

  // In MSW or PM, case is not significant
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__WXPM__)
  path2.MakeLower ();
#endif

  long cookie;
  wxTreeItemId childId = m_treeCtrl->GetFirstChild (parentId, cookie);
  while (childId.IsOk ())
  {
    PatchedDirItemData *data =
      (PatchedDirItemData *) m_treeCtrl->GetItemData (childId);

    if (data && !data->m_path.IsEmpty ())
    {
      wxString childPath (data->m_path);
      if (childPath.Last () != wxFILE_SEP_PATH)
        childPath += wxString (wxFILE_SEP_PATH);

      // In MSW and PM, case is not significant
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__WXPM__)
      childPath.MakeLower ();
#endif

      if (childPath.Len () <= path2.Len ())
      {
        wxString path3 = path2.Mid (0, childPath.Len ());
        if (childPath == path3)
        {
          if (path3.Len () == path2.Len ())
            done = TRUE;
          else
            done = FALSE;
          return childId;
        }
      }
    }

    childId = m_treeCtrl->GetNextChild (parentId, cookie);
  }
  wxTreeItemId invalid;
  return invalid;
}

// Try to expand as much of the given path as possible,
// and select the given tree item.
bool
PatchedGenericDirCtrl::ExpandPath (const wxString & path)
{
  bool done = FALSE;
  wxTreeItemId id = FindChild (m_rootId, path, done);
  wxTreeItemId lastId = id;     // The last non-zero id
  while (id.IsOk () && !done)
  {
    ExpandDir (id);

    id = FindChild (id, path, done);
    if (id.IsOk ())
      lastId = id;
  }
  if (lastId.IsOk ())
  {
    PatchedDirItemData *data =
      (PatchedDirItemData *) m_treeCtrl->GetItemData (lastId);
    if (data->m_isDir)
    {
      m_treeCtrl->Expand (lastId);
    }
    if ((GetWindowStyle () & PATCHED_DIRCTRL_SELECT_FIRST) && data->m_isDir)
    {
      // Find the first file in this directory
      long cookie;
      wxTreeItemId childId = m_treeCtrl->GetFirstChild (lastId, cookie);
      bool selectedChild = FALSE;
      while (childId.IsOk ())
      {
        PatchedDirItemData *data =
          (PatchedDirItemData *) m_treeCtrl->GetItemData (childId);

        if (data && data->m_path != wxT ("") && !data->m_isDir)
        {
          m_treeCtrl->SelectItem (childId);
          m_treeCtrl->EnsureVisible (childId);
          selectedChild = TRUE;
          break;
        }
        childId = m_treeCtrl->GetNextChild (lastId, cookie);
      }
      if (!selectedChild)
      {
        m_treeCtrl->SelectItem (lastId);
        m_treeCtrl->EnsureVisible (lastId);
      }
    }
    else
    {
      m_treeCtrl->SelectItem (lastId);
      m_treeCtrl->EnsureVisible (lastId);
    }

    return TRUE;
  }
  else
    return FALSE;
}

wxString
PatchedGenericDirCtrl::GetPath () const
{
  wxTreeItemId id = m_treeCtrl->GetSelection ();
  if (id)
  {
    PatchedDirItemData *data =
      (PatchedDirItemData *) m_treeCtrl->GetItemData (id);
    return data->m_path;
  }
  else
    return wxEmptyString;
}

wxString
PatchedGenericDirCtrl::GetFilePath () const
{
  wxTreeItemId id = m_treeCtrl->GetSelection ();
  if (id)
  {
    PatchedDirItemData *data =
      (PatchedDirItemData *) m_treeCtrl->GetItemData (id);
    if (data->m_isDir)
      return wxEmptyString;
    else
      return data->m_path;
  }
  else
    return wxEmptyString;
}

void
PatchedGenericDirCtrl::SetPath (const wxString & path)
{
  m_defaultPath = path;
  if (m_rootId)
    ExpandPath (path);
}

// Not used
#if 0
void
PatchedGenericDirCtrl::FindChildFiles (wxTreeItemId id, int dirFlags,
                                       wxArrayString & filenames)
{
  PatchedDirItemData *data =
    (PatchedDirItemData *) m_treeCtrl->GetItemData (id);

  // This may take a longish time. Go to busy cursor
  wxBusyCursor busy;

  wxASSERT (data);

  wxString search, path, filename;

  wxString dirName (data->m_path);

#if defined(__WXMSW__) || defined(__WXPM__)
  if (dirName.Last () == ':')
    dirName += wxString (wxFILE_SEP_PATH);
#endif

  wxDir d;
  wxString eachFilename;

  wxLogNull log;
  d.Open (dirName);

  if (d.IsOpened ())
  {
    if (d.GetFirst (&eachFilename, m_currentFilterStr, dirFlags))
    {
      do
      {
        if ((eachFilename != wxT (".")) && (eachFilename != wxT ("..")))
        {
          filenames.Add (eachFilename);
        }
      }
      while (d.GetNext (&eachFilename));
    }
  }
}
#endif

void
PatchedGenericDirCtrl::SetFilterIndex (int n)
{
  m_currentFilter = n;

  wxString f, d;
  if (ExtractWildcard (m_filter, n, f, d))
    m_currentFilterStr = f;
  else
    m_currentFilterStr = wxT ("*.*");
}

void
PatchedGenericDirCtrl::SetFilter (const wxString & filter)
{
  m_filter = filter;

  wxString f, d;
  if (ExtractWildcard (m_filter, m_currentFilter, f, d))
    m_currentFilterStr = f;
  else
    m_currentFilterStr = wxT ("*.*");
}

// Extract description and actual filter from overall filter string
bool
PatchedGenericDirCtrl::ExtractWildcard (const wxString & filterStr, int n,
                                        wxString & filter,
                                        wxString & description)
{
  wxArrayString filters, descriptions;
  int count = ParseFilter (filterStr, filters, descriptions);
  if (count > 0 && n < count)
  {
    filter = filters[n];
    description = descriptions[n];
    return TRUE;
  }

  return FALSE;
}

// Parses the global filter, returning the number of filters.
// Returns 0 if none or if there's a problem.
// filterStr is in the form:
//
// "All files (*.*)|*.*|JPEG Files (*.jpeg)|*.jpg"

int
PatchedGenericDirCtrl::ParseFilter (const wxString & filterStr,
                                    wxArrayString & filters,
                                    wxArrayString & descriptions)
{
  wxString str (filterStr);

  wxString description, filter;
  int pos;
  bool finished = FALSE;
  do
  {
    pos = str.Find (wxT ('|'));
    if (pos == -1)
      return 0;                 // Problem
    description = str.Left (pos);
    str = str.Mid (pos + 1);
    pos = str.Find (wxT ('|'));
    if (pos == -1)
    {
      filter = str;
      finished = TRUE;
    }
    else
    {
      filter = str.Left (pos);
      str = str.Mid (pos + 1);
    }
    descriptions.Add (description);
    filters.Add (filter);
  }
  while (!finished);

  return filters.Count ();
}

void
PatchedGenericDirCtrl::DoResize ()
{
  wxSize sz = GetClientSize ();
  int verticalSpacing = 3;
  if (m_treeCtrl)
  {
    wxSize filterSz;
    if (m_filterListCtrl)
    {
#ifdef __WXMSW__
      // For some reason, this is required in order for the
      // correct control height to always be returned, rather
      // than the drop-down list height which is sometimes returned.
      wxSize oldSize = m_filterListCtrl->GetSize ();
      m_filterListCtrl->SetSize (-1, -1, oldSize.x + 10, -1,
                                 wxSIZE_USE_EXISTING);
      m_filterListCtrl->SetSize (-1, -1, oldSize.x, -1, wxSIZE_USE_EXISTING);
#endif
      filterSz = m_filterListCtrl->GetSize ();
      sz.y -= (filterSz.y + verticalSpacing);
    }
    m_treeCtrl->SetSize (0, 0, sz.x, sz.y);
    if (m_filterListCtrl)
    {
      m_filterListCtrl->SetSize (0, sz.y + verticalSpacing, sz.x, filterSz.y);
      // Don't know why, but this needs refreshing after a resize (wxMSW)
      m_filterListCtrl->Refresh ();
    }
  }
}


void
PatchedGenericDirCtrl::OnSize (wxSizeEvent & WXUNUSED (event))
{
  DoResize ();
}

wxTreeItemId
PatchedGenericDirCtrl::AppendItem (const wxTreeItemId & parent,
                                   const wxString & text,
                                   int image = -1, int selectedImage = -1,
                                   wxTreeItemData * data = NULL)
{
  wxTreeCtrl *treeCtrl = GetTreeCtrl ();

  wxASSERT (treeCtrl);

  if (treeCtrl)
  {
    return treeCtrl->AppendItem (parent, text, image, selectedImage, data);
  }
  else
  {
    return wxTreeItemId((long)0);
  }
}

//-----------------------------------------------------------------------------
// PatchedDirFilterListctrl
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS (PatchedDirFilterListCtrl, wxChoice)
BEGIN_EVENT_TABLE (PatchedDirFilterListCtrl, wxChoice)
EVT_CHOICE (-1, PatchedDirFilterListCtrl::OnSelFilter) END_EVENT_TABLE ()
     bool
     PatchedDirFilterListCtrl::Create (PatchedGenericDirCtrl * parent,
                                       const wxWindowID id,
                                       const wxPoint & pos,
                                       const wxSize & size, long style)
{
  m_dirCtrl = parent;
  return wxChoice::Create (parent, id, pos, size, 0, NULL, style);
}

void
PatchedDirFilterListCtrl::Init ()
{
  m_dirCtrl = NULL;
}

void
PatchedDirFilterListCtrl::OnSelFilter (wxCommandEvent & WXUNUSED (event))
{
  int sel = GetSelection ();

  wxString currentPath = m_dirCtrl->GetPath ();

  m_dirCtrl->SetFilterIndex (sel);

  // If the filter has changed, the view is out of date, so
  // collapse the tree.
  m_dirCtrl->ReCreateTree ();

  // Try to restore the selection, or at least the directory
  m_dirCtrl->ExpandPath (currentPath);
}

void
PatchedDirFilterListCtrl::FillFilterList (const wxString & filter,
                                          int defaultFilter)
{
  Clear ();
  wxArrayString descriptions, filters;
  size_t n = (size_t) m_dirCtrl->ParseFilter (filter, filters, descriptions);

  if (n > 0 && defaultFilter < (int) n)
  {
    size_t i = 0;
    for (i = 0; i < n; i++)
      Append (descriptions[i]);
    SetSelection (defaultFilter);
  }
}

#endif // wxUSE_DIRDLG
