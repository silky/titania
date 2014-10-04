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

#include "GridTool.h"

#include "../../Bits/config.h"
#include "../../Execution/X3DExecutionContext.h"

namespace titania {
namespace X3D {

const ComponentType GridTool::component      = ComponentType::TITANIA;
const std::string   GridTool::typeName       = "GridTool";
const std::string   GridTool::containerField = "grid";

GridTool::Fields::Fields () :
	      dimension (new MFInt32 ({ 10, 10, 10 })),
	 majorLineEvery (new MFInt32 ({ 5, 5, 5 })),
	majorLineOffset (new MFInt32 ({ 0, 0, 0 }))
{ }

GridTool::GridTool (X3DExecutionContext* const executionContext) :
	X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DGridTool (),
	     fields ()
{
	addType (X3DConstants::GridTool);

	addField (inputOutput, "metadata",        metadata ());
	addField (inputOutput, "translation",     translation ());
	addField (inputOutput, "rotation",        rotation ());
	addField (inputOutput, "scale",           scale ());
	addField (inputOutput, "dimension",       dimension ());
	addField (inputOutput, "majorLineEvery",  majorLineEvery ());
	addField (inputOutput, "majorLineOffset", majorLineOffset ());
	addField (inputOutput, "color",           color ());
	addField (inputOutput, "lineColor",       lineColor ());
	addField (inputOutput, "majorLineColor",  majorLineColor ());
	addField (inputOutput, "snapToCenter",    snapToCenter ());
	addField (inputOutput, "snapDistance",    snapDistance ());
}

X3DBaseNode*
GridTool::create (X3DExecutionContext* const executionContext) const
{
	return new GridTool (executionContext);
}

void
GridTool::initialize ()
{
	X3DGridTool::initialize ();

	requestAsyncLoad ({ get_tool ("GridTool.x3dv") .str () });
}

void
GridTool::realize ()
{
	X3DGridTool::realize ();

	try
	{
		auto & set_dimension = getToolNode () -> getField <MFInt32> ("set_dimension");
		dimension ()  .addInterest (set_dimension);
		set_dimension .addInterest (dimension ());
		set_dimension .addEvent (dimension ());

		auto & set_majorLineEvery = getToolNode () -> getField <MFInt32> ("set_majorLineEvery");
		majorLineEvery ()  .addInterest (set_majorLineEvery);
		set_majorLineEvery .addInterest (majorLineEvery ());
		set_majorLineEvery .addEvent (majorLineEvery ());

		auto & set_majorLineOffset = getToolNode () -> getField <MFInt32> ("set_majorLineOffset");
		majorLineOffset ()  .addInterest (set_majorLineOffset);
		set_majorLineOffset .addInterest (majorLineOffset ());
		set_majorLineOffset .addEvent (majorLineOffset ());
	}
	catch (const X3DError & error)
	{ }
}

Vector3d
GridTool::getSnapPosition (const Vector3d & position)
{
	auto translation = position;

	const auto x = getSnapPosition (0, translation);

	if (std::abs (x - translation .x ()) < std::abs (snapDistance ()))
		translation .x (x);

	const auto y = getSnapPosition (1, translation);

	if (std::abs (y - translation .y ()) < std::abs (snapDistance ()))
		translation .y (y);

	const auto z = getSnapPosition (2, translation);

	if (std::abs (z - translation .z ()) < std::abs (snapDistance ()))
		translation .z (z);

	return translation;
}

double
GridTool::getSnapPosition (const size_t axis, const Vector3d & position)
{
	const auto o  = dimension () .get1Value (axis) % 2 * 0.5; // Add a half scale if dimension is odd.
	const auto p  = std::round (position [axis]);
	const auto p1 = p - o;
	const auto p2 = p + o;

	return std::abs (p1 - position [axis]) < std::abs (p2 - position [axis]) ? p1 : p2;
}

} // X3D
} // titania
