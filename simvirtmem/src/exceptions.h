#pragma once
// Auteur: Yves Chiricota 11/11/2000


// Exceptions

#include <sstream>
#include "Logger.h"


struct EXCEPTION {

enum {


	// Exceptions matérielles
	MEM_OVERFLOW = 0x0001,



	// Exceptions logicielles
	PAGE_FAULT
};



	EXCEPTION(short _code, void* _params) :
		code(_code),
		params(_params)
		{}

	short 	code;
	void*	params;

};

//Kim Lavoie: Quelques classes pour gérer les exceptions.
//    Les classes d'exceptions log automatiquement l'exception dans un fichier.
class MemoryException
{
    protected:
        void log(std::string description)
        {
            Logger::getInstance().log(description);
        }
};

class MemoryOverflow : public MemoryException
{
    public:
    MemoryOverflow(adresse a)
    {
        std::ostringstream oss;
        oss << "Invalid memory address (overflow) : " << a;
        log(oss.str());
    }
};

class TLBMissException : public MemoryException
{
    public:
    TLBMissException(adresse a)
    {
        std::ostringstream oss;
        oss << "TLBMiss for virtual address: " << a;
        log(oss.str());
    }
};

class PageFaultException : public MemoryException
{
    public:
    PageFaultException(adresse a)
    {
        std::ostringstream oss;
        oss << "Page Fault for virtual address: " << a;
        log(oss.str());
    }
};
