!! Curl Script, Steuerung: 2 Zeichen am Zeilenanfang
!! Kommentar, >> Out, << In, @@ Exec,  ?? IF, ?! IF not ...



>> login.xml
 
 <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ses="https://finanzonline.bmf.gv.at/fon/ws/session">
 <soapenv:Header/>
   <soapenv:Body>
      <ses:loginRequest>
         <ses:tid>(%%6)</ses:tid>
         <ses:benid>(%%7)</ses:benid>
         <ses:pin>(%%8)</ses:pin>
         <ses:herstellerid>ATU31537101</ses:herstellerid>
!!     ACHTUNG unsere UID  -----------  NICHT VERWENDEN in Echtbetrieb
      </ses:loginRequest>
   </soapenv:Body>
 </soapenv:Envelope>
 
!! nach erste Leerzeile,  >> Ausgabe erstellt 
!! starte curl, -k -s -S  ( -k  https ohne zert., -s silent, -S aber Errormeldungen doch)
 
@@ curl -k -s -S https://finanzonline.bmf.gv.at/fonws/ws/session -o step1.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: login" -d @login.xml

<< step1.txt <ns5:id> <ns5:rc>

?! <ns5:rc> 0 lasses

>> prubel.xml

 <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:rkdb="https://finanzonline.bmf.gv.at/rkdb">
   <soapenv:Header/>
   <soapenv:Body>
      <rkdb:rkdbRequest>
         <rkdb:tid>(%%6)</rkdb:tid>
         <rkdb:benid>(%%7)</rkdb:benid>
         <rkdb:id>(%%v<ns5:id>)</rkdb:id>
         <rkdb:art_uebermittlung>P</rkdb:art_uebermittlung>
         <rkdb:rkdb>
            <rkdb:paket_nr>1</rkdb:paket_nr>
            <rkdb:ts_erstellung>(%%z)</rkdb:ts_erstellung>
            <rkdb:belegpruefung>
               <rkdb:satznr>1</rkdb:satznr>
               <rkdb:beleg>(%%i lastqr.txt)</rkdb:beleg>
           </rkdb:belegpruefung>
         </rkdb:rkdb>
      </rkdb:rkdbRequest>
   </soapenv:Body>
 </soapenv:Envelope>

@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o step2.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: rkdb" -d @prubel.xml

<< step2.txt <ns5:verificationState>

?! <ns5:verificationState> FAIL

>>> 

 Hello
 Ergebniss: (%%v<ns5:verificationState>)


!?! 

>> logout.xml FONlogout.tmp 
 
@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o stepl.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: logout" -d @logout.xml


