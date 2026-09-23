#include <nds.h>
#include "rcore_ds.h"
#include <stdlib.h>
#include "rtext_ds.h"


#define MAX_TEXT_BUFFER_LENGTH  1024
#define MAX_TEXTSPLIT_COUNT  128

Font GetFontDefault(void)
{
    Font f = LoadFont("nitro:/default.fnt");
    return f;
}
char* findStringRetString(char *data,char *find,int *offset)
{
    char* stringLocation = strstr(&data[*offset],find);
    stringLocation += strlen(find);

    char buffer[64];
    int index = 0;

    while (stringLocation[index] != '"')
    {
        buffer[index] = stringLocation[index];
        index++;
    }
    *offset = *offset + index + strlen(find);
    buffer[index] = '\0';
    char* result = malloc(strlen(buffer)+1);
    strcpy(result,buffer);
    //printf("%s",result);
    return result;
}// ie something="bananas" returns "bananas"

int findValInString(char *data, char *find, int *offset)
{
    char* stringLocation = strstr(data + *offset,find);
    stringLocation += strlen(find);
    char buffer[64];
    int index = 0;
    while (stringLocation[index] != ' ' && stringLocation[index] != '\n' && stringLocation[index] != '\0')
    {
        buffer[index] = stringLocation[index];
        index++;
    }
    *offset = (stringLocation - data) + index;
    buffer[index] = '\0';
    return atoi(buffer);
}// ie input "something=" returns 42

Font LoadFont(char* name)
{
    Font f;
    char* fileData = LoadFileText(name);
    int offset = 0;
    f.f.face = findStringRetString(fileData, "face=\"",&offset);

    f.f.fileName = findStringRetString(fileData,"file=\"",&offset);
    f.tex = LoadTexture(f.f.fileName);
    f.f.chars = findValInString(fileData,"chars count=",&offset);

    f.glyph = malloc(sizeof(Glyph) * f.f.chars);

    for (int i = 0 ; i < f.f.chars; i++)
    {

        f.glyph[i].id = findValInString(fileData,"char id=",&offset);
        f.glyph[i].x = findValInString(fileData,"x=",&offset);
        f.glyph[i].y = findValInString(fileData,"y=",&offset);
        f.glyph[i].width = findValInString(fileData,"width=",&offset);
        f.glyph[i].height = findValInString(fileData,"height=",&offset);
        f.glyph[i].xOffset = findValInString(fileData,"xoffset=",&offset);
        f.glyph[i].yOffset = findValInString(fileData,"yoffset=",&offset);
        f.glyph[i].xAdvance = findValInString(fileData,"xadvance=",&offset);
    }

    return f;
}//pretty slow, best to load at start

void UnloadFont(Font font)
{
    free(font.glyph);
    UnloadTexture(font.tex);

}
int drawChar(Font* f,Vector2 pos,char c,Color col,float size)
{
    int i;
    for (i = 0; i < f->f.chars; i++)
    {
        if (f->glyph[i].id == c) break;
    }
    pos.x += (float)f->glyph[i].xOffset;
    pos.y += (float)f->glyph[i].yOffset;

    DrawTextureRecAndScale(f->tex,(Rectangle){f->glyph[i].x ,f->glyph[i].y, f->glyph[i].width, f->glyph[i].height},pos, col,floattof32(size),floattof32(size));
    return i;

}// returns position of character drawn
void drawString(Font* f, Vector2 pos, char* s,Color col, int spacing,float size)
{

    int cursorX = (int)(pos.x / size);
    int cursorY = (int)(pos.y / size);
    int len =  strlen(s);
    for (int i = 0; i < len ; i++)
    {
        int id = drawChar(f,(Vector2){(float)cursorX,(float)cursorY},s[i],col,size);
        cursorX += f->glyph[id].xAdvance;
        cursorX += spacing;
    }
}

void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    drawString(&font,position,text,tint,(int)spacing,fontSize);


}

