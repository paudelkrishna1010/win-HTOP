bool getProcessIDList(unsigned long *processIDArray, unsigned long arraySizeInBytes, unsigned long *processCount);

bool getProcessNameList(unsigned long *processIDArray, unsigned long processCount, char **processNameArray);

void filterProcessArray(unsigned long *processIDArray, unsigned long &processCount);

void extractBaseName(char **processNameArray, int i, char *fullPath);