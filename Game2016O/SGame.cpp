#include "stdafx.h"
#include <stdio.h>
#include "SGame.h"
#include "SMain.h"
#include "HSM\StateMachineManager.h"

void CSGame::OnEntry()
{
	printf("CSGame::OnEntry()"); fflush(stdout);
	//Projection Matrix
	// Para colocar la matrix en perspectiva para modo 3D
	MAIN->m_pDXPainter->m_Params.Projection = 
		PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100); // ancho de ventana hor vetical, z min, z max
	VECTOR4D EyePos = { 5, 5, 5, 1 };
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 1, 0, 0 };
	MAIN->m_pDXPainter->m_Params.View =
		View(EyePos, Target, Up);
	MAIN->m_pDXPainter->m_Params.World = Identity();
	m_pGeometry = new CMesh();
	m_pGeometry->LoadSuzzane(); // Carga la mesh la primtiva, carga la geometria
}

#include "ActionEvent.h"
unsigned long CSGame::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType) {
		auto Action = (CActionEvent*)pEvent;
		MATRIX4D Camera = FastInverse(MAIN->m_pDXPainter->m_Params.View); // DXPainter que es quien tiene la vista
		VECTOR4D Pos, Dir;
		MATRIX4D Orientation = Camera;
		Orientation.vec[3] = { 0,0,0,1 }; // Una matriz de orientacion no tiene traslativa
		Pos = Camera.vec[3];
		Dir = Camera.vec[2];
		if (JOY_AXIS_RY == Action->m_nAction) {
			float Stimulus = (fabs(Action->m_fAxis) < 0.2) ? 0.0f : Action->m_fAxis;
			Pos = Pos + Dir*Stimulus*0.01;
		}
		if (JOY_AXIS_RX == Action->m_nAction) {
			float Stimulus = (fabs(Action->m_fAxis) < 0.2) ? 0.0f : Action->m_fAxis;

			// A travez de un cuaterniano
			Orientation = Orientation * RotationAxis(Stimulus*0.01, Camera.vec[1]);
		}
		
		if (JOY_AXIS_LX == Action->m_nAction){
			// Dead Zone
			float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
			Orientation = Orientation * RotationAxis(Stimulus*0.01, Camera.vec[0]);
		}

		if (JOY_AXIS_LY == Action->m_nAction) {
			// Dead Zone
			float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
			Pos = Pos + Dir*Stimulus*0.01;
		}

		Camera.vec[0] = Orientation.vec[0];
		Camera.vec[1] = Orientation.vec[1];
		Camera.vec[2] = Orientation.vec[2];
		Camera.vec[3] = Pos;
		MAIN->m_pDXPainter->m_Params.View = Orthogonalize(FastInverse(Camera));
	}

	if (APP_LOOP == pEvent->m_ulEventType) {
		auto Paint = MAIN->m_pDXPainter;
		auto DXManager = MAIN->m_pDXManager;

		// Necesito utilizar el buffer Z, no me interesa como esta el estado anterior
		// yo configuro este estado
		Paint->SetRenderTarget(
			DXManager->GetMainRTV(), // BackBuffer
			DXManager->GetMainDSV()); // ZBuffer

		VECTOR4D DeepBlue = { 0.2, 0.2, 0.7 };

		DXManager->GetContext()->ClearDepthStencilView(
			DXManager->GetMainDSV(),
			D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH,
			1.0f, 0);
		DXManager->GetContext()->ClearRenderTargetView(
			DXManager->GetMainRTV(), (float*) &DeepBlue);

		// Agregamos una luz
		Paint->m_Params.Lights[0].Type = LIGHT_DIRECTIONAL;
		Paint->m_Params.Lights[0].Flags = LIGHT_ON; // Prendido y apagado
		Paint->m_Params.Lights[0].Direction = { 0, -1, 0 }; // Un vector de arriba hacai abajo
		Paint->m_Params.Lights[0].Diffuse = { 1,1,1,0 }; // Una luz blanca, modelo difuso
		Paint->m_Params.Flags = LIGHTING_DIFFUSE;
		Paint->m_Params.Material.Diffuse = {1,1,1,0}; // Refleje toda la luz
		Paint->m_Params.Material.Emissive = { 0,0,0,0 }; // No emite luz propia

		Paint->DrawIndexed(
			&m_pGeometry->m_Vertices[0],
			m_pGeometry->m_Vertices.size(),
			&m_pGeometry->m_Indices[0],
			m_pGeometry->m_Indices.size());

		DXManager->GetSwapChain()->Present(1, 0);
	}
	return __super::OnEvent(pEvent);
}

void CSGame::OnExit()
{
	printf("CSGame::OnExit()"); fflush(stdout);
	SAFE_DELETE(m_pGeometry);
}

CSGame::CSGame()
{
}


CSGame::~CSGame()
{
}
