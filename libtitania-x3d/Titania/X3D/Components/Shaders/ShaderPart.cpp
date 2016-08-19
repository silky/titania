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

#include "ShaderPart.h"

#include "../../Browser/ContextLock.h"
#include "../../Browser/X3DBrowser.h"
#include "../../Execution/X3DExecutionContext.h"
#include "../../InputOutput/Loader.h"
#include "../../Browser/Shading/Shader.h"

namespace titania {
namespace X3D {

const ComponentType ShaderPart::component      = ComponentType::SHADERS;
const std::string   ShaderPart::typeName       = "ShaderPart";
const std::string   ShaderPart::containerField = "parts";

ShaderPart::Fields::Fields () :
	type (new SFString ("VERTEX"))
{ }

ShaderPart::ShaderPart (X3DExecutionContext* const executionContext) :
	 X3DBaseNode (executionContext -> getBrowser (), executionContext),
	     X3DNode (),
	X3DUrlObject (),
	      fields (),
	    shaderId (0),
	       valid (false),
	    openGLES (false)
{
	addType (X3DConstants::ShaderPart);

	addField (inputOutput,    "metadata", metadata ());
	addField (initializeOnly, "type",     type ());
	addField (inputOutput,    "url",      url ());
}

X3DBaseNode*
ShaderPart::create (X3DExecutionContext* const executionContext) const
{
	return new ShaderPart (executionContext);
}

void
ShaderPart::initialize ()
{
	X3DNode::initialize ();
	X3DUrlObject::initialize ();

	type () .addInterest (this, &ShaderPart::set_url);
	url ()  .addInterest (this, &ShaderPart::set_url);

	if (glXGetCurrentContext ())
		requestImmediateLoad ();
}

void
ShaderPart::setExecutionContext (X3DExecutionContext* const executionContext)
throw (Error <INVALID_OPERATION_TIMING>,
       Error <DISPOSED>)
{
	X3DUrlObject::setExecutionContext (executionContext);
	X3DNode::setExecutionContext (executionContext);
}

GLenum
ShaderPart::getShaderType () const
{
	// http://www.opengl.org/wiki/Shader
	// http://www.opengl.org/wiki/Rendering_Pipeline_Overview

	static const std::map <std::string, GLenum> shaderTypes {
		std::make_pair ("VERTEX",          GL_VERTEX_SHADER),
		std::make_pair ("TESS_CONTROL",    GL_TESS_CONTROL_SHADER),
		std::make_pair ("TESS_EVALUATION", GL_TESS_EVALUATION_SHADER),
		std::make_pair ("GEOMETRY",        GL_GEOMETRY_SHADER),
		std::make_pair ("FRAGMENT",        GL_FRAGMENT_SHADER)

		#ifdef GL_COMPUTE_SHADER
		// Requires GL 4.3 or ARB_compute_shader

		, std::make_pair ("COMPUTE", GL_COMPUTE_SHADER)

		#endif
	};

	try
	{
		return shaderTypes .at (type ());
	}
	catch (const std::out_of_range &)
	{
		return GL_VERTEX_SHADER;
	}
}

void
ShaderPart::requestImmediateLoad ()
{
	if (checkLoadState () == COMPLETE_STATE or checkLoadState () == IN_PROGRESS_STATE)
		return;

	setLoadState (IN_PROGRESS_STATE);

	for (const auto & URL : url ())
	{
		try
		{
			Loader            loader (getExecutionContext ());
			const std::string document     = loader .loadDocument (URL);
			const std::string shaderSource = X3D::getShaderSource (this, document, loader .getWorldURL ());
			const char*       string       = shaderSource .c_str ();

			openGLES = X3D::isOpenGLES (shaderSource);

			if (shaderId)
				glDeleteShader (shaderId);

			shaderId = glCreateShader (getShaderType ());

			glShaderSource  (shaderId, 1, &string, nullptr);
			glCompileShader (shaderId);

			glGetShaderiv (shaderId, GL_COMPILE_STATUS, &valid);

			printShaderInfoLog ();

			if (not valid)
				continue;
		}
		catch (const X3DError & error)
		{
			std::clog << error .what () << std::endl;
		}
	}

	setLoadState (valid ? COMPLETE_STATE : FAILED_STATE);
}

void
ShaderPart::printShaderInfoLog () const
{
	GLint infoLogLength;

	glGetShaderiv (shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 1)
	{
		char infoLog [infoLogLength];

		glGetShaderInfoLog (shaderId, infoLogLength, 0, infoLog);

		getBrowser () -> print (std::string (80, '#'), '\n',
		                        "ShaderPart InfoLog (", type (), "):\n",
		                        std::string (infoLog),
		                        std::string (80, '#'), '\n');
	}
}

void
ShaderPart::set_url ()
{
	setLoadState (NOT_STARTED_STATE);

	requestImmediateLoad ();
}

void
ShaderPart::dispose ()
{
	try
	{
		ContextLock lock (getBrowser ());

		if (shaderId)
			glDeleteShader (shaderId);
	}
	catch (const Error <INVALID_OPERATION_TIMING> &)
	{ }

	X3DUrlObject::dispose ();
	X3DNode::dispose ();
}

} // X3D
} // titania
