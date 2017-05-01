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
         <ses:pin>..........</ses:pin>
         <ses:herstellerid>ATU31537101</ses:herstellerid>
      </ses:loginRequest>
   </soapenv:Body>
 </soapenv:Envelope>
 
@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o step1.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: login" -d @login.xml

<< step1.txt <ns5:id> <ns5:rc>

?! <ns5:rc> 0 lasses

>> pruid.xml

 <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:uid="https://finanzonline.bmf.gv.at/fon/ws/uidAbfrage">
   <soapenv:Header/>
   <soapenv:Body>
      <uid:uidAbfrageServiceRequest>
         <uid:tid>61779540b308</uid:tid>
         <uid:benid>FNWS2017</uid:benid>
         <uid:id>(%%v<ns5:id>)</uid:id>
         <uid:uid_tn>ATU69996089</uid:uid_tn>
         <uid:uid>ATU31537101</uid:uid>
         <uid:stufe>2</uid:stufe>
      </uid:uidAbfrageServiceRequest>
   </soapenv:Body>
 </soapenv:Envelope>

@@ curl --insecure https://finanzonline.bmf.gv.at/fon/ws/uidAbfrage -o step2.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: uidAbfrage" -d @pruid.xml

<< step2.txt <ns5:rc>  <ns5:name> <ns5:adrz1> <ns5:adrz2>

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


