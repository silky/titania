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

#include "FaceSelection.h"

#include "../../Execution/X3DExecutionContext.h"
#include "../../Components/Geometry3D/IndexedFaceSet.h"
#include "../../Components/Geospatial/GeoCoordinate.h"
#include "../../Components/NURBS/CoordinateDouble.h"
#include "../../Components/Rendering/Coordinate.h"
#include "../../Rendering/Tessellator.h"

namespace titania {
namespace X3D {

const ComponentType FaceSelection::component      = ComponentType::TITANIA;
const std::string   FaceSelection::typeName       = "FaceSelection";
const std::string   FaceSelection::containerField = "selection";

FaceSelection::Fields::Fields () :
	geometry (new SFNode ())
{ }

FaceSelection::FaceSelection (X3DExecutionContext* const executionContext) :
	     X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DFaceSelection (),
	          fields (),
	    geometryNode (),
	       coordNode (),
	       faceIndex (),
	     faceNumbers (),
	      pointIndex ()
{
	addType (X3DConstants::FaceSelection);

	addField (inputOutput, "geometry", geometry ());

	addChildren (geometryNode,
                coordNode);
}

FaceSelection*
FaceSelection::create (X3DExecutionContext* const executionContext) const
{
	return new FaceSelection (executionContext);
}

void
FaceSelection::initialize ()
{
	X3DFaceSelection::initialize ();

	geometry () .addInterest (this, &FaceSelection::set_geometry);

	set_geometry (geometry ());
}

void
FaceSelection::set_geometry (const SFNode & value)
{
	if (geometryNode)
	{
		geometryNode -> coordIndex () .removeInterest (this, &FaceSelection::set_coordIndex);
		geometryNode -> getCoord ()   .removeInterest (this, &FaceSelection::set_coord);
	}

	geometryNode = value;

	if (geometryNode)
	{
		geometryNode -> coordIndex () .addInterest (this, &FaceSelection::set_coordIndex);
		geometryNode -> getCoord ()   .addInterest (this, &FaceSelection::set_coord);

		set_coordIndex (geometryNode -> coordIndex ());
		set_coord (geometryNode -> getCoord ());
	}
	else
	{
		set_coordIndex (MFInt32 ());
		set_coord (nullptr);
	}
}

void
FaceSelection::set_coordIndex (const MFInt32 & coordIndex)
{
	// Update face index

	faceIndex   .clear ();
	faceNumbers .clear ();

	size_t face       = 0;
	size_t vertex     = 0;
	size_t faceNumber = 0;

	for (const int32_t index : coordIndex)
	{
		faceNumbers .emplace_back (faceNumber);

		if (index < 0)
		{
			faceNumber += 1;
			face       += vertex + 1;
			vertex      = 0;
			continue;
		}

		faceIndex .emplace (index, Face { face, vertex });

		++ vertex;
	}
}

///  Updates the coordinate node of this selection.
void
FaceSelection::set_coord (const X3DPtr <X3DCoordinateNode> & value)
{
	if (coordNode)
	{
		switch (coordNode -> getType () .back ())
		{
			case X3DConstants::Coordinate:
				X3DPtr <Coordinate> (coordNode) -> point () .removeInterest (this, &FaceSelection::set_point);
				break;
			case X3DConstants::CoordinateDouble:
				X3DPtr <CoordinateDouble> (coordNode) -> point () .removeInterest (this, &FaceSelection::set_point);
				break;
			case X3DConstants::GeoCoordinate:
				X3DPtr <GeoCoordinate> (coordNode) -> point () .removeInterest (this, &FaceSelection::set_point);
				break;
			default:
				break;
		}
	}

	coordNode = value;
	
	if (coordNode)
	{
		switch (coordNode -> getType () .back ())
		{
			case X3DConstants::Coordinate:
				X3DPtr <Coordinate> (coordNode) -> point () .addInterest (this, &FaceSelection::set_point);
				break;
			case X3DConstants::CoordinateDouble:
				X3DPtr <CoordinateDouble> (coordNode) -> point () .addInterest (this, &FaceSelection::set_point);
				break;
			case X3DConstants::GeoCoordinate:
				X3DPtr <GeoCoordinate> (coordNode) -> point () .addInterest (this, &FaceSelection::set_point);
				break;
			default:
				break;
		}
	}

	set_point ();
}

///  Updates the coordinate node of this selection.
void
FaceSelection::set_point ()
{
	pointIndex .clear ();

	if (coordNode)
	{
		for (size_t i = 0, size = coordNode -> getSize (); i < size; ++ i)
			pointIndex .emplace (coordNode -> get1Point (i), i);
	}
}

///  Returns a array of all faces in geometry.
std::vector <size_t>
FaceSelection::getFaces () const
{
	std::vector <size_t> faces;

	if (not geometryNode)
		return faces;

	size_t face  = 0;
	size_t count = 0;

	for (const int32_t index : geometryNode -> coordIndex ())
	{
		if (index < 0)
		{
			if (count < 3 and count > 0)
				faces .pop_back ();

			face  += count + 1;
			count  = 0;
			continue;
		}

		if (count == 0)
			faces .emplace_back (face);

		++ count;
	}

	return faces;
}

FaceSelection::Faces
FaceSelection::getAdjacentFaces (const int32_t index) const
{
	Faces faces;

	for (const auto & face : faceIndex .equal_range (index))
		faces .emplace_back (face .second);

	return faces;
}

FaceSelection::Faces
FaceSelection::getAdjacentFaces (const Points & points) const
{
	Faces faces;

	for (const auto & index : points)
	{
		for (const auto & face : faceIndex .equal_range (index))
			faces .emplace_back (face .second);
	}

	return faces;
}

std::vector <size_t>
FaceSelection::getAdjacentFaces (const Edge & edge) const
{
	const auto faces0 = getAdjacentFaces (Points { geometryNode -> coordIndex () [edge .index0] });
	const auto faces1 = getAdjacentFaces (Points { geometryNode -> coordIndex () [edge .index1] });

	std::vector <size_t> set0;
	std::vector <size_t> set1;
	std::vector <size_t> intersection;

	for (const auto & face : faces0)
	   set0 .emplace_back (face .index);

	for (const auto & face : faces1)
	   set1 .emplace_back (face .index);

	std::sort (set0 .begin (), set0 .end ());
	std::sort (set1 .begin (), set1 .end ());

	std::set_intersection (set0 .begin(), set0 .end(),
	                       set1 .begin(), set1 .end (),
	                       std::back_inserter (intersection));


	return intersection;
}

///  Finds the nearest face for hitPoint in faces.
FaceSelection::Face
FaceSelection::getClosestFace (const Vector3d & hitPoint, const Faces & faces)
{
	// Get distances of faces to hitPoint.
	
	double minDistance = std::numeric_limits <double>::infinity ();
	size_t minIndex    = 0;
	size_t i           = 0;

	for (const auto & face : faces)
	{
		const auto vertices = getFaceVertices (face .index);

		if (vertices .size () < 3)
		{
			++ i;
			continue;
		}

		if (geometryNode -> convex () or vertices .size () == 3)
		{
			for (size_t v = 0, size = vertices .size (); v < size; ++ v)
			{
			   const auto i0       = vertices [v == 0 ? vertices .size () - 1 : v - 1];
			   const auto i1       = vertices [v];
			   const auto i2       = vertices [(v + 1) % vertices .size ()];
				const auto index0   = geometryNode -> coordIndex () [i0] .getValue ();
				const auto index1   = geometryNode -> coordIndex () [i1] .getValue ();
				const auto index2   = geometryNode -> coordIndex () [i2] .getValue ();
				const auto point0   = coordNode -> get1Point (index0);
				const auto point1   = coordNode -> get1Point (index1);
				const auto point2   = coordNode -> get1Point (index2);
				const auto distance = triangle_distance_to_point (point0, point1, point2, hitPoint);
	
				if (distance < minDistance)
				{
					minDistance = distance;
					minIndex    = i;
				}
			}
		}
		else
		{
			opengl::tessellator <size_t> tessellator;
		
			tessellator .begin_polygon ();
			tessellator .begin_contour ();
		
			for (const auto & vertex : vertices)
				tessellator .add_vertex (coordNode -> get1Point (geometryNode -> coordIndex () [vertex]), vertex);
		
			tessellator .end_contour ();
			tessellator .end_polygon ();
		
			const auto triangles = tessellator .triangles ();
	
			for (size_t v = 0, size = triangles .size (); v < size; )
			{
				const auto point0   = triangles [v ++] .point ();
				const auto point1   = triangles [v ++] .point ();
				const auto point2   = triangles [v ++] .point ();
				const auto distance = triangle_distance_to_point (point0, point1, point2, hitPoint);

				if (distance < minDistance)
				{
					minDistance = distance;
					minIndex    = i;
				}
			}
		}

		++ i;
	}

	return faces [minIndex];
}

///  Finds the nearest face for hitPoint in faces.
FaceSelection::Face
FaceSelection::getClosestFace (const Vector3d & hitPoint, const std::vector <size_t> & indices)
{
	Faces faces;

	for (const auto & faceIndex : indices)
	   faces .emplace_back (Face { faceIndex, 0 });
	
	return getClosestFace (hitPoint, faces);
}

///  Returns all indices to the coordIndex for this face.
std::vector <size_t>
FaceSelection::getFaceVertices (const size_t face) const
{
	std::vector <size_t> vertices;

	for (size_t i = face, size = geometryNode -> coordIndex () .size (); i < size; ++ i)
	{
		const auto index = geometryNode -> coordIndex () [i] .getValue ();

		if (index < 0)
			break;

		vertices .emplace_back (i);
	}

	return vertices;
}

size_t
FaceSelection::getFaceNumber (const size_t vertex) const
{
	return faceNumbers [vertex];
}

size_t
FaceSelection::getNumFaces () const
{
	return faceNumbers .size ();
}

///  Return the nearest edge for hitPoint.
FaceSelection::Edge
FaceSelection::getClosestEdge (const Vector3d & hitPoint, const std::vector <size_t> & vertices) const
{
	std::vector <std::vector <size_t>> polygon;

	if (geometryNode -> convex ())
		polygon .emplace_back (vertices);
	
	else
	{
		opengl::tessellator <size_t> tessellator;
	
		tessellator .begin_polygon ();
		tessellator .begin_contour ();
	
		for (const auto & vertex : vertices)
			tessellator .add_vertex (coordNode -> get1Point (geometryNode -> coordIndex () [vertex]), vertex);
	
		tessellator .end_contour ();
		tessellator .end_polygon ();
	
		const auto triangles = tessellator .triangles ();

		for (size_t v = 0, size = triangles .size (); v < size; )
		{
		   polygon .emplace_back ();
			polygon .back () .emplace_back (std::get <0> (triangles [v ++] .data ()));
			polygon .back () .emplace_back (std::get <0> (triangles [v ++] .data ()));
			polygon .back () .emplace_back (std::get <0> (triangles [v ++] .data ()));
		}
	}

	std::vector <std::pair <size_t, size_t>> indices;
	std::vector <LineSegment3d>              segments;
	std::vector <double>                     distances;

	for (const auto & element : polygon)
	{
		for (size_t i = 0, size = element .size (); i < size; ++ i)
		{
		   const auto i0       = element [i];
		   const auto i1       = element [(i + 1) % size];
			const auto point0   = coordNode -> get1Point (geometryNode -> coordIndex () [i0]);
			const auto point1   = coordNode -> get1Point (geometryNode -> coordIndex () [i1]);
			const auto segment  = LineSegment3d (point0, point1);
			const auto distance = segment .distance (hitPoint);

			indices   .emplace_back (std::make_pair (i0, i1));
			segments  .emplace_back (segment);
			distances .emplace_back (distance);
		}
	}

	const auto iter = std::min_element (distances .begin (), distances .end ());
	const auto i    = iter - distances .begin ();

	return Edge {
		indices [i] .first,
		indices [i] .second,
		segments [i],
		isEdge (vertices, indices [i] .first, indices [i] .second)
	};
}

///  Return the nearest edge for hitPoint.
FaceSelection::Edge
FaceSelection::getClosestEdge (const Line3d & hitRay, const std::vector <size_t> & vertices) const
{
	using Key = std::pair <double, double>;

	static constexpr double epsilon = 1e-4;

	std::map <Key, Edge> distances;

	for (size_t i = 0, size = vertices .size (); i < size; ++ i)
	{
		const auto i0           = vertices [i];
		const auto i1           = vertices [(i + 1) % size];
		const auto point0       = coordNode -> get1Point (geometryNode -> coordIndex () [i0]);
		const auto point1       = coordNode -> get1Point (geometryNode -> coordIndex () [i1]);
		const auto segment      = LineSegment3d (point0, point1);
		auto       closestPoint = Vector3d (0, 0, 0);
		const auto theta        = hitRay .closest_point (segment .line (), closestPoint);
		const auto distance     = abs (hitRay .point () - closestPoint);
		const auto between      = segment .is_between (closestPoint);

		distances .emplace (distance > epsilon and abs (theta) < 0.9
		                    ? Key (not between, distance)
		                    : Key (2, 0),
		                    Edge { i0, i1, LineSegment3d (point0, point1), true });
	}

	return distances .begin () -> second;
}

bool
FaceSelection::isEdge (const std::vector <size_t> & vertices, const size_t index0, const size_t index1) const
{
	for (size_t i = 0, size = vertices .size (); i < size; ++ i)
	{
		if (index0 == vertices [i] and index1 ==vertices [(i + 1) % size])
		   return true;
	}

	return false;
}

///  Finds the all points that are equal to point, the result is an array of point indices.
FaceSelection::Points
FaceSelection::getCoincidentPoints (const Vector3d & point) const
{
	Points coincidentPoints;

	for (const auto & index : pointIndex .equal_range (point))
		coincidentPoints .emplace_back (index .second);

	return coincidentPoints;
}

///  Return the indices to the coordIndex to a given point index.
std::vector <size_t>
FaceSelection::getSharedVertices (const int32_t coordIndex) const
{
	std::vector <size_t> indices;

	for (const auto & face : faceIndex .equal_range (coordIndex))
		indices .emplace_back (face .second .index + face .second .vertex);
	
	return indices;
}

std::vector <size_t>
FaceSelection::getVertices (const int32_t point) const
{
	std::vector <size_t> vertices;

	for (const auto & vertex : faceIndex .equal_range (point))
		vertices .emplace_back (vertex .second .index + vertex .second .vertex);

	return vertices;
}

int32_t
FaceSelection::getClosestPoint (const Vector3d & hitPoint, const std::vector <size_t> & vertices) const
{
	double  distance = std::numeric_limits <double>::infinity ();
	int32_t index    = -1;

	for (const auto i : vertices)
	{
		const auto i0    = geometryNode -> coordIndex () [i];
		const auto point = coordNode -> get1Point (i0);
		const auto d     = abs (hitPoint - point);

		if (d < distance)
		{
			distance = d;
			index    = i0;
		}
	}

	return index;
}

FaceSelection::~FaceSelection ()
{ }

} // X3D
} // titania
