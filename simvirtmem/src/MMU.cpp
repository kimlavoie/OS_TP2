// Auteur: Yves Chiricota 11/11/2000
#include "stdafx.h"

#include "MMU.h"




// --------------------------------------------------------------------------------
//  Classe:	MMU
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
//  Fonction: MMU
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
MMU::MMU(RAM* _ram, MemSecondaire* _memsec) :
	ram(_ram),
	memsec(_memsec)
{
}



// --------------------------------------------------------------------------------
//  Fonction: virtalloc
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
bool	MMU::virtalloc(int nb_pages)
{
    maxAdress = nb_pages * 64;
	return true;
}

adresse MMU::getPageTable(adresse a)
{

    return 0;
}

adresse MMU::getFrameFromPT(adresse pt, adresse a)
{
    //TODO
    return 0;
}

adresse MMU::getRamFromFrame(adresse frame, adresse a)
{
    //TODO
    return 0;
}

void MMU::updatePTE(adresse pt, adresse frame, adresse a)
{
    //TODO
}

void MMU::updatePDE(adresse pt, adresse a)
{
    //TODO
}

adresse MMU::pageWalk(adresse a, bool writeOp)
{
    adresse pageTable = getPageTable(a);
    adresse frame = getFrameFromPT(pageTable, a);
    adresse ramAddress = getRamFromFrame(frame, a);
    if(writeOp)
    {
        updatePTE(pageTable, frame, a);
        updatePDE(pageTable, a);
    }
    return ramAddress;
}

adresse MMU::TLBLookup(adresse a)
{
    //TODO: Lookup in TLB
    throw TLBMissException(a);
    return a;
}

adresse MMU::getRamAddress(adresse a, bool writeOp)
{
    try
    {
        return TLBLookup(a);
    }
    catch (TLBMissException const& e)
    {
        return pageWalk(a, writeOp);
    }
}


// --------------------------------------------------------------------------------
//  Fonction: read
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
byte	MMU::read(adresse a)
{
    cout << "Dans read" << endl;
    cout << "  adresse: " << a << endl;

    adresse ramAddress = getRamAddress(a, false);

    return (*ram)[ramAddress];
}


// --------------------------------------------------------------------------------
//  Fonction: write
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
bool	MMU::write(byte b, adresse a)
{
    cout << "Dans write" << endl;
    cout << "  adresse: " << a << endl;
    cout << "  valeur: " << b << endl;
    adresse ramAddress = getRamAddress(a, true);

    (*ram)[ramAddress] = b;

	return false;
}



// --------------------------------------------------------------------------------
//  Fonction: load
//  Fonction d'échange de pages
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
void	MMU::load(int page, int frame)
{
}


// --------------------------------------------------------------------------------
//  Fonction: store
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
void	MMU::store(int page, int frame)
{
}




