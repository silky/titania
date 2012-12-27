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
 ******************************************************************************/

#include "Viewpoint.h"

#include "../../Browser/Browser.h"
#include "../../Execution/X3DExecutionContext.h"
#include <iostream>

namespace titania {
namespace X3D {

Viewpoint::Viewpoint (X3DExecutionContext* const executionContext, bool addToList) :
	         X3DBaseNode (executionContext -> getBrowser (), executionContext), 
	    X3DViewpointNode (addToList),                                           
	            position (0, 0, 10),                                            // SFVec3f [in,out] position           0 0 10        (-∞,∞)
	         fieldOfView (0.785398),                                            // SFFloat [in,out] fieldOfView        π/4           (0,π)
	          timeSensor (),                                                    
	positionInterpolator ()                                                     
{
	setComponent ("Navigation");
	setTypeName ("Viewpoint");

	addField (inputOutput, "metadata",          metadata);
	addField (inputOutput, "description",       description);
	addField (inputOnly,   "set_bind",          set_bind);
	addField (inputOutput, "position",          position);
	addField (inputOutput, "orientation",       orientation);
	addField (inputOutput, "centerOfRotation",  centerOfRotation);
	addField (inputOutput, "fieldOfView",       fieldOfView);
	addField (inputOutput, "jump",              jump);
	addField (inputOutput, "retainUserOffsets", retainUserOffsets);
	addField (outputOnly,  "bindTime",          bindTime);
	addField (outputOnly,  "isBound",           isBound);

	setChildren (timeSensor,
	             positionInterpolator);
}

X3DBaseNode*
Viewpoint::create (X3DExecutionContext* const executionContext) const
{
	return new Viewpoint (executionContext, true);
}

void
Viewpoint::initialize ()
{
	X3DViewpointNode::initialize ();

	timeSensor                  = new TimeSensor (getExecutionContext ());
	timeSensor -> stopTime      = 1;
	timeSensor -> cycleInterval = 0.2;
	timeSensor -> setup ();

	positionInterpolator        = new PositionInterpolator (getExecutionContext ());
	positionInterpolator -> key = { 0, 1 };
	positionInterpolator -> setup ();

	timeSensor           -> fraction_changed .addInterest (positionInterpolator -> set_fraction);
	timeSensor           -> isActive         .addInterest (this, &Viewpoint::set_active);
	positionInterpolator -> value_changed    .addInterest (translation);

	set_bind .addInterest (this, &Viewpoint::_set_bind);
}

Vector3f
Viewpoint::getPosition () const
{
	return position + translation;
}

void
Viewpoint::lookAt (Box3f bbox)
{
	std::clog << "Look at using viewpoint: " << description << "." << std::endl;
	
	__LOG__ << bbox << std::endl;
	__LOG__ << abs (bbox .size ()) << std::endl;
	
	bbox *= ~getModelViewMatrix ();

	Vector3f translation = bbox .center ()
	                       + getOrientation () * (Vector3f (0, 0, bbox .radius2 () / std::tan (fieldOfView * 0.5f)))
	                       - position;

	timeSensor -> startTime          = 1;
	positionInterpolator -> keyValue = { this -> translation, translation };

	centerOfRotation = bbox .center ();
	center           = Vector3f ();
	set_bind         = true;

	std::clog << getTypeName () << " {" << std::endl;
	std::clog << "  position " << getPosition () << std::endl;
	std::clog << "  orientation " << getOrientation () << std::endl;
	std::clog << "  centerOfRotation " << getCenterOfRotation () + center << std::endl;
	std::clog << "}" << std::endl;
}

void
Viewpoint::set_active (const bool & value)
{
	if (not value)
	{
		for (const auto & layer : getLayers ())
			layer -> navigationInfoStack .top () -> transitionComplete = getCurrentTime ();
	}
}

void
Viewpoint::_set_bind ()
{
	if (set_bind)
	{
		if (not jump)
		{
			Vector3f   t;
			Rotation4f r;
			getDifferenceMatrix () .get (t, r);

			translation = t;
			rotation    = r;
		}
	}
}

void
Viewpoint::reshape (const float zNear, const float zFar)
{
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	GLfloat viewport [4];
	glGetFloatv (GL_VIEWPORT, viewport);

	GLfloat width  = viewport [2];
	GLfloat height = viewport [3];

	float ratio = std::tan (fieldOfView / 2) * zNear;

	if (width > height)
	{
		float aspect = width * ratio / height;
		glFrustum (-aspect, aspect, -ratio, ratio, zNear, zFar);
	}
	else
	{
		float aspect = height * ratio / width;
		glFrustum (-ratio, ratio, -aspect, aspect, zNear, zFar);
	}

	glMatrixMode (GL_MODELVIEW);
}

void
Viewpoint::display ()
{
	setModelViewMatrix (ModelViewMatrix4f ());

	Matrix4f transformationMatrix = ModelViewMatrix4f ();

	if (isBound)
	{
		if (jump)
		{
			transformationMatrix .translate (getPosition ());
			transformationMatrix .rotate (getOrientation ());

			setTransformationMatrix (transformationMatrix);
		}
		else
		{
			transformationMatrix .translate (getPosition ());
			transformationMatrix .rotate (getOrientation ());

			setTransformationMatrix (transformationMatrix);
		}
	}
	else
	{
		if (not jump)
		{
			transformationMatrix .translate (position);
			transformationMatrix .rotate (orientation);

			setDifferenceMatrix (getCurrentViewpoint () -> getTransformationMatrix () * ~transformationMatrix);
		}
	}
}

} // X3D
} // titania
