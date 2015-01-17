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

#include "Global.h"

#include "../Bits/pbMath.h"
#include "../Objects/NativeFunction.h"
#include "../Standard/Math.h"

namespace titania {
namespace pb {
namespace Standard {

const std::string GlobalObject::className = "Global";

GlobalObject::GlobalObject (pbExecutionContext* const ec, const std::function <bool (const Identifier &)> & resolveCallback) :
	     pb::Object (ec),
	resolveCallback (resolveCallback)
{
	// Properties

	addOwnProperty ("this",      this,                 WRITABLE | CONFIGURABLE);
	addOwnProperty ("NaN",       NaN (),               WRITABLE | CONFIGURABLE);
	addOwnProperty ("Infinity",  POSITIVE_INFINITY (), WRITABLE | CONFIGURABLE);
	addOwnProperty ("undefined", undefined,            WRITABLE | CONFIGURABLE);
	addOwnProperty ("Math",      new Math (ec),        WRITABLE | CONFIGURABLE);

	// Functions

	addOwnProperty ("isNaN",      new NativeFunction (ec, "isNaN",      isNaN,      1), WRITABLE | CONFIGURABLE);
	addOwnProperty ("parseInt",   new NativeFunction (ec, "parseInt",   parseInt,   1), WRITABLE | CONFIGURABLE);
	addOwnProperty ("parseFloat", new NativeFunction (ec, "parseFloat", parseFloat, 1), WRITABLE | CONFIGURABLE);
}

bool
GlobalObject::resolve (const Identifier & identifier)
throw (pbError)
{
	if (resolveCallback (identifier))
		return true;

	return pb::Object::resolve (identifier);
}

var
GlobalObject::isNaN (const ptr <pbExecutionContext> & ec, const var & object, const std::vector <var> & arguments)
{
	if (arguments .empty ())
		return true;

	return pb::isNaN (arguments [0] .toNumber ());
}

var
GlobalObject::parseInt (const ptr <pbExecutionContext> & ec, const var & object, const std::vector <var> & arguments)
{
	if (arguments .empty ())
		return NaN ();

	return pb::parseInt (arguments [0] .toString ());
}

var
GlobalObject::parseFloat (const ptr <pbExecutionContext> & ec, const var & object, const std::vector <var> & arguments)
{
	if (arguments .empty ())
		return NaN ();

	return pb::parseFloat (arguments [0] .toString ());
}

} // Standard
} // pb
} // titania