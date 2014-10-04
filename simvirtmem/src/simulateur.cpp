// Auteur: Yves Chiricota 11/11/2000
#include "stdafx.h"

#include "simulateur.h"
#include "MMU.h"
#include "memoire.h"
#include "interpreter.h"



// --------------------------------------------------------------------------------
//  Classe:	Simulateur
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
//  Fonction: Simulateur
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
Simulateur::Simulateur(int log2_sz_ram, int log2_sz_memsec) :
	ram(new RAM(log2_sz_ram)),
	memsec(new MemSecondaire(log2_sz_memsec)),
	cmd_stream(0)
	
{
	mmu = new MMU(ram, memsec);
	interpreter = new Interpreter(mmu);
}


// --------------------------------------------------------------------------------
//  Fonction: ~Simulateur
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
Simulateur::~Simulateur()
{
	if ( mmu )
		delete  mmu;
		
	if ( ram )
		delete ram;
		
	if ( memsec )
		delete memsec;
		
	if ( interpreter )
		delete interpreter;
		
	if ( cmd_stream )
	{
		cmd_stream->close();
		delete cmd_stream;
	}
}



// --------------------------------------------------------------------------------
//  Fonction: start
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
bool	Simulateur::start(const string& fname)
{
	if ( !cmd_stream )
	{
		cmd_stream = new ifstream();
		cmd_stream->open(fname.c_str());
	}
	
	if ( !cmd_stream->is_open() )
		return false;
		
		
	interpreter->Process(cmd_stream);
	
	return true;
}

// --------------------------------------------------------------------------------
//  Fonction: terminate
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
void	Simulateur::terminate()
{
	if ( cmd_stream )
	{
		cmd_stream->close();
		delete cmd_stream;
		cmd_stream = 0;
	}
}

// --------------------------------------------------------------------------------
//  Fonction: alloc
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
bool	Simulateur::alloc(int nb_pages)
{
	return mmu->virtalloc(nb_pages);
}

