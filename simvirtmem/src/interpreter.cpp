// Auteur: Yves Chiricota 11/11/2000
#include "stdafx.h"

#include "interpreter.h"
#include "MMU.h"


// --------------------------------------------------------------------------------
//  Fonction: operator>>
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
istream&	operator>>(istream& is, Command& cmd)
{
	string	fcmd;

	is >> fcmd;
	is >> cmd.adr;

	if ( fcmd == string("read") )
		cmd.cmd = Command::READ;
	else if ( fcmd == string("write") )
		cmd.cmd = Command::WRITE;
    else if ( fcmd == "") throw 0;              //Kim Lavoie (Un bogue a été corrigé ici)
    else throw string("Invalid command");

	if ( cmd.cmd == Command::WRITE )
	{
	    int i;
	    is >> i;
	    cmd.val = (byte) i;
	}


	return is;
}



// --------------------------------------------------------------------------------
//  Fonction: operator==
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
bool operator==(const Command& c0, const Command& c1)
{
	return c0.adr == c1.adr && c0.cmd == c1.cmd && c0.val == c1.val ;
}





// --------------------------------------------------------------------------------
//  Classe:	Interpreter
//  Description:
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
//  Fonction: Interpreter
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
Interpreter::Interpreter(MMU* _mmu) :
	mmu(_mmu)
{
}


// --------------------------------------------------------------------------------
//  Fonction: ~Interpreter
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
Interpreter::~Interpreter()
{
}




// --------------------------------------------------------------------------------
//  Fonction: Process
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
bool Interpreter::Process(ifstream* _f_in)
{
	Command cmd;

	if ( !_f_in )
		throw;

	f_in = _f_in;

	while ( Fetch(cmd) )
	{
		Exec(cmd);
	}

	return true;
}


// --------------------------------------------------------------------------------
//  Fonction: Fetch
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
bool	Interpreter::Fetch(Command& cmd)
{
	if ( !f_in )
		throw;

	if ( f_in->eof() )
		return false;

	try
	{
		(*f_in) >> cmd;
	}
	catch (string const& s)
	{
	    std::cerr << "Invalid command" << std::endl;
	    return false;
	}
	catch ( ... )
	{
		return false;
	}

	return true;
}


// --------------------------------------------------------------------------------
//  Fonction: Exec
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
void	Interpreter::Exec(Command& cmd)
{
	switch ( cmd.cmd )
	{
		case Command::READ:
			mmu->read(cmd.adr);
			break;


		case Command::WRITE:
			mmu->write(cmd.val, cmd.adr);   // Kim Lavoie (adresse et valeur étaient inversées)
			break;

		default:
			break;
	}
}













