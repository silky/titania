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

#ifndef __TITANIA_OPEN_GL_SURFACE_H__
#define __TITANIA_OPEN_GL_SURFACE_H__

#include <gtkmm/drawingarea.h>

#include <memory>
#include <thread>

namespace titania {
namespace OpenGL {

class Context;

class Surface :
	public Gtk::DrawingArea
{
public:

	///  @name Construction

	Surface ();

	Surface (const Surface & sharingSurface);

	///  @name Member access

	void
	setAntialiasing (const size_t value)
	{ antialiasing = value; createContext (); }

	size_t
	getAntialiasing () const
	{ return antialiasing; }

	void
	setAccumBuffer (const bool value)
	{ accumBuffer = value; createContext (); }

	bool
	getAccumBuffer () const
	{ return accumBuffer; }

	void
	setSwapInterval (const size_t value);

	///  @name Operations

	bool
	makeCurrent ();

	///  @name Destruction

	virtual
	void
	dispose ();

	virtual
	~Surface () override;


protected:

	/// @name OpenGL handler

	virtual
	void
	on_map () override;

	virtual
	void
	on_unmap () override;

	virtual
	void
	on_unrealize () override;


private:

	///  @name Construction

	Surface (const std::shared_ptr <Context> & sharingContext);

	void
	createContext ();

	///  @name Members

	const std::thread::id     treadId;
	std::shared_ptr <Context> context;
	std::shared_ptr <Context> sharingContext;
	size_t                    antialiasing;
	bool                      accumBuffer;

};

} // OpenGL
} // titania

#endif
