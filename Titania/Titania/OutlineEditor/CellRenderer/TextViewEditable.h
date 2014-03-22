/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstra�e 31a, Leipzig, Germany 2011.
 *
 * All rights reserved. Holger Seelig <holger.seelig@yahoo.de>.
 *
 * THIS IS UNPUBLISHED SOURCE CODE OF create3000.
 *
 * The copyright notice above does not evidence any actual of intended
 * publication of such source code, and is an unpublished work by create3000.
 * This material contains CONFIDENTIAL INFORMATION that is the property of
 * create3000.
 *
 * No permission is granted to copy, distribute, or create derivative works from
 * the contents of this software, in whole or in part, without the prior written
 * permission of create3000.
 *
 * NON-MILITARY USE ONLY
 *
 * All create3000 software are effectively free software with a non-military use
 * restriction. It is free. Well commented source is provided. You may reuse the
 * source in any way you please with the exception anything that uses it must be
 * marked to indicate is contains 'non-military use only' components.
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 1999, 2012 Holger Seelig <holger.seelig@yahoo.de>.
 *
 * This file is part of the Titania Project.
 *
 * Titania is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 3 only, as published by the
 * Free Software Foundation.
 *
 * Titania is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License version 3 for more
 * details (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version 3
 * along with Titania.  If not, see <http://www.gnu.org/licenses/gpl.html> for a
 * copy of the GPLv3 License.
 *
 * For Silvio, Joy and Adi.
 *
 ******************************************************************************/

#ifndef __TITANIA_OUTLINE_EDITOR_CELL_RENDERER_TEXT_VIEW_EDITABLE_H__
#define __TITANIA_OUTLINE_EDITOR_CELL_RENDERER_TEXT_VIEW_EDITABLE_H__

#include "../OutlineTreeData.h"
#include <Titania/X3D.h>
#include <gtkmm.h>

namespace titania {
namespace puck {

class TextViewEditable :
	public Gtk::ScrolledWindow, public Gtk::CellEditable
{
public:

	TextViewEditable (OutlineTreeData* const, const Glib::ustring &, bool = true);

	// Properties
	Glib::Property <bool> &
	property_editing_canceled ()
	{ return editing_canceled_property; }

	const Glib::Property <bool> &
	property_editing_canceled () const
	{ return editing_canceled_property; }
	
	void
	set_validated (bool value)
	{ validated = value; }

	bool
	get_validated () const
	{ return validated; }

	void
	set_text (const Glib::ustring & value)
	{ textview .get_buffer () -> set_text (value); }

	Glib::ustring
	get_text () const
	{ return textview .get_buffer () -> get_text (); }

	OutlineTreeData*
	get_data () const
	{ return data; }

	const Glib::ustring &
	get_path () const
	{ return path; }

	virtual
	~TextViewEditable ();


private:

	virtual
	void
	start_editing_vfunc (GdkEvent*) final override;

	virtual
	void
	on_grab_focus () final override;

	bool
	on_textview_button_press_event (GdkEventButton*);

	bool
	on_textview_focus_out_event (GdkEventFocus*);

	bool
	on_textview_key_press_event (GdkEventKey*);

	void
	editing_canceled ();

	Glib::Property <bool>  editing_canceled_property;
	Gtk::TextView          textview;
	OutlineTreeData* const data;
	bool                   multiline;
	const Glib::ustring    path;
	bool                   validated;
	bool                   handleFocusOut;

};

} // puck
} // titania

#endif
