// Message.cpp
#include "Message.h"
#include "basicIO.h"

void DataMessage::displayMessage() const {
    io.outputstring("Data Message [ID: ");
    io.outputint(messageId);
    io.outputstring(", Size: ");
    io.outputint(size);
    io.outputstring(" bytes]");
}

void VoiceMessage::displayMessage() const {
    io.outputstring("Voice Message [ID: ");
    io.outputint(messageId);
    io.outputstring(", Size: ");
    io.outputint(size);
    io.outputstring(" bytes]");
}