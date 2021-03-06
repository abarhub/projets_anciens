/////////////////////////////////////////////////////////////////////////////
// Name:        forty.h
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id: forty.h,v 1.1 2000/01/08 15:27:38 VZ Exp $
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:   	wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWindows 2.0
/////////////////////////////////////////////////////////////////////////////
#ifndef _FORTY_H_
#define _FORTY_H_

class FortyApp: public wxApp
{
public:
	bool OnInit();

	static const wxColour& BackgroundColour();
	static const wxColour& TextColour();
	static const wxBrush&  BackgroundBrush();

private:
	static wxColour* m_backgroundColour;
	static wxColour* m_textColour;
	static wxBrush*  m_backgroundBrush;
};

#endif
