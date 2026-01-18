struct CHAR_INFO;

void* createConsoleBuffer();
bool writeFrameToConsoleBuffer(void* consoleBuffer, CHAR_INFO *data);
bool setConsoleBufferActive(void* consoleBuffer);
