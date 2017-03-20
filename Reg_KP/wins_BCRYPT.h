
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

#include <Wincrypt.h>
#pragma comment (lib, "Crypt32.lib")

#define NT_OK(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL    ((NTSTATUS)0xC0000001L)

int ToBase64Crypto(const CHAR* pSrc, int nLenSrc, CHAR* pDst, int nLenDst)
{
	DWORD nLenOut = nLenDst;
	BOOL fRet = CryptBinaryToStringA( (const BYTE*) pSrc, nLenSrc,	CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, pDst, &nLenOut);
	if (!fRet) nLenOut = 0;  // failed
	return(nLenOut);
}

int ToBase64Cryptourl(const CHAR* pSrc, int nLenSrc, CHAR* pDst, int nLenDst)
{
	DWORD nLenOut = nLenDst;
	BOOL fRet = CryptBinaryToStringA((const BYTE*)pSrc, nLenSrc,CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,pDst, &nLenOut);
	if (!fRet) nLenOut = 0;  // failed
	CHAR* p = pDst;
	int solang = nLenOut;
	for (int i = 0; i < solang; i++) {
		if (*p == '+')  *p = '-';
		if (*p == '/')  *p = '_';

		if (*p == '=') {
			solang = i;
			*p = '\0';
		}
		else	p++;
	};
	return(solang);
}


int FromBase64Crypto(const CHAR* pSrc, int nLenSrc, CHAR* pDst, int nLenDst)
{
	DWORD nLenOut = nLenDst;
	BOOL fRet = CryptStringToBinaryA((LPCSTR)pSrc, nLenSrc,	CRYPT_STRING_BASE64,(BYTE*)pDst, &nLenOut, NULL, NULL);
	if (!fRet) nLenOut = 0;  // failed
	return(nLenOut);
}

int FromBase64Cryptourl(CHAR* pSrc, int nLenSrc, CHAR* pDst, int nLenDst)
{

	CHAR* p = pSrc;
	int solang = nLenSrc;
	for (int i = 0; i < solang; i++) {
		if (*p == '-')  *p = '+';
		if (*p == '_')  *p = '/';
		p++;
	};

	switch (solang % 4) // Pad with trailing '='s
	{
	case 2:
		*p = '=';
		p++;
		nLenSrc++;
	case 3:
		*p = '=';
		nLenSrc++;
		p++;
	case 0: break; // One pad char
	case 1: break; // No pad chars in this case
	};

	DWORD nLenOut = nLenDst;
	BOOL fRet = CryptStringToBinaryA(
		(LPCSTR)pSrc, nLenSrc,
		CRYPT_STRING_BASE64,
		(BYTE*)pDst, &nLenOut,
		NULL,        // pdwSkip (not needed)
		NULL         // pdwFlags (not needed)
		);
	if (!fRet) nLenOut = 0;  // failed
	return(nLenOut);
}


int hash_SHA256(const BYTE* pSrc, int nLenSrc, CHAR* pDst)
{
	NTSTATUS                status = STATUS_UNSUCCESSFUL;


	BCRYPT_ALG_HANDLE       hAlg = NULL;
	BCRYPT_HASH_HANDLE      hHash = NULL;
	DWORD                   cbData = 0, cbHash = 0, cbHashObject = 0;
	PBYTE                   pbHashObject = NULL;
	PBYTE                   pbHash = NULL;
	int back = 0;

	//open an algorithm handle
	if (!NT_OK(status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0)))	{
		printf("**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
		goto Cleanup;
	}

	//calculate the size of the buffer to hold the hash object
	if (!NT_OK(status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0)))	{
		printf("**** Error 0x%x returned by BCryptGetProperty\n", status);
		goto Cleanup;
	}

	//allocate the hash object on the heap
	pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
	if (NULL == pbHashObject)	{
		printf("**** memory allocation failed\n");
		goto Cleanup;
	}

	//calculate the length of the hash
	if (!NT_OK(status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0)))	{
		printf("**** Error 0x%x returned by BCryptGetProperty\n", status);
		goto Cleanup;
	}

	//allocate the hash buffer on the heap
	pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
	if (NULL == pbHash)	{
		printf("**** memory allocation failed\n");
		goto Cleanup;
	}

	//create a hash
	if (!NT_OK(status = BCryptCreateHash(hAlg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0)))	{
		printf("**** Error 0x%x returned by BCryptCreateHash\n", status);
		goto Cleanup;
	}
	
	//hash some data
	if (!NT_OK(status = BCryptHashData(hHash, (PBYTE)pSrc, nLenSrc, 0))) {
		printf("**** Error 0x%x returned by BCryptHashData\n", status);
		goto Cleanup;
	}

	//close the hash
	if (!NT_OK(status = BCryptFinishHash(hHash, pbHash, cbHash, 0)))	{
		printf("**** Error 0x%x returned by BCryptFinishHash\n", status);
		goto Cleanup;
	};

	int i = 0;
	for (; i < cbHash; i++) pDst[i] = pbHash[i];
	pDst[i] = '\0';

	back = 1;

