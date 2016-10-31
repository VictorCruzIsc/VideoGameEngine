#include "StdAfx.h"
#include "WindowBase.h"


LRESULT CALLBACK CWindowBase::WindowProc(HWND hWnd,UINT msg,
							   WPARAM wParam,LPARAM lParam)
{
	CWindowBase* pWindow;
	pWindow=(CWindowBase*)GetWindowLong(hWnd,GWL_USERDATA);
	if(!pWindow)
	{
		if(msg==WM_CREATE)
		{
			pWindow=(CWindowBase*)((CREATESTRUCT*)lParam)->lpCreateParams;
			SetWindowLong(hWnd,GWL_USERDATA,(LONG)pWindow);
			pWindow->m_hWnd=hWnd;
			return pWindow->WMCreate(wParam,lParam);
		}
	}
	else switch(msg)
	{		
		case WM_TIMER:
			return pWindow->WMTimer(wParam,lParam);
		case WM_CHAR:
			return pWindow->WMChar(wParam,lParam);
		case WM_KEYUP:
			return pWindow->WMKeyUp(wParam,lParam);
		case WM_KEYDOWN:
			return pWindow->WMKeyDown(wParam,lParam);
		case WM_SYSKEYDOWN:
			return pWindow->WMSYSKeyDown(wParam,lParam);
		case WM_COMMAND:
			return pWindow->WMCommand(wParam,lParam);
		case WM_PAINT:
			return pWindow->WMPaint(wParam,lParam);
		case WM_NOTIFY:
			return pWindow->WMNotify(wParam,lParam);
		case WM_MOVE:
			return pWindow->WMMove(wParam,lParam);
		case WM_MOVING:
			return pWindow->WMMoving(wParam,lParam);
		case WM_SIZE:
			return pWindow->WMSize(wParam,lParam);
		case WM_SIZING:
			return pWindow->WMSizing(wParam,lParam);
		case WM_HSCROLL:
			return pWindow->WMHScroll(wParam,lParam);
		case WM_VSCROLL:
			return pWindow->WMVScroll(wParam,lParam);
		case WM_LBUTTONDOWN:
			return pWindow->WMLButtonDown(wParam,lParam);
		case WM_MBUTTONDOWN:
			return pWindow->WMMButtonDown(wParam,lParam);
		case WM_RBUTTONDOWN:
			return pWindow->WMRButtonDown(wParam,lParam);
		case WM_LBUTTONUP:
			return pWindow->WMLButtonUp(wParam,lParam);
		case WM_MBUTTONUP:
			return pWindow->WMMButtonUp(wParam,lParam);
		case WM_RBUTTONUP:
			return pWindow->WMRButtonUp(wParam,lParam);
		case WM_LBUTTONDBLCLK:
			return pWindow->WMLButonDblClk(wParam,lParam);
		case WM_MBUTTONDBLCLK:
			return pWindow->WMMButonDblClk(wParam,lParam);
		case WM_RBUTTONDBLCLK:
			return pWindow->WMRButonDblClk(wParam,lParam);
		case WM_MOUSEMOVE:
			return pWindow->WMMouseMove(wParam,lParam);
		case WM_MOUSEHOVER:
			return pWindow->WMMouseHover(wParam,lParam);
		case WM_MOUSEWHEEL:
			return pWindow->WMMouseWheel(wParam,lParam);
		case WM_INITDIALOG:
			return pWindow->WMInitDialog(wParam,lParam);
		case WM_CLOSE:
			return pWindow->WMClose(wParam,lParam);
		case WM_DESTROY:
			{
				LRESULT retcode=pWindow->WMDestroy(wParam,lParam);
				//Desasociar esta ventana del objeto ventana
				SetWindowLong(hWnd,GWL_USERDATA,NULL);
				return retcode;
			}
		case WM_ACTIVATE:
			return pWindow->WMActivate(wParam,lParam);
		default:
			return pWindow->WMUnhandled(msg,wParam,lParam);
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

CWindowBase::CWindowBase(void)
{
	m_hWnd=NULL;
}

CWindowBase::~CWindowBase(void)
{
}

HINSTANCE CWindowBase::GetInstance()
{
	return m_hInstance;
}

HWND CWindowBase::GetWindowHandler()
{
	return m_hWnd;
}
ATOM CWindowBase::RegisterWindowClass(HINSTANCE hInstance)
{
	m_hInstance=hInstance;
	WNDCLASSEX wnc;
	memset(&wnc,0,sizeof(WNDCLASSEX));
	wnc.cbSize=sizeof(WNDCLASSEX);
	wnc.lpszClassName=GetWindowClassName();
	wnc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wnc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wnc.hInstance=GetInstance();
	wnc.style=CS_HREDRAW|CS_VREDRAW;
	//Memoria de ventana para nuestro uso (para almacenar this)
	wnc.cbWndExtra=sizeof(void*);
	wnc.lpfnWndProc=WindowProc;
	PreRegisterClass(&wnc);
	return RegisterClassEx(&wnc);
}
HWND CWindowBase::CreateWindowFrom(LPCREATESTRUCT lpcs)
{
	m_hInstance=lpcs->hInstance;
	lpcs->hInstance=GetInstance();
	lpcs->lpszClass=GetWindowClassName();
	PreCreateWindow(lpcs);
	m_hWnd=CreateWindowEx(lpcs->dwExStyle,
		lpcs->lpszClass,
		lpcs->lpszName,
		lpcs->style,
		lpcs->x,
		lpcs->y,
		lpcs->cx,
		lpcs->cy,
		lpcs->hwndParent,
		lpcs->hMenu,
		lpcs->hInstance,
		this);
	return m_hWnd;
}

void CWindowBase::PreRegisterClass(LPWNDCLASSEX lpwnc)
{
	//Las clases derivadas implementan éste método para hacer cambios en la estructura de registro de ventana, antes
	// de que se proceda al registro
}
void CWindowBase::PreCreateWindow(LPCREATESTRUCT lpcs)
{
	//Las clases derivadas implementan éste método para hacer cambios en la estructura de parámetros de creación de venana , antes
	//de que se proceda a la creación.
}

LRESULT CWindowBase::WMCreate(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMCommand(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMClose(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMDestroy(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMTimer(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_TIMER,wParam,lParam);
}
LRESULT CWindowBase::WMPaint(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_PAINT,wParam,lParam);
}
LRESULT CWindowBase::WMInitDialog(WPARAM,LPARAM)
{
	return FALSE;
}
LRESULT CWindowBase::WMNotify(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_NOTIFY,wParam,lParam);
}
LRESULT CWindowBase::WMActivate(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_ACTIVATE,wParam,lParam);
}

//Posicionamimiento y tamaño de la ventana
LRESULT CWindowBase::WMMove(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_MOVE,wParam,lParam);
}
LRESULT CWindowBase::WMMoving(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_MOVING,wParam,lParam);
}
LRESULT CWindowBase::WMSize(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_SIZE,wParam,lParam);
}
LRESULT CWindowBase::WMSizing(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_SIZING,wParam,lParam);
}

