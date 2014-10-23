#pragma once
// Auteur: Yves Chiricota 11/11/2000


#include <list>
#include <sstream>
#include "memoire.h"
#include "Logger.h"

//Kim Lavoie
class TLB{
    // Signification des bits des entrées:
    // 15..6:    adresse virtuelle, sans l'offset
    // 5:        1 si l'entrée est valide (elle n'est pas valide à l'initialisation du tampon)
    // 4..0:     adresse du frame / 64

    static const short MAX_ENTRIES = 8;
    word entries[MAX_ENTRIES] = {0};
    byte entry_ptr = 0;

    word isValid(word entry){return entry & 0x20;}
    word match(word entry, word virtualAdress){return entry & 0xffc0 == virtualAdress & 0xffc0;}

    // Cherche si l'adresse virtuelle correspond à une entrée. Si oui, retourne l'adresse de la donnée.
    // Sinon, retourne l'adresse invalide 0xffff, qui correspond à un TLB miss.
    adresse lookup(word virtualAddress)
    {
        for(int i = 0; i<MAX_ENTRIES; i++)
        {
            if(match(entries[i], virtualAddress) && isValid(entries[i]))
            {
                return ((entries[i] & 0x1f) << 6) + (virtualAddress & 0x3f);
            }
        }
        return 0xffff;   // Implémenter des exceptions à la place?
    }

    // Doit être appelé après un lookup manqué (TLB miss) pour que la table reste cohérente.
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
//  15: Présence
//  14: Modifié
//  13: Référencé
//  12,11: inutilisés
//  10..6: adresse
//  5..0: inutilisés
//
//  Par:   Yves Chiricota
//  Date:  12/11/00
//  MAJ:
// --------------------------------------------------------------------------------
struct TableElem {
	TableElem(word _val = 0) : val(_val) {}
	word	val;

	// Si Present() == vrai, représente une adresse physique,
	// sinon, c'est l'adresse d'une page dans la mémoire secondaire.
	adresse	PageBase() const { return val & 0x07C0; }


	bool	Present() const { return val & 0x8000; }
	bool	Modified() const { return val & 0x4000; }
	bool	Referenced() const { return val & 0x2000; }


};

typedef TableElem PTE;
typedef TableElem PDE;

// --------------------------------------------------------------------------------
//  Classe:	DirPages
//  Répertoire de pages
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
//  Référence à un tableau de pages. Cette classe est utilitaire. Elle sert
//  à interpréter l'adresse "base" comme un tableau de pages en mémoire.
//
//
//  Voici un exemple d'utilisation (en fait c'est un test d'unité (unit test)).
//  Ce concept consiste à tester individuellement les composantes d'un système
//  informatique. Si chaque composante se comporte comme désiré, le nombre de bug
//  qui surviennet lorsqu'on met les composantes ensemble est de beaucoup diminué.
//
//
//	// Une petite mémoire RAM de 1024 octets.
//	RAM	mem(10);
//
//
//	// "Handle" vers un tableau de pages dans la mémoire.
//	// Le tableau débute à l'adresse 32
//	TabPages tp(mem,32);
//
//	// Écrivons deux PTE
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

	// Accès au i-ème élément du tableau. On retourne une référence pour pouvoir
	// modifier le contenu (par exemple, en écrivant tab_page[i]= PTE()).
	PTE& operator[](int i) { return (PTE&)ram[base+2*i]; }

private:

	RAM&		ram;	// Référence à la mémoire RAM.
	adresse		base;	// Adresse du début de tableau.
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
//  Implémentation de l'unité de gestion de de mémoire matérielle.
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

	// Fonction d'échange de pages
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
	DirPages	dirpages;	// Répertoire de pages. Note: dans un vrai système, ce tableau
							// serait conservé en mémoire principale.

	RAM* ram;
	MemSecondaire* memsec;

	// Kim Lavoie
	TLB tlb;
	adresse endAddress = 0;
};



