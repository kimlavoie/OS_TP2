#pragma once

#include <queue>
#include "memoire.h"
#include "MMU.h"
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
        ReplacementPolicy()
        {
            for(int i = 0; i < 16; i++)
            {
                Frame* frame = new Frame();
                frame->virt_address = 0xFFFF;
                frame->ram_address = i * 64;
                frames.push(frame);
            }
        }
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
