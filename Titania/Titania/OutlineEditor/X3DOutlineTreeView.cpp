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

#include "X3DOutlineTreeView.h"

#include "../Browser/BrowserWindow.h"
#include "../Configuration/config.h"
#include "CellRenderer/OutlineCellRenderer.h"
#include "OutlineRouteGraph.h"
#include "OutlineTreeModel.h"
#include "OutlineTreeObserver.h"

namespace titania {
namespace puck {

X3DOutlineTreeView::X3DOutlineTreeView (const X3D::X3DSFNode <X3D::X3DExecutionContext> & executionContext) :
	              Gtk::TreeView (),
	X3DOutlineTreeViewInterface (get_ui ("OutlineTreeView.ui"), gconf_dir ()),
	               treeObserver (new OutlineTreeObserver (this)),
	                 routeGraph (new OutlineRouteGraph (this)),
	                      model (OutlineTreeModel::create (getBrowserWindow (), executionContext)),
	               cellrenderer (Gtk::manage (new OutlineCellRenderer (getBrowser (), this))),
	                expandLevel (0)
{
	// Options

	set_headers_visible (true);
	set_enable_search (false);
	Gtk::TreeView::get_selection () -> set_mode (Gtk::SELECTION_NONE);

	// Color

	auto selectedForegroundColor = get_style_context () -> get_color (Gtk::STATE_FLAG_SELECTED);
	auto selectedBackgroundColor = get_style_context () -> get_background_color (Gtk::STATE_FLAG_SELECTED);

	// Columns

	Gtk::TreeViewColumn* treeviewcolumn_name = Gtk::manage (new Gtk::TreeViewColumn (_ ("Tree")));
	treeviewcolumn_name -> set_expand (false);

	// CellRenderer

	treeviewcolumn_name -> pack_start (*cellrenderer, false);
	treeviewcolumn_name -> add_attribute (*cellrenderer, "tree-data",           OutlineTreeModel::DATA_COLUMN);
	treeviewcolumn_name -> add_attribute (*cellrenderer, "foreground-set",      OutlineTreeModel::SELECTED_COLUMN);
	treeviewcolumn_name -> add_attribute (*cellrenderer, "cell-background-set", OutlineTreeModel::SELECTED_COLUMN);
	cellrenderer -> property_foreground_rgba ()      = selectedForegroundColor;
	cellrenderer -> property_cell_background_rgba () = selectedBackgroundColor;

	// Append column

	append_column (*treeviewcolumn_name);

	// Pad column

	Gtk::TreeViewColumn* treeviewcolumn_pad = Gtk::manage (new Gtk::TreeViewColumn ());
	treeviewcolumn_pad -> set_expand (true);

	auto padCellrenderer = Gtk::manage (new Gtk::CellRendererText ());
	treeviewcolumn_pad -> pack_start (*padCellrenderer, false);
	treeviewcolumn_pad -> add_attribute (*padCellrenderer, "cell-background-set", OutlineTreeModel::SELECTED_COLUMN);
	padCellrenderer -> property_cell_background_rgba () = selectedBackgroundColor;

	append_column (*treeviewcolumn_pad);

	//

	set_execution_context (executionContext);
}

Gtk::TreeModel::Path
X3DOutlineTreeView::get_path_at_position (double x, double y) const
{
	Gtk::TreeViewColumn* column = nullptr;

	return get_path_at_position (x, y, column);
}

Gtk::TreeModel::Path
X3DOutlineTreeView::get_path_at_position (double x, double y, Gtk::TreeViewColumn* & column) const
{
	Gtk::TreeModel::Path path;
	int                  cell_x = 0;
	int                  cell_y = 0;

	get_path_at_pos (x, y, path, column, cell_x, cell_y);

	return path;
}

void
X3DOutlineTreeView::expand_to (X3D::X3DChildObject* const object)
{
	Gtk::TreeModel::Path path;

	disable_shift_key ();

	auto hierarchy = std::move (X3D::find (getBrowser () -> getExecutionContext () -> getRootNodes (), object));

	if (not hierarchy .empty ())
		expand_to (get_model () -> children (), hierarchy, path);

	enable_shift_key ();
}

bool
X3DOutlineTreeView::expand_to (const Gtk::TreeModel::Children & children, std::deque <X3D::X3DChildObject*> & hierarchy, Gtk::TreeModel::Path & path)
{
	auto top = hierarchy .front ();

	hierarchy .pop_front ();

	path .push_back (0);

	for (const auto child : children)
	{
		auto data   = get_model () -> get_data (child);
		auto object = data -> get_object ();

		if (data -> get_type () == OutlineIterType::X3DBaseNode)
			object = static_cast <X3D::SFNode*> (object) -> getValue ();

		if (object == top)
		{
			if (hierarchy .empty ())
				return true;

			expand_row (path, false);

			return expand_to (child -> children (), hierarchy, path);
		}

		path .next ();
	}

	return false;
}

void
X3DOutlineTreeView::collapse_row (const Gtk::TreeModel::Path & path)
{
	if (row_expanded (path))
		Gtk::TreeView::collapse_row (path);
}

void
X3DOutlineTreeView::set_model (const Glib::RefPtr <OutlineTreeModel> & value)
{
	Gtk::TreeView::set_model (value);

	model = value;
}

std::deque <Gtk::TreeModel::iterator>
X3DOutlineTreeView::get_iters (X3D::X3DChildObject* const object) const
{
	return get_model () -> get_iters (object);
}

OutlineUserDataPtr
X3DOutlineTreeView::get_user_data (const Gtk::TreeModel::iterator & iter) const
{
	return get_model () -> get_user_data (iter);
}

OutlineUserDataPtr
X3DOutlineTreeView::get_user_data (X3D::X3DChildObject* const object) const
{
	return get_model () -> get_user_data (object);
}

OutlineIterType
X3DOutlineTreeView::get_data_type (const Gtk::TreeModel::iterator & iter) const
{
	return get_model () -> get_data_type (iter);
}

X3D::X3DChildObject*
X3DOutlineTreeView::get_object (const Gtk::TreeModel::iterator & iter) const
{
	return get_model () -> get_object (iter);
}

void
X3DOutlineTreeView::set_open_path (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & value)
{
	auto userData = get_user_data (iter);

	if (userData)
		userData -> open_path = value;
}

Gtk::TreeModel::Path
X3DOutlineTreeView::get_open_path (const Gtk::TreeModel::iterator & iter) const
{
	auto userData = get_user_data (iter);

	if (userData)
		return userData -> open_path;

	return Gtk::TreeModel::Path ();
}

void
X3DOutlineTreeView::set_expanded (const Gtk::TreeModel::iterator & iter, bool value)
{
	auto userData = get_user_data (iter);

	if (userData)
		userData -> expanded = value;
}

bool
X3DOutlineTreeView::get_expanded (const Gtk::TreeModel::iterator & iter) const
{
	auto userData = get_user_data (iter);

	if (userData)
		return userData -> expanded;

	return false;
}

void
X3DOutlineTreeView::set_all_expanded (const Gtk::TreeModel::iterator & iter, bool value)
{
	auto userData = get_user_data (iter);

	if (userData)
		userData -> all_expanded = value;
}

bool
X3DOutlineTreeView::get_all_expanded (const Gtk::TreeModel::iterator & iter) const
{
	auto userData = get_user_data (iter);

	if (userData)
		return userData -> all_expanded;

	return false;
}

bool
X3DOutlineTreeView::get_shift_key ()
{
	return expandLevel == 0 and getBrowserWindow () -> getKeys () .shift ();
}

bool
X3DOutlineTreeView::get_control_key ()
{
	return getBrowserWindow () -> getKeys () .control ();
}

bool
X3DOutlineTreeView::get_alt_key ()
{
	return getBrowserWindow () -> getKeys () .alt ();
}

void
X3DOutlineTreeView::set_execution_context (const X3D::X3DSFNode <X3D::X3DExecutionContext> & executionContext)
{
	//__LOG__ << std::endl;

	for (const auto & child : get_model () -> children ())
	{
		routeGraph -> collapse (child);
		treeObserver -> unwatch_tree (child);
	}

	get_model () -> get_execution_context () -> getRootNodes () .removeInterest (this, &X3DOutlineTreeView::set_rootNodes);

	set_model (OutlineTreeModel::create (getBrowserWindow (), executionContext));

	executionContext -> getRootNodes () .addInterest (this, &X3DOutlineTreeView::set_rootNodes);

	set_rootNodes ();
}

void
X3DOutlineTreeView::set_rootNodes ()
{
	//__LOG__ << std::endl;

	for (const auto & child : get_model () -> children ())
	{
		routeGraph -> collapse (child);
		treeObserver -> unwatch_tree (child);
	}

	unset_model ();
	get_model () -> clear ();

	for (auto & rootNode : getBrowser () -> getExecutionContext () -> getRootNodes ())
		get_model () -> append (OutlineIterType::X3DBaseNode, &rootNode);

	set_model (get_model ());

	disable_shift_key ();

	for (auto & iter : get_model () -> children ())
	{
		// Expand row again if it was previously expanded.

		if (get_expanded (iter))
			expand_row (get_model () -> get_path (iter), false);
	}

	enable_shift_key ();
}

bool
X3DOutlineTreeView::on_test_expand_row (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	collapse_clone (iter);

	model_expand_row (iter); // (model expand_row)

	// Return false to allow expansion, true to reject.
	return false;
}

void
X3DOutlineTreeView::on_row_expanded (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	//__LOG__ << path .to_string () << std::endl;

	// Set expanded first to prevent loop with clones.

	set_expanded (iter, true);
	set_open_path (iter, path);

	routeGraph -> expand (iter);
	treeObserver -> watch (iter, path);

	auto_expand (iter);
}

bool
X3DOutlineTreeView::on_test_collapse_row (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	// Return false to allow collapse, true to reject.
	return false;
}

void
X3DOutlineTreeView::on_row_collapsed (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	//__LOG__ << path .to_string () << std::endl;

	set_expanded (iter, false);
	set_all_expanded (iter, false);

	routeGraph -> collapse (iter);
	treeObserver -> unwatch_tree (iter);

	get_model () -> clear (iter);

	toggle_expand (iter, path);

	columns_autosize ();
}

void
X3DOutlineTreeView::collapse_clone (const Gtk::TreeModel::iterator & iter)
{
	if (get_data_type (iter) == OutlineIterType::X3DBaseNode)
		collapse_row (get_open_path (iter));
}

void
X3DOutlineTreeView::model_expand_row (const Gtk::TreeModel::iterator & iter)
{
	switch (get_data_type (iter))
	{
		case OutlineIterType::X3DInputRoute:
		case OutlineIterType::X3DOutputRoute:
		case OutlineIterType::X3DFieldValue:
			break;

		case OutlineIterType::X3DField:
		{
			auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (iter));

			if (get_shift_key () or get_all_expanded (iter))
			{
				expand_routes (iter, field);
				set_all_expanded (iter, true);
			}
			else
				set_all_expanded (iter, false);

			switch (field -> getType ())
			{
				case X3D::X3DConstants::SFNode:
				{
					get_model () -> append (iter, OutlineIterType::X3DBaseNode, field);
					break;
				}
				case X3D::X3DConstants::MFNode:
				{
					auto mfnode = static_cast <X3D::MFNode*> (field);

					if (mfnode -> empty () and not get_all_expanded (iter))
					{
						expand_routes (iter, field);
						set_all_expanded (iter, true);
					}

					for (auto & value : *mfnode)
						get_model () -> append (iter, OutlineIterType::X3DBaseNode, &value);

					break;
				}
				default:
				{
					get_model () -> append (iter, OutlineIterType::X3DFieldValue, field);
					break;
				}
			}

			break;
		}
		case OutlineIterType::X3DBaseNode:
		{
			auto nodeUserData = get_user_data (iter);

			if (nodeUserData)
			{
				auto sfnode = static_cast <X3D::SFNode*> (get_object (iter));
				auto node   = sfnode -> getValue ();

				if (node)
				{
					// Select visible fields

					if (get_shift_key () or get_all_expanded (iter))
					{
						for (const auto & field : get_fields (node))
							get_model () -> append (iter, OutlineIterType::X3DField, field);

						set_all_expanded (iter, true);
					}
					else
					{
						bool visibles = false;

						for (const auto & field : get_fields (node))
						{
							if (not field -> isInitializeable () or node -> isDefaultValue (field))
							{
								if (not get_model () -> get_input_routes_size (field) and not get_model () -> get_output_routes_size (field))
									continue;
							}

							get_model () -> append (iter, OutlineIterType::X3DField, field);
							visibles = true;
						}

						if (not visibles)
						{
							for (const auto & field : get_fields (node))
								get_model () -> append (iter, OutlineIterType::X3DField, field);
						}

						set_all_expanded (iter, false);
					}
				}
			}
		}
	}
}

X3D::FieldDefinitionArray
X3DOutlineTreeView::get_fields (X3D::X3DBaseNode* const node) const
{
	auto fields            = node -> getPreDefinedFields ();
	auto userDefinedFields = node -> getUserDefinedFields ();

	if (dynamic_cast <X3D::X3DNode*> (node))
		fields .insert (fields .begin () + 1, userDefinedFields .begin (), userDefinedFields .end ());

	else
		fields .insert (fields .begin (), userDefinedFields .begin (), userDefinedFields .end ());

	return fields;
}

void
X3DOutlineTreeView::expand_routes (const Gtk::TreeModel::iterator & iter, X3D::X3DFieldDefinition* field)
{
	for (const auto & route : get_model () -> get_input_routes (field))
	{
		get_model () -> append (iter, OutlineIterType::X3DInputRoute, route);
	}

	for (const auto & route : get_model () -> get_output_routes (field))
	{
		get_model () -> append (iter, OutlineIterType::X3DOutputRoute, route);
	}
}

void
X3DOutlineTreeView::toggle_expand (const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path)
{
	switch (get_data_type (iter))
	{
		case OutlineIterType::X3DField:
		case OutlineIterType::X3DBaseNode:
		{
			if (get_shift_key ())
				expand_row (path, false);

			break;
		}
		default:
			break;
	}
}

void
X3DOutlineTreeView::auto_expand (const Gtk::TreeModel::iterator & parent)
{
	disable_shift_key ();

	switch (get_data_type (parent))
	{
		case OutlineIterType::X3DInputRoute:
		case OutlineIterType::X3DOutputRoute:
		case OutlineIterType::X3DFieldValue:
			break;

		case OutlineIterType::X3DField:
		{
			for (const auto & child : parent -> children ())
			{
				switch (get_data_type (child))
				{
					case OutlineIterType::X3DBaseNode :
						{
							if (get_expanded (child))
							{
								if (get_open_path (child) .empty () or not get_open_path (child) .is_ancestor (get_model () -> get_path (child)))
								{
									expand_row (Gtk::TreePath (child), false);
								}
							}

							break;
						}
					default:
						break;
				}
			}

			break;
		}
		case OutlineIterType::X3DBaseNode:
		{
			for (const auto & child : parent -> children ())
			{
				auto field = static_cast <X3D::X3DFieldDefinition*> (get_object (child));

				switch (field -> getType ())
				{
					case X3D::X3DConstants::SFNode:
					{
						auto sfnode = static_cast <X3D::SFNode*> (field);

						if ((field -> isInitializeable () and *sfnode) or get_expanded (child))
						{
							expand_row (Gtk::TreePath (child), false);
						}

						break;
					}
					case X3D::X3DConstants::MFNode:
					{
						auto mfnode = static_cast <X3D::MFNode*> (field);

						if (mfnode -> size () and (field -> isInitializeable () or get_expanded (child)))
						{
							expand_row (Gtk::TreePath (child), false);
						}

						break;
					}
					default:
					{
						if (get_expanded (child))
						{
							expand_row (Gtk::TreePath (child), false);
						}

						break;
					}
				}
			}
		}
	}

	enable_shift_key ();
}

X3DOutlineTreeView::~X3DOutlineTreeView ()
{ }

} // puck
} // titania
