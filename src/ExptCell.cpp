// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2014-2016 Gunter Königsmann <wxMaxima@physikbuch.de>
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
  This file defines the class ExptCell

  ExptCell is the MathCell type that represents exponents.
 */

#include "ExptCell.h"
#include "TextCell.h"

#define EXPT_DEC 2

ExptCell::ExptCell() : MathCell()
{
  m_last1=NULL;
  m_last2=NULL;
  m_baseCell = NULL;
  m_powCell = NULL;
  m_isMatrix = false;
  m_exp = new TextCell(wxT("^"));
  m_open = new TextCell(wxT("("));
  m_close = new TextCell(wxT(")"));
}

ExptCell::~ExptCell()
{
  if (m_baseCell != NULL)
    delete m_baseCell;
  if (m_powCell != NULL)
    delete m_powCell;
  if (m_next != NULL)
    delete m_next;
  delete m_exp;
  delete m_open;
  delete m_close;
}

void ExptCell::SetParent(MathCell *parent)
{
  m_group = parent;
  if (m_baseCell != NULL)
    m_baseCell->SetParentList(parent);
  if (m_powCell != NULL)
    m_powCell->SetParentList(parent);
  if (m_open != NULL)
    m_open->SetParentList(parent);
  if (m_close != NULL)
    m_close->SetParentList(parent);
}

MathCell* ExptCell::Copy()
{
  ExptCell* tmp = new ExptCell;
  CopyData(this, tmp);
  tmp->SetBase(m_baseCell->CopyList());
  tmp->SetPower(m_powCell->CopyList());

  return tmp;
}

void ExptCell::Destroy()
{
  if (m_baseCell != NULL)
    delete m_baseCell;
  if (m_powCell != NULL)
    delete m_powCell;
  m_baseCell = NULL;
  m_powCell = NULL;
  m_next = NULL;
}

void ExptCell::SetPower(MathCell *power)
{
  if (power == NULL)
    return ;
  if (m_powCell != NULL)
    delete m_powCell;
  m_powCell = power;

  if (!m_powCell->IsCompound())
  {
    m_open->m_isHidden = true;
    m_close->m_isHidden = true;
  }

  m_last2 = power;
  if(m_last2 != NULL)
    while (m_last2->m_next != NULL)
      m_last2 = m_last2->m_next;
}

void ExptCell::SetBase(MathCell *base)
{
  if (base == NULL)
    return ;
  if (m_baseCell != NULL)
    delete m_baseCell;
  m_baseCell = base;

  m_last1 = base;
  if(m_last1 != NULL)
    while (m_last1->m_next != NULL)
      m_last1 = m_last1->m_next;
}

void ExptCell::RecalculateWidths(CellParser& parser, int fontsize)
{
  double scale = parser.GetScale();
  m_baseCell->RecalculateWidthsList(parser, fontsize);
  if (m_isBroken)
    m_powCell->RecalculateWidthsList(parser, fontsize);
  else
    m_powCell->RecalculateWidthsList(parser, MAX(MC_MIN_SIZE, fontsize - EXPT_DEC));
  m_width = m_baseCell->GetFullWidth(scale) + m_powCell->GetFullWidth(scale) -
            SCALE_PX(MC_TEXT_PADDING, scale);
  m_exp->RecalculateWidthsList(parser, fontsize);
  m_open->RecalculateWidthsList(parser, fontsize);
  m_close->RecalculateWidthsList(parser, fontsize);
  ResetData();
}

void ExptCell::RecalculateSize(CellParser& parser, int fontsize)
{
  double scale = parser.GetScale();
  m_baseCell->RecalculateSizeList(parser, fontsize);
  if (m_isBroken)
    m_powCell->RecalculateSizeList(parser, fontsize);
  else
    m_powCell->RecalculateSizeList(parser, MAX(MC_MIN_SIZE, fontsize - EXPT_DEC));
  m_height = m_baseCell->GetMaxHeight() + m_powCell->GetMaxHeight() -
             SCALE_PX((8 * fontsize) / 10 + MC_EXP_INDENT, scale);
  m_center = m_powCell->GetMaxHeight() + m_baseCell->GetMaxCenter() -
             SCALE_PX((8 * fontsize) / 10 + MC_EXP_INDENT, scale);
  m_exp->RecalculateSizeList(parser, fontsize);
  m_open->RecalculateSizeList(parser, fontsize);
  m_close->RecalculateSizeList(parser, fontsize);
}

