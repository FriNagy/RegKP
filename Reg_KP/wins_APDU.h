
// C++ APDU Demos f. Registrierkassen von A-Trust homepage
// http://labs.a-trust.at/developer/ShowSource.aspx?id=114
// APDU = smart cards application protocol data unit

char toSign[99];          // In_Out! In=hash256 zu signieren, Out=signatur 64 Byte 

#include <winscard.h>

#pragma comment(lib, "winscard.lib")

const unsigned char ACOS_04_ATR_1[] = { 0x3B, 0xBF, 0x13, 0x00, 0x81, 0x31, 0xFE, 0x45, 0x45, 0x50, 0x41 };
const unsigned char ACOS_04_ATR_2[] = { 0x3B, 0xBF, 0x11, 0x00, 0x81, 0x31, 0xFE, 0x45, 0x45, 0x50, 0x41 };
const unsigned char CARDOS_53_ATR[] = { 0x3b, 0xdf, 0x18, 0x00, 0x81, 0x31, 0xfe, 0x58, 0x80, 0x31, 0x90, 0x52, 0x41, 0x01, 0x64, 0x05, 0xc9, 0x03, 0xac, 0x73, 0xb7, 0xb1, 0xd4, 0x44 };


bool CheckApduSW(BYTE *pbRecvBuffer, DWORD dwRecvLength, unsigned char sw1, unsigned char sw2)
{
	if (dwRecvLength < 2)
	{
		return false;
	}
	unsigned char c1 = pbRecvBuffer[dwRecvLength - 2];
	unsigned char c2 = pbRecvBuffer[dwRecvLength - 1];

	bool bRet = true;

	if (c1 != sw1)
	{
		bRet = false;
	}

	if (c2 != sw2)
	{
		bRet = false;
	}
	return bRet;
}



int ReadFile(SCARD_IO_REQUEST pioSendPci, SCARDHANDLE hCard, char* pDst)
{
	BYTE pbRecvBuffer[500];
	int lauf = 0;
	unsigned long fileIndex = 0;
	DWORD dwRecvLength;
	bool success = true;
	unsigned char cmd[] = { 0x00, 0xB0, 0x00, 0x00, 0xFF }; // read data
	unsigned long ulongmax = 0L - 1L;
	bool eof = false;
	char *p = pDst;
	char *po = pDst;
	 

	while (!eof)
	{
		unsigned char b1 = (unsigned char)(fileIndex >> 8);
		unsigned char b2 = (unsigned char)fileIndex;
		unsigned char b3 = 0xFF;
		cmd[2] = b1;
		cmd[3] = b2;
		cmd[4] = b3;

		if (success)
		{
			dwRecvLength = sizeof(pbRecvBuffer);
			LONG rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd, sizeof(cmd), NULL, pbRecvBuffer, &dwRecvLength);
		}
		if (success)
		{
			success = CheckApduSW((BYTE*)&pbRecvBuffer, dwRecvLength, 0x90, 0x00);
		}

		if (!success)
		{
			if (CheckApduSW((BYTE*)&pbRecvBuffer, dwRecvLength, 0x62, 0x82))
			{
				// EOF
				success = true;
				eof = true;
			}
		}

		if (success)
		{
			unsigned long len = dwRecvLength;
			if (len > 2)
				len -= 2;

			for (int i = 0; i < len; ++i) {
			//	buf.push_back(pbRecvBuffer[i]);
				*p++ =  pbRecvBuffer[i];
			};
			if ((ulongmax - fileIndex) <= len)
			{
				break;
			}
			else
			{
				fileIndex += len;
			}
		}
		else
		{
			break;
		}
	}
 
	int len = (p - po);

	return len;
}



