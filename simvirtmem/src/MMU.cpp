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
    endAddress = nb_pages * 64;

    for(int i = 0; i < nb_pages/32+1; i++)
    {
        dirpages.getPDE(i).val = ((endAddress+(i*64)) << 6) | 0x8000;
    }

    for(int i = 0; i < nb_pages; i++)
    {

        (*memsec)[endAddress+(i*2)] = ((i*64) << 6) | 0x8000;
    }

	return true;
}

adresse MMU::getPageTable(adresse a)
{
    int offsetPDE = a >> 11;
    PDE entry = dirpages.getPDE(offsetPDE);
    try
    {
        if(!entry.Present()) throw PageFaultException(a);
        else return entry.PageBase();

    }
    catch (PageFaultException const& e)
    {
        load(entry.val & 0x3FC0, 0);
        return dirpages.getPDE(offsetPDE).PageBase();
    }
    return 0;
}

adresse MMU::getFrameFromPT(adresse pt, adresse a)
{
    adresse offset = (a & 0x07C0) >> 5; // 5 and not 6, because 2 bytes per entry
    adresse ramAddress =  pt + offset;
    word entry = 0;
    entry += (*ram)[ramAddress];
    entry = (entry << 8) + (*ram)[ramAddress+1];
    PTE pte;
    pte.val = entry;
    try
    {
        if(!pte.Present()) throw PageFaultException(a);
        else return pte.PageBase();
    }
    catch (PageFaultException const& e)
    {
        load(pte.val & 0x3FC0, 0);
        word entry = 0;
        entry += (*ram)[ramAddress];
        entry = (entry << 8) + (*ram)[ramAddress+1];
        return entry;
    }
}

adresse MMU::getRamFromFrame(adresse frame, adresse a)
{
    return frame + (a & 0x003F);
}

void MMU::updatePTE(adresse pt, adresse frame, adresse a)
{
    adresse ramAddress = frame + ((a & 0x07C0) >> 6);
    (*ram)[ramAddress] = (*ram)[ramAddress] | 0x40; //We set the modified bit
}

void MMU::updatePDE(adresse pt, adresse a)
{
    PDE pde = dirpages.getPDE(a >> 11);
    pde.val = pde.val | 0x40;
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