void ExptCell::Draw(CellParser& parser, wxPoint point, int fontsize)
{
  MathCell::Draw(parser, point, fontsize);

  if (DrawThisCell(parser, point) && InUpdateRegion())
  {
    double scale = parser.GetScale();
    wxPoint bs, pw;
    bs.x = point.x;
    bs.y = point.y;
    m_baseCell->DrawList(parser, bs, fontsize);

    pw.x = point.x + m_baseCell->GetFullWidth(scale) - SCALE_PX(MC_TEXT_PADDING, scale);
    pw.y = point.y - m_baseCell->GetMaxCenter() - m_powCell->GetMaxHeight()
           + m_powCell->GetMaxCenter() +
           SCALE_PX((8 * fontsize) / 10 + MC_EXP_INDENT, scale);
    m_powCell->DrawList(parser, pw, MAX(MC_MIN_SIZE, fontsize - EXPT_DEC));
  }
}

wxString ExptCell::ToString()
{
  if (m_isBroken)
    return wxEmptyString;
  wxString s = m_baseCell->ListToString() + wxT("^");
  if (m_isMatrix)
    s += wxT("^");
  if (m_powCell->IsCompound())
    s += wxT("(") + m_powCell->ListToString() + wxT(")");
  else
    s += m_powCell->ListToString();
  return s;
}

wxString ExptCell::ToTeX()
{
  if (m_isBroken)
    return wxEmptyString;
  wxString s = wxT("{{") + m_baseCell->ListToTeX() + wxT("}^{") +
               m_powCell->ListToTeX() + wxT("}}");
  return s;
}

wxString ExptCell::GetDiffPart()
{
  wxString s(wxT(","));
  if (m_baseCell != NULL)
    s += m_baseCell->ListToString();
  s += wxT(",");
  if (m_powCell != NULL)
    s += m_powCell->ListToString();
  return s;
}

wxString ExptCell::ToMathML()
{
  return wxT("<msup>") +
    m_baseCell -> ListToMathML() +
    m_powCell -> ListToMathML() +
    wxT("</msup>\n");
//  return wxT("<apply><power/>") + m_baseCell->ListToMathML() + m_powCell->ListToMathML() + wxT("</apply>");
}

wxString ExptCell::ToOMML()
{
  return wxT("<m:sSup><m:e>") + m_baseCell->ListToOMML() + wxT("</m:e><m:sup>") + 
    m_powCell->ListToOMML() + wxT("</m:sup></m:sSup>\n");
}

wxString ExptCell::ToXML()
{
//  if (m_isBroken)
//    return wxEmptyString;
  return _T("<e><r>") + m_baseCell->ListToXML() + _T("</r><r>") +
    m_powCell->ListToXML() + _T("</r></e>");
}

void ExptCell::SelectInner(wxRect& rect, MathCell **first, MathCell **last)
{
  *first = NULL;
  *last = NULL;
  if (m_powCell->ContainsRect(rect))
    m_powCell->SelectRect(rect, first, last);
  else if (m_baseCell->ContainsRect(rect))
    m_baseCell->SelectRect(rect, first, last);
  if (*first == NULL || *last == NULL)
  {
    *first = this;
    *last = this;
  }
}

bool ExptCell::BreakUp()
{
  if (!m_isBroken)
  {
    m_isBroken = true;
    m_baseCell->m_previousToDraw = this;
    wxASSERT_MSG(m_last1 != NULL,_("Bug: No last cell in the base of an exptCell!"));
    if(m_last1 != NULL)
    {
      m_last1->m_nextToDraw = m_exp;
      m_exp->m_previousToDraw = m_last1;
    }
    m_exp->m_nextToDraw = m_open;
    m_open->m_previousToDraw = m_exp;
    m_open->m_nextToDraw = m_powCell;
    m_powCell->m_previousToDraw = m_open;
    wxASSERT_MSG(m_last2 != NULL,_("Bug: No last cell in an exponent of an exptCell!"));
    if(m_last2 != NULL)
    {
      m_last2->m_nextToDraw = m_close;
      m_close->m_previousToDraw = m_last2;
    }
    m_close->m_nextToDraw = m_nextToDraw;
    if (m_nextToDraw != NULL)
      m_nextToDraw->m_previousToDraw = m_close;
    m_nextToDraw = m_baseCell;
    return true;
  }
  return false;
}

void ExptCell::Unbreak()
{
  if (m_isBroken)
  {
    m_baseCell->UnbreakList();
    m_powCell->UnbreakList();
  }
  MathCell::Unbreak();
}
