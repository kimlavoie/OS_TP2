#include "remplacement.h"
#include <iostream>

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
    return *frame;
}

Frame& FIFO::getReplacementFrame()
{
    Frame* frame = frames.front();
    frames.pop();
    frames.push(frame);
    return *frame;
}
