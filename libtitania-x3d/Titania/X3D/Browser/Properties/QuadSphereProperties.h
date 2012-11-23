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

#ifndef __TITANIA_X3D_BROWSER_PROPERTIES_QUAD_SPHERE_PROPERTIES_H__
#define __TITANIA_X3D_BROWSER_PROPERTIES_QUAD_SPHERE_PROPERTIES_H__

#include "../Properties/X3DSpherePropertyNode.h"

namespace titania {
namespace X3D {

//	Property Name           Value data type      Description
//	U_DIMESIONS             Boolean              The browser implementation supports the ab
//	V_DIMESIONS             Boolean              The browser implementation supports the ab

class QuadSphereProperties :
	public X3DSpherePropertyNode
{
public:

	SFInt32 uDimension;
	SFInt32 vDimension;

	QuadSphereProperties (X3DExecutionContext* const);


private:

	virtual
	QuadSphereProperties*
	create (X3DExecutionContext* const) const;

	virtual
	void
	initialize ();

	virtual
	void
	eventsProcessed ();

	std::deque <int32_t>
	createTexIndices ();

	std::deque <Vector2f>
	createTexCoord ();
	
	std::deque <int32_t>
	createIndices ();

	std::deque <Vector3f>
	createPoints ();
	
	virtual
	void
	build ();

	std::vector <Vector3f> normals;
	std::vector <Vector3f> vertices;

};

} // X3D
} // titania

#endif
