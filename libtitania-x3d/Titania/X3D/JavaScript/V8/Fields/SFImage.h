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

#ifndef __TITANIA_X3D_JAVA_SCRIPT_V8_FIELDS_SFIMAGE_H__
#define __TITANIA_X3D_JAVA_SCRIPT_V8_FIELDS_SFIMAGE_H__

#include <v8.h>

#include "../Context.h"
#include "../X3DField.h"

namespace titania {
namespace X3D {
namespace GoogleV8 {

class SFImage :
	public X3DField <X3D::SFImage>
{
public:

	using T          = X3D::SFImage;
	using value_type = T;

	using X3DField <T>::TypeName;
	using X3DField <T>::Type;

	static
	v8::Local <v8::FunctionTemplate>
	initialize (const v8::Local <v8::External> &);


private:

	using X3DField <T>::createFunctionTemplate;
	using X3DField <T>::getPropertyAttributes;
	using X3DField <T>::getFunctionAttributes;
	using X3DField <T>::addProperty;
	using X3DField <T>::addObject;
	using X3DField <T>::getObject;
	using X3DField <T>::getName;
	using X3DField <T>::getTypeName;
	using X3DField <T>::getType;
	using X3DField <T>::isReadable;
	using X3DField <T>::isWritable;
	using X3DField <T>::toString;

	///  @name Construction

	static
	v8::Handle <v8::Value>
	construct (const v8::Arguments &);

	///  @name Member access

	static
	v8::Handle <v8::Integer>
	hasIndex (uint32_t, const v8::AccessorInfo &);

	static
	v8::Handle <v8::Value>
	set1Value (uint32_t, v8::Local <v8::Value>, const v8::AccessorInfo &);

	static
	v8::Handle <v8::Value>
	get1Value (uint32_t, const v8::AccessorInfo &);

	static
	v8::Handle <v8::Array>
	getIndices (const v8::AccessorInfo &);

	///  @name Properties

	static
	void
	width (v8::Local <v8::String> property, v8::Local <v8::Value> value, const v8::AccessorInfo & info)
	{ getObject (info) -> setWidth (value -> ToUint32 () -> Value ()); }

	static
	v8::Handle <v8::Value>
	width (v8::Local <v8::String> property, const v8::AccessorInfo & info)
	{ return v8::Uint32::New (getObject (info) -> getWidth ()); }

	static
	void
	height (v8::Local <v8::String> property, v8::Local <v8::Value> value, const v8::AccessorInfo & info)
	{ getObject (info) -> setHeight (value -> ToUint32 () -> Value ()); }

	static
	v8::Handle <v8::Value>
	height (v8::Local <v8::String> property, const v8::AccessorInfo & info)
	{ return v8::Uint32::New (getObject (info) -> getHeight ()); }

	static
	void
	comp (v8::Local <v8::String> property, v8::Local <v8::Value> value, const v8::AccessorInfo & info)
	{ getObject (info) -> setComponents (value -> ToUint32 () -> Value ()); }

	static
	v8::Handle <v8::Value>
	comp (v8::Local <v8::String> property, const v8::AccessorInfo & info)
	{ return v8::Uint32::New (getObject (info) -> getComponents ()); }

	static
	void
	array (v8::Local <v8::String>, v8::Local <v8::Value>, const v8::AccessorInfo &);

	static
	v8::Handle <v8::Value>
	array (v8::Local <v8::String>, const v8::AccessorInfo &);

};

} // GoogleV8
} // X3D
} // titania

#endif