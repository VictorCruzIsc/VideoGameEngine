#include "stdafx.h"
#include "NetProcessor.h"


unsigned long CNetProcessor::OnEvent(CEventBase * pEvent)
{
	return 0;
}

bool CNetProcessor::InitNetwork(void)
{
	WSAStartup(MAKEWORD(2, 2), &m_wsa);
	DWORD dwThreadId;
	SOCKADDR_IN Service;
	m_Listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	InetPton(AF_INET, L"0.0.0.0", &Service.sin_addr);
	Service.sin_port = htons(6112);
	Service.sin_family = AF_INET;
	bind(m_Listener, (SOCKADDR*)&Service, sizeof(SOCKADDR_IN));
	listen(m_Listener, SOMAXCONN);
	CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ServiceListener, this,0,&dwThreadId);
	return false;
}

DWORD CNetProcessor::ServiceListener(CNetProcessor * pNP)
{
	while (1) {
		DWORD dwThread;
		SOCKADDR_IN ClientAddress;
		int nClientAddrSize = 0;
		SOCKET Client = accept(
			pNP->m_Listener,
			(SOCKADDR*)&ClientAddress,
			&nClientAddrSize);
		CLIENT &NewClient = *new CLIENT;
		NewClient.Client = Client;
		NewClient.NickName = "Anonimo"; // Recomendado una vez aceptado el juego, el siguiente paquete pide datos
		NewClient.pOwner = pNP;
		pNP->Lock(); // Abro la seccion critica para agregar clientes al mapa
		pNP->m_mapClients.insert_or_assign(Client, NewClient); // Cliente registrado
		pNP->UnLock(); // Cierro la seccion critica

		// Hago el hilo que atiende a este cliente
		CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ClientThread, &NewClient, 0, &dwThread);
	}
	return 0;
}

DWORD CNetProcessor::ClientThread(CLIENT* pClient){
	while (1) {
		CEventNetwork* pEventNetwork = new CEventNetwork;
		pEventNetwork->m_ulEventType = EVENT_NETWORK;
		if (SOCKET_ERROR == recv(pClient->Client,
			(char*)pEventNetwork->nDataSize,
			sizeof(unsigned long), 0)) {
			delete pEventNetwork;
			break;
		}
		if (SOCKET_ERROR == recv(pClient->Client,
			(char*)pEventNetwork->m_Data,
			pEventNetwork->nDataSize, 0)) {
			delete pEventNetwork;
			break;
		}
		pClient->pOwner->Lock();
		pClient->pOwner->m_lstEventsReceived.push_back(pEventNetwork);
		pClient->pOwner->UnLock();
	}
	// Unsuscript from the client map
	pClient->pOwner->Lock();
	pClient->pOwner->m_mapClients.erase(pClient->Client);
	closesocket(pClient->Client);
	pClient->pOwner->UnLock();
	delete pClient;
	return 0;
}

CNetProcessor::CNetProcessor()
{
	InitializeCriticalSection(&m_csLock);
}


CNetProcessor::~CNetProcessor()
{
	DeleteCriticalSection(&m_csLock);
}
