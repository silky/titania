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

#include "Combine.h"

#include "../Browser/X3DBrowser.h"
#include "../Components/Grouping/X3DGroupingNode.h"
#include "../Components/Texturing/TextureCoordinate.h"
#include "../Components/Rendering/Coordinate.h"
#include "../Components/Rendering/Color.h"
#include "../Components/Rendering/X3DNormalNode.h"
#include "../Rendering/Tessellator.h"
#include "../Editing/Editor.h"

#include <Titania/Utility/Map.h>

namespace titania {
namespace X3D {

Combine::Combine ()
{ }

X3DPtrArray <IndexedFaceSet>
Combine::getIndexedFaceSets (const X3DPtrArray <X3DShapeNode> & shapes)
{
	// Filter geometries.

	X3DPtrArray <IndexedFaceSet> geometryNodes;

	for (const auto & shape : shapes)
	{
		const X3DPtr <IndexedFaceSet> geometryNode (shape -> geometry ());

		if (not geometryNode)
			continue;

		geometryNodes .emplace_back (geometryNode);
	}

	return std::move (geometryNodes);
}

/// Creates a mesh from @a geometryNode and @a coordNode and applys matrix to points.
mesh <double>
Combine::toMesh (const X3DPtr <IndexedFaceSet> & geometryNode, const X3DPtr <X3DCoordinateNode> & coordNode, const Matrix4d & matrix)
{
	// Create mesh.

	auto indices      = mesh <double>::indices_type ();
	auto points       = mesh <double>::points_type ();
	auto coordIndices = std::vector <int32_t> ();

	for (const auto & index : geometryNode -> coordIndex ())
	{
		if (index < 0)
		{
			switch (coordIndices .size ())
			{
				case 0:
				case 1:
				case 2:
					break;
				case 3:
				{
					for (const auto & index : coordIndices)
						indices .emplace_back (index);

					break;
				}
				default:
				{
					opengl::tessellator <size_t> tessellator;
	
					tessellator .begin_polygon ();
					tessellator .begin_contour ();
				
					for (const auto & index : coordIndices)
					{
						const auto point = coordNode -> get1Point (index) * matrix;
			
						tessellator .add_vertex (point, index);
					}
				
					tessellator .end_contour ();
					tessellator .end_polygon ();
				
					const auto triangles = tessellator .triangles ();
			
					for (size_t v = 0, size = triangles .size (); v < size; )
					{
						indices .emplace_back (std::get <0> (triangles [v ++] .data ()));
						indices .emplace_back (std::get <0> (triangles [v ++] .data ()));
						indices .emplace_back (std::get <0> (triangles [v ++] .data ()));
					}
					
					break;
				}
			}

			coordIndices .clear ();
		}
		else
		{
			coordIndices .emplace_back (index);
		}
	}

	for (size_t i = 0, size = coordNode -> getSize (); i < size; ++ i)
		points .emplace_back (coordNode -> get1Point (i) * matrix);

	return mesh <double> (std::move (indices), std::move (points));
}

bool
Combine::geometryUnion (const X3DExecutionContextPtr & executionContext,
                        const X3DPtrArray <X3DShapeNode> & shapes,
                        const UndoStepPtr & undoStep)
throw (Error <INVALID_NODE>,
       Error <DISPOSED>,
       std::domain_error)
{
	return geometryBoolean (mesh_union, executionContext, shapes, undoStep);
}

bool
Combine::geometryDifference (const X3DExecutionContextPtr & executionContext,
                             const X3DPtrArray <X3DShapeNode> & shapes,
                             const UndoStepPtr & undoStep)
throw (Error <INVALID_NODE>,
       Error <DISPOSED>,     
       std::domain_error)
{
	return geometryBoolean (mesh_difference, executionContext, shapes, undoStep);
}

bool
Combine::geometryIntersection (const X3DExecutionContextPtr & executionContext,
                               const X3DPtrArray <X3DShapeNode> & shapes,
                               const UndoStepPtr & undoStep)
throw (Error <INVALID_NODE>,
       Error <DISPOSED>,     
       std::domain_error)
{
	return geometryBoolean (mesh_intersection, executionContext, shapes, undoStep);
}

bool
Combine::geometryExclusion (const X3DExecutionContextPtr & executionContext,
                            const X3DPtrArray <X3DShapeNode> & shapes,
                            const UndoStepPtr & undoStep)
throw (Error <INVALID_NODE>,
       Error <DISPOSED>,     
       std::domain_error)
{
	return geometryBoolean (mesh_exclusion, executionContext, shapes, undoStep);
}

bool
Combine::geometryBoolean (const BooleanOperation & booleanOperation,
                          const X3DExecutionContextPtr & executionContext,
                          const X3DPtrArray <X3DShapeNode> & shapes,
                          const UndoStepPtr & undoStep)
throw (Error <INVALID_NODE>,
       Error <DISPOSED>,
       std::domain_error)
{
	try
	{
		if (not executionContext -> hasComponent (ComponentType::GEOMETRY_3D))
			executionContext -> updateComponent (executionContext -> getBrowser () -> getComponent ("Geometry3D", 2));

		// Choose target.
	
		const auto & masterShape    = shapes .back ();
		const auto   targetGeometry = executionContext -> createNode <IndexedFaceSet> ();
		const auto   targetCoord    = executionContext -> createNode <Coordinate> ();
		const auto   targetMatrix   = ~Editor () .getModelViewMatrix (executionContext -> getMasterScene (), SFNode (masterShape));
	
		targetGeometry -> coord () = targetCoord;
	
		// Filter geometry nodes.
	
		const X3DPtrArray <IndexedFaceSet> geometryNodes = getIndexedFaceSets (shapes);
	
		if (not geometryNodes .empty ())
			targetGeometry -> creaseAngle () = geometryNodes .front () -> creaseAngle ();
	
		// Combine Coordinates.
	
		std::vector <mesh <double>> meshes;
	
		for (const auto & geometryNode : geometryNodes)
		{
			const auto & coordNode = geometryNode -> getCoord ();
	
			if (not coordNode)
				continue;

			// Merge coincident points as preparation for Boolean operation.

			Editor () .mergePoints (geometryNode, 0, undoStep);

			// Generate mesh.

			const auto matrix = Editor () .getModelViewMatrix (geometryNode -> getMasterScene (), SFNode (geometryNode)) * targetMatrix;
		
			meshes .emplace_back (toMesh (geometryNode, coordNode, matrix));
		}
	
		// Test if there is something to do.

		if (meshes .size () < 2)
			return false;

		// Apply Boolean operation.

		auto result = std::move (meshes .front ());

		for (const auto & mesh : std::make_pair (meshes .begin () + 1, meshes .end ()))
			result = booleanOperation (result, mesh);
	
		// Store result in target geometry.

		for (size_t i = 0, size = result .indices () .size (); i < size; )
		{
			targetGeometry -> coordIndex () .emplace_back (result .indices () [i++]);
			targetGeometry -> coordIndex () .emplace_back (result .indices () [i++]);
			targetGeometry -> coordIndex () .emplace_back (result .indices () [i++]);
			targetGeometry -> coordIndex () .emplace_back (-1);
		}

		targetCoord -> point () .assign (result .points () .begin (), result .points () .end ());

		// Sometimes CGAL does not return solid geometries, thus we merge this points.
      executionContext -> realize ();
MFInt32 c = targetGeometry -> coordIndex ();
__LOG__ << targetCoord -> getSize () << std::endl;
		targetGeometry -> mergePoints (1e-6);
__LOG__ << (c == targetGeometry -> coordIndex ()) << std::endl;
__LOG__ << targetCoord -> getSize () << std::endl;

		// Replace node.

		Editor () .replaceNode (masterShape -> getExecutionContext (), SFNode (masterShape), masterShape -> geometry (), SFNode (targetGeometry), undoStep);

		return true;
	}
	catch (const std::exception & error)
	{
		__LOG__ << error .what () << std::endl;

		throw Error <INVALID_NODE> ("Boolean operation not possible.");
	}
	catch (...)
	{
		throw Error <INVALID_NODE> ("Boolean operation not possible.");
	}
}

bool
Combine::combineGeometry (const X3DExecutionContextPtr & executionContext,
                          const X3DPtrArray <X3DShapeNode> & shapes,
                          const UndoStepPtr & undoStep)
throw (Error <INVALID_NODE>,
       Error <DISPOSED>,
       std::domain_error)
{
	if (not executionContext -> hasComponent (ComponentType::GEOMETRY_3D))
		executionContext -> updateComponent (executionContext -> getBrowser () -> getComponent ("Geometry3D", 2));

	// Choose target

	const auto & masterShape    = shapes .back ();
	const auto   targetGeometry = executionContext -> createNode <IndexedFaceSet> ();
	const auto   targetCoord    = X3DPtr <X3DCoordinateNode> (executionContext -> createNode <Coordinate> ());
	const auto   targetMatrix   = ~Editor () .getModelViewMatrix (executionContext -> getMasterScene (), SFNode (masterShape));

	targetGeometry -> coord () = targetCoord;

	// Filter geometry nodes.

	const X3DPtrArray <IndexedFaceSet> geometryNodes = getIndexedFaceSets (shapes);

	if (not geometryNodes .empty ())
		targetGeometry -> creaseAngle () = geometryNodes .back () -> creaseAngle ();

	// Combine Coordinates

	if (geometryNodes .size () < 2)
		return false;

	for (const auto & geometryNode : geometryNodes)
	{
		undoStep -> addUndoFunction (&MFInt32::setValue, std::ref (geometryNode -> colorIndex    ()), geometryNode -> colorIndex    ());
		undoStep -> addUndoFunction (&MFInt32::setValue, std::ref (geometryNode -> texCoordIndex ()), geometryNode -> texCoordIndex ());
		undoStep -> addUndoFunction (&MFInt32::setValue, std::ref (geometryNode -> normalIndex   ()), geometryNode -> normalIndex   ());
		undoStep -> addUndoFunction (&SFNode::setValue,  std::ref (geometryNode -> color         ()), geometryNode -> color         ());
		undoStep -> addUndoFunction (&SFNode::setValue,  std::ref (geometryNode -> texCoord      ()), geometryNode -> texCoord      ());
		undoStep -> addUndoFunction (&SFNode::setValue,  std::ref (geometryNode -> normal        ()), geometryNode -> normal        ());
	}

	combine (executionContext, geometryNodes, targetGeometry, targetCoord, targetMatrix);

	for (const auto & geometryNode : geometryNodes)
	{
		undoStep -> addRedoFunction (&MFInt32::setValue, std::ref (geometryNode -> colorIndex    ()), geometryNode-> colorIndex    ());
		undoStep -> addRedoFunction (&MFInt32::setValue, std::ref (geometryNode -> texCoordIndex ()), geometryNode-> texCoordIndex ());
		undoStep -> addRedoFunction (&MFInt32::setValue, std::ref (geometryNode -> normalIndex   ()), geometryNode-> normalIndex   ());
		undoStep -> addRedoFunction (&SFNode::setValue,  std::ref (geometryNode -> color         ()), geometryNode-> color         ());
		undoStep -> addRedoFunction (&SFNode::setValue,  std::ref (geometryNode -> texCoord      ()), geometryNode-> texCoord      ());
		undoStep -> addRedoFunction (&SFNode::setValue,  std::ref (geometryNode -> normal        ()), geometryNode-> normal        ());
	}

	// Replace node

	Editor () .replaceNode (masterShape -> getExecutionContext (), SFNode (masterShape), masterShape -> geometry (), SFNode (targetGeometry), undoStep);

	return true;
}

std::vector <int32_t>
Combine::combine (const X3DExecutionContextPtr & executionContext,
                  const X3DPtrArray <IndexedFaceSet> & geometryNodes,
                  const X3DPtr <IndexedFaceSet> & targetGeometry,
                  const X3DPtr <X3DCoordinateNode> & targetCoord,
                  const Matrix4d & targetMatrix)
{
	std::vector <int32_t> points;

	// Add colors, texCoords and normals if needed.

	bool addColors    = targetGeometry -> getColor ();
	bool addTexCoords = targetGeometry -> getTexCoord ();
	bool addNormals   = targetGeometry -> getNormal ();

	for (const auto & geometryNode : geometryNodes)
	{
		if (geometryNode -> getColor ())
			addColors = true;
	
		if (geometryNode -> getTexCoord ())
			addTexCoords = true;

		if (geometryNode -> getNormal ())
			addNormals = true;
	}

	const auto numVertices = targetGeometry -> coordIndex () .size ();
	const auto numFaces    = targetGeometry -> colorPerVertex () or targetGeometry -> normalPerVertex ()
	                         ? std::count_if (targetGeometry -> coordIndex () .begin (), targetGeometry -> coordIndex () .end (), [ ] (const int32_t i) { return i < 0; })
	                         : 0;

	if (addColors)
	{
		if (targetGeometry -> getColor ())
		{
			// If there are not enough indices in colorIndex, add them.

			if (targetGeometry -> colorPerVertex ())
			{
				for (size_t i = targetGeometry -> colorIndex () .size (); i < numVertices; ++ i)
					targetGeometry -> colorIndex () .emplace_back (targetGeometry -> coordIndex () [i]);
			}
			else
			{
				for (size_t i = targetGeometry -> colorIndex () .size (); i < numFaces; ++ i)
					targetGeometry -> colorIndex () .emplace_back (i);
			}
		}
		else
			targetGeometry -> addColors ();
	}

	if (addTexCoords)
	{
		if (targetGeometry -> getTexCoord ())
		{
			// If there are not enough indices in texCoordIndex, add them.

			for (size_t i = targetGeometry -> texCoordIndex () .size (); i < numVertices; ++ i)
				targetGeometry -> texCoordIndex () .emplace_back (targetGeometry -> coordIndex () [i]);
		}
		else
			targetGeometry -> addTexCoords ();
	}

	if (addNormals)
	{
		if (targetGeometry -> getNormal ())
		{
			// If there are not enough indices in normalIndex, add them.

			if (targetGeometry -> normalPerVertex ())
			{
			   for (size_t i = targetGeometry -> normalIndex () .size (); i < numVertices; ++ i)
					targetGeometry -> normalIndex () .emplace_back (targetGeometry -> coordIndex () [i]);
			}
			else
			{
				for (size_t i = targetGeometry -> normalIndex () .size (); i < numFaces; ++ i)
					targetGeometry -> normalIndex () .emplace_back (i);
			}
		}
		else
			targetGeometry -> addNormals ();
	}

	for (const auto & geometryNode : geometryNodes)
	{
		const auto numVertices = geometryNode -> coordIndex () .size ();
		const auto numFaces    = geometryNode -> colorPerVertex () or geometryNode -> normalPerVertex ()
	                            ? std::count_if (geometryNode -> coordIndex () .begin (), geometryNode -> coordIndex () .end (), [ ] (const int32_t i) { return i < 0; })
	                            : 0;

		if (addColors)
		{
			if (geometryNode -> getColor ())
			{
				if (geometryNode -> colorPerVertex ())
				{
					for (size_t i = geometryNode -> colorIndex () .size (); i < numVertices; ++ i)
						geometryNode -> colorIndex () .emplace_back (geometryNode -> coordIndex () [i]);
				}
				else
				{
					for (size_t i = geometryNode -> colorIndex () .size (); i < numFaces; ++ i)
						geometryNode -> colorIndex () .emplace_back (i);
				}
			}
			else
				geometryNode -> addColors ();
		}

		if (addTexCoords)
		{
			if (geometryNode -> getTexCoord ())
			{
				for (size_t i = geometryNode -> texCoordIndex () .size (); i < numVertices; ++ i)
					geometryNode -> texCoordIndex () .emplace_back (geometryNode -> coordIndex () [i]);
			}
			else
				geometryNode -> addTexCoords ();
		}

		if (addNormals)
		{
			if (geometryNode -> getNormal ())
			{
				if (geometryNode -> normalPerVertex ())
				{
				   for (size_t i = geometryNode -> normalIndex () .size (); i < numVertices; ++ i)
						geometryNode -> normalIndex () .emplace_back (geometryNode -> coordIndex () [i]);
				}
				else
				{
					for (size_t i = geometryNode -> normalIndex () .size (); i < numFaces; ++ i)
						geometryNode -> normalIndex () .emplace_back (i);
				}
			}
			else
				geometryNode -> addNormals ();
		}
	}

	// Combine

	// We must get the nodes in this way because we have probably called addColor/TexCoord/Normal.
	const auto targetColor    = X3DPtr <X3DColorNode> (targetGeometry -> color ());
	const auto targetTexCoord = X3DPtr <TextureCoordinate> (targetGeometry -> texCoord ());
	const auto targetNormal   = X3DPtr <X3DNormalNode> (targetGeometry -> normal ());

	for (const auto & geometryNode : geometryNodes)
	{
		const auto coordNode = X3DPtr <X3DCoordinateNode> (geometryNode -> coord ());

		if (not coordNode)
			continue;

		// We must get the nodes in this way because we have probably called addColor/TexCoord/Normal.
		const auto color    = X3DPtr <X3DColorNode> (geometryNode -> color ());
		const auto texCoord = X3DPtr <TextureCoordinate> (geometryNode -> texCoord ());
		const auto normal   = X3DPtr <X3DNormalNode> (geometryNode -> normal ());

		std::map <int32_t, int32_t> colorArray;
		std::map <int32_t, int32_t> texCoordArray;
		std::map <int32_t, int32_t> normalArray;
		std::map <int32_t, int32_t> coordArray;

		size_t face = 0;

		for (size_t i = 0, size = geometryNode -> coordIndex () .size (); i < size; ++ i)
		{
		   const int32_t index = geometryNode -> coordIndex () [i];

		   if (index < 0)
		   {
		      ++ face;
		      continue;
		   }

			if (geometryNode -> colorPerVertex ())
				colorArray .emplace (geometryNode -> getVertexColorIndex (i), colorArray .size ());
			else
				colorArray .emplace (geometryNode -> getFaceColorIndex (face), colorArray .size ());

			texCoordArray .emplace (geometryNode -> getVertexTexCoordIndex (i), texCoordArray .size ());

			if (geometryNode -> normalPerVertex ())
				normalArray .emplace (geometryNode -> getVertexNormalIndex (i), normalArray .size ());
			else
				normalArray .emplace (geometryNode -> getFaceNormalIndex (face), normalArray .size ());

			coordArray .emplace (index, coordArray .size ());
		}

		const auto matrix = Editor () .getModelViewMatrix (geometryNode -> getMasterScene (), SFNode (geometryNode)) * targetMatrix;

		face              = 0;
		size_t first      = 0;
		size_t targetSize = targetGeometry -> coordIndex () .size ();

		for (size_t i = 0, size = geometryNode -> coordIndex () .size (); i < size; ++ i)
		{
			const int32_t index = geometryNode -> coordIndex () [i];

			if (index < 0)
			{
				if (geometryNode -> ccw () not_eq targetGeometry -> ccw ())
				{
					// Flip vertex ordering.

					const size_t s = i - first;

					for (size_t f = 0, s1_2 = s / 2; f < s1_2; ++ f)
					{
						const auto i0 = targetSize + first + f;
						const auto i1 = targetSize + first + (s - f - 1);

						if (targetColor and targetGeometry -> colorIndex () .size () >= i1)
							std::swap (targetGeometry -> colorIndex () [i0], targetGeometry -> colorIndex () [i1]);

						if (targetTexCoord and targetGeometry -> texCoordIndex () .size () >= i1)
							std::swap (targetGeometry -> texCoordIndex () [i0], targetGeometry -> texCoordIndex () [i1]);

						if (targetNormal and targetGeometry -> normalIndex () .size () >= i1)
							std::swap (targetGeometry -> normalIndex () [i0], targetGeometry -> normalIndex () [i1]);

						std::swap (targetGeometry -> coordIndex () [i0], targetGeometry -> coordIndex () [i1]);
					}
				}

				++ face;

				first = i + 1;

				if (targetColor)
					targetGeometry -> colorIndex () .emplace_back (-1);

				if (targetTexCoord)
					targetGeometry -> texCoordIndex () .emplace_back (-1);

				if (targetNormal)
					targetGeometry -> normalIndex () .emplace_back (-1);

				targetGeometry -> coordIndex () .emplace_back (-1);
				continue;
			}

			const auto point = coordArray [index] + targetCoord -> getSize ();

			if (targetColor)
			{
				if (geometryNode -> colorPerVertex ())
				{
					try
					{
						targetGeometry -> colorIndex () .emplace_back (colorArray .at (geometryNode -> getVertexColorIndex (i)) + targetColor -> getSize ());
					}
					catch (const std::out_of_range &)
					{
						targetGeometry -> colorIndex () .emplace_back (point);
					}
				}
				else
				{
					try
					{
						targetGeometry -> colorIndex () .emplace_back (colorArray .at (geometryNode -> getFaceColorIndex (face)) + targetColor -> getSize ());
					}
					catch (const std::out_of_range &)
					{
						targetGeometry -> colorIndex () .emplace_back (face);
					}
				}
			}

			if (targetTexCoord)
			{
				try
				{
					targetGeometry -> texCoordIndex () .emplace_back (texCoordArray .at (geometryNode -> getVertexTexCoordIndex (i)) + targetTexCoord -> getSize ());
				}
				catch (const std::out_of_range &)
				{
					targetGeometry -> texCoordIndex () .emplace_back (point);
				}
			}

			if (targetNormal)
			{
				if (geometryNode -> normalPerVertex ())
				{
					try
					{
						targetGeometry -> normalIndex () .emplace_back (normalArray .at (geometryNode -> getVertexNormalIndex (i)) + targetNormal -> getSize ());
					}
					catch (const std::out_of_range &)
					{
						targetGeometry -> normalIndex () .emplace_back (point);
					}
				}
				else
				{
					try
					{
						targetGeometry -> normalIndex () .emplace_back (normalArray .at (geometryNode -> getFaceNormalIndex (face)) + targetNormal -> getSize ());
					}
					catch (const std::out_of_range &)
					{
						targetGeometry -> normalIndex () .emplace_back (face);
					}
				}
			}

			targetGeometry -> coordIndex () .emplace_back (point);
			points .emplace_back (point);
		}

		if (targetColor)
		{
			for (const auto & index : basic::reverse (colorArray))
				targetColor -> set1Color (targetColor -> getSize (), color -> get1Color (index .second));
		}

		if (targetTexCoord)
		{
			for (const auto & index : basic::reverse (texCoordArray))
				targetTexCoord -> set1Point (targetTexCoord -> getSize (), texCoord -> get1Point (index .second));
		}

		if (targetNormal)
		{
			for (const auto & index : basic::reverse (normalArray))
				targetNormal -> set1Vector (targetNormal -> getSize (), normal -> get1Vector (index .second));
		}

		for (const auto & index : basic::reverse (coordArray))
			targetCoord -> set1Point (targetCoord -> getSize (), coordNode -> get1Point (index .second) * matrix);
	}

	// Add Color instead of ColorRGBA if not transparent.

	if (targetColor)
	{
		if (not targetColor -> getTransparent ())
		{
			const auto colorNode = executionContext -> createNode <Color> ();

			for (size_t i = 0, size = targetColor -> getSize (); i < size; ++ i)
				colorNode -> set1Color (i, targetColor -> get1Color (i));
		
			targetGeometry -> color () = colorNode;
		}
	}

	std::sort (points .begin (), points .end ());

	points .erase (std::unique (points .begin (), points .end ()), points .end ());

	return points;
}

void
Combine::removeShapes (const X3DExecutionContextPtr & executionContext,
                       const MFNode & selection,
                       const X3DPtrArray <X3DGroupingNode> & groups,
                       const X3DPtrArray <X3DShapeNode> & shapes,
                       const X3DPtr <X3DShapeNode> & masterShape,
                       const UndoStepPtr & undoStep)
{
	// Remove Shape nodes that are direct selected.

	MFNode nodes;

	for (const auto & node : selection)
	{
		X3DPtr <X3DShapeNode> shape (node);

		if (not shape or shape == masterShape)
			continue;

		nodes .emplace_back (node);
	}

	Editor () .removeNodesFromScene (executionContext, nodes, true, undoStep);

	// Remove Shape nodes from selection.

	nodes .assign (shapes .begin (), shapes .end ());
	nodes .remove (SFNode (masterShape));

	Editor () .removeNodesFromSceneGraph (selection, std::set <SFNode> (nodes .begin (), nodes .end ()), undoStep);

	for (const auto & node : nodes)
		Editor () .removeNodesFromSceneIfNotExists (executionContext, { node }, undoStep);

	// Find empty groups in selection and remove from scene.

	nodes .clear ();

	for (const auto & group : groups)
	{
		if (group -> children () .empty ())
			nodes .emplace_back (group);
	}

	Editor () .removeNodesFromScene (executionContext, nodes, true, undoStep);
}

Combine::~Combine ()
{ }

} // X3D
} // titania
