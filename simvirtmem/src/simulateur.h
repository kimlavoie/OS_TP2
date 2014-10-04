#pragma once
// Auteur: Yves Chiricota 11/11/2000

class MMU;
class RAM;
class MemSecondaire;
class istream;
class Interpreter;

// --------------------------------------------------------------------------------
//  Classe:	Simulateur
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
class Simulateur {
public:
	Simulateur(int log2_sz_ram, int log2_sz_memsec);
	virtual ~Simulateur();
	
	bool	start(const string& fname);
	void	terminate();
	bool	alloc(int nb_pages);
	
private:

	ifstream*		cmd_stream;
	Interpreter*	interpreter;
	
	MMU*			mmu;
	RAM*			ram;
	MemSecondaire*	memsec;
	
	
};
