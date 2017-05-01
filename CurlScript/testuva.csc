!! curl script  UID Abfrage
!! leider webservice Benutzer darf nicht gleich Reg.Kassen User sein!!!
!! sonst hätte ich ...

>> login.xml 
  
 <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ses="https://finanzonline.bmf.gv.at/fon/ws/session">
 <soapenv:Header/>
   <soapenv:Body>
      <ses:loginRequest>
         <ses:tid>61779540b308</ses:tid>
         <ses:benid>FNWS2017</ses:benid>
         <ses:pin>WSBEN2017</ses:pin>
         <ses:herstellerid>ATU31537101</ses:herstellerid>
      </ses:loginRequest>
   </soapenv:Body>
 </soapenv:Envelope>
 
@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o step1.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: login" -d @login.xml

<< step1.txt <ns5:id> <ns5:rc>

?! <ns5:rc> 0 lasses

>> upuva.xml

 <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:fil="https://finanzonline.bmf.gv.at/fon/ws/fileupload">
   <soapenv:Header/>
   <soapenv:Body>
      <fil:fileuploadRequest>
         <fil:tid>61779540b308</fil:tid>
         <fil:benid>FNWS2017</fil:benid>
         <fil:id>(%%v<ns5:id>)</fil:id>
         <fil:art>U30</fil:art>
         <fil:uebermittlung>T</fil:uebermittlung>
         <fil:data><![CDATA[<?xml version="1.0" encoding="iso-8859-1"?>
(%%i 03_fdt03.xml)
         ]]>
      </fil:data>
      </fil:fileuploadRequest>
   </soapenv:Body>
 </soapenv:Envelope>


@@ curl --insecure https://finanzonline.bmf.gv.at/fon/ws/fileupload -o step2.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: upload" -d @upuva.xml

<< step2.txt <ns5:rc>  <ns5:msg> 

?! <ns5:rc> 0 lasses

>>> 





 UID OK!
 (%%v<ns5:name>) 
 (%%v<ns5:adrz1>) 
 (%%v<ns5:adrz2>) 




>> logout.xml 

<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ses="https://finanzonline.bmf.gv.at/fon/ws/session">
   <soapenv:Header/>
   <soapenv:Body>
      <ses:logoutRequest>
         <ses:tid>61779540b308</ses:tid>
         <ses:benid>FNWS2017</ses:benid>
         <ses:id>(%%v<ns5:id>)</ses:id>
      </ses:logoutRequest>
   </soapenv:Body>
</soapenv:Envelope> 
 
@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o stepl.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: logout" -d @logout.xml


