/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstraße 31a, Leipzig, Germany 2011.
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
 * Copyright 1999, 2016 Holger Seelig <holger.seelig@yahoo.de>.
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
#ifndef __TMP_GLAD2CPP_BROWSER_PANEL_H__
#define __TMP_GLAD2CPP_BROWSER_PANEL_H__

#include "../Base/X3DPanelInterface.h"
#include <gtkmm.h>
#include <string>

namespace titania {
namespace puck {

/**
 *  Gtk Interface for BrowserPanel.
 */
class X3DBrowserPanelInterface :
	public X3DPanelInterface
{
public:

	///  @name Construction

	X3DBrowserPanelInterface () :
		X3DPanelInterface ()
	{ }

	template <class ... Arguments>
	X3DBrowserPanelInterface (const std::string & filename, const Arguments & ... arguments) :
		X3DPanelInterface (arguments ...)
	{ create (filename); }

	template <class ... Arguments>
	X3DBrowserPanelInterface (std::initializer_list <std::string> filenames, const Arguments & ... arguments) :
		X3DPanelInterface (arguments ...)
	{ create (filenames); }

	///  @name Member access

	const Glib::RefPtr <Gtk::Builder> &
	getBuilder () const
	{ return m_builder; }

	Gtk::Window &
	getWindow () const
	{ return *m_Window; }

	Gtk::Box &
	getWidget () const
	{ return *m_Widget; }

	Gtk::MenuBar &
	getMenuBar () const
	{ return *m_MenuBar; }

	Gtk::MenuItem &
	getViewMenuItem () const
	{ return *m_ViewMenuItem; }

	Gtk::ImageMenuItem &
	getUndoViewMenuItem () const
	{ return *m_UndoViewMenuItem; }

	Gtk::ImageMenuItem &
	getRedoViewMenuItem () const
	{ return *m_RedoViewMenuItem; }

	Gtk::CheckMenuItem &
	getStraightenHorizonMenuItem () const
	{ return *m_StraightenHorizonMenuItem; }

	Gtk::SeparatorMenuItem &
	getStraightenHorizonSeparatorMenuItem () const
	{ return *m_StraightenHorizonSeparatorMenuItem; }

	Gtk::ImageMenuItem &
	getLookAtSelectionMenuItem () const
	{ return *m_LookAtSelectionMenuItem; }

	Gtk::ImageMenuItem &
	getLookAtAllMenuItem () const
	{ return *m_LookAtAllMenuItem; }

	Gtk::ImageMenuItem &
	getResetUserOffsetsMenuItem () const
	{ return *m_ResetUserOffsetsMenuItem; }

	Gtk::MenuItem &
	getShadingMenuItem () const
	{ return *m_ShadingMenuItem; }

	Gtk::RadioMenuItem &
	getPhongMenuItem () const
	{ return *m_PhongMenuItem; }

	Gtk::RadioMenuItem &
	getGouraudMenuItem () const
	{ return *m_GouraudMenuItem; }

	Gtk::RadioMenuItem &
	getFlatMenuItem () const
	{ return *m_FlatMenuItem; }

	Gtk::RadioMenuItem &
	getWireframeMenuItem () const
	{ return *m_WireframeMenuItem; }

	Gtk::RadioMenuItem &
	getPointsetMenuItem () const
	{ return *m_PointsetMenuItem; }

	Gtk::MenuItem &
	getDisplayMenuItem () const
	{ return *m_DisplayMenuItem; }

	Gtk::MenuItem &
	getPanelsMenuItem () const
	{ return *m_PanelsMenuItem; }

	Gtk::MenuItem &
	getCameraMenuItem () const
	{ return *m_CameraMenuItem; }

	Gtk::MenuItem &
	getMainViewMenuItem () const
	{ return *m_MainViewMenuItem; }

	Gtk::SeparatorMenuItem &
	getMainViewSeparatorMenuItem () const
	{ return *m_MainViewSeparatorMenuItem; }

	Gtk::MenuItem &
	getPerspectiveViewMenuItem () const
	{ return *m_PerspectiveViewMenuItem; }

	Gtk::MenuItem &
	getTopViewMenuItem () const
	{ return *m_TopViewMenuItem; }

	Gtk::MenuItem &
	getRightViewMenuItem () const
	{ return *m_RightViewMenuItem; }

	Gtk::MenuItem &
	getFrontViewMenuItem () const
	{ return *m_FrontViewMenuItem; }

	Gtk::MenuItem &
	getBottomViewMenuItem () const
	{ return *m_BottomViewMenuItem; }

	Gtk::MenuItem &
	getLeftViewMenuItem () const
	{ return *m_LeftViewMenuItem; }

	Gtk::MenuItem &
	getBackViewMenuItem () const
	{ return *m_BackViewMenuItem; }

	Gtk::Box &
	getBrowserBox () const
	{ return *m_BrowserBox; }

	///  @name Signal handlers

	virtual
	void
	on_map () = 0;

	virtual
	void
	on_unmap () = 0;

	virtual
	void
	undo_view_activate () = 0;

	virtual
	void
	redo_view_activate () = 0;

	virtual
	void
	on_straighten_horizon_toggled () = 0;

	virtual
	void
	on_look_at_selection_activate () = 0;

	virtual
	void
	on_look_at_all_activate () = 0;

	virtual
	void
	on_reset_user_offsets_activate () = 0;

	virtual
	void
	on_phong_toggled () = 0;

	virtual
	void
	on_gouraud_toggled () = 0;

	virtual
	void
	on_flat_toggled () = 0;

	virtual
	void
	on_wireframe_toggled () = 0;

	virtual
	void
	on_pointset_toggled () = 0;

	virtual
	void
	on_main_view_activate () = 0;

	virtual
	void
	on_perspective_view_activate () = 0;

	virtual
	void
	on_top_view_activate () = 0;

	virtual
	void
	on_right_view_activate () = 0;

	virtual
	void
	on_front_view_activate () = 0;

	virtual
	void
	on_bottom_view_activate () = 0;

	virtual
	void
	on_left_view_activate () = 0;

	virtual
	void
	on_back_view_activate () = 0;

	///  @name Destruction

	virtual
	~X3DBrowserPanelInterface () override;


private:

	///  @name Construction

	void
	create (const std::string &);

	void
	create (std::initializer_list <std::string>);

	void
	create ();

	///  @name Static members

	///  @name Members

	Glib::RefPtr <Gtk::Builder> m_builder;
	Gtk::Window* m_Window;
	Gtk::Box* m_Widget;
	Gtk::MenuBar* m_MenuBar;
	Gtk::MenuItem* m_ViewMenuItem;
	Gtk::ImageMenuItem* m_UndoViewMenuItem;
	Gtk::ImageMenuItem* m_RedoViewMenuItem;
	Gtk::CheckMenuItem* m_StraightenHorizonMenuItem;
	Gtk::SeparatorMenuItem* m_StraightenHorizonSeparatorMenuItem;
	Gtk::ImageMenuItem* m_LookAtSelectionMenuItem;
	Gtk::ImageMenuItem* m_LookAtAllMenuItem;
	Gtk::ImageMenuItem* m_ResetUserOffsetsMenuItem;
	Gtk::MenuItem* m_ShadingMenuItem;
	Gtk::RadioMenuItem* m_PhongMenuItem;
	Gtk::RadioMenuItem* m_GouraudMenuItem;
	Gtk::RadioMenuItem* m_FlatMenuItem;
	Gtk::RadioMenuItem* m_WireframeMenuItem;
	Gtk::RadioMenuItem* m_PointsetMenuItem;
	Gtk::MenuItem* m_DisplayMenuItem;
	Gtk::MenuItem* m_PanelsMenuItem;
	Gtk::MenuItem* m_CameraMenuItem;
	Gtk::MenuItem* m_MainViewMenuItem;
	Gtk::SeparatorMenuItem* m_MainViewSeparatorMenuItem;
	Gtk::MenuItem* m_PerspectiveViewMenuItem;
	Gtk::MenuItem* m_TopViewMenuItem;
	Gtk::MenuItem* m_RightViewMenuItem;
	Gtk::MenuItem* m_FrontViewMenuItem;
	Gtk::MenuItem* m_BottomViewMenuItem;
	Gtk::MenuItem* m_LeftViewMenuItem;
	Gtk::MenuItem* m_BackViewMenuItem;
	Gtk::Box* m_BrowserBox;

};

} // puck
} // titania

#endif