void DrawText(const char *text, int posX, int posY, int fontSize, Color color)
{
    drawString(&DS.fontDefault,(Vector2){(float)posX,(float)posY},text,color,1,(float)fontSize);
}

int findIndexOfChar(Font* f, char c)
{
    int i = 0;
    while (i < f->f.chars && f->glyph[i].id != c) i++;
    if (i >= f->f.chars) i = -1;
    return i;
}
void SetTextLineSpacing(int spacing)
{
//todo support newlines
};                                                 // Set vertical line spacing when drawing with line-breaks
int MeasureText(const char *text, int fontSize)
{
    int totSize = 0;

    for (int i = 0; i < strlen(text); i++)
    {
        int index = findIndexOfChar(&DS.fontDefault,text[i]);
        if (index < 0) continue;
        totSize += DS.fontDefault.glyph[index].xAdvance;
        totSize += 1;

    }
    return totSize * fontSize;
};


int TextFindIndex(const char *text, const char *search)
{
    int position = -1;

    if (text != NULL)
    {
        char *ptr = strstr(text, search);

        if (ptr != NULL) position = (int)(ptr - text);
    }

    return position;
}


unsigned int TextLength(const char *text)
{
    unsigned int length = 0;

    if (text != NULL)
    {
        while (text[length] != '\0') length++;
    }

    return length;
}
// Measure string width for default font
Vector2 MeasureTextEx(Font font, const char *text, float fontSize, float spacing);

char **LoadTextLines(const char *text, int *count)
{
    char **lines = NULL;
    int lineCount = 0;

    if (text != NULL)
    {
        int textLength = TextLength(text);
        lineCount = 1;

        // First text scan pass to get required line count
        for (int i = 0; i < textLength; i++)
        {
            if (text[i] == '\n') lineCount++;
        }

        lines = (char **)calloc(lineCount, sizeof(char *));
        for (int i = 0, l = 0, lineLen = 0; i <= textLength; i++)
        {
            if ((text[i] == '\n') || (text[i] == '\0'))
            {
                lines[l] = (char *)calloc(lineLen + 1, 1);
                memcpy(lines[l], &text[i - lineLen], lineLen);
                lineLen = 0;
                l++;
            }
            else lineLen++;
        }
    }

    *count = lineCount;
    return lines;
}

// Unload text lines
void UnloadTextLines(char **lines, int lineCount)
{
    for (int i = 0; i < lineCount; i++) free(lines[i]);
    free(lines);
}

// Get text length in bytes, check for \0 character


// Formatting of text with variables to 'embed'
// WARNING: String returned will expire after this function is called MAX_TEXTFORMAT_BUFFERS times
const char *TextFormat(const char *text, ...)
{
#ifndef MAX_TEXTFORMAT_BUFFERS
    #define MAX_TEXTFORMAT_BUFFERS 4        // Maximum number of static buffers for text formatting
#endif

    // Create an array of buffers so strings don't expire until MAX_TEXTFORMAT_BUFFERS invocations
    static char buffers[MAX_TEXTFORMAT_BUFFERS][MAX_TEXT_BUFFER_LENGTH] = { 0 };
    static int index = 0;

    char *currentBuffer = buffers[index];
    memset(currentBuffer, 0, MAX_TEXT_BUFFER_LENGTH); // Clear buffer before using

    if (text != NULL)
    {
        va_list args;
        va_start(args, text);
        int requiredByteCount = vsnprintf(currentBuffer, MAX_TEXT_BUFFER_LENGTH, text, args);
        va_end(args);

        // If requiredByteCount is larger than the MAX_TEXT_BUFFER_LENGTH, then overflow occurred
        if (requiredByteCount >= MAX_TEXT_BUFFER_LENGTH)
        {
            // Inserting "..." at the end of the string to mark as truncated
            char *truncBuffer = buffers[index] + MAX_TEXT_BUFFER_LENGTH - 4; // Adding 4 bytes = "...\0"
            snprintf(truncBuffer, 4, "...");
        }

        index += 1;     // Move to next buffer for next function call
        if (index >= MAX_TEXTFORMAT_BUFFERS) index = 0;
    }

    return currentBuffer;
}

