@echo off

REM REGKP.Test 
REM REGKP braucht zwei steuerungs Dateien 
REM .\kpar.txt mit kassen-ID AES-Key und. Cer_Ser_Nr
REM und fr die einzelne Belege: .\bel2dep.txt
REM mit Beleg-Nummer, Beleg-Art, und die Einzelne Betr„ge in cent

rem ZUERST  System-Parameter in .\kpar.txt setzen
ECHO CASHBOX-DEMO-1 > kpar.txt
ECHO WQRtiiya3hYh/Uz44Bv3x8ETl1nrH6nCdErn69g5/lU= >>kpar.txt
ECHO 126e174a >>kpar.txt

rem und jetzt die Einzelne Belege erstellen und signieren
ECHO CASHBOX-DEMO-1-Receipt-ID0 > .\bel2dep.txt
ECHO START >>.\bel2dep.txt
ECHO 0 >>.\bel2dep.txt

regkp

ECHO CASHBOX-DEMO-1-Receipt-ID1 > .\bel2dep.txt
ECHO STAND >>.\bel2dep.txt
ECHO 200 >>.\bel2dep.txt
ECHO 100 >>.\bel2dep.txt


regkp


ECHO CASHBOX-DEMO-1-Receipt-ID2 > .\bel2dep.txt
ECHO STORNO >>.\bel2dep.txt
ECHO -200 >>.\bel2dep.txt
ECHO -100 >>.\bel2dep.txt

regkp

ECHO CASHBOX-DEMO-1-Receipt-ID3 > .\bel2dep.txt
ECHO STAND >>.\bel2dep.txt
ECHO 105 >>.\bel2dep.txt
ECHO 204 >>.\bel2dep.txt
ECHO 303 >>.\bel2dep.txt
ECHO 403 >>.\bel2dep.txt

regkp

REM ... usw ....

REM Certifikat und DEP ausgeben ...
regkp -XJ



