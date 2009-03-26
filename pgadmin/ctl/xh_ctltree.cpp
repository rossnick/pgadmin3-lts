//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// xh_ctltree.cpp - ctlTree handler
//
//////////////////////////////////////////////////////////////////////////
 
#include "pgAdmin3.h"

#include "wx/wx.h"

#include "utils/misc.h"
#include "ctl/ctlTree.h"
#include "ctl/xh_ctltree.h"

IMPLEMENT_DYNAMIC_CLASS(ctlTreeXmlHandler, wxTreeCtrlXmlHandler)


wxObject *ctlTreeXmlHandler::DoCreateResource()
{ 
    ctlTree *ctl=new ctlTree(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle());
    
    SetupWindow(ctl);
   
    return ctl;
}

bool ctlTreeXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("ctlTree"));
}
