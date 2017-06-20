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

#ifndef __TITANIA_BROWSER_NOTEBOOK_BROWSER_VIEW_X3DBROWSER_VIEW_H__
#define __TITANIA_BROWSER_NOTEBOOK_BROWSER_VIEW_X3DBROWSER_VIEW_H__

#include "../../UserInterfaces/X3DBrowserViewInterface.h"

namespace titania {
namespace puck {

class NotebookPage;

enum BrowserViewType :
	int32_t
{
	MAIN,
	TOP,
	RIGHT,
	FRONT,
	BOTTOM,
	LEFT,
	BACK

};

class X3DBrowserView :
	virtual public X3DBrowserViewInterface
{
public:

	///  @name Member access

	const X3D::BrowserPtr &
	getLocalBrowser () const
	{ return browser; }

	///  @name Destruction

	virtual
	void
	dispose () override;

	virtual
	~X3DBrowserView () override;


protected:

	///  @name Construction

	X3DBrowserView (NotebookPage* const page, const BrowserViewType type);

	X3DBrowserView ();

	virtual
	void
	initialize () override;

	NotebookPage*
	getPage () const
	{ return page; }

	void
	setType (const BrowserViewType value);

	BrowserViewType
	getType () const;

	virtual
	void
	setLocalBrowser (const X3D::BrowserPtr & value);

	///  @name Event handlers

	virtual
	void
	on_map () override;

	virtual
	void
	on_unmap () override;


private:

	///  @name Construction

	X3D::BrowserPtr
	createBrowser (const BrowserViewType type) const;

	///  @name Member access

	int32_t
	getPlane () const;

	///  @name Event handlers

	void
	set_dependent_browser ();

	void
	set_activeLayer ();

	void
	set_viewpoint ();

	void
	set_grid ();

	///  @name Static members

	static const std::vector <std::string>     names;
	static const std::vector <X3D::Vector3d>   axes;
	static const std::vector <X3D::Vector3d>   positions;
	static const std::vector <X3D::Rotation4d> orientations;

	///  @name Members

	NotebookPage* const               page;
	BrowserViewType                   type;
	X3D::BrowserPtr                   browser;
	X3D::X3DPtr <X3D::X3DLayerNode>   activeLayer;
	X3D::X3DPtr <X3D::OrthoViewpoint> viewpoint;
	X3D::X3DPtr <X3D::Transform>      gridTransform;
	X3D::X3DPtr <X3D::Switch>         gridSwitch;
	X3D::SFNode                       grid;

};

} // puck
} // titania

#endif
