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

#include "Inline.h"

#include "../../Browser/X3DBrowser.h"
#include "../../InputOutput/Loader.h"
#include "../../Thread/SceneLoader.h"
#include "../../Tools/Networking/InlineTool.h"

namespace titania {
namespace X3D {

static constexpr bool X3D_PARALLEL = true;

const std::string Inline::componentName  = "Networking";
const std::string Inline::typeName       = "Inline";
const std::string Inline::containerField = "children";

Inline::Fields::Fields () :
	load (new SFBool (true))
{ }

Inline::Inline (X3DExecutionContext* const executionContext) :
	     X3DBaseNode (executionContext -> getBrowser (), executionContext),
	    X3DChildNode (),
	X3DBoundedObject (),
	    X3DUrlObject (),
	          fields (),
	           scene (),
	           group (new Group (executionContext)),
	          future (),
	       wasLoaded (false)
{
	addField (inputOutput,    "metadata",   metadata ());
	addField (inputOutput,    "load",       load ());
	addField (inputOutput,    "url",        url ());
	addField (initializeOnly, "bboxSize",   bboxSize ());
	addField (initializeOnly, "bboxCenter", bboxCenter ());

	addChildren (scene, group);
}

X3DBaseNode*
Inline::create (X3DExecutionContext* const executionContext) const
{
	return new Inline (executionContext);
}

void
Inline::initialize ()
{
	X3DChildNode::initialize ();
	X3DBoundedObject::initialize ();
	X3DUrlObject::initialize ();

	load () .addInterest (this, &Inline::set_load);
	url ()  .addInterest (this, &Inline::set_url);

	if (scene)
	{
		setScene (std::move (scene));
		setLoadState (COMPLETE_STATE);
	}
	else
	{
		if (X3D_PARALLEL)
		{
			setScene (ScenePtr (getBrowser () -> getScene ()));

			if (load ())
				requestAsyncLoad ();
		}
		else
		{
			if (load ())
				requestImmediateLoad ();

			else
				setScene (ScenePtr (getBrowser () -> getScene ()));
		}
	}

	group -> isInternal (true);
	group -> setup ();
}

void
Inline::setSceneAsync (ScenePtr && value)
{
	if (value)
	{
		setScene (std::move (value));
		setLoadState (COMPLETE_STATE);
	}
	else
	{
		setScene (ScenePtr (getBrowser () -> getScene ()));
		setLoadState (FAILED_STATE);
	}
}

void
Inline::setScene (ScenePtr && value)
{
	if (scene)
		scene -> getRootNodes () .removeInterest (group -> children ());

	// First initialize,

	if (isInitialized ())
		value -> setup ();

	else
		getExecutionContext () -> addUninitializedNode (value);

	// then assign.

	scene = std::move (value);

	scene -> getRootNodes () .addInterest (group -> children ());
	group -> children () = scene -> getRootNodes ();
}

const ScenePtr &
Inline::getScene () const
throw (Error <NODE_NOT_AVAILABLE>,
	    Error <INVALID_OPERATION_TIMING>,
       Error <DISPOSED>)
{
	if (load ())
	{
		if (isInitialized ())
		{
			if (X3D_PARALLEL and checkLoadState () == IN_PROGRESS_STATE)
				future -> wait ();

			if (checkLoadState () == COMPLETE_STATE)
				return scene;
		}
		else
		{
			// Thread save part

			try
			{
				if (not scene)
				{
					ScenePtr scene = getBrowser () -> createScene ();

					Loader (getExecutionContext ()) .parseIntoScene (scene, url ());

					const_cast <Inline*> (this) -> scene .set (scene);
				}
			}
			catch (const X3DError & error)
			{ }

			if (scene)
				return scene;

			// End thread save part
		}
	}

	throw Error <NODE_NOT_AVAILABLE> ("Inline node '" + getName () + "' has not yet been loaded.");
}

Box3f
Inline::getBBox () const
{
	if (bboxSize () == Vector3f (-1, -1, -1))
		return group -> getBBox ();

	return Box3f (bboxSize (), bboxCenter ());
}

SFNode
Inline::getExportedNode (const std::string & exportedName) const
throw (Error <INVALID_NAME>,
       Error <NODE_NOT_AVAILABLE>,
	    Error <INVALID_OPERATION_TIMING>,
       Error <DISPOSED>)
{
	return getScene () -> getExportedNode (exportedName);
}

const ExportedNodeIndex &
Inline::getExportedNodes () const
throw (Error <NODE_NOT_AVAILABLE>,
	    Error <INVALID_OPERATION_TIMING>,
       Error <DISPOSED>)
{
	return getScene () -> getExportedNodes ();
}

void
Inline::requestAsyncLoad ()
{
	using namespace std::placeholders;

	if (checkLoadState () == COMPLETE_STATE or checkLoadState () == IN_PROGRESS_STATE)
		return;

	setLoadState (IN_PROGRESS_STATE);

	if (future)
		future -> dispose ();

	future .reset (new SceneLoader (getExecutionContext (),
	                                url (),
	                                std::bind (std::mem_fn (&Inline::setSceneAsync), this, _1)));
}

void
Inline::requestImmediateLoad ()
{
	if (X3D_PARALLEL and checkLoadState () == IN_PROGRESS_STATE)
	{
		future -> wait ();
		return;
	}

	if (checkLoadState () == COMPLETE_STATE or checkLoadState () == IN_PROGRESS_STATE)
		return;

	setLoadState (IN_PROGRESS_STATE);

	Loader loader (getExecutionContext ());

	try
	{
		setScene (loader .createX3DFromURL (url ()));

		setLoadState (COMPLETE_STATE);
	}
	catch (const X3DError & error)
	{
		setLoadState (FAILED_STATE);

		for (const auto & string : loader .getUrlError ())
			getBrowser () -> println (string .str ());

		getBrowser () -> println (error .what ());
	}
}

void
Inline::requestUnload ()
{
	if (checkLoadState () == NOT_STARTED_STATE or checkLoadState () == FAILED_STATE)
		return;

	if (future)
		future -> dispose ();

	setScene (ScenePtr (getBrowser () -> getScene ()));

	setLoadState (NOT_STARTED_STATE);
}

void
Inline::set_load ()
{
	if (load ())
	{
		setLoadState (NOT_STARTED_STATE);

		if (X3D_PARALLEL)
			requestAsyncLoad ();

		else
			requestImmediateLoad ();
	}
	else
		requestUnload ();
}

void
Inline::set_url ()
{
	if (load ())
	{
		setLoadState (NOT_STARTED_STATE);

		if (X3D_PARALLEL)
			requestAsyncLoad ();

		else
			requestImmediateLoad ();
	}
}

void
Inline::saveState ()
{
	if (isSaved ())
		return;

	X3DChildNode::saveState ();

	wasLoaded = load ();

	if (load ())
		load () = false;
}

void
Inline::restoreState ()
{
	if (not isSaved ())
		return;

	X3DChildNode::restoreState ();

	if (wasLoaded)
		load () = true;
}

void
Inline::traverse (const TraverseType type)
{
	group -> traverse (type);
}

void
Inline::addTool ()
{
	X3DChildNode::addTool (new InlineTool (this));
}

void
Inline::dispose ()
{
	if (future)
		future -> dispose ();

	X3DUrlObject::dispose ();
	X3DBoundedObject::dispose ();
	X3DChildNode::dispose ();
}

} // X3D
} // titania
