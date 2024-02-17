#include "SD_MMC.h"

void clearAll()
{
    if (SD_MMC.remove("/samples/"))
    {
        Serial.println("samples deleted cleared");
    }
    else
    {
        Serial.println("samples deletion failed");
    }
}

void writeSample(uint16_t *data, String filename)
{
    File file = SD_MMC.open(filename, FILE_WRITE);
    if (!file)
    {
        Serial.println("File create failed");
        return;
    }
    file.write(*data);
    file.close();
    Serial.println("File " + filename + " write successful");
}

void setupSD()
{
    if (!SD_MMC.begin())
    {
        Serial.println("SD Card initialization failed!");
        return;
    }

    if (SD_MMC.exists("/samples/"))
    {
        Serial.println("folder exists");
    }
    else
    {
        Serial.println("File does not exist");
        if (SD_MMC.mkdir("/samples/"))
        {
            Serial.println("samples directory created");
        }
        else
        {
            Serial.println("samples directory creation failed");
        }
    }
}

void setup()
{
    setupSD();
    const char *payload = "hey this is bulletlaunch";
    uint16_t number = (uint16_t)strtoul(payload, NULL, 10);
    writeSample(&number, "sample.txt");
}

void loop()
{
    // put your main code here, to run repeatedly:
}