Cleanup:
	if (hAlg)			BCryptCloseAlgorithmProvider(hAlg, 0);
	if (hHash)			BCryptDestroyHash(hHash);
	if (pbHashObject)	HeapFree(GetProcessHeap(), 0, pbHashObject);
	if (pbHash)			HeapFree(GetProcessHeap(), 0, pbHash);
	return back;

}


int bcrypt_AES_ECB(const BYTE* pSrc, int nLenSrc, CHAR* myKEY, CHAR* pDst)
{

	BCRYPT_ALG_HANDLE       hAesAlg = NULL;
	BCRYPT_KEY_HANDLE       hKey = NULL;
	NTSTATUS                status = STATUS_UNSUCCESSFUL;
	DWORD                   cbCipherText = 0, cbPlainText = 0, cbData = 0, cbKeyObject = 0, cbBlockLen = 0, cbBlob = 0;
	PBYTE                   pbCipherText = NULL, pbPlainText = NULL, pbKeyObject = NULL;

	int back = 0;


	// Open an algorithm handle.
	if (!NT_OK(status = BCryptOpenAlgorithmProvider(&hAesAlg, BCRYPT_AES_ALGORITHM, NULL, 0)))	{
		printf("**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
		goto Cleanup;
	}

	// Calculate the size of the buffer to hold the KeyObject.
	if (!NT_OK(status = BCryptGetProperty(hAesAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbKeyObject, sizeof(DWORD), &cbData, 0)))	{
		printf("**** Error 0x%x returned by BCryptGetProperty\n", status);
		goto Cleanup;
	}

	// Allocate the key object on the heap.
	pbKeyObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbKeyObject);
	if (NULL == pbKeyObject) {
		printf("**** memory allocation failed\n");
		goto Cleanup;
	}

	if (!NT_OK(status = BCryptSetProperty(hAesAlg, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_ECB, sizeof(BCRYPT_CHAIN_MODE_ECB), 0)))	{
		printf("**** Error 0x%x returned by BCryptSetProperty\n", status);
		goto Cleanup;
	}

	// Generate the key from supplied input key bytes.
	if (!NT_OK(status = BCryptGenerateSymmetricKey(hAesAlg, &hKey, pbKeyObject, cbKeyObject, (PBYTE)myKEY, strlen(myKEY), 0)))	{
		printf("**** Error 0x%x returned by BCryptGenerateSymmetricKey\n", status);
		goto Cleanup;
	}

	pbPlainText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, 128);
	if (NULL == pbPlainText) {
		printf("**** memory allocation failed\n");
		goto Cleanup;
	}

	cbPlainText = 16;

	memcpy(pbPlainText, pSrc, nLenSrc);

	//
	// Get the output buffer size.
	//
	if (!NT_OK(status = BCryptEncrypt(
		hKey,
		pbPlainText,
		cbPlainText,
		NULL,
		NULL, // kein IV bei ECB,
		0,    // cbBlockLen,
		NULL,
		0,
		&cbCipherText,
		0)))
	{
		printf("**** Error 0x%x returned by BCryptEncrypt\n", status);
		goto Cleanup;
	}

	pbCipherText = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbCipherText);
	if (NULL == pbCipherText)	{
		printf("**** memory allocation failed\n");
		goto Cleanup;
	}

	// Use the key to encrypt the plaintext buffer.
	// For block sized messages, block padding will add an extra block.
	if (!NT_OK(status = BCryptEncrypt(
		hKey,
		pbPlainText,
		cbPlainText,
		NULL,
		NULL, // pbIV,
		0, // cbBlockLen,
		pbCipherText,
		cbCipherText,
		&cbData,
		0)))
	{
		printf("**** Error 0x%x returned by BCryptEncrypt\n", status);
		goto Cleanup;
	}


	int i = 0;
	for (; i < cbCipherText; i++) pDst[i] = pbCipherText[i];
	pDst[i] = '\0';

	back = 1;
Cleanup:

	if (hAesAlg)		BCryptCloseAlgorithmProvider(hAesAlg, 0);
	if (hKey)     		BCryptDestroyKey(hKey);
	if (pbCipherText)	HeapFree(GetProcessHeap(), 0, pbCipherText);
	if (pbPlainText)	HeapFree(GetProcessHeap(), 0, pbPlainText);
	if (pbKeyObject)	HeapFree(GetProcessHeap(), 0, pbKeyObject);

	return back;
}
