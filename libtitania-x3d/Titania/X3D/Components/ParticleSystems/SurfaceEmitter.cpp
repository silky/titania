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

#include "SurfaceEmitter.h"

#include "../../Execution/X3DExecutionContext.h"

namespace titania {
namespace X3D {

SurfaceEmitter::SurfaceEmitter (X3DExecutionContext* const executionContext) :
	          X3DBaseNode (executionContext -> getBrowser (), executionContext), 
	X3DParticleEmitterNode (),                                                    
	        set_coordinate (),                                                    // SFInt32 [in] set_coordinate
	            coordIndex ({ -1 }),                                              // MFInt32 [ ]  coordIndex      -1          [0,∞) or -1
	               surface ()                                                     // SFNode  [ ]  surface         NULL        [X3DGeometryNode]
{
	setComponent ("ParticleSystems");
	setTypeName ("SurfaceEmitter");

	addField (inputOutput,    "metadata",       metadata);
	addField (inputOutput,    "speed",          speed);
	addField (inputOutput,    "variation",      variation);
	addField (initializeOnly, "mass",           mass);
	addField (initializeOnly, "surfaceArea",    surfaceArea);
	addField (inputOnly,      "set_coordinate", set_coordinate);
	addField (initializeOnly, "coordIndex",     coordIndex);
	addField (initializeOnly, "surface",        surface);
}

X3DBaseNode*
SurfaceEmitter::create (X3DExecutionContext* const executionContext) const
{
	return new SurfaceEmitter (executionContext);
}

} // X3D
} // titania
