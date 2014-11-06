// Auteur: Yves Chiricota 11/11/2000
#include "stdafx.h"

#include "MMU.h"




// --------------------------------------------------------------------------------
//  Classe:	MMU
//
//  Par:   Kim Lavoie
//  Date:  05/11/2014
// --------------------------------------------------------------------------------


// --------------------------------------------------------------------------------
//  Fonction: MMU
//
//  Par:   Kim Lavoie
//  Date:  05/11/2014
// --------------------------------------------------------------------------------
MMU::MMU(RAM* _ram, MemSecondaire* _memsec) :
	ram(_ram),
	memsec(_memsec),
	#ifdef RP_CLOCK_ALGORITHM                   //Kim Lavoie: choix de l'algo selon un define
        replacementPolicy(new Horloge(this))
    #else
        replacementPolicy(new FIFO())
    #endif
{
}



// --------------------------------------------------------------------------------
//  Fonction: virtalloc
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Initialise le répertoire de pages et les tableaux de pages.
// --------------------------------------------------------------------------------
bool	MMU::virtalloc(int nb_pages)
{
    endAddress = nb_pages * 64;

    // On initialise les PDEs
    for(int i = 0; i < nb_pages/32+1; i++)
    {
        dirpages.getPDE(i).val = (endAddress+(i*64));

    }

    // On initialise les PTEs
    for(int i = 0; i < nb_pages; i++)
    {
        word w = i << 6;
        (*memsec)[endAddress+(i*2)] = (w & 0xff00) >> 8;
        (*memsec)[endAddress+(i*2)+1] = w & 0xff;
    }

	return true;
}

// --------------------------------------------------------------------------------
//  Fonction: getPageTable
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Permet d'obtenir l'adresse en mémoire principale du tableau de pages
//              associé à l'adresse virtuelle a. Si le tableau de pages n'est pas
//              en mémoire virtuelle, il y a page fault et on le charge avant de
//              décoder.
// --------------------------------------------------------------------------------
adresse MMU::getPageTable(adresse a)
{
    int offsetPDE = a >> 11;
    PDE& entry = dirpages.getPDE(offsetPDE);
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

// --------------------------------------------------------------------------------
//  Fonction: getFrameFromPT
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Renvoie l'adresse en mémoire principale de l'adresse virtuelle a,
//              à partir de l'adresse de la table de pages. Si la page n'est pas
//              en mémoire, il y a page fault, et la page est chargée en mémoire,
//              avant d'être décodée.
// --------------------------------------------------------------------------------
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
        return ramAddress;
    }
}

// --------------------------------------------------------------------------------
//  Fonction: getRamFromFrame
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Permet de trouver l'adresse en mémoire principale à partir
//              d'une frame et d'une adresse virtuelle.
// --------------------------------------------------------------------------------
adresse MMU::getRamFromFrame(adresse frame, adresse a)
{
    return frame + (a & 0x003F);
}

// --------------------------------------------------------------------------------
//  Fonction: updatePTE
//
//  Par:   Kim Lavoie
//  Date:  05/11/2014
// --------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------
//  Fonction: updatePDE
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
// --------------------------------------------------------------------------------
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
    PDE& pde = dirpages.getPDE(a >> 11);
    pde.val = pde.val | modif;
}

// --------------------------------------------------------------------------------
//  Fonction: pageWalk
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Renvoie l'adresse demandée en faisant un "page walk".
//              Met également à jour les entrées des tables.
// --------------------------------------------------------------------------------
adresse MMU::pageWalk(adresse a, bool writeOp)
{
    adresse pageTable = getPageTable(a);
    adresse frame = getFrameFromPT(pageTable, a);
    adresse ramAddress = getRamFromFrame(frame, a);
    updatePTE(pageTable, a, writeOp);
    updatePDE(a, writeOp);
    return ramAddress;
}

// --------------------------------------------------------------------------------
//  Fonction: TLBLookup
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Permet un lookup dans la TLB. Non implémenté, alors renvoie toujours
//              une exception, comme si on n'avait pas trouvé dans la TLB.
// --------------------------------------------------------------------------------
adresse MMU::TLBLookup(adresse a)
{
    // TLB n'a pas été implémenté
    throw TLBMissException(a);
}

