// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2009-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//  Copyright (C) 2015-2016 Gunter Königsmann     <wxMaxima@physikbuch.de>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/*! \file
  This file declares the class ContentAssistantPopup.

  The content assistant offers more functionality than AutocompletePopup but 
  only works on systems that allow popups to handle key presses.
*/

#ifndef CONTENTASSISTANTPOPUP_H
#define CONTENTASSISTANTPOPUP_H

#include "Autocomplete.h"
#include "EditorCell.h"
#include <wx/popupwin.h>

//! The maximum number of popup menu entries we show at the same time
#define AC_MENU_LENGTH 25

class ContentAssistantPopup : public wxPopupTransientWindow
{
private:
  wxArrayString m_completions;
  AutoComplete *m_autocomplete;
  size_t m_length;
  EditorCell *m_editor;
  AutoComplete::autoCompletionType m_type;
  wxListBox *m_autocompletions;
public:
  ContentAssistantPopup(wxWindow *parent, EditorCell* editor, AutoComplete *autocomplete, AutoComplete::autoCompletionType type);
  void UpdateResults();
  void OnKeyPress(wxKeyEvent& event);
  void OnClick(wxCommandEvent& event);
};

#endif // CONTENTASSISTANTPOPUP_H
