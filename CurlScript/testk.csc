!! curl script

>> login.xml FONlogin.tmp 
 
@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o step1.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: login" -d @login.xml

<< step1.txt <ns5:id> <ns5:rc>

?! <ns5:rc> 0 lasses

>> prubk.xml

 <soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:rkdb="https://finanzonline.bmf.gv.at/rkdb">
   <soapenv:Header/>
   <soapenv:Body>
      <rkdb:rkdbRequest>
         <rkdb:tid>(%%6)</rkdb:tid>
         <rkdb:benid>(%%7)</rkdb:benid>
         <rkdb:id>(%%v<ns5:id>)</rkdb:id>
         <rkdb:art_uebermittlung>P</rkdb:art_uebermittlung>
         <rkdb:status_kasse>
            <rkdb:paket_nr>1</rkdb:paket_nr>
            <rkdb:ts_erstellung>(%%z)</rkdb:ts_erstellung>
            <rkdb:satznr>1</rkdb:satznr>
            <rkdb:kassenidentifikationsnummer>RegK-IBK-D9C</rkdb:kassenidentifikationsnummer>
         </rkdb:status_kasse>
      </rkdb:rkdbRequest>
   </soapenv:Body>
 </soapenv:Envelope>

@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o step2.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: rkdb" -d @prubk.xml

<< step2.txt <ns5:rc> <status> <ts_status>

?! <ns5:rc> 0 lasses

>>> 

 Hello
 Ergebniss: (%%v<status>) UND???
        TS: (%%v<ts_status>) 


!?!  anykey

>> logout.xml FONlogout.tmp 
 
@@ curl --insecure https://finanzonline.bmf.gv.at/fonws/ws/session -o stepl.txt -H "Content-Type: text/xml;charset=UTF-8" -H "SOAPAction: logout" -d @logout.xml


