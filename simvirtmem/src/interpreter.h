#pragma once
// Auteur: Yves Chiricota 11/11/2000


#include "memoire.h"


// --------------------------------------------------------------------------------
//  struct:	Command
//  Description:	
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
struct Command {

enum TypeCmd {
	READ,
	WRITE,
	NIL
};
	Command(TypeCmd _cmd = NIL, adresse _adr = 0, byte _val = 0) : 
		cmd(_cmd), adr(_adr), val(_val) {}

	TypeCmd		cmd;
	adresse		adr;
	byte		val;

	friend bool operator==(const Command&, const Command&);

};


class MMU;

// --------------------------------------------------------------------------------
//  Classe:	Interpreter
//  Interpreteur de commandes.
//  
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:   
// --------------------------------------------------------------------------------
class Interpreter {
public:
	Interpreter(MMU*);
	virtual ~Interpreter();
	
	bool Process(ifstream*);

private:
	MMU*		mmu;	// Référence au MMU.
	ifstream*	f_in;
	
	bool	Fetch(Command&);
	void	Exec(Command&);
};
