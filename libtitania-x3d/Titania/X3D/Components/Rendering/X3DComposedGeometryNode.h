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

#ifndef __TITANIA_X3D_COMPONENTS_RENDERING_X3DCOMPOSED_GEOMETRY_NODE_H__
#define __TITANIA_X3D_COMPONENTS_RENDERING_X3DCOMPOSED_GEOMETRY_NODE_H__

#include "../EnvironmentalEffects/FogCoordinate.h"
#include "../Rendering/X3DColorNode.h"
#include "../Rendering/X3DCoordinateNode.h"
#include "../Rendering/X3DGeometryNode.h"
#include "../Rendering/X3DNormalNode.h"
#include "../Shaders/X3DVertexAttributeNode.h"
#include "../Texturing/X3DTextureCoordinateNode.h"

namespace titania {
namespace X3D {

class X3DComposedGeometryNode :
	public X3DGeometryNode
{
public:

	SFBool &
	colorPerVertex ()
	{ return *fields .colorPerVertex; }

	const SFBool &
	colorPerVertex () const
	{ return *fields .colorPerVertex; }

	SFBool &
	normalPerVertex ()
	{ return *fields .normalPerVertex; }

	const SFBool &
	normalPerVertex () const
	{ return *fields .normalPerVertex; }

	SFBool &
	solid ()
	{ return *fields .solid; }

	const SFBool &
	solid () const
	{ return *fields .solid; }

	SFBool &
	ccw ()
	{ return *fields .ccw; }

	const SFBool &
	ccw () const
	{ return *fields .ccw; }

	MFNode &
	attrib ()
	{ return *fields .attrib; }

	const MFNode &
	attrib () const
	{ return *fields .attrib; }

	SFNode <X3DBaseNode> &
	fogCoord ()
	{ return *fields .fogCoord; }

	const SFNode <X3DBaseNode> &
	fogCoord () const
	{ return *fields .fogCoord; }

	SFNode <X3DBaseNode> &
	texCoord ()
	{ return *fields .texCoord; }

	const SFNode <X3DBaseNode> &
	texCoord () const
	{ return *fields .texCoord; }

	SFNode <X3DBaseNode> &
	color ()
	{ return *fields .color; }

	const SFNode <X3DBaseNode> &
	color () const
	{ return *fields .color; }

	SFNode <X3DBaseNode> &
	normal ()
	{ return *fields .normal; }

	const SFNode <X3DBaseNode> &
	normal () const
	{ return *fields .normal; }

	SFNode <X3DBaseNode> &
	coord ()
	{ return *fields .coord; }

	const SFNode <X3DBaseNode> &
	coord () const
	{ return *fields .coord; }


protected:

	X3DComposedGeometryNode ();

	virtual
	void
	initialize ();
	
	virtual
	size_t
	getIndex (size_t index)
	{ return index; }
	
	void
	buildTriangles (size_t, bool);

	virtual
	void
	buildTriangleNormals (size_t);


private:

	struct Fields
	{
		Fields ();

		SFBool* const colorPerVertex;
		SFBool* const normalPerVertex;
		SFBool* const solid;
		SFBool* const ccw;
		MFNode* const attrib;
		SFNode <X3DBaseNode>* const fogCoord;
		SFNode <X3DBaseNode>* const texCoord;
		SFNode <X3DBaseNode>* const color;
		SFNode <X3DBaseNode>* const normal;
		SFNode <X3DBaseNode>* const coord;
	};

	Fields fields;

};

} // X3D
} // titania

#endif
