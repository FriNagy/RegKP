
// RegKP Registrierkassen - Konverter nach RKSV v. 1.2 siehe: 
// https://github.com/a-sit-plus/at-registrierkassen-mustercode
//
// Test-Software ohne Gewähr
// Erstellt f.nagy siehe atkassa.at

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

 char Kassen_ID[36], Zert_SerNr[36], AES_KEY[36], Beleg_NR[36], Beleg_ART[36], Sig_vor_Beleg[36], Lastart[9];
 int   iBetr_N=0, iBetr_E1=0, iBetr_E2=0, iBetr_0=0, iBetr_S=0, Lastcount=0, LastTag=0;
 _int64 i64KStand = 0;


 #include "wins_APDU.h"    // Card-Reader Funktionen von A-Trust ?id=114

 #include "wins_BCRYPT.h"  // Microsoft-Crypt Funktionen Hash und Crypt ...
                           // https://msdn.microsoft.com/en-us/library/windows/desktop/aa376234(v=vs.85).aspx

 #include "read_kpars.h"   // lade kassen-Parameter von Text-Dateien
                           // kpar.txt    <  Kassen_ID,  AES_KEY,   Zert_SerNr
                           // bel2dep.txt <  Beleg_NR,   Beleg_ART, iBetr_N ... iBetr_S
                           // lastbel.txt <  Sig_vor_Beleg, Lastcount, LastTag 



void Fehler(char *s)  // Stop Programm und Exit
{
	printf("Ein Fehler ist aufgetreten ...\n");
	printf("%s\n", s);
	printf("Error #: %x\n.", GetLastError());
	exit(1);
}


// -----------------------------------------

