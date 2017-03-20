# Reg_KP
Registrierkassen Prüftool
nach österreichischen [Registrierkassensicherheitsverordnung (RKSV)](https://www.bmf.gv.at/top-themen/Registrierkassen.html#heading_Folder__Informationen_zur_Registrierkassen__und_Belegerteilungspflicht) 


# Muster-Code

In diesem Projekt stelle ich mein C Source.Code (Visual Studio 2013) zur Verfügung 

Verwendete Tools

* APDU winscard.lib
* MS. Cryptography API: Next Generation (CNG) bcrypt.lib

# EXE 
und auch das fertige EXE Programm ist dabei, sofort ersatzbereit zu erstellen und signieren von Kassa-Belegen nach RKSV

WICHTIG: Beachten Sie, dass wir für Freeware keine Garantie und keinen Support leisten! Jegliche Haftung wird ausgeschlossen, die Verwendung erfolgt auf eigenes Risiko! Mit dem Download akzeptieren Sie die Lizenzbestimmung

# Erstellen von Start Beleg

wir brauchen zwei steuerungs Dateien 
* .\kpar.txt  mit kassen-ID AES-Key und. Cert_Ser_Nr
* .\bel2dep.txt  mit Beleg-Nummer, Beleg-Art, und die einzelne Beträge in Cent

die durch aufruf von REGKP.EXE signiert werden, dabei werden die Dateien (klastbel.txt,deplog.txt) erstell


mit REGKP.EXE -xj  werden die json Dateien exportiert (dep-export.json, crypto.json) 

mit "REGKP -xc" kann das Zertifikat auf .\cert.hex ausgeben um zB. bei https://certlogik.com/decoder/  zu testen.
