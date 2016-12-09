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

#include "CollidableOffset.h"

#include "../../Execution/X3DExecutionContext.h"
#include "../../Rendering/X3DRenderObject.h"
#include "../RigidBodyPhysics/X3DNBodyCollidableNode.h"

namespace titania {
namespace X3D {

const ComponentType CollidableOffset::component      = ComponentType::RIGID_BODY_PHYSICS;
const std::string   CollidableOffset::typeName       = "CollidableOffset";
const std::string   CollidableOffset::containerField = "children";

CollidableOffset::Fields::Fields () :
	collidable (new SFNode ())
{ }

CollidableOffset::CollidableOffset (X3DExecutionContext* const executionContext) :
	           X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DNBodyCollidableNode (),
	                fields (),
	        collidableNode ()
{
	addType (X3DConstants::CollidableOffset);

	addField (inputOutput,    "metadata",    metadata ());
	addField (inputOutput,    "enabled",     enabled ());
	addField (inputOutput,    "translation", translation ());
	addField (inputOutput,    "rotation",    rotation ());
	addField (initializeOnly, "bboxSize",    bboxSize ());
	addField (initializeOnly, "bboxCenter",  bboxCenter ());
	addField (initializeOnly, "collidable",  collidable ());

	addChildren (collidableNode);
}

X3DBaseNode*
CollidableOffset::create (X3DExecutionContext* const executionContext) const
{
	return new CollidableOffset (executionContext);
}

void
CollidableOffset::initialize ()
{
	X3DNBodyCollidableNode::initialize ();

	collidable () .addInterest (this, &CollidableOffset::set_collidable);
	addInterest (this, &CollidableOffset::eventsProcessed);

	set_collidable ();
	eventsProcessed ();
}

Box3d
CollidableOffset::getBBox () const
{
	if (bboxSize () == Vector3f (-1, -1, -1))
	{
		const auto boundedObject = x3d_cast <X3DBoundedObject*> (collidableNode);

		if (boundedObject)
			return boundedObject -> getBBox () * getMatrix ();

		return Box3d ();
	}

	return Box3d (bboxSize () .getValue (), bboxCenter () .getValue ()) * getMatrix ();
}

const CollidableGeometry &
CollidableOffset::getCollidableGeometry () const
{
	if (collidableNode)
	{
		const auto & collidableGeometry = collidableNode -> getCollidableGeometry ();
	
		collidableGeometry .matrix *= getMatrix ();
	
		return collidableGeometry;
	}

	throw Error <INVALID_NODE> ("CollidableShape::getCollidableGeometry");
}

void
CollidableOffset::set_collidable ()
{
	if (collidableNode)
		collidableNode -> isCameraObject () .removeInterest (const_cast <SFBool &> (isCameraObject ()));

	collidableNode .set (x3d_cast <X3DNBodyCollidableNode*> (collidable ()));

	if (collidableNode)
	{
		collidableNode -> isCameraObject () .addInterest (const_cast <SFBool &> (isCameraObject ()));

		setCameraObject (collidableNode -> isCameraObject ());
	}
	else
		setCameraObject (false);
}

void
CollidableOffset::eventsProcessed ()
{
	setMatrix (translation () .getValue (), rotation () .getValue ());
}

void
CollidableOffset::traverse (const TraverseType type, X3DRenderObject* const renderObject)
{
	if (collidableNode)
	{
		renderObject -> getModelViewMatrix () .push ();
		renderObject -> getModelViewMatrix () .mult_left (getMatrix ());

		collidableNode -> traverse (type, renderObject);
	
		renderObject -> getModelViewMatrix () .pop ();
	}
}

CollidableOffset::~CollidableOffset ()
{ }

} // X3D
} // titania