int main(int argc, CHAR* argv[])
{
	
	union myBWert  {   // Umsatzzähler 
		__int64   longwert;
		BYTE      barray[8];
	               };
	myBWert bUmsatzZ;   

	char cryptded_uz[64] = " ", sDate[16], sTime[16], sZwi[64];
	char myMsg[4024];   
	FILE *sF1;
	int len;
	boolean addsumm = 1;

	char stemp[512];
	char sTurnOverValue[64];
	char JWS_Payload[640];
	char toBeSignedStr[1024];

	char myMsg2[4024];

	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(sDate, 16, "%04i-%02i-%02i", st.wYear, st.wMonth, st.wDay);
	sprintf_s(sTime, 16, "%02i:%02i:%02i", st.wHour, st.wMinute, st.wSecond);
	printf("RegKP... version 0.99 siehe www.atkassa.at \n");

	for (int i = 0; i < argc; i++) {

		if (!_strnicmp(argv[i], "-XC", 3)) {  // Export Certificate
			printf("cert.hex exportieren...\n");
			len = (readcer(myMsg));
			if (len) {
				FILE *fp;
				fp = fopen(".\\cert.hex", "w");
				if (fp) {
					len = ToBase64Crypto(myMsg, len, myMsg2, 4000);
					fwrite(myMsg2, 1, len, fp);
					fclose(fp);
				}
				exit(0);
			};
		};

		if (!_strnicmp(argv[i], "-XJ", 3)) {  // Export DEP nach JSON */
			printf("json export ...\n");
			len = (readcer(myMsg));
			if (len) {

				if (!readpars('P')) Fehler(".\\KPAR.TXT Fehlerhaft");
				
				FILE *fp;
				fp = fopen(".\\crypto.json", "w");
				if (fp) {
					fputs("{\n \"base64AESKey\" : \"", fp);
					fputs(sB64_AES, fp);
					fputs("\",\n \"certificateOrPublicKeyMap\" : {\n  \"", fp);
					fputs(Zert_SerNr, fp);
					fputs("\" : {\n     \"id\" : \"", fp);
					fputs(Zert_SerNr, fp);
					fputs("\", \n     \"signatureDeviceType\" : \"CERTIFICATE\",\n", fp);
					fputs("     \"signatureCertificateOrPublicKey\" : \"", fp);
					len = ToBase64Crypto(myMsg, len, myMsg2, 4000);
					fwrite(myMsg2, 1, len, fp);
					fputs("\"\n } } }\n", fp);
					fclose(fp);
					printf(".\\crypto.json mit %d Byte exportiert\n", len);
				};


				fp = fopen(".\\dep-export.json", "w");

				if (fp) {
					FILE *fpin;

					int count = 0;
					fputs("{\n \"Belege-Gruppe\" :[{ \n  \"Signaturzertifikat\" : \"\",\n  \"Zertifizierungsstellen\" : [], \n   \"Belege-kompakt\" : [ \n", fp);
					fpin = fopen(".\\deplog.txt", "r");
					if (fpin) {

						while (fgets(myMsg2, 1000, fpin) != 0) {
							len = (strlen(myMsg2));
							if (myMsg2[len] == '\n') myMsg2[len] = '\0';
							if (myMsg2[len - 1] == '\n') myMsg2[len - 1] = '\0';
							if (len > 100) {
								if (count) fputs("\",\n", fp);
								fputs("\"", fp);
								fputs(myMsg2, fp);
								count++;
							};
						};
						if (count) fputs("\"\n", fp);

						fputs("\n ] } ] }\n", fp);

					};

					fclose(fp);
					fclose(fpin);

					printf("%d Zeilen nach .\\dep-export.json exportiert...\n", count);

				};
			}

			exit(0);
		}; // -XJ


	}; // loop






	// jetzt geht's los
	// ------------------------------------

	if (!readpars('P')) Fehler(".\\KPAR.TXT Fehlerhaft"); 
	if (!readpars('B')) Fehler(".\\BEL2DEP.TXT  Fehlerhaft");
	
	if (!_strnicmp(Beleg_ART, "START", 5)) {                     
		// START Beleg, also neuanfang  >>> Variablen auf init-Wert (0) lassen
		// Sig-Voriger-Beleg berechnen (siehe Prozess 2.4.1)
		if (!hash_SHA256((BYTE *)Kassen_ID, (strlen(Kassen_ID)), sTurnOverValue)) Fehler("hashstart");
		ToBase64Crypto(sTurnOverValue, 8, Sig_vor_Beleg, 16);
		goto BelartOK;
	};

	// wenn kein START-Beleg muss last-Beleg vorhanden sein,  >>> Variablen laden
	if (!readpars('L')) Fehler(".\\KLASTBEL.TXT Fehlerhaft"); 

	Lastcount++;



	if (!_strnicmp(Beleg_ART, "NUL", 3)) {
		printf("NULL Beleg... #%d\n", Lastcount);
		// NULL Beleg
		iBetr_N = 0; iBetr_E1 = 0; iBetr_E2 = 0; iBetr_0 = 0; iBetr_S = 0;
		goto BelartOK;
	};

	if (!_strnicmp(Beleg_ART, "STO", 3)) {
		printf("STORNO Beleg... #%d\n", Lastcount);
		// Storno Beleg
		strcpy(cryptded_uz, "U1RP");    // "STO" Base64 kodiert
		goto BelartOK;
	};

	if (!_strnicmp(Beleg_ART, "TRA", 3)) {
		printf("TRAINING Beleg... #%d\n", Lastcount);
		// Trainigsbeleg, 
		strcpy(cryptded_uz, "VFJB");   // "TRA" Base64 kodiert
		addsumm = 0;
		goto BelartOK;
	};

	// und sonst, darf es nun mehr Standard Beleg sein
	if (_strnicmp(Beleg_ART, "STA", 3)) Fehler("BELEGART UNGÜLTIG!");  // nur START, NUL,STO,TRA,STA sind zulässig
	printf("STANDARD Beleg... #%d\n", Lastcount);

 BelartOK:



	// Kassen-Stand neu berechnen
	if (addsumm) i64KStand = i64KStand + iBetr_N + iBetr_E1 + iBetr_E2 + iBetr_0 + iBetr_S;

	if ((strlen(cryptded_uz)) < 3 ) {  // Kassen-Zähler verschlüsseln  (siehe Process: 2.5.4)
		strcpy(stemp, Kassen_ID);  strcat(stemp, Beleg_NR);
		if (!hash_SHA256((BYTE *)stemp, (strlen(stemp)), stemp)) Fehler("hash1");
	    // IV in ECB Modus verschlüsseln
		if (!bcrypt_AES_ECB((BYTE *)stemp, 16, AES_KEY, stemp)) Fehler("cryptmee");
      	// danach XOR mit Umsatzzähler
		// wegen little/big endian wechsel in umgekehrten Byte reihenfolge [7-i]
		bUmsatzZ.longwert = i64KStand;
		for (int i = 0; i < 8; i++) sZwi[i] = bUmsatzZ.barray[7 - i] ^ stemp[i];
		ToBase64Crypto(sZwi, 8, cryptded_uz, 16);
     }

    // Variablen sind gesezt, nun Maschinenlesbare Code erstellen
	
	 strcpy(myMsg, "_R1-AT1_");
	 strcat(myMsg, Kassen_ID); strcat(myMsg, "_");
	 strcat(myMsg, Beleg_NR);  strcat(myMsg, "_");
	 strcat(myMsg, sDate);     strcat(myMsg, "T");
	 strcat(myMsg, sTime);     strcat(myMsg, "_");

	 dezZahl(iBetr_N, sZwi);  //  Beträge
	 strcat(myMsg, sZwi);         strcat(myMsg, "_");
	 dezZahl(iBetr_E1, sZwi);     strcat(myMsg, sZwi); strcat(myMsg, "_");
	 dezZahl(iBetr_E2, sZwi);     strcat(myMsg, sZwi); strcat(myMsg, "_");
	 dezZahl(iBetr_0,  sZwi);     strcat(myMsg, sZwi); strcat(myMsg, "_");
	 dezZahl(iBetr_S,  sZwi);     strcat(myMsg, sZwi); strcat(myMsg, "_");
	 strcat(myMsg, cryptded_uz);  strcat(myMsg, "_");
	 strcat(myMsg, Zert_SerNr);   strcat(myMsg, "_");
	 strcat(myMsg, Sig_vor_Beleg);

	 ToBase64Cryptourl(myMsg, (strlen(myMsg)), JWS_Payload, 600);

	 strcpy(toBeSignedStr, "eyJhbGciOiJFUzI1NiJ9"); // = Base64url("{"alg":"ES256"}");
	 strcat(toBeSignedStr, ".");
	 strcat(toBeSignedStr, JWS_Payload);

	 char sSignVar[166];

	 if (!hash_SHA256((BYTE *)toBeSignedStr, (strlen(toBeSignedStr)), sSignVar)) Fehler("JWS Hash");
	 
	 if (APDU_Sign(sSignVar) == 64) {  // signmee liefert in sSignVar das Signatur zurück (wenn OK)
		 ToBase64Cryptourl(sSignVar, 64, stemp, 300);
	 }
	 else {
		 strcpy_s(toSign, "Sicherheitseinrichtung ausgefallen");
		 ToBase64Cryptourl(toSign, (strlen(toSign)), stemp, 300);
	 };
	 
	 strcat(toBeSignedStr, ".");
	 strcat(toBeSignedStr, stemp);
	 
	 if (Lastcount)  fopen_s(&sF1, ".\\deplog.txt", "a+"); // anhängen
	       	 else    fopen_s(&sF1, ".\\deplog.txt", "w+"); // neu erstellen

	 fputs(toBeSignedStr, sF1); 
	 fputc('\n', sF1);
	 fclose(sF1);
	 

	 if (!hash_SHA256((BYTE *)toBeSignedStr, (strlen(toBeSignedStr)), toSign)) Fehler("JWS2");
	 ToBase64Crypto(toSign, 8, Sig_vor_Beleg, 300);

	 fopen_s(&sF1, ".\\siglog.txt", "a+");
	 _i64toa(i64KStand, sZwi, 10);  // Kassenstand
	 fprintf(sF1, "#:%d %s %s %s\n", Lastcount, Beleg_ART, sZwi, Sig_vor_Beleg);
	 fputs(myMsg, sF1);  fputc('\n', sF1);
	 fputs(stemp, sF1); fputc('\n', sF1);  // nur Signatur
	 fclose(sF1);
	 
	 fopen_s(&sF1, ".\\klastbel.txt", "w");
	 fprintf(sF1, "%s\n", Sig_vor_Beleg);
	 _i64toa(i64KStand, stemp, 10);  // Kassenstand
	 fprintf(sF1, "%s\n", stemp);
	 _itoa(Lastcount, stemp, 10);    // mein counter
	 fprintf(sF1, "%s\n", stemp);
	 fprintf(sF1, "%d\n", LastTag);
	 fclose(sF1);
	 printf("OK... \n\n");

	 return 0;
}

