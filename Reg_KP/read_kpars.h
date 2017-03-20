
void dezZahl(int n, CHAR* sloc)
// Konvertieren von Cent nach Euro immer zwei nachkomma
// zb: 17 > 0,17  -1234 > -12,34   0 > 0,00 
{
	char svor[9], snach[5];
	int  iLoc;

	iLoc = (abs(n));
	_itoa(iLoc / 100, svor, 10);
	_itoa(iLoc % 100, snach, 10);

	if (n < 0) {
		strcpy(sloc, "-");
		strcat(sloc, svor);
	}
	else strcpy(sloc, svor);
	strcat(sloc, ",");
	if ((iLoc % 100) < 10) strcat(sloc, "0");
	strcat(sloc, snach);

}

char sB64_AES[64];

int readpars(char welche)
{

	FILE *myKpar;
	char fname[64];
	char spar1[32], spar2[32], spar3[32], spar4[32], spar5[32];
	char mylongline[1024];

	switch (welche) {
	case 'L':  // Letzte Beleg
		strcpy(fname, ".\\klastbel.txt");
		break;
	case 'B':  // Beleg Daten
		strcpy(fname, ".\\bel2dep.txt");
		break;
	case 'P':  // Kassa Parameter
		strcpy(fname, ".\\kpar.txt");
		break;
	default:
		printf("Falsche Parameter... \r\n");
		return 0;
	};


	errno_t err = fopen_s(&myKpar, fname, "r");

	if (err) {
		printf("Fehler beim oeffnen: %s!\r\n", fname);
		return 0;
	};

	int anzread;
	anzread = fread(mylongline, sizeof(char), 1000, myKpar);
	if (anzread < 10) {
		printf("Fehler beim lesen: %s!\r\n", fname);
		goto Cleanup;
	};
	mylongline[anzread] = '\0';

	switch (welche) {
	case 'L':  // Letzte Beleg-Daten (wird von Programm selbst erstellt)
		anzread = sscanf(mylongline, "%15s %12s %9s %9s ", Sig_vor_Beleg, spar1, spar2, spar3, spar4);
		if (anzread > 2) {
			i64KStand = _atoi64(spar1);
			if (anzread > 3) Lastcount = atoi(spar2);
			if (anzread > 4) LastTag = atoi(spar3);
			goto  BackOK;
		};

		break;
	case 'B':  //Beleg  Daten     "bpar.txt"
		anzread = sscanf(mylongline, "%35s %7s %9s %9s %9s %9s %9s", Beleg_NR, Beleg_ART, spar1, spar2, spar3, spar4, spar5);
		if (anzread > 2) {
			iBetr_N = atoi(spar1);
			if (anzread > 3) iBetr_E1 = atoi(spar2);
			if (anzread > 4) iBetr_E2 = atoi(spar3);
			if (anzread > 5) iBetr_0  = atoi(spar4);
			if (anzread > 6) iBetr_S  = atoi(spar5);
			goto  BackOK;
		};
		break;
	case 'P':  //Kassa  Parameter "kpar.txt"
		anzread = sscanf(mylongline, "%35s %60s %21s", Kassen_ID, fname, Zert_SerNr);
		if (anzread == 3) {
			strcpy(sB64_AES, fname);
			anzread = FromBase64Crypto(fname, (strlen(fname)), AES_KEY, 33);
			if ((strlen(AES_KEY)) == 32) goto  BackOK;
			printf("AES KEY Check Fehler: %s!\n", fname);
		};
	};


Cleanup:
	fclose(myKpar);
	return 0;
BackOK:
	fclose(myKpar);
	return 1;
}