// Get integer value from text
// NOTE: This function replaces atoi() [stdlib.h]
int TextToInteger(const char *text)
{
    int value = 0;
    int sign = 1;

    if (text != NULL)
    {
        if ((text[0] == '+') || (text[0] == '-'))
        {
            if (text[0] == '-') sign = -1;
            text++;
        }

        for (int i = 0; ((text[i] >= '0') && (text[i] <= '9')); i++) value = value*10 + (int)(text[i] - '0');
    }

    return value*sign;
}

// Get float value from text
// NOTE: This function replaces atof() [stdlib.h]
// WARNING: Only '.' character is understood as decimal point
float TextToFloat(const char *text)
{
    float value = 0.0f;
    float sign = 1.0f;

    if (text != NULL)
    {
        if ((text[0] == '+') || (text[0] == '-'))
        {
            if (text[0] == '-') sign = -1.0f;
            text++;

        }

        int i = 0;
        for (; ((text[i] >= '0') && (text[i] <= '9')); i++) value = value*10.0f + (float)(text[i] - '0');

        if (text[i++] == '.')
        {
            float divisor = 10.0f;
            for (; ((text[i] >= '0') && (text[i] <= '9')); i++)
            {
                value += ((float)(text[i] - '0'))/divisor;
                divisor = divisor*10.0f;
            }
        }
    }

    return value*sign;
}

// Copy one string to another, returns bytes copied
// NOTE: Alternative implementation to strcpy(dst, src) from C standard library
int TextCopy(char *dst, const char *src)
{
    int bytes = 0;

    if ((src != NULL) && (dst != NULL))
    {
        while (*src != '\0')
        {
            *dst = *src;
            dst++;
            src++;

            bytes++;
        }

        *dst = '\0';
    }

    return bytes;
}

// Check if two text strings are equal
// REQUIRES: strcmp()
bool TextIsEqual(const char *text1, const char *text2)
{
    bool result = false;
                    
    if ((text1 != NULL) && (text2 != NULL))
    {
        if (strcmp(text1, text2) == 0) result = true;
    }

    return result;
}

// Get a piece of a text string
const char *TextSubtext(const char *text, int position, int length)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if ((text != NULL) && (position >= 0) && (length > 0))
    {
        int textLength = TextLength(text);

        if (position < textLength)
        {
            int maxLength = textLength - position;
            if (length > maxLength) length = maxLength;
            if (length >= MAX_TEXT_BUFFER_LENGTH) length = MAX_TEXT_BUFFER_LENGTH - 1;

            // NOTE: Alternative: memcpy(buffer, text + position, length)

            for (int c = 0; c < length; c++) buffer[c] = text[position + c];

            buffer[length] = '\0';
        }
    }

    return buffer;
}

// Remove text spaces, concat words
const char *TextRemoveSpaces(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if (text != NULL)
    {
        // Avoid copying the ' ' characters
        for (int i = 0, j = 0; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[i] != '\0'); i++)
        {
            if (text[i] != ' ') { buffer[j] = text[i]; j++; }
        }
    }

    return buffer;
}

// Get text between two strings
char *GetTextBetween(const char *text, const char *begin, const char *end)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    int beginIndex = TextFindIndex(text, begin);

    if (beginIndex > -1)
    {
        int beginLen = TextLength(begin);
        int endIndex = TextFindIndex(text + beginIndex + beginLen, end);

        if (endIndex > -1)
        {
            endIndex += (beginIndex + beginLen);
            int len = (endIndex - beginIndex - beginLen);
            if (len < (MAX_TEXT_BUFFER_LENGTH - 1)) memcpy(buffer, text + beginIndex + beginLen, len);
            else snprintf(buffer, MAX_TEXT_BUFFER_LENGTH, "%s", text + beginIndex + beginLen);
        }
    }

    return buffer;
}

