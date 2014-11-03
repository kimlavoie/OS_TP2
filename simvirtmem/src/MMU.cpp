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
	memsec(_memsec),
	replacementPolicy(new FIFO())
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

    // Set PDEs
    for(int i = 0; i < nb_pages/32+1; i++)
    {
        dirpages.getPDE(i).val = (endAddress+(i*64));

    }

    // set PTEs
    for(int i = 0; i < nb_pages; i++)
    {
        word w = i << 6;
        (*memsec).set(endAddress+(i*2), (w & 0xff00) >> 8);
        (*memsec).set(endAddress+(i*2)+1, w & 0xff);
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
        adresse pageTableAddress = swap(dirpages.getPDE(offsetPDE).val);
        dirpages.getPDE(offsetPDE).val = pageTableAddress | 0b1000000000000000;
        return pageTableAddress;
    }
    return 0;
}

adresse MMU::getFrameFromPT(adresse pt, adresse a)
{
    adresse offset = (a & 0b0000011111000000) >> 5; // 5 and not 6, because 2 bytes per entry
    adresse ramAddress =  pt + offset;
    word entry = 0;
    entry += (*ram)[ramAddress];
    entry = (entry << 8) + (*ram)[ramAddress+1];
    PTE pte;
    pte.val = entry;
    try
    {
        if(!pte.Present()) throw PageFaultException(a);
        return pte.PageBase();
    }
    catch (PageFaultException const& e)
    {
        adresse ramAddress = swap(a);
//        word entry2 = 0;
//        entry2 += (*ram)[ramAddress];
//        entry2 = (entry2 << 8) + (*ram)[ramAddress+1];
//        cout << "Entry2: " << entry2 << endl;
        return ramAddress;
    }
}

adresse MMU::getRamFromFrame(adresse frame, adresse a)
{
    return frame + (a & 0x003F);
}

void MMU::updatePTE(adresse pt, adresse a, bool writeOp)
{
    int modif;
    if(writeOp)
    {
        modif = 0b01100000; //modified and accessed
    }
    else
    {
        modif = 0b00100000; //accessed only
    }
    adresse ramAddress = pt + ((a & 0x07C0) >> 5);
    (*ram)[ramAddress] = (*ram)[ramAddress] | modif; //We set the modified bit
}

void MMU::updatePDE(adresse a, bool writeOp)
{
    int modif;
    if(writeOp)
    {
        modif = 0b0110000000000000; //modified and accessed
    }
    else
    {
        modif = 0b0010000000000000; //accessed only
    }
    PDE pde = dirpages.getPDE(a >> 11);
    pde.val = pde.val | modif;
}

adresse MMU::pageWalk(adresse a, bool writeOp)
{
    adresse pageTable = getPageTable(a);
    adresse frame = getFrameFromPT(pageTable, a);
    adresse ramAddress = getRamFromFrame(frame, a);
    updatePTE(pageTable, a, writeOp);
    updatePDE(a, writeOp);
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

    if(a >= endAddress)
    {
        throw MemoryOverflow(a);
    }

    adresse ramAddress = getRamAddress(a, false);

    cout << "  Byte read: " << (int)(*ram)[ramAddress] << endl;
    cout << endl;

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
    cout << "  valeur: " << (int)b << endl;

    if(a >= endAddress)
    {
        throw MemoryOverflow(a);
    }

    adresse ramAddress = getRamAddress(a, true);
    (*ram)[ramAddress] = b;


    cout << endl;
	return false;
}

adresse MMU::swap(adresse virtualAddress)
{
    Frame nextFrame = replacementPolicy->getReplacementFrame();
    if(nextFrame.virt_address != 0xFFFF)
    {
        int offsetPDE = nextFrame.virt_address >> 11;
        PDE entry = dirpages.getPDE(offsetPDE);
        if(entry.Present())
        {
            adresse offset = (nextFrame.virt_address & 0x07C0) >> 5; // 5 and not 6, because 2 bytes per entry
            adresse ramAddress =  entry.PageBase() + offset;
            word PTEntry = 0;
            PTEntry += (*ram)[ramAddress];
            PTEntry = (PTEntry << 8) + (*ram)[ramAddress+1];
            PTE pte;
            pte.val = PTEntry;
            if(pte.Modified())
            {
                // Change PTE
                (*ram)[ramAddress] = nextFrame.virt_address >> 8;
                (*ram)[ramAddress+1] = nextFrame.virt_address & 0xFF;
                //Change PDE
                entry.val = entry.val | 0b0110000000000000;
                // Store Frame
                store((nextFrame.virt_address >> 6), nextFrame.ram_address >> 6);
            }
            else
            {
                cout << ramAddress << endl;
                // Change PTE
                (*ram)[ramAddress] = nextFrame.virt_address >> 8;
                (*ram)[ramAddress+1] = nextFrame.virt_address & 0xFF;
                //Change PDE
                entry.val = entry.val | 0b0110000000000000;
            }
        }
        else
        {
            store((nextFrame.virt_address >> 6), nextFrame.ram_address);
            int page = (entry.val & 0x3FC0) >> 6;
            load(page, nextFrame.ram_address);
            adresse ramAddress = nextFrame.ram_address + ((nextFrame.virt_address & 0x07C0) >> 6);
            // Change PTE
            (*ram)[ramAddress] = nextFrame.virt_address >> 8;
            (*ram)[ramAddress+1] = nextFrame.virt_address & 0xFF;
            store(page, nextFrame.ram_address >> 6);
        }
    }
    load(virtualAddress >> 6, nextFrame.ram_address >> 6);
    PDE& entry2 = dirpages.getPDE(virtualAddress >> 11);
    if(virtualAddress >= endAddress) //if it's a table, we don't have to change the PTE
    {
        entry2.val = nextFrame.ram_address | 0x8000;

    }
    else
    {
        entry2.val = entry2.val | 0b0110000000000000;
        adresse ramAddress = (entry2.val & 0b0000011111000000) + ((virtualAddress & 0b0000011111000000) >> 5);
        (*ram)[ramAddress] = (nextFrame.ram_address >> 8) | 0b10000000;
        (*ram)[ramAddress+1] = nextFrame.ram_address & 0xFF;
    }
    nextFrame.virt_address = virtualAddress & 0b1111111111000000;
    return nextFrame.ram_address;
}

bool MMU::accessed(Frame* frame)
{
    PDE& pde = dirpages.getPDE(frame->virt_address >> 11);
    if(!pde.Present())
    {
        throw "PTE not present for page search ... dammit!";
    }
    pde.val = pde.val | 0b0010000000000000;
    adresse pte = (pde.val & 0x07C0) + ((frame->virt_address & 0b0000011111000000) >> 5);
    return ((*ram)[pte] & 0b00100000) == 0b00100000;


}

void MMU::set_unaccessed(Frame* frame)
{
    PDE& pde = dirpages.getPDE(frame->virt_address >> 11);
    if(!pde.Present())
    {
        throw "PTE not present for page search ... dammit!";
    }
    pde.val = pde.val | 0b0010000000000000;
    adresse pte = (pde.val & 0x07C0) + ((frame->virt_address & 0b0000011111000000) >> 5);
    (*ram)[pte] &= 0b11011111;
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
    for(int i = 0; i < 64; i++)
    {
        (*ram)[frame*64+i] = (*memsec)[page*64+i];
    }
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
    for(int i = 0; i < 64; i++)
    {
        (*memsec)[page*64+i] = (*ram)[frame*64+i];
    }
}




