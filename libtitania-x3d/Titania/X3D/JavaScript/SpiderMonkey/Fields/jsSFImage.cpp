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

#include "jsSFImage.h"

#include "jsMFInt32.h"
#include "../jsContext.h"

namespace titania {
namespace X3D {

JSClass jsSFImage::static_class = {
	"SFImage", JSCLASS_HAS_PRIVATE | JSCLASS_NEW_ENUMERATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize,
	JSCLASS_NO_OPTIONAL_MEMBERS

};

JSPropertySpec jsSFImage::properties [ ] = {
	{ "width",  WIDTH,  JSPROP_ENUMERATE | JSPROP_SHARED | JSPROP_PERMANENT, width,  width },
	{ "x",      WIDTH,                     JSPROP_SHARED | JSPROP_PERMANENT, width,  width },  // VRML
	{ "height", HEIGHT, JSPROP_ENUMERATE | JSPROP_SHARED | JSPROP_PERMANENT, height, height },
	{ "y",      HEIGHT,                    JSPROP_SHARED | JSPROP_PERMANENT, height, height }, // VRML
	{ "comp",   COMP,   JSPROP_ENUMERATE | JSPROP_SHARED | JSPROP_PERMANENT, comp,   comp },
	{ "array",  ARRAY,  JSPROP_ENUMERATE | JSPROP_SHARED | JSPROP_PERMANENT, array,  array },
	{ 0 }

};

JSFunctionSpec jsSFImage::functions [ ] = {
	{ "getName",     getName <X3DChildObject>,     0, 0 },
	{ "getTypeName", getTypeName <X3DChildObject>, 0, 0 },
	{ "getType",     getType <X3DFieldDefinition>, 0, 0 },

	{ "toString",    toString <X3DChildObject>, 0, 0 },

	{ 0 }

};

void
jsSFImage::init (JSContext* context, JSObject* global)
{
	JS_InitClass (context, global, NULL, &static_class, construct,
	              0, properties, functions, NULL, NULL);
}

JSBool
jsSFImage::create (JSContext* context, SFImage* field, jsval* vp, const bool seal)
{
	JSObject* result = JS_NewObject (context, &static_class, NULL, NULL);

	if (result == NULL)
		return JS_FALSE;

	JS_SetPrivate (context, result, field);

	//if (seal)
	//	JS_SealObject (context, result, JS_FALSE);

	static_cast <jsContext*> (JS_GetContextPrivate (context)) -> addField (field);

	*vp = OBJECT_TO_JSVAL (result);

	return JS_TRUE;
}

JSBool
jsSFImage::construct (JSContext* context, uintN argc, jsval* vp)
{
	if (argc == 0)
	{
		return create (context, new SFImage (), &JS_RVAL (context, vp));
	}
	else if (argc == 4)
	{
		uint32    width, height, comp;
		JSObject* obj2;

		jsval* argv = JS_ARGV (context, vp);

		if (not JS_ConvertArguments (context, argc, argv, "uuuo", &width, &height, &comp, &obj2))
			return JS_FALSE;

		if (not JS_InstanceOf (context, obj2, jsMFInt32::getClass (), NULL))
		{
			JS_ReportError (context, "Type of argument 4 is invalid - should be MFInt32, is %s", JS_GetClass (context, obj2) -> name);
			return JS_FALSE;
		}

		MFInt32* array = (MFInt32*) JS_GetPrivate (context, obj2);

		return create (context, new SFImage (width, height, comp, *array), &JS_RVAL (context, vp));
	}

	JS_ReportError (context, "wrong number of arguments");

	return JS_FALSE;
}

JSBool
jsSFImage::width (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	return JS_NewNumberValue (context, sfimage -> getWidth (), vp);
}

JSBool
jsSFImage::width (JSContext* context, JSObject* obj, jsid id, JSBool strict, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	uint32 value;

	if (not JS_ValueToECMAUint32 (context, *vp, &value))
		return JS_FALSE;

	sfimage -> setWidth (value);

	return JS_TRUE;
}

JSBool
jsSFImage::height (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	return JS_NewNumberValue (context, sfimage -> getHeight (), vp);
}

JSBool
jsSFImage::height (JSContext* context, JSObject* obj, jsid id, JSBool strict, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	uint32 value;

	if (not JS_ValueToECMAUint32 (context, *vp, &value))
		return JS_FALSE;

	sfimage -> setHeight (value);

	return JS_TRUE;
}

JSBool
jsSFImage::comp (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	return JS_NewNumberValue (context, sfimage -> getComponents (), vp);
}

JSBool
jsSFImage::comp (JSContext* context, JSObject* obj, jsid id, JSBool strict, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	uint32 value;

	if (not JS_ValueToECMAUint32 (context, *vp, &value))
		return JS_FALSE;

	sfimage -> setComponents (value);

	return JS_TRUE;
}

JSBool
jsSFImage::array (JSContext* context, JSObject* obj, jsid id, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	return jsMFInt32::create (context, &sfimage -> getArray (), vp);
}

JSBool
jsSFImage::array (JSContext* context, JSObject* obj, jsid id, JSBool strict, jsval* vp)
{
	SFImage* sfimage = (SFImage*) JS_GetPrivate (context, obj);

	JSObject* obj2;

	if (not JS_ValueToObject (context, *vp, &obj2))
		return JS_FALSE;

	if (not JS_InstanceOf (context, obj2, jsMFInt32::getClass (), NULL))
	{
		JS_ReportError (context, "Type of argument is invalid - should be MFInt32, is %s", JS_GetClass (context, obj2) -> name);
		return JS_FALSE;
	}

	sfimage -> setArray (*(MFInt32*) JS_GetPrivate (context, obj2));

	return JS_TRUE;
}

} // X3D
} // titania