/*
void ACOS_ReadCertificate(SCARD_IO_REQUEST pioSendPci, SCARDHANDLE hCard)
{
	BYTE pbRecvBuffer[258];
	DWORD dwRecvLength;
	LONG rv;
	unsigned char cmd1[] = { 0x00, 0xA4, 0x00, 0x0C, 0x02, 0xDF, 0x70 };
	unsigned char cmd2[] = { 0x00, 0xA4, 0x00, 0x0C, 0x02, 0xC0, 0x02 };

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd1, sizeof(cmd1), NULL, pbRecvBuffer, &dwRecvLength);
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd2, sizeof(cmd2), NULL, pbRecvBuffer, &dwRecvLength);

//	std::vector<unsigned char> certificate;
//	ReadFile(pioSendPci, hCard, certificate);

	//FILE *fp = fopen("c:\\temp\\cert.cer", "wb");
	//if (fp)
	//{
	//	fwrite(certificate.data(), 1, certificate.size(), fp);
	//	fclose(fp); 
	//}
}



void ACOS_PerformeSignature(SCARD_IO_REQUEST pioSendPci, SCARDHANDLE hCard)
{
	BYTE pbRecvBuffer[258];
	BYTE Hash[32 + 5];
	DWORD dwRecvLength;

	LONG rv;

	Hash[0] = 0x00;
	Hash[1] = 0x2A;
	Hash[2] = 0x90;
	Hash[3] = 0x81;
	Hash[4] = 32;

	for (int i = 0; i < 32; i++) Hash[5 + i] = toSign[i];

	dwRecvLength = sizeof(pbRecvBuffer);
	unsigned char cmd1[] = { 0x00, 0xA4, 0x00, 0x0C, 0x02, 0xDF, 0x70 };
	unsigned char cmd2[] = { 0x00, 0x22, 0x41, 0xB6, 0x06, 0x84, 0x01, 0x88, 0x80, 0x01, 0x44 };
	unsigned char cmd3[] = { 0x00, 0x20, 0x00, 0x81, 0x08, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00, 0x00 };
	unsigned char cmd4[] = { 0x00, 0x2A, 0x9E, 0x9A, 0x00 };

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd1, sizeof(cmd1), NULL, pbRecvBuffer, &dwRecvLength);
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd2, sizeof(cmd2), NULL, pbRecvBuffer, &dwRecvLength);
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd3, sizeof(cmd3), NULL, pbRecvBuffer, &dwRecvLength);
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&Hash, sizeof(Hash), NULL, pbRecvBuffer, &dwRecvLength);
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd4, sizeof(cmd4), NULL, pbRecvBuffer, &dwRecvLength);

//	std::vector<unsigned char> sig;
//	for (size_t i = 0; i < dwRecvLength - 2; i++)	sig.push_back(pbRecvBuffer[i]);
	Bytesback = dwRecvLength - 2;
	for (int i = 0; i < Bytesback; i++)  toSign[i] = pbRecvBuffer[i];


}

*/

int CARDOS_ReadCertificate(SCARD_IO_REQUEST pioSendPci, SCARDHANDLE hCard, CHAR* pDst)
{
	BYTE pbRecvBuffer[258];
	DWORD dwRecvLength;
	LONG rv;
	unsigned char cmd1[] = { 0x00, 0xA4, 0x00, 0x0C, 0x02, 0xDF, 0x01 };
	unsigned char cmd2[] = { 0x00, 0xA4, 0x00, 0x0C, 0x02, 0xC0, 0x00 };

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd1, sizeof(cmd1), NULL, pbRecvBuffer, &dwRecvLength);
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd2, sizeof(cmd2), NULL, pbRecvBuffer, &dwRecvLength);

	return ( ReadFile(pioSendPci, hCard, pDst) );

}


int CARDOS_PerformeSignature(SCARD_IO_REQUEST pioSendPci, SCARDHANDLE hCard)
{
	BYTE pbRecvBuffer[258];
	BYTE Hash[32 + 6];
	DWORD dwRecvLength;

	LONG rv;

	Hash[0] = 0x00;
	Hash[1] = 0x2A;
	Hash[2] = 0x9E;
	Hash[3] = 0x9A;
	Hash[4] = 32;

	for (int i = 0; i < 32; i++) Hash[5 + i] = toSign[i];
	Hash[37] = 0xFF;


	dwRecvLength = sizeof(pbRecvBuffer);
	unsigned char cmd1[] = { 0x00, 0xA4, 0x00, 0x0C, 0x02, 0xDF, 0x01 };
	unsigned char cmd2[] = { 0x00, 0x20, 0x00, 0x81, 0x08, 0x26, 0x12, 0x34, 0x56, 0xFF, 0xFF, 0xFF, 0xFF };

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd1, sizeof(cmd1), NULL, pbRecvBuffer, &dwRecvLength);

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&cmd2, sizeof(cmd2), NULL, pbRecvBuffer, &dwRecvLength);
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard, &pioSendPci, (unsigned char*)&Hash, sizeof(Hash), NULL, pbRecvBuffer, &dwRecvLength);

	int Bytesback = dwRecvLength - 2;
	if (Bytesback > 65) Bytesback = 65; // Fehler nur 64 wird bei mir als OK Signatur verwendet !!
	for (int i = 0; i < Bytesback; i++)  toSign[i] = pbRecvBuffer[i];
	return Bytesback;
}

