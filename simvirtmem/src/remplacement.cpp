#include "remplacement.h"
#include <iostream>

ReplacementPolicy::ReplacementPolicy()
{
    for(int i = 0; i < 16; i++)
    {
        Frame* frame = new Frame();
        frame->virt_address = 0xFFFF;
        frame->ram_address = i * 64;
        frames.push(frame);
    }
}

Frame& Horloge::getReplacementFrame()
{
    Frame* frame = frames.front();
    while(frame->virt_address != 0xFFFF && mmu->accessed(frame))
    {
        mmu->set_unaccessed(frame);
        frames.pop();
        frames.push(frame);
        frame = frames.front();
    }
    frames.pop();
    frames.push(frame);
    return *frame;
}

Frame& FIFO::getReplacementFrame()
{
    Frame* frame = frames.front();
    frames.pop();
    frames.push(frame);
    return *frame;
}
