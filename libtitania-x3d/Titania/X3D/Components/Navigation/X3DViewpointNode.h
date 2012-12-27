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

#ifndef __TITANIA_X3D_COMPONENTS_NAVIGATION_X3DVIEWPOINT_NODE_H__
#define __TITANIA_X3D_COMPONENTS_NAVIGATION_X3DVIEWPOINT_NODE_H__

#include "../Core/X3DBindableNode.h"
#include "../../Types/Geometry.h"

namespace titania {
namespace X3D {

class X3DViewpointNode :
	public X3DBindableNode
{
public:

	SFString   description;
	SFRotation orientation;
	SFVec3f    centerOfRotation;
	SFBool     jump;
	SFBool     retainUserOffsets;
	
	SFVec3f    translation;
	SFRotation rotation;
	SFVec3f    center;
	
	virtual
	Vector3f
	getPosition () const = 0;

	Rotation4f
	getOrientation () const;

	Vector3f
	getCenterOfRotation () const;

	void
	setTransformationMatrix (const Matrix4f &);

	const Matrix4f &
	getTransformationMatrix () const { return transformationMatrix; }

	const Matrix4f &
	getInverseTransformationMatrix () const { return inverseTransformationMatrix; }
	
	virtual
	void
	lookAt (Box3f) = 0;

	virtual
	void
	reshape (const float, const float) = 0;

	virtual
	void
	display ();

	virtual
	void
	draw ();


protected:

	X3DViewpointNode (bool);
	
	void
	setModelViewMatrix (const Matrix4f & value) { modelViewMatrix = value; }
	
	const Matrix4f &
	getModelViewMatrix () const { return modelViewMatrix; }

	void
	setDifferenceMatrix (const Matrix4f & value) { differenceMatrix = value; }
	
	const Matrix4f &
	getDifferenceMatrix () const { return differenceMatrix; }

	virtual
	void
	initialize ();

	virtual
	void
	dispose ();


private:

	virtual
	void
	bindToLayer (X3DLayerNode* const);

	virtual
	void
	removeFromLayer (X3DLayerNode* const);
	
	void
	_set_bind ();

	Matrix4f modelViewMatrix;
	Matrix4f transformationMatrix;
	Matrix4f inverseTransformationMatrix;
	Matrix4f differenceMatrix;
	
	bool addToList;

};

} // X3D
} // titania

#endif
