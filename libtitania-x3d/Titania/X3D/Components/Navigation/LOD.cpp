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

#include "LOD.h"

#include "../../Components/Layering/X3DLayerNode.h"
#include "../../Execution/X3DExecutionContext.h"
#include "../../Rendering/Matrix.h"

namespace titania {
namespace X3D {

LOD::LOD (X3DExecutionContext* const executionContext) :
	     X3DBaseNode (executionContext -> getBrowser (), executionContext), 
	 X3DGroupingNode (),                                                    
	forceTransitions (),                                                    // SFBool  [ ]   forceTransitions  FALSE
	          center (),                                                    // SFVec3f [ ]   center            0 0 0        (-∞,∞)
	           range (),                                                    // MFFloat [ ]   range             [ ]           [0,∞) or -1
	   level_changed ()                                                     // SFInt32 [out] level_changed
{
	setComponent ("Navigation");
	setTypeName ("LOD");

	addField (inputOutput,    "metadata",         metadata);
	addField (initializeOnly, "forceTransitions", forceTransitions);
	addField (initializeOnly, "center",           center);
	addField (initializeOnly, "range",            range);
	addField (outputOnly,     "level_changed",    level_changed);
	addField (initializeOnly, "bboxSize",         bboxSize);
	addField (initializeOnly, "bboxCenter",       bboxCenter);
	addField (inputOnly,      "addChildren",      addChildren);
	addField (inputOnly,      "removeChildren",   removeChildren);
	addField (inputOutput,    "children",         children);

	addFieldAlias ("level", "children");
}

X3DBaseNode*
LOD::create (X3DExecutionContext* const executionContext) const
{
	return new LOD (executionContext);
}

int32_t
LOD::getLevel (TraverseType type)
{
	Matrix4f matrix = ModelViewMatrix4f ();
	
	if (type == TraverseType::CAMERA)
		matrix *= getCurrentViewpoint () -> getInverseTransformationMatrix ();
	
	matrix .translate (center);

	float distance = math::abs (matrix .translation ());

	int32_t level = -1;

	if (range .size ())
	{
		int32_t n = std::min (range .size (), children .size () - 1);

		if (distance < range [0])
			level = 0;

		else
		{
			int32_t i;

			for (i = 0; i < n - 1; ++ i)
			{
				if (range [i] <= distance and distance < range [i + 1])
					level = i + 1;
			}

			if (level == -1)
				level = n;
		}
	}
	else
	{
		level = 0;
	}

	return level;
}

void
LOD::traverse (TraverseType type)
{
	if (not children .size ())
		return;

	int32_t level = getLevel (type);

	if (type == TraverseType::CAMERA)
	{
		if (level_changed not_eq level)
			level_changed = level;
	}

	if (children [level])
		children [level] -> traverse (type);
}

} // X3D
} // titania
