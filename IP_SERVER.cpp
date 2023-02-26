int _tmain(int argc, _TCHAR* argv[])
{
	int		b, l, on = 1;
	char	recvbuf[100], banforever='0'; 
	char	buf[100];
	SOCKET	s, sa;
	struct	sockaddr_in channel;  // holds IP address
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	int		bytesRecv;
	unsigned char MACData[6];
	char HostNameData[20];
	char RemoteIP[17];
	char ban_status;
	char User_Name[20]={0};
	char add_ban;
	char BanReason[30]={0};
	int i;
	SYSTEMTIME st;


	printf(" \n");
	printf("-------------------------------------------------------------- \n");
	printf(" \n");
	printf(" === Anti-Hack Server by RoiLopez === \n");
	printf(" \n");
	//GetMACaddress();
	ReadInis(); // Load Config
	printf(" \n");
	printf(" Users Connections : \n");
	printf(" \n");


	InitLogFile();

	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
		printf("WSAStartup error %ld", WSAGetLastError() );
		WSACleanup();
		return false;
	}
	//------------------------------------------------------

	                

	//---- Build address structure to bind to socket.--------  
	memset(&channel, 0, sizeof(channel));// zerochannel 
	channel.sin_family = AF_INET; 
	channel.sin_addr.s_addr = htonl(INADDR_ANY); 
	channel.sin_port = htons(SERVER_PORT); 
	//--------------------------------------------------------

	// keep track of the number of connections
	int numConnections = 0;

	// ---- create SOCKET--------------------------------------
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
	if (s < 0) {
		printf("socket error %ld",WSAGetLastError() );
		WSACleanup();
		return false;
	}

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)); 
	//---------------------------------------------------------

	//---- BIND socket ----------------------------------------
	b = bind(s, (struct sockaddr *) &channel, sizeof(channel)); 

	if (b < 0) {
		printf("bind error %ld", WSAGetLastError() ); 
		WSACleanup();
		return false;
	}
	//----------------------------------------------------------


	//---- LISTEN socket ----------------------------------------
	l = listen(s, QUEUE_SIZE);                 // specify queue size 
	if (l < 0) {
		printf("listen error %ld",WSAGetLastError() );
		WSACleanup();
		return false;
	}
	//-----------------------------------------------------------
	while (1) {
	//---- ACCEPT connection ------------------------------------
	//sa = accept(s, 0 , 0);                  // block for connection request  
	int clientAddrLen = sizeof(channel);
	sa = accept(s, (SOCKADDR*)&channel, &clientAddrLen);

	if (sa < 0) {
		printf("accept error %ld ", WSAGetLastError() ); 
		WSACleanup();
		return false;
	}
	//------------------------------------------------------------
	// Socket is now set up and bound. Wait for connection and process it. 
	
	//---- RECV bytes --------------------------------------------
	bytesRecv = recv( sa, recvbuf, sizeof(recvbuf), 0 );

	//err = WSAGetLastError( );// 10057 = A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) 
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET  || bytesRecv == INVALID_SOCKET) {
      printf( "Connection Closed.\n");
	  WSACleanup();
	  return false;

    }

	//------------------------------------------------------------
	// START OF DEFINE DATA RECV!!!!!!!!!!!
	//---------------------------------------------------------------


	for(i=0;i<=5;i++){
		MACData[i]=recvbuf[i];
	}
	for(i=6;i <= 20 ; i++){
		HostNameData[i-6]=recvbuf[i];	
	}
	for(i=21;i<=37;i++){
		RemoteIP[i-21]=recvbuf[i];
		ban_status=recvbuf[38];				//SAVE BAN STATUS
	}


	for(i=39; i <= 59 ; i++){
		User_Name[i-39]=recvbuf[i];	//SAVE USERNAME
	}

	if (strcmp(User_Name,"") == 0){
		strcpy(User_Name,"No_Name");
	}


	for(i=61; i <= 90 ; i++){
		BanReason[i-61]=recvbuf[i]; //SAVE USERNAME
	}				

	//BanReason = 1 = Memory_scan , 2 = Title Scan , 3 = Wrond Client

	add_ban=recvbuf[60];	//SAVE ADD BAN
	GetLocalTime(&st);


	//if(didlogonscr==1 || add_ban=='1'){
	if(add_ban=='1' || Check_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban) ==  '1'){


		if(Check_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban) ==  '1'){
		printf(" -User Blocking : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)
		
		}else if(bytesRecv > 0){
		printf(" -System Block User : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)
		}


	}else{
	printf(" -User connected : (%s), Date: %02d/%02d/%02d, Time: %02d:%02d:%02d, \n",RemoteIP, st.wDay , st.wMonth , st.wYear,st.wHour, st.wMinute, st.wSecond); //, //BanReason//ban_status)	
	}


	if(didlogtxt == 1 ){
	PrintLog(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban);
	}

	if (add_ban=='1'){
		Print_Ban_Logs(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban,BanReason);

		if(BanReason[0] != '2' || didbanname == 1 ){
			Print_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban,BanReason);
			buf[0]='3';	
		}

		else{
			buf[0]='5';	
		}
	}

	else{
		buf[0]=Check_Ban(HostNameData , RemoteIP , MACData , User_Name , ban_status , add_ban);
	}
	

	
	if (recvbuf[99]=='9'){
		exit(0);
	}

	if (recvbuf[98]=='9'){
		banforever='1';
	}


	buf[1]=ServerVers[0];
	buf[2]=ServerVers[1];
	buf[3]=AutoBanTime;

	if (banforever=='1')
	buf[0]='1'; 

	bytesRecv = send( sa, buf, 100, 0 ); 
	closesocket( sa );
	}




	closesocket( s );
	WSACleanup();
	return 0;
}