// Replace text string
// REQUIRES: strstr(), memcpy()
// NOTE: Limited text replace functionality, using static string
char *TextReplace(const char *text, const char *search, const char *replacement)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if ((text != NULL) && (search != NULL) && (search[0] != '\0'))
    {
        if (replacement == NULL) replacement = "";

        char *insertPoint = NULL;   // Next insert point
        char *tempPtr = NULL;       // Temp pointer
        int textLen = 0;            // Text string length
        int searchLen = 0;          // Search string length of (the string to remove)
        int replaceLen = 0;         // Replacement length (the string to replace by)
        int lastReplacePos = 0;     // Distance between next search and end of last replace
        int count = 0;              // Number of replacements

        textLen = TextLength(text);
        searchLen = TextLength(search);
        replaceLen = TextLength(replacement);

        // Count the number of replacements needed
        insertPoint = (char *)text;
        for (count = 0; (tempPtr = strstr(insertPoint, search)); count++) insertPoint = tempPtr + searchLen;

        if ((textLen + count*(replaceLen - searchLen)) < (MAX_TEXT_BUFFER_LENGTH - 1))
        {
            // TODO: Allow copying data replaced up to maximum buffer size and stop

            tempPtr = buffer; // Point to result start

            // First time through the loop, all the variable are set correctly from here on,
            //  - 'temp' points to the end of the result string
            //  - 'insertPoint' points to the next occurrence of replace in text
            //  - 'text' points to the remainder of text after "end of replace"
            while (count > 0)
            {
                insertPoint = strstr(text, search);
                lastReplacePos = (int)(insertPoint - text);

                memcpy(tempPtr, text, lastReplacePos);
                tempPtr += lastReplacePos;

                if (replaceLen > 0)
                {
                    memcpy(tempPtr, replacement, replaceLen);
                    tempPtr += replaceLen;
                }

                text += (lastReplacePos + searchLen); // Move to next "end of replace"
                count--;
            }

            // Copy remaind text part after replacement to result (pointed by moving temp)
            // NOTE: Text pointer internal copy has been updated along the process
            memcpy(tempPtr, text, TextLength(text));
        }
        else TRACELOG(LOG_WARNING, "Text with replacement is longer than internal buffer, use TextReplaceAlloc()");
    }

    return buffer;
}

// Replace text string
// REQUIRES: strstr(), memcpy()
// WARNING: Allocated memory must be manually freed
char *TextReplaceAlloc(const char *text, const char *search, const char *replacement)
{
    char *result = NULL;

    if ((text != NULL) && (search != NULL) && (search[0] != '\0'))
    {
        if (replacement == NULL) replacement = "";

        char *insertPoint = NULL;   // Next insert point
        char *temp = NULL;          // Temp pointer
        int textLen = 0;            // Text string length
        int searchLen = 0;          // Search string length of (the string to remove)
        int replaceLen = 0;         // Replacement length (the string to replace by)
        int lastReplacePos = 0;     // Distance between next search and end of last replace
        int count = 0;              // Number of replacements

        textLen = TextLength(text);
        searchLen = TextLength(search);
        replaceLen = TextLength(replacement);

        // Count the number of replacements needed
        insertPoint = (char *)text;
        for (count = 0; (temp = strstr(insertPoint, search)); count++) insertPoint = temp + searchLen;

        // Allocate returning string and point temp to it
        int tempLen = textLen + (replaceLen - searchLen)*count + 1;
        temp = result = (char *)calloc(tempLen, sizeof(char));

        if (result != NULL) // Memory was allocated
        {
            // First time through the loop, all the variable are set correctly from here on,
            //  - 'temp' points to the end of the result string
            //  - 'insertPoint' points to the next occurrence of replace in text
            //  - 'text' points to the remainder of text after "end of replace"
            while (count > 0)
            {
                insertPoint = strstr(text, search);
                lastReplacePos = (int)(insertPoint - text);

                memcpy(temp, text, lastReplacePos);
                temp += lastReplacePos;

                if (replaceLen > 0)
                {
                    memcpy(temp, replacement, replaceLen);
                    temp += replaceLen;
                }

                text += (lastReplacePos + searchLen); // Move to next "end of replace"

                count--;
            }

            // Copy remaind text part after replacement to result (pointed by moving temp)
            // NOTE: Text pointer internal copy has been updated along the process
            memcpy(temp, text, TextLength(text));
        }
    }

    return result;
}

