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

#include "Proto.h"

#include "../Components/Core/X3DPrototypeInstance.h"
#include "ExternProto.h"
#include <iomanip>

namespace titania {
namespace X3D {

const std::string Proto::componentName  = "Browser";
const std::string Proto::typeName       = "Proto";
const std::string Proto::containerField = "proto";

Proto::Proto (X3DExecutionContext* const executionContext) :
	        X3DBaseNode (executionContext -> getBrowser (), executionContext),
	           X3DProto (),
	X3DExecutionContext ()
{
	addField (inputOutput, "metadata", metadata ());

	setEncoding             (executionContext -> getEncoding ());
	setSpecificationVersion (executionContext -> getSpecificationVersion ());
	setCharacterEncoding    (executionContext -> getCharacterEncoding ());
	setComment              (executionContext -> getComment ());

	setWorldURL (executionContext -> getWorldURL ());

	setProfile (executionContext -> getProfile ());

	for (const auto & component : executionContext -> getComponents ())
		addComponent (component);

	setUnits (executionContext -> getUnits ());
}

Proto*
Proto::create (X3DExecutionContext* const executionContext) const
{
	return new Proto (executionContext);
}

Proto*
Proto::clone (X3DExecutionContext* const executionContext) const
throw (Error <INVALID_NAME>,
       Error <NOT_SUPPORTED>)
{
	executionContext -> updateProtoDeclaration (this -> getName (), const_cast <Proto*> (this));

	return const_cast <Proto*> (this);
}

Proto*
Proto::copy (X3DExecutionContext* const executionContext) const
throw (Error <INVALID_NAME>,
       Error <NOT_SUPPORTED>)
{
	Proto* const copy = create (executionContext);

	copy -> setName (getName ());

	for (const auto & field : getUserDefinedFields ())
	{
		copy -> addUserDefinedField (field -> getAccessType (),
		                             field -> getName (),
		                             field -> clone (executionContext));
	}
	
	copy -> setup ();

	executionContext -> updateProtoDeclaration (copy -> getName (), copy);

	copy -> copyExternProtos (this);
	copy -> copyProtos (this);
	copy -> copyRootNodes (this);
	copy -> copyImportedNodes (this);
	copy -> copyRoutes (this);

	return copy;
}

X3DPrototypeInstance*
Proto::createInstance (X3DExecutionContext* const executionContext)
{
	return new X3DPrototypeInstance (executionContext, this);
}

void
Proto::setup ()
{
	getExecutionContext () -> addParent (this);
}

X3DBaseNode*
Proto::getRootNode () const
{
	return getRootNodes () .front ();
}

void
Proto::toStream (std::ostream & ostream) const
{
	Generator::PushContext ();

	if (not getComments () .empty ())
	{
		for (const auto & comment : getComments ())
		{
			ostream
				<< Generator::Indent
				<< Generator::Comment
				<< comment
				<< Generator::ForceBreak;
		}

		ostream << Generator::TidyBreak;
	}

	ostream
		<< Generator::Indent
		<< "PROTO"
		<< Generator::Space
		<< getName ()
		<< Generator::TidySpace
		<< '[';

	const FieldDefinitionArray fields = getUserDefinedFields ();

	if (fields .empty ())
	{
		if (not getInterfaceComments () .empty ())
		{
			ostream
				<< Generator::TidyBreak
				<< Generator::IncIndent;

			for (const auto & comment : getInterfaceComments ())
			{
				ostream
					<< Generator::Indent
					<< Generator::Comment
					<< comment
					<< Generator::Break;
			}

			ostream
				<< Generator::DecIndent
				<< Generator::Indent;
		}

		else
			ostream << Generator::TidySpace;
	}
	else
	{
		ostream
			<< Generator::TidyBreak
			<< Generator::IncIndent;

		size_t typeLength       = 0;
		size_t accessTypeLength = 0;

		switch (Generator::Style ())
		{
			case Generator::SMALLEST:
			case Generator::SMALL:
			{
				break;
			}
			default:
			{
				for (const auto & field : fields)
				{
					typeLength = std::max (typeLength, field -> getTypeName () .length ());

					accessTypeLength = std::max (accessTypeLength, Generator::AccessTypes [field] .length ());
				}

				break;
			}
		}

		for (const auto & field : basic::adapter (fields .begin (), fields .end () - 1))
		{
			toStreamField (ostream, field, accessTypeLength, typeLength);
			ostream << Generator::Break;			
		}

		toStreamField (ostream, fields .back (), accessTypeLength, typeLength);
		ostream << Generator::TidyBreak;			

		for (const auto & comment : getInterfaceComments ())
		{
			ostream
				<< Generator::Indent
				<< Generator::Comment
				<< comment
				<< Generator::Break;
		}

		ostream
			<< Generator::DecIndent
			<< Generator::Indent;
	}

	ostream << ']';

	Generator::PopContext ();

	ostream << Generator::TidyBreak;

	ostream
		<< Generator::Indent
		<< '{'
		<< Generator::TidyBreak
		<< Generator::IncIndent;

	X3DExecutionContext::toStream (ostream);

	ostream
		<< Generator::DecIndent
		<< Generator::Indent
		<< '}';
}
void
Proto::toStreamField (std::ostream & ostream, X3DFieldDefinition* const field, const size_t accessTypeLength, const size_t typeLength) const
{
	for (const auto & comment : field -> getComments ())
	{
		ostream
			<< Generator::Indent
			<< Generator::Comment
			<< comment
			<< Generator::Break;
	}

	ostream
		<< Generator::Indent
		<< std::setiosflags (std::ios::left)
		<< std::setw (accessTypeLength);

	ostream << Generator::AccessTypes [field];

	ostream
		<< Generator::Space
		<< std::setiosflags (std::ios::left) << std::setw (typeLength) << field -> getTypeName ()
		<< Generator::Space
		<< field -> getName ();

	if (field -> isInitializeable ())
	{
		ostream
			<< Generator::Space
			<< *field;
	}
}


} // X3D
} // titania
