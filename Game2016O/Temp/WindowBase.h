#pragma once
#include <windows.h>

class CWindowBase
{
protected:
	static LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);
	HINSTANCE m_hInstance;
	HWND m_hWnd;
public:
	CWindowBase(void);
	//La clase base debe implementar éste metodo (Nombre de clase de ventana)
	virtual TCHAR* GetWindowClassName()=0;
	ATOM RegisterWindowClass(HINSTANCE hInstance);
	HWND CreateWindowFrom(LPCREATESTRUCT lpcs);
	HINSTANCE GetInstance();
	HWND GetWindowHandler();
protected:	
	//Callbacks de preregistro y precreación de ventanas
	virtual void PreRegisterClass(LPWNDCLASSEX lpwnc);
	virtual void PreCreateWindow(LPCREATESTRUCT lpcs);
	//Mensajes fundamentales de ventana
	virtual LRESULT WMCreate(WPARAM,LPARAM);
	virtual LRESULT WMCommand(WPARAM,LPARAM);
	virtual LRESULT WMClose(WPARAM,LPARAM);
	virtual LRESULT WMDestroy(WPARAM,LPARAM);
	virtual LRESULT WMTimer(WPARAM,LPARAM);
	virtual LRESULT WMPaint(WPARAM,LPARAM);
	virtual LRESULT WMInitDialog(WPARAM,LPARAM);
	virtual LRESULT WMNotify(WPARAM,LPARAM);
	virtual LRESULT WMActivate(WPARAM,LPARAM);
	
	//Posicionamimiento y tamaño de la ventana
	//Las ventanas heredadas deberían invocar éstos métodos tras procesar el evento.
	virtual LRESULT WMMove(WPARAM,LPARAM);
	virtual LRESULT WMMoving(WPARAM,LPARAM);
	virtual LRESULT WMSize(WPARAM,LPARAM);
	virtual LRESULT WMSizing(WPARAM,LPARAM);
	virtual LRESULT WMHScroll(WPARAM,LPARAM);
	virtual LRESULT WMVScroll(WPARAM,LPARAM);

	//Mensajes de entrada comunes del mouse
	virtual LRESULT WMLButtonDown(WPARAM,LPARAM);
	virtual LRESULT WMLButtonUp(WPARAM,LPARAM);
	virtual LRESULT WMLButonDblClk(WPARAM,LPARAM);
	virtual LRESULT WMMButtonDown(WPARAM,LPARAM);
	virtual LRESULT WMMButtonUp(WPARAM,LPARAM);
	virtual LRESULT WMMButonDblClk(WPARAM,LPARAM);
	virtual LRESULT WMRButtonDown(WPARAM,LPARAM);
	virtual LRESULT WMRButtonUp(WPARAM,LPARAM);
	virtual LRESULT WMRButonDblClk(WPARAM,LPARAM);
	virtual LRESULT WMMouseMove(WPARAM,LPARAM);
	virtual LRESULT WMMouseHover(WPARAM,LPARAM);
	virtual LRESULT WMMouseWheel(WPARAM,LPARAM);

	//Mensajes del teclado
	virtual LRESULT WMKeyDown(WPARAM,LPARAM);
	virtual LRESULT WMKeyUp(WPARAM,LPARAM);
	virtual LRESULT WMChar(WPARAM,LPARAM);
	virtual LRESULT WMSYSKeyDown(WPARAM,LPARAM);

	//Agregar aquí más manipuladores de evento... (Actualizando el switch(msg) de WindowProc también)

	//Mensaje actualmente no implementado como método independiente "WMUnhandled"
	//Todo mensaje de ventana que no haya sido definido como método, podrá interceptarse mediante éste método.
	virtual LRESULT WMUnhandled(UINT msg,WPARAM,LPARAM);

	virtual ~CWindowBase(void);
};
