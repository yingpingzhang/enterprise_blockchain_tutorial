#include "template.h"

static void attr_set_value(CK_ATTRIBUTE_PTR attr, const char* value, uint32_t valueLen) {
	try {
		attr->pValue = (char*)malloc(valueLen);
		memcpy(attr->pValue, value, valueLen);
		attr->ulValueLen = valueLen;
	}
	CATCH_ERROR;
}

static Scoped<CK_ATTRIBUTE> v2c_ATTRIBUTE(Local<Value> v8Attribute) {
	try {
		if (!v8Attribute->IsObject()) {
			THROW_ERROR("Parameter 1 MSUT be Object", NULL);
		}

		Local<Object> v8Object = v8Attribute->ToObject();

		Local<Value> v8Type = v8Object->Get(Nan::New(STR_TYPE).ToLocalChecked());
		if (!v8Type->IsNumber()) {
			THROW_ERROR("Member 'type' MUST be Number", NULL);
		}

		Local<Value> v8Value = v8Object->Get(Nan::New(STR_VALUE).ToLocalChecked());
		if (!(v8Value->IsUndefined() || v8Value->IsNull() ||
			node::Buffer::HasInstance(v8Value) ||
			v8Value->IsBoolean() ||
			v8Value->IsString() ||
			v8Value->IsNumber())) {
			THROW_ERROR("Member 'value' MUST has wrong type", NULL);
		}

		Scoped<CK_ATTRIBUTE> attr(new CK_ATTRIBUTE());
		attr->pValue = NULL;
		attr->ulValueLen = 0;

		attr->type = Nan::To<v8::Number>(v8Type).ToLocalChecked()->Uint32Value();
		if (node::Buffer::HasInstance(v8Value)) {
			// Buffer
			GET_BUFFER_SMPL(data, v8Value);
			attr_set_value(attr.get(), data, (uint32_t)dataLen);
		}
		else if (v8Value->IsBoolean()) {
			// Boolean
			char data[1];
			data[0] = (char)v8Value->ToBoolean()->Value();
			attr_set_value(attr.get(), data, 1);
		}
		else if (v8Value->IsNumber()) {
			// Number
			CK_ULONG num = Nan::To<v8::Number>(v8Value).ToLocalChecked()->Uint32Value();

			uint32_t long_size = sizeof(CK_ULONG);

			attr->pValue = (char*)malloc(long_size);
			for (uint32_t i = 0; i < long_size; i++)
				((char*)attr->pValue)[i] = (char)(num >> (i * 8));
			attr->ulValueLen = long_size;
		}
		else if (v8Value->IsString()) {
			// String
			String::Utf8Value utf8Val(v8Value);
			char* val = *utf8Val;
			int valLen = utf8Val.length();
			attr_set_value(attr.get(), val, valLen);
		}

		return attr;
	}
	CATCH_ERROR;
}

static Local<Object> c2v_ATTRIBUTE(CK_ATTRIBUTE_PTR attr) {
	try {
		Nan::HandleScope();

		if (!attr) {
			THROW_ERROR("Parameter 1 is EMPTY", NULL);
		}

		Local<Object> v8Attribute = Nan::New<Object>();

		// Type
		v8Attribute->Set(Nan::New(STR_TYPE).ToLocalChecked(), Nan::New<Number>(attr->type));

		// Value
		Local<Object> v8Value = node::Buffer::Copy(Isolate::GetCurrent(), (char *)attr->pValue, attr->ulValueLen).ToLocalChecked();
		v8Attribute->Set(Nan::New(STR_VALUE).ToLocalChecked(), v8Value);

		return v8Attribute;
	}
	CATCH_ERROR;
}

static void TEMPLATE_free(TEMPLATE* tmpl) {
	if (tmpl && tmpl->items) {
		// Free attr values
		for (CK_ULONG i = 0; i < tmpl->size; i++) {
			if (&tmpl->items[i] && tmpl->items[i].pValue) {
				free(tmpl->items[i].pValue);
			}
		}
		free(tmpl->items);
	}
}

Attributes::Attributes() {
	data = TEMPLATE();
	data.size = 0;
	data.items = NULL;
}

Attributes::~Attributes() {
	TEMPLATE_free(Get());
}

TEMPLATE* Attributes::New() {
	TEMPLATE_free(Get());
	data = TEMPLATE();
	data.size = 0;
	data.items = NULL;
	return Get();
}

void Attributes::Push(CK_ATTRIBUTE_PTR attr) {
	try {
		if (!attr)
			THROW_ERROR("Parameter 1 is EMPTY", NULL);
		if (Get()) {
			data.items = (CK_ATTRIBUTE_PTR)realloc(data.items, ++data.size * sizeof(CK_ATTRIBUTE));
			data.items[data.size - 1] = *(attr);
		}
	}
	CATCH_ERROR;
}

void Attributes::FromV8(Local<Value> v8Value) {
	try {
		Nan::HandleScope();

		if (!v8Value->IsArray()) {
			THROW_ERROR("Parameter 1 MUST be Array", NULL);
		}

		Local<Object> v8Template = v8Value->ToObject();
		uint32_t templateLen = Nan::To<v8::Number>(v8Template->Get(Nan::New("length").ToLocalChecked())).ToLocalChecked()->Uint32Value();

		uint32_t i = 0;
		New();
		for (i = 0; i < templateLen; i++) {
			Local<Value> v8Attribute = v8Template->Get(i);
			Scoped<CK_ATTRIBUTE> attr = v2c_ATTRIBUTE(v8Attribute);
			this->Push(attr.get());
		}
	}
	CATCH_ERROR;
}

Local<Object> Attributes::ToV8() {
	try {
		Nan::HandleScope();

		Local<Array> v8Res = Nan::New<Array>();
		for (uint32_t i = 0; i < data.size; i++) {
			CK_ATTRIBUTE_PTR pItem = &data.items[i];
			Local<Object> v8Attribute = c2v_ATTRIBUTE(pItem);

			v8Res->Set(i, v8Attribute);
		}
		return v8Res;
	}
	CATCH_ERROR;
}