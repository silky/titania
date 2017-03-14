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

#ifndef __TITANIA_X3D_BROWSER_TOOLS_X3DTOOL_CONTEXT_H__
#define __TITANIA_X3D_BROWSER_TOOLS_X3DTOOL_CONTEXT_H__

#include "../../Basic/X3DBaseNode.h"
#include "../../Fields/X3DPtr.h"
#include "../../Fields/X3DWeakPtrArray.h"
#include "../../Types/Geometry.h"
#include "SupportedTools.h"

#include <stack>

namespace titania {
namespace X3D {

class TransformToolOptions;

using RenderToolsStack = std::stack <bool, std::vector <bool>>;

class X3DToolContext :
	virtual public X3DBaseNode
{
public:

	///  @name Member access

	const SupportedTools::Function &
	getSupportedTool (const std::string & typeName) const
	throw (Error <INVALID_NAME>,
	       Error <DISPOSED>);

	const SupportedTools &
	getSupportedTools () const
	throw (Error <INVALID_NAME>,
	       Error <DISPOSED>)
	{ return supportedTools; }

	RenderToolsStack &
	getDisplayTools ()
	{ return displayTools; }

	const RenderToolsStack &
	getDisplayTools () const
	{ return displayTools; }

	const X3DPtr <TransformToolOptions> &
	getTransformToolOptions () const
	{ return transformToolOptions; }

	X3DWeakPtrArray <X3DTransformNodeTool> &
	getTransformTools ()
	{ return transformTools; }

	const X3DWeakPtrArray <X3DTransformNodeTool> &
	getTransformTools () const
	{ return transformTools; }

	X3DWeakPtrArray <X3DLightNodeTool> &
	getLightTools ()
	{ return lightTools; }

	const X3DWeakPtrArray <X3DLightNodeTool> &
	getLightTools () const
	{ return lightTools; }

	X3DWeakPtrArray <ProximitySensorTool> &
	getProximitySensorTools ()
	{ return proximitySensorTools; }

	const X3DWeakPtrArray <ProximitySensorTool> &
	getProximitySensorTools () const
	{ return proximitySensorTools; }

	X3DWeakPtrArray <SoundTool> &
	getSoundTools ()
	{ return soundTools; }

	const X3DWeakPtrArray <SoundTool> &
	getSoundTools () const
	{ return soundTools; }

	X3DWeakPtrArray <TransformSensorTool> &
	getTransformSensorTools ()
	{ return transformSensorTools; }

	const X3DWeakPtrArray <TransformSensorTool> &
	getTransformSensorTools () const
	{ return transformSensorTools; }

	X3DWeakPtrArray <VisibilitySensorTool> &
	getVisibilitySensorTools ()
	{ return visibilitySensorTools; }

	const X3DWeakPtrArray <VisibilitySensorTool> &
	getVisibilitySensorTools () const
	{ return visibilitySensorTools; }

	X3DWeakPtrArray <X3DViewpointNodeTool> &
	getViewpointTools ()
	{ return viewpointTools; }

	const X3DWeakPtrArray <X3DViewpointNodeTool> &
	getViewpointTools () const
	{ return viewpointTools; }

	void
	setCutLine (const Line2d & value)
	{ cutLine = value; }

	const Line2d &
	getCutLine () const
	{ return cutLine; }

	///  @name Destruction

	virtual
	void
	dispose () override;

	virtual
	~X3DToolContext () override;


protected:

	///  @name Construction

	X3DToolContext ();

	virtual
	void
	initialize () override;


private:

	SupportedTools  supportedTools;
	RenderToolsStack displayTools;

	X3DPtr <TransformToolOptions>          transformToolOptions;
	X3DWeakPtrArray <X3DTransformNodeTool> transformTools;

	X3DWeakPtrArray <X3DLightNodeTool>     lightTools;
	X3DWeakPtrArray <ProximitySensorTool>  proximitySensorTools;
	X3DWeakPtrArray <SoundTool>            soundTools;
	X3DWeakPtrArray <TransformSensorTool>  transformSensorTools;
	X3DWeakPtrArray <VisibilitySensorTool> visibilitySensorTools;
	X3DWeakPtrArray <X3DViewpointNodeTool> viewpointTools;

	Line2d cutLine;

};

} // X3D
} // titania

#endif
