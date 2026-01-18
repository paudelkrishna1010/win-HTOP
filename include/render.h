struct CHAR_INFO;

CHAR_INFO *createFrameBuffer(short width, short height);

void paintFrame(CHAR_INFO *frameBuffer, short width, short row, short col, char *data);

void clearFrameBuffer(CHAR_INFO *frameBuffer, short width, short height);