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

#ifndef __TITANIA_X3D_COMPONENTS_RENDERING_X3DGEOMETRY_NODE_H__
#define __TITANIA_X3D_COMPONENTS_RENDERING_X3DGEOMETRY_NODE_H__

#include "../../Browser/Intersection.h"
#include "../../Types/Geometry.h"
#include "../Core/X3DNode.h"
#include "../Texturing/TextureCoordinateGenerator.h"

#include <map>
#include <memory>

namespace titania {
namespace X3D {

class X3DGeometryNode :
	virtual public X3DNode
{
public:

	const Box3f
	getBBox ();

	bool
	isTransparent () const
	{ return colorsRGBA .size (); }

	virtual
	void
	setup ();

	bool
	intersect (const Line3f &, std::deque <std::shared_ptr <Intersection>> &) const;

	bool
	intersect (const Matrix4f &, const Sphere3f &) const;

	virtual
	void
	notify () final;

	virtual
	void
	draw ();

	void
	draw (bool, bool, bool);

	virtual
	void
	dispose ();


protected:

	typedef std::map <size_t, std::deque <size_t>> NormalIndex;

	X3DGeometryNode ();

	///  @name Properties

	std::vector <Vector3f> &
	getTexCoord ()
	{ return texCoords; }

	void
	setTextureCoordinateGenerator (TextureCoordinateGenerator* value)
	{ textureCoordinateGenerator = value; }

	std::vector <Color3f> &
	getColors ()
	{ return colors; }

	std::vector <Color4f> &
	getColorsRGBA ()
	{ return colorsRGBA; }

	std::vector <Vector3f> &
	getNormals ()
	{ return normals; }

	std::vector <Vector3f> &
	getVertices ()
	{ return vertices; }

	void
	setSolid (const bool & value)
	{ solid = value; }

	void
	setCCW (const bool & value)
	{ ccw = value ? GL_CCW : GL_CW; }

	static
	GLenum
	getVertexMode (size_t n)
	{
		switch (n)
		{
			case 3:
				return GL_TRIANGLES;
			case 4:
				return GL_QUADS;
			default:
				return GL_POLYGON;
		}
	}

	void
	addElements (const GLenum count, size_t vertexCount)
	{ elements .emplace_back (count, vertexCount); }

	virtual
	Box3f
	createBBox ();

	void
	buildTexCoord ();

	void
	getTexCoordParams (Vector3f &, float &, int &, int &);

	void
	refineNormals (const NormalIndex &, std::vector <Vector3f> &, float, bool);

	void
	addMirrorVertices (GLenum, const bool);

	void
	update ();

	virtual
	void
	build ();

	virtual
	void
	eventsProcessed ();


private:

	struct Element
	{
		Element (GLenum vertexMode, size_t count) :
			vertexMode (vertexMode),
			count (count)
		{ }

		GLenum vertexMode;
		size_t count;

	};

	void
	clear ();

	void
	transfer ();

	Box3f                       bbox;
	std::vector <Vector3f>      texCoords;
	TextureCoordinateGenerator* textureCoordinateGenerator;
	std::vector <Color3f>       colors;
	std::vector <Color4f>       colorsRGBA;
	std::vector <Vector3f>      normals;
	std::vector <Vector3f>      vertices;
	bool                        solid;
	GLenum                      ccw;
	std::deque <Element>        elements;

	GLenum bufferUsage;
	GLuint texCoordBufferId;
	GLuint colorBufferId;
	GLuint normalBufferId;
	GLuint pointBufferId;

};

} // X3D
} // titania

#endif
