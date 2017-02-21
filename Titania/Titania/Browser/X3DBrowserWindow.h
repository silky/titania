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

#ifndef __TITANIA_BROWSER_X3DBROWSER_WINDOW_H__
#define __TITANIA_BROWSER_X3DBROWSER_WINDOW_H__

#include "../Browser/X3DBrowserEditor.h"

#include <Titania/X3D/Browser/KeyDeviceSensor/Keys.h>

namespace titania {
namespace puck {

class Footer;
class GeometryEditor;
class OutlineTreeViewEditor;
class Sidebar;
class X3DGridTool;

class ViewpointObserver;

class X3DBrowserWindow :
	public X3DBrowserEditor
{
public:

	using X3DBrowserEditor::getWorldInfo;
	using X3DBrowserEditor::save;

	virtual
	void
	setEditing (const bool) override;

	/// @name Key device handling

	X3D::Keys &
	getKeys ()
	{ return keys; }

	const X3D::Keys &
	getKeys () const
	{ return keys; }

	void
	hasAccelerators (const bool);

	bool
	hasAccelerators () const
	{ return accelerators; }

	///  @name Tool handling

	const X3D::ClipboardPtr &
	getClipboard () const
	{ return clipboard; }

	const std::shared_ptr <GeometryEditor> &
	getGeometryEditor () const
	{ return geometryEditor; }

	const std::shared_ptr <Sidebar> &
	getSidebar () const
	{ return sidebar; }

	const std::shared_ptr <Footer> &
	getFooter () const
	{ return footer; }

	const std::shared_ptr <X3DGridTool> &
	getGridTool () const
	{ return gridTool; }

	const std::shared_ptr <X3DGridTool> &
	getAngleTool () const
	{ return angleTool; }

	/// @name File oerations

	virtual
	bool
	save (const basic::uri & worldURL, const std::string & outputStyle, const bool copy) final override;

	/// @name Clipboard handling

	void
	cutNodes (const X3D::X3DExecutionContextPtr &, const X3D::MFNode &, const X3D::UndoStepPtr &);

	void
	copyNodes (const X3D::X3DExecutionContextPtr &, const X3D::MFNode &);

	void
	pasteNodes (const X3D::X3DExecutionContextPtr &, X3D::MFNode &, const X3D::UndoStepPtr &);

	///  @name Outline Editor

	void
	expandNodes (const X3D::MFNode &);

	///  @name Destruction

	virtual
	void
	dispose () override;

	virtual
	~X3DBrowserWindow () override;


protected:

	/// @name Construction

	X3DBrowserWindow (const X3D::BrowserPtr &);

	virtual
	void
	initialize () override;

	virtual
	void
	setBrowser (const X3D::BrowserPtr &) override;

	virtual
	void
	store () override;


private:

	/// @name Member access

	const std::shared_ptr <OutlineTreeViewEditor> &
	getOutlineTreeView () const;

	/// @name Operations

	void
	set_clipboard (const X3D::SFString &);

	virtual
	void
	on_geometry_editor_clicked () final override;

	void
	on_geometry_editor_reveal_child_changed ();

	void
	expandNodesImpl (const X3D::MFNode &);

	///  @name Members

	X3D::ClipboardPtr                   clipboard;
	std::shared_ptr <GeometryEditor>    geometryEditor;
	std::shared_ptr <Sidebar>           sidebar;
	std::shared_ptr <Footer>            footer;
	std::shared_ptr <X3DGridTool>       gridTool;
	std::shared_ptr <X3DGridTool>       angleTool;
	std::unique_ptr <ViewpointObserver> viewpointObserver;

	X3D::Keys keys;
	bool      accelerators;

};

} // puck
} // titania

#endif
