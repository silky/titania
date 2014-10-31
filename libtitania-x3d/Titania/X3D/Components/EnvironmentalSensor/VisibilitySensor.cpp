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

#include "VisibilitySensor.h"

#include "../../Browser/X3DBrowser.h"
#include "../../Execution/X3DExecutionContext.h"
#include "../../Rendering/ViewVolume.h"
#include "../Layering/X3DLayerNode.h"
#include "../../Tools/EnvironmentalSensor/VisibilitySensorTool.h"

namespace titania {
namespace X3D {

const ComponentType VisibilitySensor::component      = ComponentType::ENVIRONMENTAL_SENSOR;
const std::string   VisibilitySensor::typeName       = "VisibilitySensor";
const std::string   VisibilitySensor::containerField = "children";

VisibilitySensor::VisibilitySensor (X3DExecutionContext* const executionContext) :
	               X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DEnvironmentalSensorNode (),
	                   visible (false)
{
	addType (X3DConstants::VisibilitySensor);

	addField (inputOutput, "metadata",  metadata ());
	addField (inputOutput, "enabled",   enabled ());
	addField (inputOutput, "size",      size ());
	addField (inputOutput, "center",    center ());
	addField (outputOnly,  "enterTime", enterTime ());
	addField (outputOnly,  "exitTime",  exitTime ());
	addField (outputOnly,  "isActive",  isActive ());
}

X3DBaseNode*
VisibilitySensor::create (X3DExecutionContext* const executionContext) const
{
	return new VisibilitySensor (executionContext);
}

void
VisibilitySensor::initialize ()
{
	X3DEnvironmentalSensorNode::initialize ();

	getExecutionContext () -> isLive () .addInterest (this, &VisibilitySensor::set_enabled);
	isLive () .addInterest (this, &VisibilitySensor::set_enabled);

	enabled () .addInterest (this, &VisibilitySensor::set_enabled);
	size ()    .addInterest (this, &VisibilitySensor::set_enabled);

	set_enabled ();
}

void
VisibilitySensor::setExecutionContext (X3DExecutionContext* const executionContext)
throw (Error <INVALID_OPERATION_TIMING>,
       Error <DISPOSED>)
{
	getBrowser () -> sensors ()         .removeInterest (this, &VisibilitySensor::update);
	getExecutionContext () -> isLive () .removeInterest (this, &VisibilitySensor::set_enabled);

	X3DEnvironmentalSensorNode::setExecutionContext (executionContext);

	if (isInitialized ())
	{
		getExecutionContext () -> isLive () .addInterest (this, &VisibilitySensor::set_enabled);

		set_enabled ();
	}
}

void
VisibilitySensor::set_enabled ()
{
	if (enabled () and isLive () and getExecutionContext () -> isLive () and size () not_eq Vector3f ())
		getBrowser () -> sensors () .addInterest (this, &VisibilitySensor::update);

	else
		getBrowser () -> sensors () .removeInterest (this, &VisibilitySensor::update);
}

void
VisibilitySensor::update ()
{
	if (visible)
	{
		if (not isActive ())
		{
			isActive ()  = true;
			enterTime () = getCurrentTime ();
		}

		visible = false;
	}
	else
	{
		if (isActive ())
		{
			isActive () = false;
			exitTime () = getCurrentTime ();
		}
	}
}

void
VisibilitySensor::traverse (const TraverseType type)
{
	switch (type)
	{
		case TraverseType::CAMERA:
		{
			if (not enabled () or visible)
				break;
				
			if (size () == Vector3f (-1, -1, -1))
				visible = true;

			else
			{
				visible = getCurrentLayer () -> getViewVolumeStack () .back () .intersects (Box3f (size (), center ()) * getModelViewMatrix (type));
			}

			break;
		}
		default:
			break;
	}
}

void
VisibilitySensor::addTool ()
{
	X3DEnvironmentalSensorNode::addTool (new VisibilitySensorTool (this));
}

} // X3D
} // titania
