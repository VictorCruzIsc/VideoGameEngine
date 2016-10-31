#pragma once
#include "Stream.h"
class CObjectFactory;

/*
	Persistencia.
	Las clases que requieran de persistencia deben derivar de la clase CObjectPersistance para que implementen el método Serialize.

	El método serialize requiere de una fábrica definida por la aplicación para poder completar la funcionalidad de carga.

	Los objetos que se pueden serializar son únicamente aquellos cuya clase derive de CObjectBase
*/

class CObjectPersistance
{
public:
	//Realiza las operaciones de almacenamiento o carga
	virtual bool Serialize(CStream& stream,bool bSave,CObjectFactory& Factory)=0;
	CObjectPersistance(void);
	~CObjectPersistance(void);
};

