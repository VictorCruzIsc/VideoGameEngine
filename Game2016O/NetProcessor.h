#pragma once
/*
Su principal funcion es escuchar (listener)
Es un consumidor de eventos por lo que su clase base es CEventListener
constructor virtual para usar correctamente el destructor de forma
polimorfica.
*/
#include "HSM\EventListener.h"
#include "HSM\EventBase.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <list>
#include <map>

using namespace std;

class CNetProcessor;

struct CLIENT {
	SOCKET Client;
	string NickName;
	CNetProcessor* pOwner;
	// Agregar informacion con respecto al cliente
};

#define EVENT_NETWORK 0xaa55aa55 // Alternancia de paquetes, detectar donde empieza un paquete 1010
class CEventNetwork : public CEventBase {
public:
	unsigned long nDataSize; // Datagrama de aplicacion vs Datagrama de red
	unsigned char m_Data[512]; // No vamos a transmitir no mas de 512 bytes
};

class CNetProcessor:
	public CEventListener
{
protected:
	WSADATA m_wsa;
	// si todas las instancias de esta aplicacion van a reibir conexiones
	// necesito un socket de escucha
	// De esta forma unifico el modelo cliente - servidor
	SOCKET m_Listener;
	map<SOCKET, CLIENT> m_mapClients;
	SOCKET m_Server;
	list<CEventNetwork*> m_lstEventsToSent;
	list<CEventNetwork*> m_lstEventsReceived;
	CRITICAL_SECTION m_csLock; // Mutex

public:
	void Lock() { EnterCriticalSection(&m_csLock); };
	void UnLock() { LeaveCriticalSection(&m_csLock); };
	unsigned long OnEvent(CEventBase* pEvent);
	bool InitNetwork(void);
	// Hace algo con los elementos que tenemos (Envia lo que tienes que enviar) y recoge todos los eventos y ejecutalos
	// Si hay chance de enviar cosas las envia y recibe
	void DoNetworkTask(void);
	bool Uninitialize(void);
	static DWORD WINAPI ServiceListener(CNetProcessor* pNP);
	static DWORD ClientThread(CLIENT* pClient);
	CNetProcessor();
	virtual ~CNetProcessor();
};