int readcer(CHAR *pDst)
{
	LONG rv;
	SCARDCONTEXT hContext;
	char* mszReaders;
	SCARDHANDLE hCard;
	DWORD dwReaders, dwActiveProtocol;
	SCARD_IO_REQUEST pioSendPci;
	int back = 0;

	rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	if (rv != SCARD_S_SUCCESS) return 0;
	rv = SCardListReadersA(hContext, NULL, NULL, &dwReaders);
	if (rv != SCARD_S_SUCCESS) return 0;
	mszReaders = new char[dwReaders * sizeof(char)];
	rv = SCardListReadersA(hContext, NULL, mszReaders, &dwReaders);
	printf("reader name: %s\n", mszReaders);

	rv = SCardConnectA(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
	//	rv = SCardConnectA(hContext,  "Gemalto USB Smart Card Reader 0" , SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);

	switch (dwActiveProtocol)
	{
	case SCARD_PROTOCOL_T0:
		pioSendPci = *SCARD_PCI_T0;
		break;

	case SCARD_PROTOCOL_T1:
		pioSendPci = *SCARD_PCI_T1;
		break;
	}

	DWORD dwState, dwProtocol;
	BYTE atr[1024];
	DWORD atrsize = sizeof(atr);
	rv = SCardStatus(hCard, NULL, 0, &dwState, &dwProtocol, (LPBYTE)&atr, &atrsize);


	if (
		((sizeof(ACOS_04_ATR_1) <= atrsize) && (0 == memcmp(&atr, ACOS_04_ATR_1, sizeof(ACOS_04_ATR_1)))) ||
		((sizeof(ACOS_04_ATR_2) <= atrsize) && (0 == memcmp(&atr, ACOS_04_ATR_2, sizeof(ACOS_04_ATR_2))))
		)
	{
		printf("Card: ACOS04 !!! wird nicht unterstuetzt!\n");

		// ACOS_ReadCertificate(pioSendPci, hCard);
		back = 0;

	}
	else if ((sizeof(CARDOS_53_ATR) <= atrsize) && (0 == memcmp(&atr, CARDOS_53_ATR, sizeof(CARDOS_53_ATR))))
	{
		printf("Card: CARDOS\n");
		printf("read cert\n");
		back = CARDOS_ReadCertificate(pioSendPci, hCard, pDst);
	}

	rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
	delete[] mszReaders;
	rv = SCardReleaseContext(hContext);
	return back;
}

int APDU_Sign(char *phash)
{
	LONG rv;
	SCARDCONTEXT hContext;
	char* mszReaders;
	SCARDHANDLE hCard;
	DWORD dwReaders, dwActiveProtocol;
	SCARD_IO_REQUEST pioSendPci;
	int back = 0;
	for (int i = 0; i < 32; i++)  toSign[i] = phash[i] ;
	rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	if (rv != SCARD_S_SUCCESS) return 0;
	rv = SCardListReadersA(hContext, NULL, NULL, &dwReaders);
	if (rv != SCARD_S_SUCCESS) return 0;
	mszReaders = new char[dwReaders * sizeof(char)];
	rv = SCardListReadersA(hContext, NULL, mszReaders, &dwReaders);
	printf("reader name: %s\n", mszReaders); 

	rv = SCardConnectA(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
	//  oder Karte Direkt-Auswählen
	//	rv = SCardConnectA(hContext,  "Gemalto USB Smart Card Reader 0" , SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);

	switch (dwActiveProtocol)
	{
	case SCARD_PROTOCOL_T0:
		pioSendPci = *SCARD_PCI_T0;
		break;

	case SCARD_PROTOCOL_T1:
		pioSendPci = *SCARD_PCI_T1;
		break;
	}

	DWORD dwState, dwProtocol;
	BYTE atr[1024];
	DWORD atrsize = sizeof(atr);
	rv = SCardStatus(hCard, NULL, 0, &dwState, &dwProtocol, (LPBYTE)&atr, &atrsize);


	if (
		((sizeof(ACOS_04_ATR_1) <= atrsize) && (0 == memcmp(&atr, ACOS_04_ATR_1, sizeof(ACOS_04_ATR_1)))) ||
		((sizeof(ACOS_04_ATR_2) <= atrsize) && (0 == memcmp(&atr, ACOS_04_ATR_2, sizeof(ACOS_04_ATR_2))))
		)
	{
		printf("Card: ACOS04\n");
	//	ACOS_PerformeSignature(pioSendPci, hCard);
	//	ACOS_ReadCertificate(pioSendPci, hCard);
		back = 0;

	}
	else if ((sizeof(CARDOS_53_ATR) <= atrsize) && (0 == memcmp(&atr, CARDOS_53_ATR, sizeof(CARDOS_53_ATR))))
	{
		printf("Card: CARDOS\n");
		printf("sign data\n");
		back = CARDOS_PerformeSignature(pioSendPci, hCard);
	// 	strncpy( phash, toSign, 64);
		for (int i = 0; i < 64; i++) phash[i] = toSign[i] ;

	}

	rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
	delete[] mszReaders;
	rv = SCardReleaseContext(hContext);
	return back;
}


