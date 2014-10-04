// Auteur: Yves Chiricota 11/11/2000
#include "stdafx.h"

#include "memoire.h"
#include <cmath>

#include "exceptions.h"


// --------------------------------------------------------------------------------
//  Classe:	Memoire
//  Description:	
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
//  Fonction: Memoire
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
Memoire::Memoire(int k) : 
	size(pow(2,k))
{
	memoire = new byte[size];
}

	
// --------------------------------------------------------------------------------
//  Fonction: ~Memoire
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
Memoire::~Memoire()
{
	if ( memoire )
		delete[] memoire;
}



// --------------------------------------------------------------------------------
//  Fonction: operator[]
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
byte&	Memoire::operator[](adresse x)
{
	if ( x > size )
		throw EXCEPTION(EXCEPTION::MEM_OVERFLOW, 0);
	return memoire[x];
}


// --------------------------------------------------------------------------------
//  Fonction: operator[]
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
const byte&	Memoire::operator[](adresse x) const
{
	if ( x > size )
		throw EXCEPTION(EXCEPTION::MEM_OVERFLOW, 0);
	return memoire[x];
}


	








