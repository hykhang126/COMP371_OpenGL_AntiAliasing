#pragma once

class IBuffer {
private:
    unsigned int renderer_id;
    unsigned int count;
public:
    IBuffer(const unsigned int* data, unsigned int count);
    ~IBuffer();

    void bind() const;
    void unbind() const;

    unsigned int getCount() const { return count; }
};