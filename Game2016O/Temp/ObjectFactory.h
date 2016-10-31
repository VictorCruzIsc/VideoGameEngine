#pragma once

class CObjectBase;

/*
Fábrica base de objetos a partir del identificador único.

Las aplicaciones derivan de ésta clase para especificar sus fábricas. La creación de objetos se implementa
sobrecargando el método CreateObject. El método se limita a crear objetos únicamente que deriven de CObjectBase.
*/

class CObjectFactory
{
public:
	//Crea un objeto a partir del identificador de clase.
	virtual CObjectBase* CreateObject(unsigned long ulClassID)=0;
	CObjectFactory(void);
	~CObjectFactory(void);
};

