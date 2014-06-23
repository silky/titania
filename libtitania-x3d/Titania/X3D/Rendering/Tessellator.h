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

#ifndef __TITANIA_X3D_RENDERING_TESSELLATOR_H__
#define __TITANIA_X3D_RENDERING_TESSELLATOR_H__

#include <Titania/Math/Numbers/Vector3.h>
#include <iostream>
#include <tuple>
#include <vector>

#include "../Rendering/OpenGL.h"

namespace titania {
namespace opengl {

template <class ... Args>
class tessellator;

template <class ... Args>
class tessellator_vertex
{
public:

	using Data = std::tuple <Args ...>;

	template <class ... A>
	tessellator_vertex (const math::vector3 <double> & point, A && ... data) :
		m_point (point),
		m_data  (std::forward <A> (data) ...)
	{ }

	math::vector3 <double> &
	point ()
	{ return m_point; }

	const math::vector3 <double> &
	point () const
	{ return m_point; }

	const Data &
	data () const
	{ return m_data; }


private:

	math::vector3 <double> m_point;
	Data                   m_data;

};

template <class ... Args>
class polygon_element
{
public:

	using Vertex      = tessellator_vertex <Args ...>;
	using VertexArray = std::vector <Vertex*>;
	using size_type   = typename VertexArray::size_type;

	polygon_element (GLenum type) :
		    m_type (type),
		m_vertices ()
	{ }

	const GLenum &
	type () const
	{ return m_type; }

	const Vertex &
	operator [ ] (size_t i) const
	{ return *m_vertices [i]; }

	size_type
	size () const
	{ return m_vertices .size (); }


private:

	friend class tessellator <Args ...>;

	GLenum      m_type;
	VertexArray m_vertices;

};

template <class ... Args>
class tessellator
{
public:

	using Polygon = std::vector <polygon_element <Args ...>>;
	using Vertex  = tessellator_vertex <Args ...>;

	tessellator ();

	template <class ... A>
	void
	add_vertex (const math::vector3 <double> &, A && ... args);

	template <class ... A>
	void
	add_vertex (const math::vector3 <float> &, A && ... args);

	void
	tessellate ();

	const Polygon &
	polygon () const { return tessellatedPolygon; }

	~tessellator ();


private:

	static
	void tessBeginData (GLenum, void*);

	static
	void
	tessVertexData (void*, void*);

	static
	void tessCombineData (GLdouble [3], void* [4], GLfloat [4], void**, void*);

	static
	void
	tessEndData (void*);

	static
	void tessError (GLenum);

	GLUtesselator*       tess;
	std::vector <Vertex> vertices;
	Polygon              tessellatedPolygon;

};

template <class ... Args>
tessellator <Args ...>::tessellator () :
	              tess (gluNewTess ()),
	          vertices (),
	tessellatedPolygon ()
{
	if (tess)
	{
		gluTessProperty (tess, GLU_TESS_BOUNDARY_ONLY, GLU_FALSE);
		gluTessCallback (tess, GLU_TESS_BEGIN_DATA, _GLUfuncptr (&tessellator::tessBeginData));
		gluTessCallback (tess, GLU_TESS_VERTEX_DATA, _GLUfuncptr (&tessellator::tessVertexData));

		//gluTessCallback(tessellator, GLU_TESS_COMBINE_DATA, (_GLUfuncptr)&IndexedFaceSet::tessCombineData);
		gluTessCallback (tess, GLU_TESS_END_DATA, _GLUfuncptr (&tessellator::tessEndData));
		gluTessCallback (tess, GLU_TESS_ERROR, _GLUfuncptr (&tessellator::tessError));
	}
}

template <class ... Args>
template <class ... A>
inline
void
tessellator <Args ...>::add_vertex (const math::vector3 <double> & point, A && ... args)
{
	vertices .emplace_back ((math::vector3 <double>)point, std::forward <A> (args) ...);
}

template <class ... Args>
template <class ... A>
inline
void
tessellator <Args ...>::add_vertex (const math::vector3 <float> & point, A && ... args)
{
	vertices .emplace_back (math::vector3 <double> (point), std::forward <A> (args) ...);
}

template <class ... Args>
void
tessellator <Args ...>::tessellate ()
{
	gluTessBeginPolygon (tess, &tessellatedPolygon);
	gluTessBeginContour (tess);

	for (auto & vertex : vertices)
		gluTessVertex (tess, vertex .point () .data (), &vertex);

	//gluTessEndContour(tess);
	gluEndPolygon (tess);
}

template <class ... Args>
void
tessellator <Args ...>::tessBeginData (GLenum type, void* polygon_data)
{
	Polygon* polygon = (Polygon*) polygon_data;

	polygon -> emplace_back (type);
}

template <class ... Args>
void
tessellator <Args ...>::tessVertexData (void* vertex_data, void* polygon_data)
{
	Polygon* const polygon = (Polygon*) polygon_data;
	Vertex*  const vertex  = (Vertex*) vertex_data;

	polygon -> back () .m_vertices .emplace_back (vertex);
}

template <class ... Args>
void
tessellator <Args ...>::tessCombineData (GLdouble coords [3], void* vertex_data [4],
                                         GLfloat weight [4], void** outData,
                                         void* polygon_data)
{
	// Not used yet
}

template <class ... Args>
void
tessellator <Args ...>::tessEndData (void* polygon_data)
{ }

template <class ... Args>
void
tessellator <Args ...>::tessError (GLenum err_no)
{
	std::clog << "Warning: tessellation error: '" << (char*) gluErrorString (err_no) << "'." << std::endl;
}

template <class ... Args>
tessellator <Args ...>::~tessellator ()
{
	if (tess)
		gluDeleteTess (tess);
}

} // opengl
} // titania

#endif
