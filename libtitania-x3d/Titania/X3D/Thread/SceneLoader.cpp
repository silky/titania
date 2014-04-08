/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstra�e 31a, Leipzig, Germany 2011.
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

#include "SceneLoader.h"

#include "../Browser/X3DBrowser.h"
#include "../Execution/Scene.h"
#include "../InputOutput/Loader.h"

namespace titania {
namespace X3D {

// See http://www.web3d.org/files/specifications/19775-1/V3.3/Part01/components/networking.html#X3DUrlObject for
// how to handle the profile and component arguments/statements of inline nodes.

SceneLoader::SceneLoader (X3DExecutionContext* const executionContext, const MFString & url, const Callback & callback) :
	        X3DInput (),
	         browser (executionContext -> getBrowser ()),
	executionContext (executionContext),
	        callback (callback),
	         running (true),
	          future (getFuture (url /*, executionContext -> getProfile (), executionContext -> getComponents () */))
{
	browser -> prepareEvents () .addInterest (this, &SceneLoader::prepareEvents);
	browser -> addEvent ();
}

void
SceneLoader::wait ()
{
	if (future .valid ())
	{
		future .wait ();
		prepareEvents ();
	}
}

std::future <ScenePtr> 
SceneLoader::getFuture (const MFString & url)
{
	return std::async (std::launch::async, std::mem_fn (&SceneLoader::loadAsync), this, url);
}

ScenePtr
SceneLoader::loadAsync (const MFString & url)
{
	if (running)
	{
		std::lock_guard <std::mutex> lock (browser -> getDownloadMutex ());

		ScenePtr scene;

		if (running)
			scene = browser -> createScene ();

		if (running)
			Loader (executionContext) .parseIntoScene (scene, url);

		if (running)
			return scene;
	}

	return nullptr;
}

void
SceneLoader::prepareEvents ()
{
	if (running)
	{
		browser -> addEvent ();

		if (future .valid ())
		{
			const auto status = future .wait_for (std::chrono::milliseconds (0));

			if (status == std::future_status::ready)
			{
				try
				{
					callback (future .get ());
				}
				catch (const X3DError & error)
				{
					browser -> println (error .what ());
					callback (nullptr);
				}

				dispose ();
			}
		}
	}
}

void
SceneLoader::dispose ()
{
	if (running)
	{
		running = false;

		X3DInput::dispose ();
		
		// Clear the bound callback arguments.

		callback = [ ] (ScenePtr &&) { };
	}
}

SceneLoader::~SceneLoader ()
{
	if (future .valid ())
		future .wait ();
}

} // X3D
} // titania
