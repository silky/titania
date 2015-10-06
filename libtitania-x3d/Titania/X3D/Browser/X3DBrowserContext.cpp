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

#include "X3DBrowserContext.h"

#include "../Browser/Console.h"
#include "../Browser/ContextLock.h"
#include "../Browser/Notification.h"
#include "../Browser/Selection.h"
#include "../Rendering/FrameBuffer.h"

#include "../Debug.h"

namespace titania {
namespace X3D {

X3DBrowserContext::X3DBrowserContext () :
	                      X3DScene (),
	                X3DCoreContext (),
	             X3DShadingContext (),
	          X3DGeometry2DContext (),
	          X3DGeometry3DContext (),
	     X3DKeyDeviceSensorContext (),
	            X3DLayeringContext (),
	              X3DLayoutContext (),
	            X3DLightingContext (),
	          X3DNavigationContext (),
	          X3DNetworkingContext (),
	     X3DParticleSystemsContext (),
	X3DPointingDeviceSensorContext (),
	           X3DRenderingContext (),
	           X3DScriptingContext (),
	               X3DShapeContext (),
	                X3DTextContext (),
	           X3DTexturingContext (),
	                X3DTimeContext (),
	               X3DRouterObject (),
	             initializedOutput (),
	                reshapedOutput (),
	           prepareEventsOutput (),
	                 sensorsOutput (),
	               displayedOutput (),
	                finishedOutput (),
	                 changedOutput (),
	                   changedTime (0),
	                   freezedTime (0),
	                         world (),
	                     selection (new Selection (this)),
	                  notification (new Notification (this)),
	                       console (new Console (this))
{
	addType (X3DConstants::X3DBrowserContext);

	addChildren (initialized (),
	             world,
	             selection,
	             notification,
	             console);

	isPrivate (true);
}

void
X3DBrowserContext::initialize ()
{
	X3DScene::initialize ();
	X3DCoreContext::initialize ();
	X3DShadingContext::initialize ();
	X3DGeometry2DContext::initialize ();
	X3DGeometry3DContext::initialize ();
	X3DKeyDeviceSensorContext::initialize ();
	X3DLayeringContext::initialize ();
	X3DLayoutContext::initialize ();
	X3DLightingContext::initialize ();
	X3DNavigationContext::initialize ();
	X3DNetworkingContext::initialize ();
	X3DParticleSystemsContext::initialize ();
	X3DPointingDeviceSensorContext::initialize ();
	X3DRenderingContext::initialize ();
	X3DScriptingContext::initialize ();
	X3DShapeContext::initialize ();
	X3DTextContext::initialize ();
	X3DTexturingContext::initialize ();
	X3DTimeContext::initialize ();
	X3DRouterObject::initialize ();

	selection    -> setup ();
	notification -> setup ();
	console      -> setup ();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***
 *  Returns a snapshot of the current browser surface in the form of a Magick::Image.  Query for getMaxRenderBufferSize
 *  to get the maximum width and height of the surface.  Query for getMaxSamples to get the maximum number of samples to
 *  use for antialising.  Antialiasing is only available if the browser supports GL_EXT_framebuffer_multisample.
 *
 *  @param  width         Width of the image.
 *  @param  height        Height of the image.
 *  @param  alphaChannel  Whether or not the image should have a alpha channel.
 *  @param  antialiasing  Number of samples used for antialising.
 */
std::shared_ptr <Magick::Image>
X3DBrowserContext::getSnapshot (const size_t width, const size_t height, const bool alphaChannel, const size_t antialiasing) const
throw (Error <INVALID_OPERATION_TIMING>,
       std::runtime_error)
{
	ContextLock lock (this);

	if (getWorld () and lock)
	{
		const auto backgroundColor  = alphaChannel ? X3D::Color4f () : getBrowser () -> getBackgroundColor ();
		const bool backgroundHidden = getWorld () -> getLayerSet () -> getLayer0 () -> getBackground () -> isHidden ();
	
		getWorld () -> getLayerSet () -> getLayer0 () -> getBackground () -> isHidden (alphaChannel);

		// Render to frame buffer.

		std::vector <uint8_t> pixels;
		FrameBuffer           frameBuffer (this, width, height, antialiasing);

		frameBuffer .bind ();
		const_cast <X3DBrowserContext*> (this) -> reshape ();

		glClearColor (backgroundColor .r (), backgroundColor .g (), backgroundColor .b (), backgroundColor .a ());
		glClear (GL_COLOR_BUFFER_BIT);
		getWorld () -> traverse (TraverseType::DISPLAY);

		frameBuffer .get (pixels);
		frameBuffer .unbind ();
		const_cast <X3DBrowserContext*> (this) -> reshape ();

		getWorld () -> getLayerSet () -> getLayer0 () -> getBackground () -> isHidden (backgroundHidden);

		// Process image.

		const auto image = std::make_shared <Magick::Image> (width, height, "RGBA", Magick::CharPixel, pixels .data ());

		pixels .resize (0);
		pixels .shrink_to_fit ();

		if (alphaChannel)
			image -> type (Magick::TrueColorMatteType);
		else
		{
			image -> matte (false);
			image -> type (Magick::TrueColorType);
		}

		image -> flip ();
		image -> resolutionUnits (Magick::PixelsPerInchResolution);
		image -> density (Magick::Geometry (72, 72));

		return image;
	}

	throw Error <INVALID_OPERATION_TIMING> ("Invalid operation timing.");
}

void
X3DBrowserContext::addEvent ()
{
	if (changedTime == getCurrentTime ())
		return;

	changedTime = getCurrentTime ();
	changed () .processInterests ();
}

void
X3DBrowserContext::beginUpdateForFrame ()
{
	changedTime = freezedTime;
}

void
X3DBrowserContext::endUpdateForFrame ()
{
	freezedTime = changedTime;
	changedTime = getCurrentTime ();
}

void
X3DBrowserContext::reshape ()
{
	ContextLock lock (this);

	if (lock)
		reshaped () .processInterests ();
}

/*
 * a) Update camera based on currently bound Viewpoint's position and orientation.
 * b) Evaluate input from sensors.
 * c) Evalute routes.
 * d) If any events were generated from steps b and c, go to step b and continue.
 * e) If particle system evaluation is to take place, evaluate the particle systems here.
 * f) If physics model evaluation is to take place, evaluate the physics model.
 */

void
X3DBrowserContext::update ()
{
	try
	{
		ContextLock lock (this);

		if (lock)
		{
			// Prepare

			getClock () -> advance ();

			prepareEvents () .processInterests ();
			processEvents ();

			getWorld () -> traverse (TraverseType::CAMERA);
			getWorld () -> traverse (TraverseType::COLLISION);

			sensors () .processInterests ();
			processEvents ();

			deleteObjectsAsync ();

			// Debug
			debugRouter ();

			// Display

			const auto color = getBackgroundColor ();

			glClearColor (color .r (), color .g (), color .b (), color .a ());
			glClear (GL_COLOR_BUFFER_BIT);

			getWorld () -> traverse (TraverseType::DISPLAY);

			displayed () .processInterests ();
			swapBuffers ();

			// Finish

			finished () .processInterests ();

			const GLenum errorNum = glGetError ();

			if (errorNum not_eq GL_NO_ERROR)
				std::clog << "OpenGL Error at " << SFTime (getCurrentTime ()) .toUTCString () << ": " << gluErrorString (errorNum) << std::endl;
		}
	}
	catch (const std::exception & exception)
	{
		std::clog
			<< SFTime (getCurrentTime ()) .toUTCString () << " Unhandled exception:" << std::endl
			<< "  " << exception .what () << std::endl;
	}
}

void
X3DBrowserContext::dispose ()
{
	initializedOutput   .dispose ();
	reshapedOutput      .dispose ();
	prepareEventsOutput .dispose ();
	sensorsOutput       .dispose ();
	displayedOutput     .dispose ();
	finishedOutput      .dispose ();
	changedOutput       .dispose ();

	X3DRouterObject::dispose ();
	X3DTimeContext::dispose ();
	X3DTexturingContext::dispose ();
	X3DTextContext::dispose ();
	X3DShapeContext::dispose ();
	X3DScriptingContext::dispose ();
	X3DRenderingContext::dispose ();
	X3DPointingDeviceSensorContext::dispose ();
	X3DParticleSystemsContext::dispose ();
	X3DNetworkingContext::dispose ();
	X3DNavigationContext::dispose ();
	X3DLightingContext::dispose ();
	X3DLayoutContext::dispose ();
	X3DLayeringContext::dispose ();
	X3DKeyDeviceSensorContext::dispose ();
	X3DGeometry3DContext::dispose ();
	X3DGeometry2DContext::dispose ();
	X3DShadingContext::dispose ();
	X3DCoreContext::dispose ();
	X3DScene::dispose ();
}

X3DBrowserContext::~X3DBrowserContext ()
{
	__LOG__ << std::endl;
}

} // X3D
} // titania
