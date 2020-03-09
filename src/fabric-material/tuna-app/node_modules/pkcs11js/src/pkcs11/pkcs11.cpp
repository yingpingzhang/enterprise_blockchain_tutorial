#include <node.h>
#include <node_buffer.h>

#ifdef WIN32
#include "../dl.h"
#else
#include <dlfcn.h>
#endif // WIN32

#include "pkcs11.h"

#define CASE_PKCS11_ERROR(_value) 					\
	case _value: {									\
		Scoped<string> res(new string(#_value)); *res += ":"; *res += to_string(_value); return res; }

static Scoped<string> get_pkcs11_error(CK_ULONG value) {
	switch (value) {
		CASE_PKCS11_ERROR(CKR_OK);
		CASE_PKCS11_ERROR(CKR_CANCEL);
		CASE_PKCS11_ERROR(CKR_HOST_MEMORY);
		CASE_PKCS11_ERROR(CKR_SLOT_ID_INVALID);
		CASE_PKCS11_ERROR(CKR_GENERAL_ERROR);
		CASE_PKCS11_ERROR(CKR_FUNCTION_FAILED);
		CASE_PKCS11_ERROR(CKR_ARGUMENTS_BAD);
		CASE_PKCS11_ERROR(CKR_NO_EVENT);
		CASE_PKCS11_ERROR(CKR_NEED_TO_CREATE_THREADS);
		CASE_PKCS11_ERROR(CKR_CANT_LOCK);
		CASE_PKCS11_ERROR(CKR_ATTRIBUTE_READ_ONLY);
		CASE_PKCS11_ERROR(CKR_ATTRIBUTE_SENSITIVE);
		CASE_PKCS11_ERROR(CKR_ATTRIBUTE_TYPE_INVALID);
		CASE_PKCS11_ERROR(CKR_ATTRIBUTE_VALUE_INVALID);
		CASE_PKCS11_ERROR(CKR_DATA_INVALID);
		CASE_PKCS11_ERROR(CKR_DATA_LEN_RANGE);
		CASE_PKCS11_ERROR(CKR_DEVICE_ERROR);
		CASE_PKCS11_ERROR(CKR_DEVICE_MEMORY);
		CASE_PKCS11_ERROR(CKR_DEVICE_REMOVED);
		CASE_PKCS11_ERROR(CKR_ENCRYPTED_DATA_INVALID);
		CASE_PKCS11_ERROR(CKR_ENCRYPTED_DATA_LEN_RANGE);
		CASE_PKCS11_ERROR(CKR_FUNCTION_CANCELED);
		CASE_PKCS11_ERROR(CKR_FUNCTION_NOT_PARALLEL);
		CASE_PKCS11_ERROR(CKR_FUNCTION_NOT_SUPPORTED);
		CASE_PKCS11_ERROR(CKR_KEY_HANDLE_INVALID);
		CASE_PKCS11_ERROR(CKR_KEY_SIZE_RANGE);
		CASE_PKCS11_ERROR(CKR_KEY_TYPE_INCONSISTENT);
		CASE_PKCS11_ERROR(CKR_KEY_NOT_NEEDED);
		CASE_PKCS11_ERROR(CKR_KEY_CHANGED);
		CASE_PKCS11_ERROR(CKR_KEY_NEEDED);
		CASE_PKCS11_ERROR(CKR_KEY_INDIGESTIBLE);
		CASE_PKCS11_ERROR(CKR_KEY_FUNCTION_NOT_PERMITTED);
		CASE_PKCS11_ERROR(CKR_KEY_NOT_WRAPPABLE);
		CASE_PKCS11_ERROR(CKR_KEY_UNEXTRACTABLE);
		CASE_PKCS11_ERROR(CKR_MECHANISM_INVALID);
		CASE_PKCS11_ERROR(CKR_MECHANISM_PARAM_INVALID);
		CASE_PKCS11_ERROR(CKR_OBJECT_HANDLE_INVALID);
		CASE_PKCS11_ERROR(CKR_OPERATION_ACTIVE);
		CASE_PKCS11_ERROR(CKR_OPERATION_NOT_INITIALIZED);
		CASE_PKCS11_ERROR(CKR_PIN_INCORRECT);
		CASE_PKCS11_ERROR(CKR_PIN_INVALID);
		CASE_PKCS11_ERROR(CKR_PIN_LEN_RANGE);
		CASE_PKCS11_ERROR(CKR_PIN_EXPIRED);
		CASE_PKCS11_ERROR(CKR_PIN_LOCKED);
		CASE_PKCS11_ERROR(CKR_SESSION_CLOSED);
		CASE_PKCS11_ERROR(CKR_SESSION_COUNT);
		CASE_PKCS11_ERROR(CKR_SESSION_HANDLE_INVALID);
		CASE_PKCS11_ERROR(CKR_SESSION_PARALLEL_NOT_SUPPORTED);
		CASE_PKCS11_ERROR(CKR_SESSION_READ_ONLY);
		CASE_PKCS11_ERROR(CKR_SESSION_EXISTS);
		CASE_PKCS11_ERROR(CKR_SESSION_READ_ONLY_EXISTS);
		CASE_PKCS11_ERROR(CKR_SESSION_READ_WRITE_SO_EXISTS);
		CASE_PKCS11_ERROR(CKR_SIGNATURE_INVALID);
		CASE_PKCS11_ERROR(CKR_SIGNATURE_LEN_RANGE);
		CASE_PKCS11_ERROR(CKR_TEMPLATE_INCOMPLETE);
		CASE_PKCS11_ERROR(CKR_TEMPLATE_INCONSISTENT);
		CASE_PKCS11_ERROR(CKR_TOKEN_NOT_PRESENT);
		CASE_PKCS11_ERROR(CKR_TOKEN_NOT_RECOGNIZED);
		CASE_PKCS11_ERROR(CKR_TOKEN_WRITE_PROTECTED);
		CASE_PKCS11_ERROR(CKR_UNWRAPPING_KEY_HANDLE_INVALID);
		CASE_PKCS11_ERROR(CKR_UNWRAPPING_KEY_SIZE_RANGE);
		CASE_PKCS11_ERROR(CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT);
		CASE_PKCS11_ERROR(CKR_USER_ALREADY_LOGGED_IN);
		CASE_PKCS11_ERROR(CKR_USER_NOT_LOGGED_IN);
		CASE_PKCS11_ERROR(CKR_USER_PIN_NOT_INITIALIZED);
		CASE_PKCS11_ERROR(CKR_USER_TYPE_INVALID);
		CASE_PKCS11_ERROR(CKR_USER_ANOTHER_ALREADY_LOGGED_IN);
		CASE_PKCS11_ERROR(CKR_USER_TOO_MANY_TYPES);
		CASE_PKCS11_ERROR(CKR_WRAPPED_KEY_INVALID);
		CASE_PKCS11_ERROR(CKR_WRAPPED_KEY_LEN_RANGE);
		CASE_PKCS11_ERROR(CKR_WRAPPING_KEY_HANDLE_INVALID);
		CASE_PKCS11_ERROR(CKR_WRAPPING_KEY_SIZE_RANGE);
		CASE_PKCS11_ERROR(CKR_WRAPPING_KEY_TYPE_INCONSISTENT);
		CASE_PKCS11_ERROR(CKR_RANDOM_SEED_NOT_SUPPORTED);
		CASE_PKCS11_ERROR(CKR_RANDOM_NO_RNG);
		CASE_PKCS11_ERROR(CKR_DOMAIN_PARAMS_INVALID);
		CASE_PKCS11_ERROR(CKR_BUFFER_TOO_SMALL);
		CASE_PKCS11_ERROR(CKR_SAVED_STATE_INVALID);
		CASE_PKCS11_ERROR(CKR_INFORMATION_SENSITIVE);
		CASE_PKCS11_ERROR(CKR_STATE_UNSAVEABLE);
		CASE_PKCS11_ERROR(CKR_CRYPTOKI_NOT_INITIALIZED);
		CASE_PKCS11_ERROR(CKR_CRYPTOKI_ALREADY_INITIALIZED);
		CASE_PKCS11_ERROR(CKR_MUTEX_BAD);
		CASE_PKCS11_ERROR(CKR_MUTEX_NOT_LOCKED);
		CASE_PKCS11_ERROR(CKR_NEW_PIN_MODE);
		CASE_PKCS11_ERROR(CKR_NEXT_OTP);
		CASE_PKCS11_ERROR(CKR_EXCEEDED_MAX_ITERATIONS);
		CASE_PKCS11_ERROR(CKR_FIPS_SELF_TEST_FAILED);
		CASE_PKCS11_ERROR(CKR_LIBRARY_LOAD_FAILED);
		CASE_PKCS11_ERROR(CKR_PIN_TOO_WEAK);
		CASE_PKCS11_ERROR(CKR_PUBLIC_KEY_INVALID);
		CASE_PKCS11_ERROR(CKR_FUNCTION_REJECTED);
		CASE_PKCS11_ERROR(CKR_VENDOR_DEFINED);
	default:
		return Scoped<string>(new string("Unknown error"));
	}
}

#define CHECK_PKCS11_RV(func)								\
{   CK_RV rv = func;										\
    if (rv != CKR_OK) {										\
		THROW_ERROR(get_pkcs11_error(rv)->c_str(), NULL);	\
    }														\
}

void PKCS11::Load(Scoped<string> path) {
	try {
		int mode = RTLD_LAZY;

		dlHandle = dlopen(path->c_str(), mode);
		if (!dlHandle) {
			THROW_ERROR(dlerror(), NULL);
		}

		// reset errors
		dlerror();
		CK_C_GetFunctionList f_C_GetFunctionList = (CK_C_GetFunctionList)dlsym(dlHandle, "C_GetFunctionList");
		const char *dlsym_error = dlerror();
		if (dlsym_error) {
			dlclose(dlHandle);
			THROW_ERROR("Cannot load symbol 'C_GetFunctionList'", NULL);
			return;
		}

		CHECK_PKCS11_RV(f_C_GetFunctionList(&functionList));
	}
	CATCH_ERROR;
}

void PKCS11::C_Initialize() {
	try {

		CHECK_PKCS11_RV(functionList->C_Initialize(
			NULL_PTR
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_Finalize() {
	try {

		CHECK_PKCS11_RV(functionList->C_Finalize(
			NULL_PTR
		));

	}
	CATCH_ERROR;
}

Scoped<CK_INFO> PKCS11::C_GetInfo() {
	try {
		Scoped<CK_INFO> info = make_shared<CK_INFO>();

		CHECK_PKCS11_RV(functionList->C_GetInfo(
			&*info
		));

		return info;
	}
	CATCH_ERROR;
}

vector<CK_SLOT_ID> PKCS11::C_GetSlotList(CK_BBOOL tokenPresent) {
	try {
		CK_ULONG   ulCount;

		CHECK_PKCS11_RV(functionList->C_GetSlotList(
			tokenPresent,
			NULL_PTR, &ulCount
		));

		vector<CK_SLOT_ID> arSlotList(ulCount);

		CHECK_PKCS11_RV(functionList->C_GetSlotList(
			tokenPresent,
			arSlotList.data(), &ulCount
		));

		return arSlotList;
	}
	CATCH_ERROR;

}

Scoped<CK_SLOT_INFO> PKCS11::C_GetSlotInfo(CK_SLOT_ID slotId) {
	try {
		Scoped<CK_SLOT_INFO> info = make_shared<CK_SLOT_INFO>();

		CHECK_PKCS11_RV(functionList->C_GetSlotInfo(
			slotId,
			info.get()
		));

		return info;
	}
	CATCH_ERROR;
}

Scoped<CK_TOKEN_INFO> PKCS11::C_GetTokenInfo(CK_SLOT_ID slotId) {
	try {
		Scoped<CK_TOKEN_INFO> info = make_shared<CK_TOKEN_INFO>();

		CHECK_PKCS11_RV(functionList->C_GetTokenInfo(
			slotId,
			info.get()
		));

		return info;
	}
	CATCH_ERROR;
}

vector<CK_MECHANISM_TYPE> PKCS11::C_GetMechanismList(CK_SLOT_ID slotID) {
	try {
		CK_ULONG ulCount;

		CHECK_PKCS11_RV(functionList->C_GetMechanismList(
			slotID,
			NULL_PTR, &ulCount
		));

		auto pMechanismList = vector<CK_MECHANISM_TYPE>(ulCount);

		CHECK_PKCS11_RV(functionList->C_GetMechanismList(
			slotID,
			pMechanismList.data(), &ulCount
		));

		return pMechanismList;
	}
	CATCH_ERROR;
}

Scoped<CK_MECHANISM_INFO> PKCS11::C_GetMechanismInfo(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type) {
	try {
		Scoped<CK_MECHANISM_INFO> info = make_shared<CK_MECHANISM_INFO>();

		CHECK_PKCS11_RV(functionList->C_GetMechanismInfo(
			slotID,
			type,
			info.get()
		));

		return info;
	}
	CATCH_ERROR;
}


Scoped<string> PKCS11::C_InitToken(CK_SLOT_ID slotID, Scoped<string> pin,  Scoped<string> label) {
	try {
		CHECK_PKCS11_RV(functionList->C_InitToken(
			slotID,
			pin->length() ? (CK_UTF8CHAR_PTR)pin->c_str() : NULL_PTR, (CK_ULONG)pin->length(),
			label->length() ? (CK_UTF8CHAR_PTR)label->c_str() : NULL_PTR
		));

		return Scoped<string>(label);
	}
	CATCH_ERROR;
}

void PKCS11::C_InitPIN(CK_SESSION_HANDLE session, Scoped<string> pin) {
	try {

		CHECK_PKCS11_RV(functionList->C_InitPIN(
			session,
			pin->length() ? (CK_UTF8CHAR_PTR)pin->c_str() : NULL_PTR, (CK_ULONG)pin->length()
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_SetPIN(CK_SESSION_HANDLE session, Scoped<string> oldPin, Scoped<string> newPin) {
	try {

		CHECK_PKCS11_RV(functionList->C_SetPIN(
			session,
			oldPin->length() ? (CK_UTF8CHAR_PTR)oldPin->c_str() : NULL_PTR, (CK_ULONG)oldPin->length(),
			newPin->length() ? (CK_UTF8CHAR_PTR)newPin->c_str() : NULL_PTR, (CK_ULONG)newPin->length()
		));

	}
	CATCH_ERROR;
}

CK_SESSION_HANDLE PKCS11::C_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags) {
	try {
		CK_SESSION_HANDLE hSession;

		CHECK_PKCS11_RV(functionList->C_OpenSession(slotID, flags, NULL_PTR, NULL_PTR, &hSession));

		return hSession;
	}
	CATCH_ERROR;
}

void PKCS11::C_CloseSession(CK_SESSION_HANDLE session) {
	try {

		CHECK_PKCS11_RV(functionList->C_CloseSession(session));

	}
	CATCH_ERROR;
}

void PKCS11::C_CloseAllSessions(CK_SLOT_ID slotID) {
	try {

		CHECK_PKCS11_RV(functionList->C_CloseAllSessions(slotID));

	}
	CATCH_ERROR;
}

Scoped<CK_SESSION_INFO> PKCS11::C_GetSessionInfo(CK_SESSION_HANDLE session) {
	try {
		Scoped<CK_SESSION_INFO> info = make_shared<CK_SESSION_INFO>();

		CHECK_PKCS11_RV(functionList->C_GetSessionInfo(session, info.get()));

		return info;
	}
	CATCH_ERROR;
}

void PKCS11::C_Login(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, Scoped<string> pin) {
	try {

		CHECK_PKCS11_RV(functionList->C_Login(
			hSession,
			userType,
			pin->length() ? (CK_UTF8CHAR_PTR)pin->c_str() : NULL_PTR, (CK_ULONG)pin->length()
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_Logout(CK_SESSION_HANDLE hSession) {
	try {

		CHECK_PKCS11_RV(functionList->C_Logout(
			hSession
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_FindObjectsInit(CK_SESSION_HANDLE hSession, Scoped<Attributes> attrs) {
	try {

		CHECK_PKCS11_RV(functionList->C_FindObjectsInit(
			hSession,
			attrs->Get()->size ? attrs->Get()->items : NULL_PTR, attrs->Get()->size
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_FindObjectsInit(CK_SESSION_HANDLE hSession) {
	try {

		C_FindObjectsInit(hSession, Scoped<Attributes>(new Attributes()));

	}
	CATCH_ERROR;
}

CK_OBJECT_HANDLE PKCS11::C_FindObjects(CK_SESSION_HANDLE session) {
	try {
		CK_ULONG ulObjectCount;
		CK_OBJECT_HANDLE hObject;

		CHECK_PKCS11_RV(functionList->C_FindObjects(
			session,
			&hObject, 1,
			&ulObjectCount));

		return ulObjectCount ? hObject : 0;
	}
	CATCH_ERROR;
}

void PKCS11::C_FindObjectsFinal(CK_SESSION_HANDLE hSession) {
	try {

		CHECK_PKCS11_RV(functionList->C_FindObjectsFinal(hSession));

	}
	CATCH_ERROR;
}


Scoped<Attributes> PKCS11::C_GetAttributeValue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, Scoped<Attributes> tmpl) {
	try {

		auto pTemplate = tmpl->Get();

		CHECK_PKCS11_RV(functionList->C_GetAttributeValue(
			hSession,
			hObject,
			pTemplate->items, pTemplate->size
		));

		// Prepare value blocks for writing
		for (uint32_t i = 0; i < pTemplate->size; i++) {
			pTemplate->items[i].pValue = (CK_BYTE_PTR)malloc(pTemplate->items[i].ulValueLen);
		}

		CHECK_PKCS11_RV(functionList->C_GetAttributeValue(
			hSession,
			hObject,
			pTemplate->items, pTemplate->size
		));

		return tmpl;

	}
	CATCH_ERROR;

}

void PKCS11::C_SetAttributeValue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, Scoped<Attributes> tmpl) {
	try {

		auto pTemplate = tmpl->Get();

		CHECK_PKCS11_RV(functionList->C_SetAttributeValue(
			hSession,
			hObject,
			pTemplate->items, pTemplate->size
		));
	}
	CATCH_ERROR;
}

CK_OBJECT_HANDLE PKCS11::C_CreateObject(CK_SESSION_HANDLE hSession, Scoped<Attributes> tmpl) {
	try {

		auto pTemplate = tmpl->Get();

		CK_OBJECT_HANDLE hObject;

		CHECK_PKCS11_RV(functionList->C_CreateObject(
			hSession,
			pTemplate->items, pTemplate->size,
			&hObject
		));

		return hObject;

	}
	CATCH_ERROR;
}

CK_OBJECT_HANDLE PKCS11::C_CopyObject(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, Scoped<Attributes> tmpl) {
	try {

		auto pTemplate = tmpl->Get();

		CK_OBJECT_HANDLE hNewObject;
		CHECK_PKCS11_RV(functionList->C_CopyObject(
			hSession,
			hObject,
			pTemplate->items, pTemplate->size,
			&hNewObject
		));

		return hNewObject;

	}
	CATCH_ERROR;
}

void PKCS11::C_DestroyObject(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject) {
	try {

		CHECK_PKCS11_RV(functionList->C_DestroyObject(
			hSession,
			hObject
		));

	}
	CATCH_ERROR;
}


CK_ULONG PKCS11::C_GetObjectSize(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject) {
	try {

		CK_ULONG ulSize;
		CHECK_PKCS11_RV(functionList->C_GetObjectSize(
			hSession,
			hObject,
			&ulSize
		));

		return ulSize;
	}
	CATCH_ERROR;
}

static void crypto_init(
	CK_ULONG fn(
		CK_SESSION_HANDLE hSession,    /* the session's handle */
		CK_MECHANISM_PTR  pMechanism,  /* the encryption mechanism */
		CK_OBJECT_HANDLE  hKey         /* handle of encryption key */
	),
	CK_SESSION_HANDLE hSession,
	Scoped<Mechanism> mech,
	CK_OBJECT_HANDLE  hKey
) {
	try {

		auto pMechanism = mech->Get();

		CHECK_PKCS11_RV(fn(
			hSession,
			pMechanism,
			hKey
		));

	}
	CATCH_ERROR;
}

static Scoped<string> crypto_update(
	CK_ULONG fn(
		CK_SESSION_HANDLE hSession,           /* session's handle */
		CK_BYTE_PTR       pIn,              /* the plaintext data */
		CK_ULONG          ulInLen,          /* plaintext data len */
		CK_BYTE_PTR       pOut,     /* gets ciphertext */
		CK_ULONG_PTR      pulOutLen /* gets c-text size */
	),
	CK_SESSION_HANDLE hSession,
	Scoped<string> input,
	Scoped<string> output
) {
	try {

		auto outLen = output->length();

		CHECK_PKCS11_RV(fn(
			hSession,
			(CK_BYTE_PTR)input->c_str(), (CK_ULONG)input->length(),
			(CK_BYTE_PTR)output->c_str(), (CK_ULONG_PTR)&outLen
		));

		return Scoped<string>(new string(output->substr(0, outLen)));
	}
	CATCH_ERROR;
}

static void crypto_update(
	CK_ULONG fn(
		CK_SESSION_HANDLE hSession,  /* the session's handle */
		CK_BYTE_PTR       pPart,     /* data to be digested */
		CK_ULONG          ulPartLen  /* bytes of data to be digested */
	),
	CK_SESSION_HANDLE hSession,
	Scoped<string> part
) {
	try {

		CHECK_PKCS11_RV(fn(
			hSession,
			(CK_BYTE_PTR)part->c_str(),
			(CK_ULONG)part->length()
		));

	}
	CATCH_ERROR;
}

static Scoped<string> crypto_final(
	CK_ULONG fn(
		CK_SESSION_HANDLE hSession,                /* session handle */
		CK_BYTE_PTR       pLastEncryptedPart,      /* last c-text */
		CK_ULONG_PTR      pulLastEncryptedPartLen  /* gets last size */
	),
	CK_SESSION_HANDLE hSession,
	Scoped<string> output
) {
	try {

		auto outputLen = output->length();

		CHECK_PKCS11_RV(fn(
			hSession,
			(CK_BYTE_PTR)output->c_str(),
			(CK_ULONG_PTR)&outputLen
		));

		return Scoped<string>(new string(output->substr(0, outputLen)));
	}
	CATCH_ERROR;
}

void PKCS11::C_EncryptInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_SESSION_HANDLE hObject) {
	try {

		crypto_init(functionList->C_EncryptInit, hSession, mech, hObject);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_Encrypt(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> encMsg) {
	try {

		return crypto_update(functionList->C_Encrypt, hSession, msg, encMsg);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_EncryptUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part, Scoped<string> encPart) {
	try {

		return crypto_update(functionList->C_EncryptUpdate, hSession, part, encPart);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_EncryptFinal(CK_SESSION_HANDLE hSession, Scoped<string> encPart) {
	try {

		return crypto_final(functionList->C_EncryptFinal, hSession, encPart);

	}
	CATCH_ERROR;
}

void PKCS11::C_DecryptInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_SESSION_HANDLE hObject) {
	try {

		crypto_init(functionList->C_DecryptInit, hSession, mech, hObject);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_Decrypt(CK_SESSION_HANDLE hSession, Scoped<string> encMsg, Scoped<string> msg) {
	try {

		return crypto_update(functionList->C_Decrypt, hSession, encMsg, msg);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_DecryptUpdate(CK_SESSION_HANDLE hSession, Scoped<string> encPart, Scoped<string> decPart) {
	try {

		return crypto_update(functionList->C_DecryptUpdate, hSession, encPart, decPart);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_DecryptFinal(CK_SESSION_HANDLE hSession, Scoped<string> decPart) {
	try {

		return crypto_final(functionList->C_DecryptFinal, hSession, decPart);

	}
	CATCH_ERROR;
}

void PKCS11::C_DigestInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech) {
	try {

		auto pMechanism = mech->Get();

		CHECK_PKCS11_RV(functionList->C_DigestInit(
			hSession,
			pMechanism
		));

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_Digest(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> output) {
	try {

		return crypto_update(functionList->C_Digest, hSession, msg, output);

	}
	CATCH_ERROR;
}

void PKCS11::C_DigestUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part) {
	try {

		crypto_update(functionList->C_DigestUpdate, hSession, part);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_DigestFinal(CK_SESSION_HANDLE hSession, Scoped<string> output) {
	try {

		return crypto_final(functionList->C_DigestFinal, hSession, output);

	}
	CATCH_ERROR;
}

void PKCS11::C_DigestKey(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject) {
	try {

		CHECK_PKCS11_RV(functionList->C_DigestKey(hSession, hObject));

	}
	CATCH_ERROR;
}

void PKCS11::C_SignInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hKey) {
	try {

		crypto_init(functionList->C_SignInit, hSession, mech, hKey);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_Sign(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> output) {
	try {

		return crypto_update(functionList->C_Sign, hSession, msg, output);

	}
	CATCH_ERROR;
}

void PKCS11::C_SignUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part) {
	try {

		crypto_update(functionList->C_SignUpdate, hSession, part);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_SignFinal(CK_SESSION_HANDLE hSession, Scoped<string> output) {
	try {

		return crypto_final(functionList->C_SignFinal, hSession, output);

	}
	CATCH_ERROR;
}

void PKCS11::C_SignRecoverInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hKey) {
	try {

		crypto_init(functionList->C_SignRecoverInit, hSession, mech, hKey);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_SignRecover(CK_SESSION_HANDLE hSession, Scoped<string>data, Scoped<string> signature) {
	try {

		return crypto_update(functionList->C_SignRecover, hSession, data, signature);

	}
	CATCH_ERROR;
}

void PKCS11::C_VerifyInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hKey) {
	try {

		crypto_init(functionList->C_VerifyInit, hSession, mech, hKey);

	}
	CATCH_ERROR;
}

void PKCS11::C_Verify(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> signature) {
	try {

		CHECK_PKCS11_RV(functionList->C_Verify(
			hSession,
			(CK_BYTE_PTR)msg->c_str(), (CK_ULONG)msg->length(),
			(CK_BYTE_PTR)signature->c_str(), (CK_ULONG)signature->length()
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_VerifyUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part) {
	try {

		crypto_update(functionList->C_VerifyUpdate, hSession, part);

	}
	CATCH_ERROR;
}

void PKCS11::C_VerifyFinal(CK_SESSION_HANDLE hSession, Scoped<string> signature) {
	try {

		CHECK_PKCS11_RV(functionList->C_VerifyFinal(
			hSession,
			(CK_BYTE_PTR)signature->c_str(), (CK_ULONG)signature->length()
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_VerifyRecoverInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hObject) {
	try {

		crypto_init(functionList->C_SignRecoverInit, hSession, mech, hObject);

	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_VerifyRecover(CK_SESSION_HANDLE hSession, Scoped<string> signature, Scoped<string> data) {
	try {

		return crypto_update(functionList->C_VerifyRecover, hSession, signature, data);

	}
	CATCH_ERROR;
}

CK_OBJECT_HANDLE PKCS11::C_GenerateKey(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, Scoped<Attributes> tmpl) {
	try {

		auto pMechanism = mech->Get();
		auto pTemplate = tmpl->Get();
		CK_OBJECT_HANDLE hObject;

		CHECK_PKCS11_RV(functionList->C_GenerateKey(
			hSession,
			pMechanism,
			pTemplate->items, pTemplate->size,
			&hObject
		));

		return hObject;
	}
	CATCH_ERROR;
}

Scoped<KEY_PAIR> PKCS11::C_GenerateKeyPair(
	CK_SESSION_HANDLE hSession,
	Scoped<Mechanism> mech,
	Scoped<Attributes> publicKeyTemplate,
	Scoped<Attributes> privateKeyTemplate
) {
	try {

		auto pMechanism = mech->Get();
		auto pPublicKeyTemplate = publicKeyTemplate->Get();
		auto pPrivateKeyTemplate = privateKeyTemplate->Get();
		CK_OBJECT_HANDLE hPublicKey;
		CK_OBJECT_HANDLE hPrivateKey;

		CHECK_PKCS11_RV(functionList->C_GenerateKeyPair(
			hSession,
			pMechanism,
			pPublicKeyTemplate->items, pPublicKeyTemplate->size,
			pPrivateKeyTemplate->items, pPrivateKeyTemplate->size,
			&hPublicKey,
			&hPrivateKey
		));

		Scoped<KEY_PAIR> keyPair = make_shared<KEY_PAIR>();
		keyPair->privateKey = hPrivateKey;
		keyPair->publicKey = hPublicKey;

		return keyPair;
	}
	CATCH_ERROR;
}

Scoped<string> PKCS11::C_WrapKey(
	CK_SESSION_HANDLE hSession,
	Scoped<Mechanism> mech,
	CK_OBJECT_HANDLE hWrappingKey,
	CK_OBJECT_HANDLE hKey,
	Scoped<string> wrappedKey
) {
	try {

		auto pMechanism = mech->Get();
		auto wrappedKeyLen = wrappedKey->length();

		CHECK_PKCS11_RV(functionList->C_WrapKey(
			hSession,
			pMechanism,
			hWrappingKey,
			hKey,
			(CK_BYTE_PTR)wrappedKey->c_str(), (CK_ULONG_PTR)&wrappedKeyLen
		));

		return Scoped<string>(new string(wrappedKey->substr(0, wrappedKeyLen)));
	}
	CATCH_ERROR;
}

CK_OBJECT_HANDLE PKCS11::C_UnwrapKey(
	CK_SESSION_HANDLE hSession,
	Scoped<Mechanism> mech,
	CK_OBJECT_HANDLE hUnwrappingKey,
	Scoped<string> wrappedKey,
	Scoped<Attributes> tmpl
) {
	try {

		auto pMechanism = mech->Get();
		auto pTemplate = tmpl->Get();
		CK_OBJECT_HANDLE hKey;

		CHECK_PKCS11_RV(functionList->C_UnwrapKey(
			hSession,
			pMechanism,
			hUnwrappingKey,
			(CK_BYTE_PTR)wrappedKey->c_str(), (CK_ULONG)wrappedKey->length(),
			pTemplate->items, pTemplate->size,
			&hKey
		));

		return hKey;
	}
	CATCH_ERROR;
}

CK_OBJECT_HANDLE PKCS11::C_DeriveKey(
	CK_SESSION_HANDLE hSession,
	Scoped<Mechanism> mech,
	CK_OBJECT_HANDLE hBaseKey,
	Scoped<Attributes> tmpl
) {
	try {
		auto pMechanism = mech->Get();
		auto pTemplate = tmpl->Get();
		CK_OBJECT_HANDLE hDerivedKey;

		CHECK_PKCS11_RV(functionList->C_DeriveKey(
			hSession,
			pMechanism,
			hBaseKey,
			pTemplate->items, pTemplate->size,
			&hDerivedKey
		));

		return hDerivedKey;
	}
	CATCH_ERROR;
}

void PKCS11::C_SeedRandom(CK_SESSION_HANDLE hSession, char *data, size_t dataLen) {
	try {

		CHECK_PKCS11_RV(functionList->C_SeedRandom(
			hSession,
			(CK_BYTE_PTR)data, (CK_ULONG)dataLen
		));

	}
	CATCH_ERROR;
}

void PKCS11::C_GenerateRandom(CK_SESSION_HANDLE hSession, char *data, size_t dataLen) {
	try {

		CHECK_PKCS11_RV(functionList->C_GenerateRandom(
			hSession,
			(CK_BYTE_PTR)data, (CK_ULONG)dataLen
		));

	}
	CATCH_ERROR;
}