// --------------------------------------------------------------------------------
//  Fonction: getRamAddress
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Cette fonction renvoie l'adresse demandée.
//              Comme le TLB n'est pas implémenté, TLBMissException est
//              lancé à chaque fois, causant un "page walk".
// --------------------------------------------------------------------------------
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
//  Fonction: swap
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Permet de charger une page de façon sécuritaire, en enlevant
//              la page déjà présente en mémoire principale et en faisant
//              les vérifications nécessaires à garder l'étât interne
//              cohérent (cette fonction est complètement dégueulasse...)
// --------------------------------------------------------------------------------
adresse MMU::swap(adresse virtualAddress)
{
    Frame& nextFrame = replacementPolicy->getReplacementFrame();
    if(nextFrame.virt_address != 0xFFFF)
    {
        if((nextFrame.virt_address & 0b1111111111000000) >= endAddress) //si une table
        {
            int offsetPDE = (nextFrame.virt_address - endAddress) / 64;
            if(offsetPDE == (virtualAddress >> 11))
            {
                adresse temp = swap(virtualAddress);
                return temp;
            }
            PDE& tableEntry = dirpages.getPDE(offsetPDE);
            if(tableEntry.Modified())
            {
                store(nextFrame.virt_address >> 6, nextFrame.ram_address >> 6);
            }
            tableEntry.val = nextFrame.virt_address;
        }
        else
        {
            int offsetPDE = nextFrame.virt_address >> 11;
            PDE& entry = dirpages.getPDE(offsetPDE);
            if(entry.Present())
            {
                adresse offset = (nextFrame.virt_address & 0x07C0) >> 5; // 5 et non 6, car 2 bytes par entrée
                adresse ramAddress =  entry.PageBase() + offset;
                word PTEntry = 0;
                PTEntry += (*ram)[ramAddress];
                PTEntry = (PTEntry << 8) + (*ram)[ramAddress+1];
                PTE pte;
                pte.val = PTEntry;
                if(pte.Modified())
                {
                    // Changer PTE
                    (*ram)[ramAddress] = nextFrame.virt_address >> 8;
                    (*ram)[ramAddress+1] = nextFrame.virt_address & 0xFF;
                    //Changer PDE
                    entry.val = entry.val | 0b0110000000000000;
                    // Store Frame
                    store((nextFrame.virt_address >> 6), nextFrame.ram_address >> 6);
                }
                else
                {
                    // Changer PTE
                    (*ram)[ramAddress] = nextFrame.virt_address >> 8;
                    (*ram)[ramAddress+1] = nextFrame.virt_address & 0xFF;
                    // Changer PDE
                    entry.val = entry.val | 0b0110000000000000;
                }
            }
            else
            {

                store((nextFrame.virt_address >> 6), nextFrame.ram_address >> 6);
                int page = (entry.val & 0x3FC0) >> 6;
                load(page, nextFrame.ram_address >> 6);
                adresse ramAddress = nextFrame.ram_address + ((nextFrame.virt_address & 0x07C0) >> 5);
                // Change PTE
                (*ram)[ramAddress] = nextFrame.virt_address >> 8;
                (*ram)[ramAddress+1] = nextFrame.virt_address & 0xFF;
                store(page, nextFrame.ram_address >> 6);
            }
        }
    }
    load(virtualAddress >> 6, nextFrame.ram_address >> 6);
    if(virtualAddress >= endAddress) //s'il s'agit d'une table :
    {
        PDE& entry2 = dirpages.getPDE((virtualAddress - endAddress) / 64);
        entry2.val = nextFrame.ram_address | 0b1110000000000000;
    }
    else
    {
        PDE& entry2 = dirpages.getPDE(virtualAddress >> 11);
        entry2.val = entry2.val | 0b0110000000000000; //HERE
        adresse ramAddress = (entry2.val & 0b0000011111000000) + ((virtualAddress & 0b0000011111000000) >> 5);
        (*ram)[ramAddress] = (nextFrame.ram_address >> 8) | 0b10000000;
        (*ram)[ramAddress+1] = nextFrame.ram_address & 0xFF;
    }
    nextFrame.virt_address = virtualAddress & 0b1111111111000000;
    return nextFrame.ram_address;
}

