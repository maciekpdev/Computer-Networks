// Maciej Pietrewicz 325022

#include "Window.hpp"

#include <algorithm>
#include <cstring>

Window::Window(int size_, int len) {
    size = size_;
    segmentData = new char*[size];
    for (int i = 0; i < size; i++) {
        segmentData[i] = new char[len];
    }
    isSegmentReceived = new bool[size];
    std::fill_n(isSegmentReceived, size, false);
    startIdx = 0;
}

Window::~Window() {
    for (int i = 0; i < size; i++) {
        delete[] segmentData[i];
    }
    delete[] segmentData;
    delete[] isSegmentReceived;
}

void Window::shift() {
    startIdx = (startIdx + 1) % size;
}

void Window::setData(char *begin, size_t len, int pos) {
  if(!isSegmentReceived[pos]) {
    std::memcpy(segmentData[pos], begin, len);
    isSegmentReceived[pos] = true;
  }
}

