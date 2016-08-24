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

#include "X3DPrimitiveCountEditor.h"

#include <Titania/X3D/Components/Layering/LayerSet.h>
#include <Titania/X3D/Components/Shape/X3DShapeNode.h>
#include <Titania/X3D/Execution/World.h>
#include <Titania/X3D/Rendering/ShapeContainer.h>

namespace titania {
namespace puck {

X3DPrimitiveCountEditor::X3DPrimitiveCountEditor () :
	X3DGeometryPropertiesEditorInterface (),
	                             browser (),
	                    executionContext (),
	                               nodes (0),
	                        opaqueShapes (0),
	                   transparentShapes (0),
			                      polygons (0),
			                         quads (0),
			                     triangles (0),
			                         lines (0),
			                        points (0)
{
	addChildren (browser, executionContext);
}

void
X3DPrimitiveCountEditor::configure ()
{
	getPrimitiveCountCountButton () .set_active (getConfig () -> getInteger ("primitiveCount"));
}

void
X3DPrimitiveCountEditor::on_map_primitive_count ()
{
	on_primitive_count_count_changed ();
}

void
X3DPrimitiveCountEditor::on_unmap_primitive_count ()
{
	getBrowserWindow () -> getSelection () -> getChildren () .removeInterest (this, &X3DPrimitiveCountEditor::update);

	getCurrentBrowser () .removeInterest (this, &X3DPrimitiveCountEditor::update);
	getCurrentBrowser () .removeInterest (this, &X3DPrimitiveCountEditor::set_browser);
	getCurrentContext () .removeInterest (this, &X3DPrimitiveCountEditor::update);
	getCurrentContext () .removeInterest (this, &X3DPrimitiveCountEditor::set_executionContext);

	if (browser)
		browser -> displayed () .removeInterest (this, &X3DPrimitiveCountEditor::update);

	if (executionContext)
		executionContext -> sceneGraph_changed () .removeInterest (this, &X3DPrimitiveCountEditor::update);

	browser          = nullptr;
	executionContext = nullptr;
}

bool
X3DPrimitiveCountEditor::on_crossing_notify_event (GdkEventCrossing*)
{
	update ();
	return false;
}

void
X3DPrimitiveCountEditor::on_primitive_count_count_changed ()
{
	on_unmap_primitive_count ();

	getCurrentBrowser () .addInterest (this, &X3DPrimitiveCountEditor::set_browser);
	getCurrentContext () .addInterest (this, &X3DPrimitiveCountEditor::set_executionContext);
	getCurrentContext () .addInterest (this, &X3DPrimitiveCountEditor::update);

	switch (getPrimitiveCountCountButton () .get_active_row_number ())
	{
		case 0:
		{
			// Entire scene
			getCurrentBrowser () .addInterest (this, &X3DPrimitiveCountEditor::update);
			break;
		}
		case 1:
		{
			// Rendered objects
			break;
		}
		case 2:
		{
			// Selected objects
			getBrowserWindow () -> getSelection () -> getChildren () .addInterest (this, &X3DPrimitiveCountEditor::update);
			break;
		}
	}

	set_browser ();
	set_executionContext ();
	update ();
}

void
X3DPrimitiveCountEditor::update ()
{
	using namespace std::placeholders;

	if (not getCurrentWorld ())
		return;

	nodes             = 0;
	opaqueShapes      = 0;
	transparentShapes = 0;
	polygons          = 0;
	quads             = 0;
	triangles         = 0;
	lines             = 0;
	points            = 0;

	switch (getPrimitiveCountCountButton () .get_active_row_number ())
	{
		case 0:
		{
			// Entire Scene

			X3D::traverse (getCurrentContext () -> getRootNodes (),
			               std::bind (&X3DPrimitiveCountEditor::traverse, this, _1),
			               true,
			               X3D::TRAVERSE_ROOT_NODES |
			               X3D::TRAVERSE_PROTOTYPE_INSTANCES |
			               X3D::TRAVERSE_INLINE_NODES |
			               X3D::TRAVERSE_CLONED_NODES);

			break;
		}
		case 1:
		{
			for (const auto & layer : getCurrentWorld () -> getLayerSet () -> getLayers ())
			{
				X3D::traverse (layer -> children (),
				               [&] (X3D::SFNode & ) { ++ nodes; return true; },
				               true,
				               X3D::TRAVERSE_ROOT_NODES |
				               X3D::TRAVERSE_PROTOTYPE_INSTANCES |
				               X3D::TRAVERSE_INLINE_NODES |
				               X3D::TRAVERSE_VISIBLE_NODES |
				               X3D::TRAVERSE_CLONED_NODES);
			}

			// Rendered Objects

			for (const auto & layer : getCurrentWorld () -> getLayerSet () -> getLayers ())
			{
				for (const auto & container : basic::make_range (layer -> getOpaqueShapes () .begin (), layer -> getNumOpaqueShapes ()))
				{
					if (container -> getShape () -> isTransparent ())
						++ transparentShapes;
					else
						++ opaqueShapes;

					const auto & geometry = container -> getShape () -> getGeometry ();

					if (geometry)
					{
						for (const auto & element : geometry -> getElements ())
							count (element);
					}
				}

				for (const auto & container : basic::make_range (layer -> getTransparentShapes () .begin (), layer -> getNumTransparentShapes ()))
				{
					if (container -> getShape () -> isTransparent ())
						++ transparentShapes;
					else
						++ opaqueShapes;

					const auto & geometry = container -> getShape () -> getGeometry ();

					if (geometry)
					{
						for (const auto & element : geometry -> getElements ())
							count (element);
					}
				}
			}

			break;
		}
		case 2:
		{
			// Selection

			auto selection = getBrowserWindow () -> getSelection () -> getChildren ();

			X3D::traverse (selection,
		                  std::bind (&X3DPrimitiveCountEditor::traverse, this, _1),
		                  true,
		                  X3D::TRAVERSE_ROOT_NODES |
		                  X3D::TRAVERSE_PROTOTYPE_INSTANCES |
		                  X3D::TRAVERSE_INLINE_NODES |
		                  X3D::TRAVERSE_VISIBLE_NODES |
		                  X3D::TRAVERSE_CLONED_NODES);

			break;
		}
	}

	getPrimitiveCountNodesLabel ()             .set_text (basic::to_string (nodes));
	getPrimitiveCountOpaqueShapesLabel ()      .set_text (basic::to_string (opaqueShapes));
	getPrimitiveCountTransparentShapesLabel () .set_text (basic::to_string (transparentShapes));

	getPrimitiveCountPolygonsLabel ()  .set_text (basic::to_string (polygons));
	getPrimitiveCountQuadsLabel ()     .set_text (basic::to_string (quads));
	getPrimitiveCountTrianglesLabel () .set_text (basic::to_string (triangles));
	getPrimitiveCountLinesLabel ()     .set_text (basic::to_string (lines));
	getPrimitiveCountPointsLabel ()    .set_text (basic::to_string (points));
}

bool
X3DPrimitiveCountEditor::traverse (X3D::SFNode & node)
{
	++ nodes;

	for (const auto & type : basic::make_reverse_range (node -> getType ()))
	{
		switch (type)
		{
			case X3D::X3DConstants::X3DShapeNode:
			{
				const auto shapeNode = dynamic_cast <X3D::X3DShapeNode*> (node .getValue ());
			
			   if (shapeNode -> isTransparent ())
			      ++ transparentShapes;
			   else
					++ opaqueShapes;

			   continue;
			}
			case X3D::X3DConstants::X3DGeometryNode:
			{
				const auto geometry = dynamic_cast <X3D::X3DGeometryNode*> (node .getValue ());
			
				for (const auto & element : geometry -> getElements ())
					count (element);

				continue;
			}
			default:
			   continue;
		}
	}

	return true;
}

void
X3DPrimitiveCountEditor::count (const X3D::X3DGeometryNode::Element & element)
{
	switch (element .vertexMode)
	{
		case GL_POINTS:
			points += element .count;
			break;
		case GL_LINES:
			lines += element .count / 2;
			break;
		case GL_LINE_LOOP:
			lines += element .count;
			break;
		case GL_LINE_STRIP:
			lines += element .count - 1;
			break;
		case GL_TRIANGLES:
			triangles += element .count / 3;
			break;
		case GL_QUADS:
			quads += element .count / 4;
			//triangles += 2 * element .count / 4;
			break;
		case GL_QUAD_STRIP:
			quads += element .count / 2 - 1;
			//triangles += 2 * (element .count / 2 - 1);
			break;
		case GL_TRIANGLE_FAN:
		case GL_TRIANGLE_STRIP:
			triangles += element .count - 2;
			break;
		case GL_POLYGON:
			++ polygons;
			//triangles += element .count - 2;
			break;
	}
}

void
X3DPrimitiveCountEditor::set_browser ()
{
	if (browser)
		browser -> displayed () .removeInterest (this, &X3DPrimitiveCountEditor::update);

	browser = getCurrentBrowser ();

	browser -> displayed () .addInterest (this, &X3DPrimitiveCountEditor::update);
}

void
X3DPrimitiveCountEditor::set_executionContext ()
{
	if (executionContext)
		executionContext -> sceneGraph_changed () .removeInterest (this, &X3DPrimitiveCountEditor::update);

	executionContext = getCurrentContext ();
	executionContext -> sceneGraph_changed () .addInterest (this, &X3DPrimitiveCountEditor::update);
}

void
X3DPrimitiveCountEditor::store ()
{
	getConfig () -> setItem ("primitiveCount", getPrimitiveCountCountButton () .get_active_row_number ());
}

X3DPrimitiveCountEditor::~X3DPrimitiveCountEditor ()
{ }

} // puck
} // titania
