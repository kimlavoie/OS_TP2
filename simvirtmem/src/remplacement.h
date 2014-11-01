#pragma once

#include <queue>

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
    public:
        virtual Frame& getReplacementFrame()
        {
            //TODO
        }
};


class FIFO : public ReplacementPolicy
{
    public:
        virtual Frame& getReplacementFrame()
        {
            Frame* frame = frames.front();
            frames.pop();
            frames.push(frame);
            return *frame;
        }

};