// Replace text between two specific strings
// REQUIRES: strncpy()
// NOTE: If (replacement == NULL) removes "begin"[ ]"end" text
char *TextReplaceBetween(const char *text, const char *begin, const char *end, const char *replacement)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if ((text != NULL) && (begin != NULL) && (end != NULL))
    {
        int beginIndex = TextFindIndex(text, begin);

        if (beginIndex > -1)
        {
            int beginLen = TextLength(begin);
            int endIndex = TextFindIndex(text + beginIndex + beginLen, end);

            if (endIndex > -1)
            {
                endIndex += (beginIndex + beginLen);

                int textLen = TextLength(text);
                int replaceLen = (replacement == NULL)? 0 : TextLength(replacement);
                //int toreplaceLen = endIndex - beginIndex - beginLen;

                if ((beginIndex + beginLen + replaceLen + (textLen - endIndex)) < (MAX_TEXT_BUFFER_LENGTH - 1))
                {
                    strncpy(buffer, text, beginIndex + beginLen); // Copy first text part
                    if (replacement != NULL) strncpy(buffer + beginIndex + beginLen, replacement, replaceLen); // Copy replacement (if provided)
                    strncpy(buffer + beginIndex + beginLen + replaceLen, text + endIndex, textLen - endIndex); // Copy end text part
                }
                else TRACELOG(LOG_WARNING, "TEXT: Text with replaced string is longer than internal buffer (MAX_TEXT_BUFFER_LENGTH)");
            }
        }
    }

    return buffer;
}

// Replace text between two specific strings
// REQUIRES: strncpy()
// NOTE: If (replacement == NULL) remove "begin"[ ]"end" text
// WARNING: Returned string must be freed by user
char *TextReplaceBetweenAlloc(const char *text, const char *begin, const char *end, const char *replacement)
{
    char *result = NULL;

    if ((text != NULL) && (begin != NULL) && (end != NULL))
    {
        int beginIndex = TextFindIndex(text, begin);

        if (beginIndex > -1)
        {
            int beginLen = TextLength(begin);
            int endIndex = TextFindIndex(text + beginIndex + beginLen, end);

            if (endIndex > -1)
            {
                endIndex += (beginIndex + beginLen);

                int textLen = TextLength(text);
                int replaceLen = (replacement == NULL)? 0 : TextLength(replacement);
                int toreplaceLen = endIndex - beginIndex - beginLen;
                result = (char *)calloc(textLen + replaceLen - toreplaceLen + 1, sizeof(char));

                strncpy(result, text, beginIndex + beginLen); // Copy first text part
                if (replacement != NULL) strncpy(result + beginIndex + beginLen, replacement, replaceLen); // Copy replacement (if provided)
                strncpy(result + beginIndex + beginLen + replaceLen, text + endIndex, textLen - endIndex); // Copy end text part
            }
        }
    }

    return result;
}

// Insert text in a specific position, moves all text forward
// WARNING: Allocated memory must be manually freed
char *TextInsert(const char *text, const char *insert, int position)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);
    int textLen = TextLength(text);

    if ((text != NULL) && (insert != NULL) && (position >= 0))
    {
        if (position > textLen) position = textLen; // End of text string
        int insertLen = TextLength(insert);

        if ((textLen + insertLen) < (MAX_TEXT_BUFFER_LENGTH - 1))
        {
            // TODO: Allow copying data inserted up to maximum buffer size and stop

            for (int i = 0; i < position; i++) buffer[i] = text[i];
            for (int i = 0; i < insertLen; i++) buffer[i+position] = insert[i];
            for (int i = position; i < textLen; i++) buffer[i+insertLen] = text[i];

            buffer[textLen + insertLen] = '\0'; // Add EOL
        }
        else TRACELOG(LOG_WARNING, "Text with inserted string is longer than internal buffer, use TextInserExt()");
    }

    return buffer;
}

