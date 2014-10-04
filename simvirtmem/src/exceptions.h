#pragma once
// Auteur: Yves Chiricota 11/11/2000


// Exceptions


struct EXCEPTION {

enum { 


	// Exceptions matérielles
	MEM_OVERFLOW = 0x0001,
	


	// Exceptions logicielles
	PAGE_FAULT
};



	EXCEPTION(short _code, void* _params) :
		code(_code),
		params(_params)
		{}
	
	short 	code;
	void*	params;
	
};