@echo off

rem REGKP.Test (START-Beleg) 
rem wir brauchen zwei steuerungs Dateien 
rem .\kpar.txt mit kassen-ID AES-Key und. Cert_Ser_Nr

echo CASHBOX-DEMO-1 > kpar.txt
echo WQRtiiya3hYh/Uz44Bv3x8ETl1nrH6nCdErn69g5/lU= >>kpar.txt
echo 126e174a >>kpar.txt

rem und fÅr die einzelne Belege: .\bel2dep.txt
rem mit Beleg-Nummer, Beleg-Art, und die einzelne BetrÑge in Cent

echo CASHBOX-BELEG-ID0 > .\bel2dep.txt
echo START >>.\bel2dep.txt
echo 0 >>.\bel2dep.txt

rem signieren
REGKP.EXE

rem und die *.json Dateien fÅr das java Prueftool exportieren ...
REGKP.EXE -xj

PAUSE
rem java prueftool starten
copy *.json C:\java\regkassen-demo-1.0.0
cd C:\java\regkassen-demo-1.0.0
java -jar regkassen-verification-depformat-1.0.0.jar -i dep-export.json -c crypto.json -o TestOUT