// Insert text in a specific position, moves all text forward
// WARNING: Allocated memory must be manually freed
char *TextInsertAlloc(const char *text, const char *insert, int position)
{
    char *result = NULL;
    int textLen = TextLength(text);

    if ((text != NULL) && (insert != NULL) && (position >= 0))
    {
        if (position > textLen) position = textLen; // End of text string
        int insertLen = TextLength(insert);

        result = (char *)malloc(textLen + insertLen + 1);

        for (int i = 0; i < position; i++) result[i] = text[i];
        for (int i = 0; i < insertLen; i++) result[i+position] = insert[i];
        for (int i = position; i < textLen; i++) result[i+insertLen] = text[i];

        result[textLen + insertLen] = '\0'; // Add EOL
    }

    return result;
}

// Join text strings with delimiter
// REQUIRES: memset(), memcpy()
char *TextJoin(char **textList, int count, const char *delimiter)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);
    char *textPtr = buffer;

    int totalLength = 0;
    int delimiterLen = TextLength(delimiter);

    for (int i = 0; i < count; i++)
    {
        int textLength = TextLength(textList[i]);

        // Make sure joined text could fit inside MAX_TEXT_BUFFER_LENGTH
        if ((totalLength + textLength + delimiterLen) < MAX_TEXT_BUFFER_LENGTH)
        {
            memcpy(textPtr, textList[i], textLength);
            totalLength += textLength;
            textPtr += textLength;

            if ((delimiterLen > 0) && (i < (count - 1)))
            {
                memcpy(textPtr, delimiter, delimiterLen);
                totalLength += delimiterLen;
                textPtr += delimiterLen;
            }
        }
    }

    return buffer;
}

// Split string into multiple strings
// REQUIRES: memset()
char **TextSplit(const char *text, char delimiter, int *count)
{
    // NOTE: Current implementation returns a copy of the provided string with '\0' (string end delimiter)
    // inserted between strings defined by "delimiter" parameter. No memory is dynamically allocated,
    // all used memory is static... it has some limitations:
    //      1. Maximum number of possible split strings is set by MAX_TEXTSPLIT_COUNT
    //      2. Maximum size of text to split is MAX_TEXT_BUFFER_LENGTH

    static char *buffers[MAX_TEXTSPLIT_COUNT] = { NULL }; // Pointers to buffer[] text data
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 }; // Text data with '\0' separators
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    buffers[0] = buffer;
    int counter = 0;

    if (text != NULL)
    {
        counter = 1;

        // Count how many substrings ar found on text and set pointers to every one
        // NOTE: Last buffer byte is reserved to terminate the last substring
        for (int i = 0; i < MAX_TEXT_BUFFER_LENGTH - 1; i++)
        {
            buffer[i] = text[i];
            if (buffer[i] == '\0') break;
            else if (buffer[i] == delimiter)
            {
                buffer[i] = '\0';   // Set an end of string at this point
                buffers[counter] = buffer + i + 1;
                counter++;

                if (counter == MAX_TEXTSPLIT_COUNT) break;
            }
        }
    }

    *count = counter;
    return buffers;
}

// Append text at specific position and move cursor
// WARNING: It's up to the user to make sure appended text does not overflow the buffer!
void TextAppend(char *text, const char *append, int *position)
{
    if ((text != NULL) && (append != NULL))
    {
        TextCopy(text + *position, append);
        *position += TextLength(append);
    }
}



