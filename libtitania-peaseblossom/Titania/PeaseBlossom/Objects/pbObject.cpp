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

#include "pbObject.h"

#include "../Objects/pbFunction.h"

namespace titania {
namespace pb {

PropertyDescriptor::PropertyDescriptor (pbChildObject* const object,
                                        const Identifier & identifier,
                                        const var & value_,
                                        const PropertyFlagsType & flags,
                                        const ptr <pbFunction> & getter_,
                                        const ptr <pbFunction> & setter_) :
	    object (object),
	identifier (identifier),
	     value (value_),
	     flags (flags),
	    getter (getter_),
	    setter (setter_)
{
	addValue ();

	getter .addParent (object);
	setter .addParent (object);
}

PropertyDescriptor::PropertyDescriptor (pbChildObject* const object,
                                        const Identifier & identifier,
                                        var && value_,
                                        const PropertyFlagsType & flags,
                                        ptr <pbFunction> && getter_,
                                        ptr <pbFunction> && setter_) :
	    object (object),
	identifier (identifier),
	     value (std::move (value_)),
	     flags (flags),
	    getter (std::move (getter_)),
	    setter (std::move (setter_))
{
	addValue ();

	getter .addParent (object);
	setter .addParent (object);
}

void
PropertyDescriptor::addValue ()
{
	if (value .isObject ())
		const_cast <ptr <pbObject> &> (value .getObject ()) .addParent (object);
}

PropertyDescriptor::~PropertyDescriptor ()
{ }

const std::string pbObject::typeName = "Object";

pbObject::pbObject () :
	       pbChildObject (),
	pbOutputStreamObject (),
	          pbUserData (),
	          extensible (true),
	         constructor (),
	               proto (),
	          properties (),
	    cachedProperties (CACHE_SIZE, std::make_pair (-1, PropertyDescriptorPtr ())),
	     resolveFunction (),
	     disposeFunction ()
{
	addChildren (constructor, proto);
}

const std::string &
pbObject::getClassName () const
noexcept (true)
{
	return constructor -> getTypeName ();
}

void
pbObject::setProto (const ptr <pbObject> & value)
throw (std::invalid_argument)
{
	proto = value;

	if (proto)
		addPropertyDescriptor ("__proto__", proto, NONE);

	else
		addPropertyDescriptor ("__proto__", nullptr, NONE);
}

void
pbObject::setProperty (const Identifier & identifier, const var & value)
throw (std::out_of_range,
       pbException)
{
	const auto & property = getPropertyDescriptor (identifier, true);

	if (property -> getFlags () & WRITABLE)
	{
		if (property -> getSetter ())
		{
			property -> getSetter () -> apply (this, { value });
			return;
		}

		property -> setValue (value);
	}
}

var
pbObject::getProperty (const Identifier & identifier) const
throw (std::out_of_range,
       pbException)
{
	const auto & property = getPropertyDescriptor (identifier, false);

	if (property -> getGetter ())
		return property -> getGetter () -> apply (const_cast <pbObject*> (this));

	return property -> getValue ();
}

ptr <pbObject>
pbObject::getObject (const Identifier & identifier) const
throw (std::out_of_range,
       TypeError)
{
	const auto property = getProperty (identifier);

	if (property .isObject ())
		return property .getObject ();

	throw TypeError ("Property '"+ identifier .getName () +"' is not an object.");
}

void
pbObject::addPropertyDescriptor (const Identifier & identifier,
                                 const var & value,
                                 const PropertyFlagsType flags,
                                 const ptr <pbFunction> & getter,
                                 const ptr <pbFunction> & setter)
throw (std::invalid_argument)
{
	const auto pair = properties .emplace (identifier .getId (), std::make_shared <PropertyDescriptor> (this, identifier, value, flags, getter, setter));

	if (not pair .second)
		throw std::invalid_argument ("Property '" + identifier .getName () + "' already exists.");
}

void
pbObject::addPropertyDescriptor (const Identifier & identifier,
                                 var && value,
                                 const PropertyFlagsType flags,
                                 ptr <pbFunction> && getter,
                                 ptr <pbFunction> && setter)
throw (std::invalid_argument)
{
	const auto pair = properties .emplace (identifier .getId (), std::make_shared <PropertyDescriptor> (this, identifier, std::move (value), flags, std::move (getter), std::move (setter)));

	if (not pair .second)
		throw std::invalid_argument ("Property '" + identifier .getName () + "' already exists.");
}

void
pbObject::updatePropertyDescriptor (const Identifier & identifier,
                                    const var & value,
                                    const PropertyFlagsType flags,
                                    const ptr <pbFunction> & getter,
                                    const ptr <pbFunction> & setter)
noexcept (true)
{
	try
	{
		const auto & property = getPropertyDescriptor (identifier, true);

		if (not (flags & LEAVE_VALUE))
			property -> setValue (value);

		property -> setFlags (flags);

		if (getter)
			property -> setGetter (getter);

		if (setter)
			property -> setSetter (setter);
	}
	catch (const std::out_of_range &)
	{
		addPropertyDescriptor (identifier, value, flags, getter, setter);
	}
}

void
pbObject::removePropertyDescriptor (const Identifier & identifier)
noexcept (true)
{
	removeCachedPropertyDescriptors (identifier .getId ());

	properties .erase (identifier .getId ());
}

const PropertyDescriptorPtr &
pbObject::getPropertyDescriptor (const Identifier & identifier, const bool write) const
throw (std::out_of_range)
{
	try
	{
		return getPropertyDescriptor (identifier .getId ());
	}
	catch (const std::out_of_range &)
	{
		try
		{
			const auto & property = getProto () -> getPropertyDescriptor (identifier, false);

			if (write)
			{
				const_cast <pbObject*> (this) -> addPropertyDescriptor (property -> getIdentifier (),
			                                                           property -> getValue (),
			                                                           property -> getFlags (),
			                                                           property -> getGetter (),
			                                                           property -> getSetter ());

			   return getPropertyDescriptor (identifier .getId ());
			}

			return property;
		}
		catch (const std::out_of_range &)
		{
			if (resolveFunction and resolveFunction (const_cast <pbObject*> (this), identifier))
				return getPropertyDescriptor (identifier .getId ());

			throw;
		}
	}
}

const PropertyDescriptorPtr &
pbObject::getPropertyDescriptor (const size_t id) const
throw (std::out_of_range)
{
	try
	{
		return getCachedPropertyDescriptor (id);
	}
	catch (const std::out_of_range &)
	{
		const auto & property = properties .at (id);

		const_cast <pbObject*> (this) -> addCachedPropertyDescriptor (id, property);

		return property;
	}
}

void
pbObject::addCachedPropertyDescriptor (const size_t id, const PropertyDescriptorPtr & property)
noexcept (true)
{
	cachedProperties [id % CACHE_SIZE] = std::make_pair (id, property);
}

void
pbObject::removeCachedPropertyDescriptors (const size_t id)
noexcept (true)
{
	auto & value = cachedProperties [id % CACHE_SIZE];

	if (value .first not_eq id)
		return;

	value .first = -1;
	value .second .reset ();
}

const PropertyDescriptorPtr &
pbObject::getCachedPropertyDescriptor (const size_t id) const
throw (std::out_of_range)
{
	const auto & value = cachedProperties [id % CACHE_SIZE];

	if (value .first == id)
		return value .second;

	throw std::out_of_range ("getCachedPropertyDescriptor");
}

const ptr <pbObject> &
pbObject::getProto () const
throw (std::out_of_range)
{
	if (proto)
		return proto;

	throw std::out_of_range ("pbObject::getProto");
}

var
pbObject::getDefaultValue (const ValueType preferedType) const
throw (pbException)
{
	// All native ECMAScript objects except Date objects handle the absence of a hint as if the hint Number were given;
	// Date objects handle the absence of a hint as if the hint String were given. Host objects may handle the absence of
	// a hint in some other manner.

	//	if (preferedType == STRING)
	//		return toString () or valueOf () or throw TypeError ();
	//
	//	return valueOf () or toString () or throw TypeError ();

	static const Identifier toString = "toString";
	static const Identifier valueOf  = "valueOf";

	if (preferedType == STRING)
	{
		try
		{
			return apply (toString, this);
		}
		catch (const std::invalid_argument &)
		{ }

		try
		{
			return apply (valueOf, this);
		}
		catch (const std::invalid_argument &)
		{ }
	}

	try
	{
		return apply (valueOf, this);
	}
	catch (const std::invalid_argument &)
	{ }

	try
	{
		return apply (toString, this);
	}
	catch (const std::invalid_argument &)
	{ }

	throw TypeError ("can't convert object to string.");
}

var
pbObject::apply (const Identifier & identifier, const var & object, const std::vector <var> & arguments) const
throw (pbException,
       std::invalid_argument)
{
	try
	{
		const auto property = getProperty (identifier);

		if (property .isObject ())
		{
			const auto function = dynamic_cast <pbFunction*> (property .getObject () .get ());

			if (function)
				return function -> apply (object, arguments);
		}
	}
	catch (const std::out_of_range &)
	{ }

	throw std::invalid_argument ("pbObject::apply");
}

void
pbObject::toStream (std::ostream & ostream) const
{
	ostream << "[object " << getClassName () << "]";
}

void
pbObject::dispose ()
{
	if (disposeFunction)
		disposeFunction (this);

	properties       .clear ();
	cachedProperties .clear ();

	pbChildObject::dispose ();
}

pbObject::~pbObject ()
{ }

} // pb
} // titania
