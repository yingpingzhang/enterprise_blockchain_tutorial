#ifndef INCLUDE_H_PKCS11
#define INCLUDE_H_PKCS11

#include <vector>

#include "template.h"
#include "mech.h"
#include "strings.h"

using namespace v8;
using namespace node;

struct KEY_PAIR {
	CK_OBJECT_HANDLE privateKey;
	CK_OBJECT_HANDLE publicKey;
};

class PKCS11 {
public:
	void Load(Scoped<string> path);

	// PKCS11
	void C_Initialize();
	void C_Finalize();
	Scoped<CK_INFO> C_GetInfo();
	vector<CK_SLOT_ID> C_GetSlotList(CK_BBOOL tokenPresent);
	Scoped<CK_SLOT_INFO> C_GetSlotInfo(CK_SLOT_ID slotId);
	Scoped<CK_TOKEN_INFO> C_GetTokenInfo(CK_SLOT_ID slotID);
	vector<CK_MECHANISM_TYPE> C_GetMechanismList(CK_SLOT_ID slotID);
	Scoped<CK_MECHANISM_INFO> C_GetMechanismInfo(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type);
	Scoped<string> C_InitToken(CK_SLOT_ID slotID, Scoped<string> pin, Scoped<string> label);
	void C_InitPIN(CK_SESSION_HANDLE session, Scoped<string> pin);
	void C_SetPIN(CK_SESSION_HANDLE session, Scoped<string> oldPin, Scoped<string> newPin);
	CK_SESSION_HANDLE C_OpenSession(CK_SLOT_ID slotID, CK_FLAGS flags);
	void C_CloseSession(CK_SESSION_HANDLE session);
	void C_CloseAllSessions(CK_SLOT_ID slotID);
	Scoped<CK_SESSION_INFO> C_GetSessionInfo(CK_SESSION_HANDLE session);
	// C_GetOperationState);
	// C_SetOperationState);
	void C_Login(CK_SESSION_HANDLE session, CK_USER_TYPE userType, Scoped<string> pin);
	void C_Logout(CK_SESSION_HANDLE session);
	//
	///* Object management */
	CK_OBJECT_HANDLE C_CreateObject(CK_SESSION_HANDLE session, Scoped<Attributes> tmpl);
	CK_OBJECT_HANDLE C_CopyObject(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object, Scoped<Attributes> tmpl);
	void C_DestroyObject(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object);
	CK_ULONG C_GetObjectSize(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object);
	void C_FindObjectsInit(CK_SESSION_HANDLE session, Scoped<Attributes> attrs);
	void C_FindObjectsInit(CK_SESSION_HANDLE session);
	CK_OBJECT_HANDLE C_FindObjects(CK_SESSION_HANDLE session);
	void C_FindObjectsFinal(CK_SESSION_HANDLE session);
	Scoped<Attributes> C_GetAttributeValue(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object, Scoped<Attributes> tmpl);
	void C_SetAttributeValue(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object, Scoped<Attributes> tmpl);

	///* Encryption and decryption */
	void C_EncryptInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_SESSION_HANDLE hObject);
	Scoped<string> C_Encrypt(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> encMsg);
	Scoped<string> C_EncryptUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part, Scoped<string> encPart);
	Scoped<string> C_EncryptFinal(CK_SESSION_HANDLE hSession, Scoped<string> encPart);
	void C_DecryptInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_SESSION_HANDLE hObject);
	Scoped<string> C_Decrypt(CK_SESSION_HANDLE hSession, Scoped<string> encMsg, Scoped<string> msg);
	Scoped<string> C_DecryptUpdate(CK_SESSION_HANDLE hSession, Scoped<string> encPart, Scoped<string> decPart);
	Scoped<string> C_DecryptFinal(CK_SESSION_HANDLE hSession, Scoped<string> decPart);

	///* Message digesting */
	void C_DigestInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech);
	Scoped<string> C_Digest(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> output);
	void C_DigestUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part);
	Scoped<string> C_DigestFinal(CK_SESSION_HANDLE hSession, Scoped<string> output);
	void C_DigestKey(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject);

	///* Signing and MACing */
	void C_SignInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hKey);
	Scoped<string> C_Sign(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> output);
	void C_SignUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part);
	Scoped<string> C_SignFinal(CK_SESSION_HANDLE hSession, Scoped<string> output);
	void C_SignRecoverInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hKey);
	Scoped<string> C_SignRecover(CK_SESSION_HANDLE hSession, Scoped<string>data, Scoped<string> signature);

	///* Verifying signatures and MACs */
	void C_VerifyInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hKey);
	void C_Verify(CK_SESSION_HANDLE hSession, Scoped<string> msg, Scoped<string> signature);
	void C_VerifyUpdate(CK_SESSION_HANDLE hSession, Scoped<string> part);
	void C_VerifyFinal(CK_SESSION_HANDLE hSession, Scoped<string> signature);
	void C_VerifyRecoverInit(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, CK_OBJECT_HANDLE hObject);
	Scoped<string> C_VerifyRecover(CK_SESSION_HANDLE hSession, Scoped<string> signature, Scoped<string> data);

	///* Key management */
	CK_OBJECT_HANDLE C_GenerateKey(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, Scoped<Attributes> tmpl);
	Scoped<KEY_PAIR> C_GenerateKeyPair(CK_SESSION_HANDLE hSession, Scoped<Mechanism> mech, Scoped<Attributes> publicKeyTemplate, Scoped<Attributes> privateKeyTemplate);
	Scoped<string> C_WrapKey(
		CK_SESSION_HANDLE hSession,
		Scoped<Mechanism> mech,
		CK_OBJECT_HANDLE hWrappingKey,
		CK_OBJECT_HANDLE hKey,
		Scoped<string> wrappedKey
	);
	CK_OBJECT_HANDLE C_UnwrapKey(
		CK_SESSION_HANDLE hSession,
		Scoped<Mechanism> mech,
		CK_OBJECT_HANDLE hUnwrappingKey,
		Scoped<string> wrappedKey,
		Scoped<Attributes> tmpl
	);
	CK_OBJECT_HANDLE C_DeriveKey(
		CK_SESSION_HANDLE hSession,
		Scoped<Mechanism> mech,
		CK_OBJECT_HANDLE hBaseKey,
		Scoped<Attributes> tmpl
	);

	///* Random number generation */
	void C_SeedRandom(CK_SESSION_HANDLE hSession, char *data, size_t dataLen);
	void C_GenerateRandom(CK_SESSION_HANDLE hSession, char *data, size_t dataLen);

protected:

	void* dlHandle;
	CK_FUNCTION_LIST_PTR functionList;
};

#endif // INCLUDE_H_PKCS11