// Get upper case version of provided string
// WARNING: Limited functionality, only basic characters set
// TODO: Support UTF-8 diacritics to upper-case, check codepoints
char *TextToUpper(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if (text != NULL)
    {
        for (int i = 0; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[i] != '\0'); i++)
        {
            if ((text[i] >= 'a') && (text[i] <= 'z')) buffer[i] = text[i] - 32;
            else buffer[i] = text[i];
        }
    }

    return buffer;
}

// Get lower case version of provided string
// WARNING: Limited functionality, only basic characters set
char *TextToLower(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if (text != NULL)
    {
        for (int i = 0; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[i] != '\0'); i++)
        {
            if ((text[i] >= 'A') && (text[i] <= 'Z')) buffer[i] = text[i] + 32;
            else buffer[i] = text[i];
        }
    }

    return buffer;
}

// Get Pascal case notation version of provided string
// WARNING: Limited functionality, only basic characters set
char *TextToPascal(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if (text != NULL)
    {
        // Upper case first character
        if ((text[0] >= 'a') && (text[0] <= 'z')) buffer[0] = text[0] - 32;
        else buffer[0] = text[0];

        // Check for next separator to upper case another character
        for (int i = 1, j = 1; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[j] != '\0'); i++, j++)
        {
            if (text[j] != '_') buffer[i] = text[j];
            else
            {
                while (text[j] == '_') j++;     // Skip one or more separators
                if (text[j] == '\0') break;     // Text ends on a separator, nothing left to copy

                if ((text[j] >= 'a') && (text[j] <= 'z')) buffer[i] = text[j] - 32;
                else buffer[i] = text[j];       // Character can not be upper-cased, copy it as is
            }
        }
    }

    return buffer;
}

// Get snake case notation version of provided string
// WARNING: Limited functionality, only basic characters set
char *TextToSnake(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if (text != NULL)
    {
        // Check for next separator to upper case another character
        for (int i = 0, j = 0; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[j] != '\0'); j++)
        {
            if (text[j] == ' ')
            {
                if ((i > 0) && (buffer[i - 1] != '_'))
                {
                    buffer[i] = '_';
                    i++;
                }
            }
            else if ((text[j] >= 'A') && (text[j] <= 'Z'))
            {
                if ((i > 0) && (buffer[i - 1] != '_'))
                {
                    char prev = text[j - 1];
                    char next = text[j + 1];

                    // Considering multiple cap leters to be on single word (HTTPRequest --> http_request)
                    if (((prev >= 'a') && (prev <= 'z')) ||
                        (((prev >= 'A') && (prev <= 'Z')) && ((next >= 'a') && (next <= 'z'))))
                    {
                        if (i < MAX_TEXT_BUFFER_LENGTH - 2)
                        {
                            buffer[i] = '_';
                            i++;
                        }
                    }
                }

                buffer[i] = text[j] + 32;
                i++;
            }
            else
            {
                buffer[i] = text[j];
                i++;
            }
        }
    }

    return buffer;
}

// Get Camel case notation version of provided string
// WARNING: Limited functionality, only basic characters set
char *TextToCamel(const char *text)
{
    static char buffer[MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH);

    if (text != NULL)
    {
        // Lower case first character
        if ((text[0] >= 'A') && (text[0] <= 'Z')) buffer[0] = text[0] + 32;
        else buffer[0] = text[0];

        // Check for next separator to upper case another character
        for (int i = 1, j = 1; (i < MAX_TEXT_BUFFER_LENGTH - 1) && (text[j] != '\0'); i++, j++)
        {
            if (text[j] != '_') buffer[i] = text[j];
            else
            {
                while (text[j] == '_') j++;     // Skip one or more separators
                if (text[j] == '\0') break;     // Text ends on a separator, nothing left to copy

                if ((text[j] >= 'a') && (text[j] <= 'z')) buffer[i] = text[j] - 32;
                else buffer[i] = text[j];       // Character can not be upper-cased, copy it as is
            }
        }
    }

    return buffer;
}
