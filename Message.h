// Message.h
#ifndef MESSAGE_H
#define MESSAGE_H

enum class MessageType {
    DATA,
    VOICE
};

class Message {
protected:
    unsigned int messageId;
    MessageType type;
    unsigned int size;
    
public:
    Message(unsigned int id, MessageType msgType, unsigned int sz) 
        : messageId(id), type(msgType), size(sz) {}
    
    virtual ~Message() {}
    
    virtual void displayMessage() const = 0;
    virtual const char* getTypeName() const = 0;
    
    unsigned int getMessageId() const { return messageId; }
    MessageType getType() const { return type; }
    unsigned int getSize() const { return size; }
};

class DataMessage : public Message {
public:
    DataMessage(unsigned int id, unsigned int sz = 1024) 
        : Message(id, MessageType::DATA, sz) {}
    
    void displayMessage() const override;
    const char* getTypeName() const override { return "DATA"; }
};

class VoiceMessage : public Message {
public:
    VoiceMessage(unsigned int id, unsigned int sz = 512) 
        : Message(id, MessageType::VOICE, sz) {}
    
    void displayMessage() const override;
    const char* getTypeName() const override { return "VOICE"; }
};

#endif