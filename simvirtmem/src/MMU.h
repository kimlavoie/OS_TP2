#pragma once
// Auteur: Yves Chiricota 11/11/2000


#include <list>
#include <sstream>
#include "memoire.h"
#include "Logger.h"

//Kim Lavoie
class TLB{
    // Signification des bits des entr�es:
    // 15..6:    adresse virtuelle, sans l'offset
    // 5:        1 si l'entr�e est valide (elle n'est pas valide � l'initialisation du tampon)
    // 4..0:     adresse du frame / 64

    static const short MAX_ENTRIES = 8;
    word entries[MAX_ENTRIES] = {0};
    byte entry_ptr = 0;

    word isValid(word entry){return entry & 0x20;}
    word match(word entry, word virtualAdress){return entry & 0xffc0 == virtualAdress & 0xffc0;}

    // Cherche si l'adresse virtuelle correspond � une entr�e. Si oui, retourne l'adresse de la donn�e.
    // Sinon, retourne l'adresse invalide 0xffff, qui correspond � un TLB miss.
    adresse lookup(word virtualAddress)
    {
        for(int i = 0; i<MAX_ENTRIES; i++)
        {
            if(match(entries[i], virtualAddress) && isValid(entries[i]))
            {
                return ((entries[i] & 0x1f) << 6) + (virtualAddress & 0x3f);
            }
        }
        return 0xffff;   // Impl�menter des exceptions � la place?
    }

    // Doit �tre appel� apr�s un lookup manqu� (TLB miss) pour que la table reste coh�rente.
    void update(adresse virtualAddress, word frame)
    {
        entries[entry_ptr] = (virtualAddress & 0xffc0) + 0x20 + ((frame & 0x07c0) >> 6);
        entry_ptr++;
        if(entry_ptr == MAX_ENTRIES) entry_ptr = 0;
    }

};

// --------------------------------------------------------------------------------
//  struct:	TableElem
//  Signification des bits de val.
//  15,14,...,2,1,0
//  15: Pr�sence
//  14: Modifi�
//  13: R�f�renc�
//  12,11: inutilis�s
//  10..6: adresse
//  5..0: inutilis�s
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
struct TableElem {
	TableElem(word _val = 0) : val(_val) {}
	word	val;

	// Si Present() == vrai, repr�sente une adresse physique,
	// sinon, c'est l'adresse d'une page dans la m�moire secondaire.
	adresse	PageBase() const { return val & 0x07C0; }


	bool	Present() const { return val & 0x8000; }
	bool	Modified() const { return val & 0x4000; }
	bool	Referenced() const { return val & 0x2000; }


};

typedef TableElem PTE;
typedef TableElem PDE;

// --------------------------------------------------------------------------------
//  Classe:	DirPages
//  R�pertoire de pages
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class DirPages {
public:

	DirPages(int sz = 32) : size(sz), elems(new PDE[sz]) {}

	PDE& getPDE(unsigned int i) { return elems[i];}


private:

	int 	size;
	PDE*	elems;

};

// --------------------------------------------------------------------------------
//  Classe:	TabPages
//  R�f�rence � un tableau de pages. Cette classe est utilitaire. Elle sert
//  � interpr�ter l'adresse "base" comme un tableau de pages en m�moire.
//
//
//  Voici un exemple d'utilisation (en fait c'est un test d'unit� (unit test)).
//  Ce concept consiste � tester individuellement les composantes d'un syst�me
//  informatique. Si chaque composante se comporte comme d�sir�, le nombre de bug
//  qui surviennet lorsqu'on met les composantes ensemble est de beaucoup diminu�.
//
//
//	// Une petite m�moire RAM de 1024 octets.
//	RAM	mem(10);
//
//
//	// "Handle" vers un tableau de pages dans la m�moire.
//	// Le tableau d�bute � l'adresse 32
//	TabPages tp(mem,32);
//
//	// �crivons deux PTE
//	tp[3] = PTE(0x1234);
//	tp[0] = PTE(0xABCD);
//
//	// Les valeurs sont bien relues...
//	PTE pte0(tp[0]);
//	PTE pte1(tp[3]);
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class TabPages {
public:

	TabPages(RAM& _ram, adresse _base) : base(_base), ram(_ram) {}

	// Pour changer de tableau de pages.
	void	Update(adresse nouvelle_base) { base = nouvelle_base; }

	// Acc�s au i-�me �l�ment du tableau. On retourne une r�f�rence pour pouvoir
	// modifier le contenu (par exemple, en �crivant tab_page[i]= PTE()).
	PTE& operator[](int i) { return (PTE&)ram[base+2*i]; }

private:

	RAM&		ram;	// R�f�rence � la m�moire RAM.
	adresse		base;	// Adresse du d�but de tableau.
};

class MemoryException
{
    protected:
        void log(string description)
        {
            Logger::getInstance().log(description);
        }
};

class TLBMissException : public MemoryException
{
    public:
    TLBMissException(adresse a)
    {
        ostringstream oss;
        oss << "TLBMiss for virtual address: " << a;
        log(oss.str());
    }
};

class PageFaultException : public MemoryException
{
    public:
    PageFaultException(adresse a)
    {
        ostringstream oss;
        oss << "Page Fault for virtual address: " << a;
        log(oss.str());
    }
};

// --------------------------------------------------------------------------------
//  Classe:	MMU
//  Impl�mentation de l'unit� de gestion de de m�moire mat�rielle.
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
class MMU {
public:
	MMU(RAM* ram, MemSecondaire* memsec);

	bool	virtalloc(int nb_pages);
	byte	read(adresse);
	bool	write(byte, adresse);

	// Fonction d'�change de pages
	void	load(int page, int frame);
	void	store(int page, int frame);

    adresse getRamAddress(adresse a, bool writeOp);
    adresse TLBLookup(adresse a);
    adresse pageWalk(adresse a, bool writeOp);
    adresse getPageTable(adresse a);
    adresse getFrameFromPT(adresse pt, adresse a);
    adresse getRamFromFrame(adresse frame, adresse a);
    void updatePTE(adresse frame, adresse pt, adresse a);
    void updatePDE(adresse pt, adresse a);
private:
	DirPages	dirpages;	// R�pertoire de pages. Note: dans un vrai syst�me, ce tableau
							// serait conserv� en m�moire principale.

	RAM* ram;
	MemSecondaire* memsec;

	// Kim Lavoie
	TLB tlb;
	adresse endAddress = 0;
};



