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

#ifndef __TITANIA_COMPOSED_WIDGETS_BACKGROUND_TOOL_H__
#define __TITANIA_COMPOSED_WIDGETS_BACKGROUND_TOOL_H__

#include "../ComposedWidgets/X3DGradientTool.h"

namespace titania {
namespace puck {

class BackgroundTool :
	public X3DGradientTool
{
public:

	///  @name Construction

	BackgroundTool (X3DBaseInterface* const,
	                const std::string &,
	                Gtk::Box &,
	                const std::string &,
	                const std::string &);


private:

	virtual
	void
	realize () final override;

	virtual
	void
	set_addTime (const X3D::time_type);

	virtual
	X3D::MFFloat
	get_position (const X3D::MFFloat &) final override;

	virtual
	void
	set_tool_values (const X3D::MFFloat &, const X3D::MFColor &) final override;

};

inline
BackgroundTool::BackgroundTool (X3DBaseInterface* const editor,
                                const std::string & name,
                                Gtk::Box & box,
                                const std::string & positionName,
                                const std::string & colorName) :
	 X3DBaseInterface (editor -> getBrowserWindow (), editor -> getCurrentBrowser ()),
	  X3DGradientTool (editor, name, box, positionName, colorName)
{ }

inline
void
BackgroundTool::realize ()
{
	try
	{
		getTool () -> getField <X3D::SFBool>  ("enableFirst") = false;
	}
	catch (const X3D::X3DError & error)
	{
		__LOG__ << error .what () << std::endl;
	}
}

inline
void
BackgroundTool::set_addTime (const X3D::time_type value)
{
	try
	{
		const auto   tool     = getTool ();
		const auto & position = tool -> getField <X3D::MFFloat> ("position");
		auto       & color    = tool -> getField <X3D::MFColor> ("color");

		if (position .size () == 1)
		{
			if (position [0] > 0)
			{
				color .emplace_back (color [0]);
				setWhichChoice (1);
			}
		}

		X3DGradientTool::set_addTime (value);
	}
	catch (const X3D::X3DError & error)
	{
	}
}

inline
X3D::MFFloat
BackgroundTool::get_position (const X3D::MFFloat & position)
{
	X3D::MFFloat angle;

	if (not position .empty ())
	{
		const size_t offset = position [0] == 0.0f ? 1 : 0;

		for (const auto & value : std::make_pair (position .begin () + offset, position .end ()))
			angle .emplace_back (std::acos (1 - value));
	}

	return angle;
}

inline
void
BackgroundTool::set_tool_values (const X3D::MFFloat & positionValue, const X3D::MFColor & colorValue)
{
	try
	{
		X3D::MFFloat position;

		for (const auto & value : positionValue)
			position .emplace_back (1 - std::cos (math::clamp <float> (value, 0, M_PI / 2)));

		if (not colorValue .empty ())
			position .emplace_front (0);

		getTool () -> setField <X3D::MFFloat> ("inputPosition", position);
		getTool () -> setField <X3D::MFColor> ("inputColor",    colorValue);
	}
	catch (const X3D::X3DError & error)
	{
		//__LOG__ << error .what () << std::endl;
	}
}

} // puck
} // titania

#endif