// --------------------------------------------------------------------------------
//  Fonction: accessed
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Détails: Permet de vérifier si une frame a été accédée depuis le dernier
//              passage du ReplacementPolicy
// --------------------------------------------------------------------------------
bool MMU::accessed(Frame* frame)
{
    int offset;
    if(frame->virt_address >= endAddress) // table
    {
        offset = (frame->virt_address - endAddress) / 64;
    }
    else
    {
        offset = frame->virt_address >> 11;
    }
    PDE& pde = dirpages.getPDE(offset);
    if(!pde.Present())
    {
        cout << "pde.val: " << pde.val << endl;
        throw "PTE not present for page search ... dammit!";
    }
    if(frame->virt_address >= endAddress)  // table
    {
        return (pde.val & 0b0010000000000000) == 0b0010000000000000;
    }
    else
    {
        pde.val = pde.val | 0b0010000000000000;
        adresse pte = (pde.val & 0x07C0) + ((frame->virt_address & 0b0000011111000000) >> 5);
        return ((*ram)[pte] & 0b00100000) == 0b00100000;
    }
}

// --------------------------------------------------------------------------------
//  Fonction: set_unaccessed
//
//  Par:     Kim Lavoie
//  Date:    05/11/2014
//  Details: Permet de désactiver le flag "accédé" de la frame
// --------------------------------------------------------------------------------
void MMU::set_unaccessed(Frame* frame)
{
    int offset;
    if(frame->virt_address >= endAddress) // table
    {
        offset = (frame->virt_address - endAddress) / 64;
    }
    else
    {
        offset = frame->virt_address >> 11;
    }
    PDE& pde = dirpages.getPDE(offset);
    if(!pde.Present())
    {
        throw "PTE not present for page search ... dammit!";
    }
    if(frame->virt_address >= endAddress)  // table
    {
        pde.val = pde.val & 0b1101111111111111;
    }
    else
    {
        pde.val = pde.val | 0b0010000000000000;
        adresse pte = (pde.val & 0x07C0) + ((frame->virt_address & 0b0000011111000000) >> 5);
        (*ram)[pte] &= 0b11011111;
    }

}

// --------------------------------------------------------------------------------
//  Fonction: read
//
//  Par:   Kim Lavoie
//  Date:  05/11/2014
// --------------------------------------------------------------------------------
byte	MMU::read(adresse a)
{
    #ifdef DEBUG
        cout << "Dans read" << endl;
        cout << "  adresse: " << a << endl;
    #endif

    if(a >= endAddress)
    {
        throw MemoryOverflow(a);    //Si on tente d'accéder à un espace non alloué
    }

    adresse ramAddress = getRamAddress(a, false);

    #ifdef DEBUG
        cout << "  Byte read: " << (int)(*ram)[ramAddress] << endl;
        cout << endl;
    #endif

    return (*ram)[ramAddress];
}


// --------------------------------------------------------------------------------
//  Fonction: write
//
//  Par:   Kim Lavoie
//  Date:  05/11/2014
// --------------------------------------------------------------------------------
bool	MMU::write(byte b, adresse a)
{
    #ifdef DEBUG
        cout << "Dans write" << endl;
        cout << "  adresse: " << a << endl;
        cout << "  valeur: " << (int)b << endl;
    #endif

    if(a >= endAddress)
    {
        throw MemoryOverflow(a);   //Si on tente d'accéder à un espace non alloué
    }

    // On récupère l'adresse et on écrit le byte
    adresse ramAddress = getRamAddress(a, true);
    (*ram)[ramAddress] = b;

    #ifdef DEBUG
        cout << "  Byte written: " << (int)(*ram)[ramAddress] << endl;
        cout << endl;
    #endif
	return false;
}


// --------------------------------------------------------------------------------
//  Fonction: load
//  Fonction d'échange de pages
//
//  Par:   Kim Lavoie
//  Date:  05/11/2014
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
//  Par:   Kim Lavoie
//  Date:  05/11/2014
// --------------------------------------------------------------------------------
void	MMU::store(int page, int frame)
{
    for(int i = 0; i < 64; i++)
    {
        (*memsec)[page*64+i] = (*ram)[frame*64+i];
    }
}




