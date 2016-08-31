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

#include "Viewport.h"

#include "../../Browser/X3DBrowser.h"
#include "../Layering/X3DLayerNode.h"

namespace titania {
namespace X3D {

const ComponentType Viewport::component      = ComponentType::LAYERING;
const std::string   Viewport::typeName       = "Viewport";
const std::string   Viewport::containerField = "viewport";

Viewport::Fields::Fields () :
	clipBoundary (new MFFloat ({ 0, 1, 0, 1 }))
{ }

Viewport::Viewport (X3DExecutionContext* const executionContext) :
	    X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DViewportNode (),
	         fields ()
{
	addType (X3DConstants::Viewport);

	addField (inputOutput,    "metadata",       metadata ());
	addField (inputOutput,    "clipBoundary",   clipBoundary ());
	addField (initializeOnly, "bboxSize",       bboxSize ());
	addField (initializeOnly, "bboxCenter",     bboxCenter ());
	addField (inputOnly,      "addChildren",    addChildren ());
	addField (inputOnly,      "removeChildren", removeChildren ());
	addField (inputOutput,    "children",       children ());
}

X3DBaseNode*
Viewport::create (X3DExecutionContext* const executionContext) const
{
	return new Viewport (executionContext);
}

Vector4i
Viewport::getRectangle (const int width, const int height) const
{
	// The clipBoundary field of a Viewport node is specified in fractions of the -normal-render-surface-.

	const int left   = width  * getLeft ();
	const int right  = width  * getRight ();
	const int bottom = height * getBottom ();
	const int top    = height * getTop ();

	return Vector4i (left,
	                 bottom,
	                 std::max (0, right - left),
	                 std::max (0, top - bottom));
}

Vector4i
Viewport::getScissor (const int width, const int height) const
{
	if (getViewVolumes () .empty ())
		return getRectangle (width, height);

	const auto parent = getViewVolumes () .back () .getScissor ();
	const int  left   = std::max (int (width  * getLeft ()),   parent [0]);
	const int  right  = std::min (int (width  * getRight ()),  parent [0] + parent [2]);
	const int  bottom = std::max (int (height * getBottom ()), parent [1]);
	const int  top    = std::min (int (height * getTop ()),    parent [1] + parent [3]);

	return Vector4i (left,
	                 bottom,
	                 std::max (0, right - left),
	                 std::max (0, top - bottom));
}

float
Viewport::getLeft () const
{
	return clipBoundary () .size () > 0 ? clipBoundary () [0] : 0.0f;
}

float
Viewport::getRight () const
{
	return clipBoundary () .size () > 1 ? clipBoundary () [1] : 1.0f;
}

float
Viewport::getBottom () const
{
	return clipBoundary () .size () > 2 ? clipBoundary () [2] : 0.0f;
}

float
Viewport::getTop () const
{
	return clipBoundary () .size () > 3 ? clipBoundary () [3] : 1.0f;
}

void
Viewport::traverse (const TraverseType type)
{
	push ();

	X3DGroupingNode::traverse (type);

	pop ();
}

void
Viewport::push ()
{
	getViewVolumes () .emplace_back (getProjectionMatrix () .get (), getScissor ());
}

void
Viewport::pop ()
{
	getViewVolumes () .pop_back ();
}

void
Viewport::enable ()
{
	const auto viewport = getRectangle ();

	glViewport (viewport [0],
	            viewport [1],
	            viewport [2],
	            viewport [3]);

	glScissor (viewport [0],
	           viewport [1],
	           viewport [2],
	           viewport [3]);
}

void
Viewport::disable ()
{
	const auto viewport = getBrowser () -> getRectangle ();

	glViewport (viewport [0],
	            viewport [1],
	            viewport [2],
	            viewport [3]);

	glScissor (viewport [0],
	           viewport [1],
	           viewport [2],
	           viewport [3]);
}

} // X3D
} // titania
