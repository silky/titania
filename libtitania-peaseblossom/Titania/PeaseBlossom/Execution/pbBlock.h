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

#ifndef __TITANIA_X3D_PEASE_BLOSSOM_EXECUTION_VS_BLOCK_H__
#define __TITANIA_X3D_PEASE_BLOSSOM_EXECUTION_VS_BLOCK_H__

#include "../Base/pbChildObject.h"
#include "../Base/pbOutputStreamObject.h"
#include "../Expressions/pbExpression.h"
#include "../Primitives/array.h"
#include "../Primitives/ptr.h"

namespace titania {
namespace pb {

class pbExecutionContext;

class pbBlock :
	virtual public pbChildObject,
	virtual public pbOutputStreamObject
{
public:

	/// @name Member access

	///  Add an expression to the list of expressions.
	void
	addExpression (ptr <pbExpression> && value)
	{ expressions .emplace_back (std::move (value)); }

	///  Returns an array with all local root expressions.
	const array <ptr <pbExpression>> &
	getExpressions () const
	{ return expressions; }

	///  @name Input/Output

	///  Inserts this object into the output stream @a ostream.
	virtual
	void
	toStream (std::ostream & ostream) const override;


protected:

	///  @name Construction

	///  Constructs new pbBlock.
	pbBlock () :
		       pbChildObject (),
		pbOutputStreamObject (),
		         expressions ()
	{ addChildren (expressions); }

	/// @name Operations

	///  Imports all expressions from @a block into @a executionContext.
	void
	import (pbExecutionContext* const executionContext, const pbBlock* const block)
	throw (pbError)
	{
		for (const auto & expression : block -> getExpressions ())
			addExpression (expression -> copy (executionContext));
	}

	///  Executes the associated expessions of this context.
	CompletionType
	getValue ()
	throw (pbError)
	{
		CompletionType result;

		for (const auto & expression : expressions)
		{
			auto value = expression -> getValue ();

			if (value .getStatement ())
				return value;

			result = std::move (value);
		}

		return result;
	}


private:

	/// @name Members

	array <ptr <pbExpression>> expressions;

};

} // pb
} // titania

#endif