LRESULT CWindowBase::WMHScroll(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_HSCROLL,wParam,lParam);
}
LRESULT CWindowBase::WMVScroll(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_VSCROLL,wParam,lParam);
}

//Mensajes de entrada comunes del mouse
LRESULT CWindowBase::WMLButtonDown(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMLButtonUp(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMLButonDblClk(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMMButtonDown(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMMButtonUp(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMMButonDblClk(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMRButtonDown(WPARAM,LPARAM)
{
	return 0;
}
LRESULT CWindowBase::WMRButtonUp(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_RBUTTONUP,wParam,lParam);
}
LRESULT CWindowBase::WMRButonDblClk(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_RBUTTONDBLCLK,wParam,lParam);
}
LRESULT CWindowBase::WMMouseMove(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_MOUSEMOVE,wParam,lParam);
}
LRESULT CWindowBase::WMMouseHover(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_MOUSEHOVER,wParam,lParam);
}
LRESULT CWindowBase::WMMouseWheel(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_MOUSEWHEEL,wParam,lParam);;
}

//Mensajes del teclado
LRESULT CWindowBase::WMKeyDown(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_KEYDOWN,wParam,lParam);
}
LRESULT CWindowBase::WMKeyUp(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_KEYUP,wParam,lParam);
}
LRESULT CWindowBase::WMChar(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_CHAR,wParam,lParam);
}
LRESULT CWindowBase::WMSYSKeyDown(WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,WM_SYSKEYDOWN,wParam,lParam);
}


LRESULT CWindowBase::WMUnhandled(UINT msg,WPARAM wParam,LPARAM lParam)
{
	return DefWindowProc(m_hWnd,msg,wParam,lParam);
}
