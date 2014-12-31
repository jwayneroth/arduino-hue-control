
/* 
 * timedRead
*/
char timedRead(Adafruit_CC3000_Client& client) {
	unsigned long _startMillis = millis();
	char c;
	do {
		c = client.read();
		if (c >= 0) return c;
	} while(millis() - _startMillis < 1000);//_timeout);
	return -1;     // -1 indicates timeout
}

/*
 * findUntil
*/
bool findUntil( Adafruit_CC3000_Client& client, char *target, char *terminator ) {
	
	size_t targetLen = strlen(target);
	size_t termLen = strlen(terminator);
	
	size_t index = 0;  // maximum target string length is 64k bytes!
	size_t termIndex = 0;
	char c;
	
	//Serial.print("targetLen : ");Serial.println(targetLen);
	
	if( *target == 0) return true;   // return true if target is a null string
	
	while(( c = timedRead(client) ) > 0 ) {
		
		//Serial.print(c);
		
		if(c != target[index]) {
			//Serial.println("no match");
			index = 0; // reset index if any char does not match
		}
		
		if( c == target[index]) {
			//Serial.print("found ");Serial.print(target[index]);Serial.print(" at ");Serial.println(index);
			if(++index >= targetLen){ // return true if all chars in the target match
				return true;
			}
		}
	
		if(termLen > 0 && c == terminator[termIndex]){
			if(++termIndex >= termLen) {
				Serial.println("hit terminator");
				return false;       // return false if terminate string found before target string
			}
		} else {
			termIndex = 0;
		}
	}
	
	return false;
	
}

/*
 * readStringUntil
*/
String readStringUntil( Adafruit_CC3000_Client& client, char terminator) {
  String ret;
  int c = timedRead(client);
  while (c >= 0 && c != terminator)
  {
    ret += (char)c;
    c = timedRead(client);
  }
  return ret;
}

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

/*
 * wifiConnection
*/
int wifiConnection() {
	  
  // Initialise the module
  Serial.println(F("\nInitializing..."));
  
  if (!cc3000.begin()) {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    return 0; //while(1);
  }
  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    return 0; //while(1);
  }
   
  Serial.println(F("Connected!"));
  
  //Serial.println(F("Request DHCP")); // Wait for DHCP to complete 
  while (!cc3000.checkDHCP()) { delay(100); } // ToDo: Insert a DHCP timeout!  

  // Display the IP address DNS, Gateway, etc. 
  //while (! displayConnectionDetails()) { delay(1000); }
  
  return 1;
}