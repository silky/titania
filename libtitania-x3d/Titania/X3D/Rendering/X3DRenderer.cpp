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
 * Copyright 1999, 2016 Holger Seelig <holger.seelig@yahoo.de>.
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

#include "X3DRenderer.h"

#include "../Browser/BrowserOptions.h"
#include "../Browser/ContextLock.h"
#include "../Browser/X3DBrowser.h"
#include "../Components/Navigation/NavigationInfo.h"
#include "../Components/Navigation/X3DViewpointNode.h"
#include "../Components/Layering/X3DLayerNode.h"
#include "../Components/Shape/Appearance.h"
#include "../Components/Shape/X3DShapeNode.h"
#include "../Rendering/FrameBuffer.h"
#include "../Rendering/CollisionContainer.h"
#include "../Rendering/ShapeContainer.h"

#include <Titania/Math/Geometry/Camera.h>
#include <Titania/Utility/Range.h>

#include <algorithm>

namespace titania {
namespace X3D {

static constexpr size_t DEPTH_BUFFER_WIDTH    = 16;
static constexpr size_t DEPTH_BUFFER_HEIGHT   = 16;
static constexpr auto   DEPTH_BUFFER_VIEWPORT = Vector4i (0, 0, DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT);

static constexpr auto zAxis = Vector3d (0, 0, 1);

X3DRenderer::X3DRenderer () :
	               X3DNode (),
	       viewVolumeStack (),
	         globalObjects (),
	          globalLights (),
	          localObjects (),
	            clipPlanes (),
	           localLights (),
	                lights (),
	            collisions (),
	          opaqueShapes (),
	     transparentShapes (),
	       collisionShapes (),
	      activeCollisions (),
	           depthShapes (),
	           depthBuffer (new FrameBuffer (getBrowser (), DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT, 0, true)),
	                 speed (),
	       numOpaqueShapes (0),
	  numTransparentShapes (0),
	    numCollisionShapes (0),
	        numDepthShapes (0)
{
	addType (X3DConstants::X3DRenderObject);
}

void
X3DRenderer::initialize ()
{
	depthBuffer -> setup ();
}

void
X3DRenderer::setExecutionContext (X3DExecutionContext* const executionContext)
throw (Error <INVALID_OPERATION_TIMING>,
       Error <DISPOSED>)
{
	if (depthBuffer)
		depthBuffer -> setBrowser (executionContext -> getBrowser ());
}

void
X3DRenderer::addShape (X3DShapeNode* const shape)
{
	const auto bbox   = shape -> getBBox () * getModelViewMatrix () .get ();
	const auto depth  = bbox .size () .z () / 2;
	const auto min    = bbox .center () .z () - depth;
	const auto center = bbox .center () .z () + getBrowser () -> getDepthOffset () .top ();

	if (min > 0)
	   return;

	const auto & viewVolume = getViewVolumes () .back ();

	if (viewVolume .intersects (bbox))
	{
	   ShapeContainer* context = nullptr;

		if (shape -> isTransparent () or not getBrowser () -> getDepthTest () .top ())
		{
		   if (numTransparentShapes == transparentShapes .size ())
		      transparentShapes .emplace_back (new ShapeContainer (this, true));

			context = transparentShapes [numTransparentShapes] .get ();

			++ numTransparentShapes;
		}
		else
		{
		   if (numOpaqueShapes == opaqueShapes .size ())
		      opaqueShapes .emplace_back (new ShapeContainer (this, false));

			context = opaqueShapes [numOpaqueShapes] .get ();

			++ numOpaqueShapes;
		}

		context -> setScissor (viewVolume .getScissor ());
		context -> setModelViewMatrix (getModelViewMatrix () .get ());
		context -> setShape (shape);
		context -> setFog (getFog ());
		context -> setLocalObjects (getLocalObjects ());
		context -> setClipPlanes (getClipPlanes ());
		context -> setLocalLights (getLocalLights ());
		context -> setDistance (center);
	}
}

void
X3DRenderer::addCollisionShape (X3DShapeNode* const shape)
{
	// It should be possible to sort out shapes that are far away.

	if (numCollisionShapes == collisionShapes .size ())
		collisionShapes .emplace_back (new CollisionContainer ());

	const auto & context = collisionShapes [numCollisionShapes];

	++ numCollisionShapes;

	context -> setScissor (getViewVolumes () .back () .getScissor ());
	context -> setModelViewMatrix (getModelViewMatrix () .get ());
	context -> setShape (shape);
	context -> setCollisions (getCollisions ());
	context -> setLocalObjects (getLocalObjects ());
	context -> setClipPlanes (getClipPlanes ());
}

void
X3DRenderer::addDepthShape (X3DShapeNode* const shape)
{
	// It should be possible to sort out shapes that are far away.

	if (numDepthShapes == depthShapes .size ())
		depthShapes .emplace_back (new CollisionContainer ());

	const auto & context = depthShapes [numDepthShapes];

	++ numDepthShapes;

	context -> setScissor (getViewVolumes () .back () .getScissor ());
	context -> setModelViewMatrix (getModelViewMatrix () .get ());
	context -> setShape (shape);
	context -> setLocalObjects (getLocalObjects ());
	context -> setClipPlanes (getClipPlanes ());
}

///  Constrains @a translation when the viewer collides with a wall.
Vector3d
X3DRenderer::constrainTranslation (const Vector3d & translation) const
{
	const auto navigationInfo  = getNavigationInfo ();
	auto       distance        = getDistance (translation);
	const auto zFar            = navigationInfo -> getFarPlane (getViewpoint ());

	// Constrain translation when the viewer collides with a wall.

	if (zFar - distance > 0) // Are there polygons before the viewer
	{
		const auto collisionRadius = navigationInfo -> getCollisionRadius ();

		distance -= collisionRadius;

		if (distance > 0)
		{
			// Move

			const auto length = abs (translation);
			
			if (length > distance)
			{
				// Collision: The wall is reached.
				return normalize (translation) * distance;
			}

			return translation;
		}

		// Collision
		return normalize (translation) * distance;
	}

	return translation;
}

///  Returns the distance to the nearest object in @a direction.
double
X3DRenderer::getDistance (const Vector3d & direction) const
{
	try
	{
		ContextLock lock (getBrowser ());

		// Determine width and height of camera

		const auto viewpoint       = getViewpoint ();
		const auto navigationInfo  = getNavigationInfo ();
		const auto collisionRadius = navigationInfo -> getCollisionRadius ();
		const auto bottom          = navigationInfo -> getStepHeight () - navigationInfo -> getAvatarHeight ();
		const auto zNear           = navigationInfo -> getNearPlane ();
		const auto zFar            = navigationInfo -> getFarPlane (viewpoint);

		// Reshape camera

		const auto projectionMatrix = ortho (-collisionRadius, collisionRadius, std::min (bottom, -collisionRadius), collisionRadius, zNear, zFar);

		// Translate camera to user position and to look in the direction of the @a direction.

		const auto localOrientation = ~Rotation4d (viewpoint -> orientation () .getValue ()) * viewpoint -> getOrientation ();
		auto       rotation         = Rotation4d (zAxis, -direction) * localOrientation;
	
		// The viewer is alway a straight box depending on the upVector.
		// rotation *= viewpoint -> straightenHorizon (rotation);

		auto modelViewMatrix = viewpoint -> getTransformationMatrix ();
		modelViewMatrix .translate (viewpoint -> getUserPosition ());
		modelViewMatrix .rotate (rotation);
		modelViewMatrix .inverse ();

		modelViewMatrix .mult_right (projectionMatrix);
	
		// Render depth.

		const_cast <X3DRenderer*> (this) -> getProjectionMatrix () .push (modelViewMatrix);

		const auto depth = getDepth ();

		const_cast <X3DRenderer*> (this) -> getProjectionMatrix () .pop ();

		return depth;
	}
	catch (const Error <INVALID_OPERATION_TIMING> &)
	{ }
	catch (const std::domain_error &)
	{ }

	return 0;
}

double
X3DRenderer::getDepth () const
{
	static const ViewVolume viewVolume (Matrix4d (), DEPTH_BUFFER_VIEWPORT, DEPTH_BUFFER_VIEWPORT);

	// Render all objects

	depthBuffer -> bind ();

	const_cast <X3DRenderer*> (this) -> getViewVolumes () .emplace_back (viewVolume);
	const_cast <X3DRenderer*> (this) -> depth (collisionShapes, numCollisionShapes);
	const_cast <X3DRenderer*> (this) -> getViewVolumes () .pop_back ();

	// Get distance from depth buffer
		
	const auto navigationInfo  = getNavigationInfo ();
	const auto viewpoint       = getViewpoint ();
	const auto zNear           = navigationInfo -> getNearPlane ();
	const auto zFar            = navigationInfo -> getFarPlane (viewpoint);
	const auto collisionRadius = navigationInfo -> getCollisionRadius ();

	const auto distance = depthBuffer -> getDistance (collisionRadius, zNear, zFar);

	depthBuffer -> unbind ();

	return distance;
}

void
X3DRenderer::render (const TraverseType type)
{
	switch (type)
	{
		case TraverseType::COLLISION:
		{
			// Collect for collide and gravite
			numCollisionShapes = 0;

			collect (type);
			collide ();
			gravite ();
			break;
		}
		case TraverseType::DEPTH:
		{
			numDepthShapes = 0;

			collect (type);
			depth (depthShapes, numDepthShapes);
			break;
		}
		case TraverseType::DISPLAY:
		{
			numOpaqueShapes      = 0;
			numTransparentShapes = 0;

			collect (type);
			display ();
			break;
		}
		default:
			break;
	}

	getGlobalObjects () .clear ();
	getClipPlanes    () .clear ();
	getGlobalLights  () .clear ();
	getLights        () .clear ();
}

void
X3DRenderer::renderDepth (X3DGroupingNode* const group)
{
	numDepthShapes = 0;

	group -> traverse (TraverseType::DEPTH);

	depth (depthShapes, numDepthShapes);

	getGlobalObjects () .clear ();
	getClipPlanes    () .clear ();
}

void
X3DRenderer::collide ()
{
	// Collision

	const auto collisionSphere = CollisionSphere3d (/* getNavigationInfo () -> getTransformationMatix () mult_right */ getViewpoint () -> getInverseCameraSpaceMatrix (),
	                                                getNavigationInfo () -> getCollisionRadius () * 1.2f,
	                                                Vector3d ());

	std::vector <Collision*> collisions;

	for (const auto & context : basic::make_range (collisionShapes .cbegin (), numCollisionShapes))
	{
	   try
	   {
			if (context -> intersects (collisionSphere))
			{
				for (auto & collision : context -> getCollisions ())
					collisions .emplace_back (collision);
			}
		}
		catch (const std::domain_error &)
		{ }
	}

	// Set isActive to FALSE for appropriate nodes

	std::vector <Collision*> difference;

	if (collisions .empty ())
		difference = std::move (activeCollisions);

	else
	{
		std::sort (collisions .begin (), collisions .end ());

		std::set_difference (activeCollisions .begin (), activeCollisions .end (),
		                     collisions .begin (), collisions .end (),
		                     std::back_inserter (difference));
	}

	for (const auto & collision : difference)
	{
		collision -> set_active (false);
		collision -> removeParent (this);
	}

	// Set isActive to TRUE for appropriate nodes

	activeCollisions = std::move (collisions);

	for (const auto & collision : activeCollisions)
	{
		collision -> addParent (this);
		collision -> set_active (true);
	}
}

void
X3DRenderer::gravite ()
{
	try
	{
		// Terrain following and gravitation

		if (getBrowser () -> getCurrentViewer () not_eq X3DConstants::WalkViewer)
			return;

		// Get NavigationInfo values

		const auto navigationInfo  = getNavigationInfo ();
		const auto viewpoint       = getViewpoint ();
		const auto collisionRadius = navigationInfo -> getCollisionRadius ();
		const auto zNear           = navigationInfo -> getNearPlane ();
		const auto zFar            = navigationInfo -> getFarPlane (viewpoint);
		const auto height          = navigationInfo -> getAvatarHeight ();
		const auto stepHeight      = navigationInfo -> getStepHeight ();

		// Reshape viewpoint for gravite.

		const auto projectionMatrix = ortho (-collisionRadius, collisionRadius, -collisionRadius, collisionRadius, zNear, zFar);
					
		// Transform viewpoint to look down the up vector

		const auto upVector = viewpoint -> getUpVector ();
		const auto down     = Rotation4d (zAxis, upVector);

		auto modelViewMatrix = viewpoint -> getTransformationMatrix ();
		modelViewMatrix .translate (viewpoint -> getUserPosition ());
		modelViewMatrix .rotate (down);
		modelViewMatrix .inverse ();

		modelViewMatrix .mult_right (projectionMatrix);

		// Render depth.

		getProjectionMatrix () .push (modelViewMatrix);

		auto distance = getDepth ();

		getProjectionMatrix () .pop ();

		// Gravite or step up

		if (zFar - distance > 0) // Are there polygons under the viewer
		{
			distance -= height;

			const Rotation4d up (Vector3d (0, 1, 0), upVector);

			if (distance > 0)
			{
				// Gravite and fall down the floor

				const auto currentFrameRate = speed ? getBrowser () -> getCurrentFrameRate () : 1000000.0;

				speed -= getBrowser () -> getBrowserOptions () -> Gravity () / currentFrameRate;

				auto translation = speed / currentFrameRate;

				if (translation < -distance)
				{
					// The ground is reached.
					translation = -distance;
					speed       = 0;
				}

				getViewpoint () -> positionOffset () += Vector3d (0, translation, 0) * up;
			}
			else
			{
				speed = 0;

				distance = -distance;

				if (distance > 0.01 and distance < stepHeight)
				{
					// Step up
					const auto translation = constrainTranslation (Vector3d (0, distance, 0) * up);

					if (getBrowser () -> getBrowserOptions () -> AnimateStairWalks ())
					{
						auto step = getBrowser () -> getCurrentSpeed () / getBrowser () -> getCurrentFrameRate ();

						step = abs (getInverseCameraSpaceMatrix () .mult_matrix_dir (Vector3d (0, step, 0) * up));

						Vector3d offset = Vector3d (0, step, 0) * up;

						if (math::abs (offset) > math::abs (translation) or getBrowser () -> getCurrentSpeed () == 0)
							offset = translation;

						getViewpoint () -> positionOffset () += offset;
					}
					else
						getViewpoint () -> positionOffset () += translation;
				}
			}
		}
		else
		{
			speed = 0;
		}
	}
	catch (const std::domain_error &)
	{ }
}

void
X3DRenderer::display ()
{
	static constexpr auto comp = ShapeContainerComp { };

	// Render shadow maps.

	for (const auto & object : getLights ())
		object -> renderShadowMap ();

	// Enable global lights.

	for (const auto & object : getGlobalLights ())
		object -> enable ();

	// Enable global objects.

	for (const auto & object : getGlobalObjects ())
		object -> enable ();

	// Set global uniforms.

	// TODO: set global uniforms.

	// Configure viewport and background

	const auto & viewport = getViewVolumes () .back () .getViewport ();

	glViewport (viewport [0], viewport [1], viewport [2], viewport [3]);
	glScissor  (viewport [0], viewport [1], viewport [2], viewport [3]);

	glClear (GL_DEPTH_BUFFER_BIT);

	getBackground () -> draw (viewport);

	// Sorted blend

	#ifdef FIXED_PIPELINE
	if (getBrowser () -> getFixedPipelineRequired ())
	{
		// Setup projection matrix

		glMatrixMode (GL_PROJECTION);
		glLoadMatrixd (getProjectionMatrix () .get () .data ());
		glMatrixMode (GL_MODELVIEW);
	}
	#endif

	// Render opaque objects first

	glDisable (GL_BLEND);
	glEnable (GL_DEPTH_TEST);
	glDepthMask (GL_TRUE);

	for (const auto & context : basic::make_range (opaqueShapes .cbegin (), numOpaqueShapes))
		context -> display ();

	// Render transparent objects

	glEnable (GL_BLEND);
	glDepthMask (GL_FALSE);

	std::sort (transparentShapes .begin (), transparentShapes .begin () + numTransparentShapes, comp);

	for (const auto & context : basic::make_range (transparentShapes .cbegin (), numTransparentShapes))
		context -> display ();

	glDepthMask (GL_TRUE);
	glDisable (GL_BLEND);

	// Disable global objects

	for (const auto & object : basic::make_reverse_range (getGlobalObjects ()))
		object -> disable ();

	// Disable global lights
	
	for (const auto & object : basic::make_reverse_range (getGlobalLights ()))
		object -> disable ();

	#ifdef FIXED_PIPELINE
	if (getBrowser () -> getFixedPipelineRequired ())
	{
		// Reset to default OpenGL appearance

		getBrowser () -> getDefaultAppearance () -> draw ();
	}
	#endif
}

void
X3DRenderer::depth (const CollisionContainerArray & shapes, const size_t numShapes)
{
	const auto & viewport = getViewVolumes () .back () .getViewport ();

	// Setup projection matrix

	glMatrixMode (GL_PROJECTION);
	glLoadMatrixd (getProjectionMatrix () .get () .data ());
	glMatrixMode (GL_MODELVIEW);

	// Render to depth buffer

	glViewport (viewport [0], viewport [1], viewport [2], viewport [3]);
	glScissor  (viewport [0], viewport [1], viewport [2], viewport [3]);

	glClear (GL_DEPTH_BUFFER_BIT);

	glEnable (GL_DEPTH_TEST);
	glDepthMask (GL_TRUE);
	glDisable (GL_BLEND);
	glDisable (GL_CULL_FACE);

	for (const auto & context : basic::make_range (shapes .cbegin (), numShapes))
		context -> draw ();
}

void
X3DRenderer::dispose ()
{
	depthBuffer .reset ();
	activeCollisions .clear ();
}

X3DRenderer::~X3DRenderer ()
{ }

} // X3D
} // titania
