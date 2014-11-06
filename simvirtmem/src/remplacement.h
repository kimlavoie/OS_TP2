#pragma once

#include <queue>
#include "memoire.h"
#include "MMU.h"
#include <iostream>
class MMU;

// Méthodes de remplacement de frame.

struct Frame
{
    adresse virt_address;
    adresse ram_address;
};

class ReplacementPolicy
{
    protected:
        std::queue<Frame*> frames;

    public:
        ReplacementPolicy();

        virtual Frame& getReplacementFrame() = 0;
};

class Horloge : public ReplacementPolicy
{
    private:
        MMU* mmu;
    public:
        Horloge(MMU* mmu):mmu(mmu){}

        virtual Frame& getReplacementFrame();

};


class FIFO : public ReplacementPolicy
{
    public:
        virtual Frame& getReplacementFrame();


};
