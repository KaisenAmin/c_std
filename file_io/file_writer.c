#include "file_writer.h"
#include "../encoding/encoding.h"
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#endif 


FileWriter* file_writer_open(const char* filename, const WriteMode mode) {
    if (!filename) {
        fprintf(stderr, "Error: filename is null in file_writer_open.\n");
        return NULL;
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        fprintf(stderr, "Error: Can not allocate memory for FileWriter in file_writer_open.\n");
        return NULL;
    }

    const char* modeStr = NULL;

    switch (mode){
    case WRITE_TEXT:
        modeStr = "w";
        break;
    case WRITE_BINARY:
        modeStr = "wb";
        break;
    case WRITE_UNICODE:
        #if defined(_WIN32) || defined(_WIN64)
        modeStr = "w, ccs=UTF-8";
        #else
        modeStr = "w";
        #endif 
        break;
    case WRITE_BUFFERED:
        modeStr = "w";
        break;
    case WRITE_UNBUFFERED:
        modeStr = "w";
        break;
    default:
        fprintf(stdout, "Warning: Not Valid mode for writing in file_writer_open i initialize default mode that is 'w'.\n");
        break;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFileName = encoding_utf8_to_wchar(filename);
    wchar_t* wMode = encoding_utf8_to_wchar(modeStr);

    if (!wMode) {
        fprintf(stderr, "Error: Can not convert mode to wchar in file_writer_open.\n");
        return NULL;
    }
    if (!wFileName) {
        fprintf(stderr, "Error: Can not convert filename to wchar in file_writer_open.\n");
        return NULL;
    }
    writer->file_writer = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else 
    writer->file_writer = fopen(filename, modeStr);
    #endif 

    if (writer->file_writer == NULL) {
        fprintf(stderr, "Error: Can not open file in file_writer_open.\n");
        free(writer);
        return NULL;
    }
    writer->mode = mode;
    return writer;
}

bool file_writer_close(FileWriter *writer)
{
    if (writer->file_writer == NULL) {
        fprintf(stdout, "Warning: Right now the file is NULL no need to close it in file_writer_close.\n");
        return false;
    }
    if (fclose(writer->file_writer)) {
        fprintf(stderr, "Error: Failed to close file in file_writer_close.\n");
        return false;
    }
    return true;
} 

size_t file_writer_write(void* buffer, size_t size, size_t count, FileWriter* writer) {
    if (!writer || !writer->file_writer || !buffer) {
        fprintf(stderr, "Error: Invalid argument in file_writer_write.\n");
        return 0;
    }

    size_t written = 0;

    #if defined(_WIN32) || defined(_WIN64)
    // For Unicode mode, convert buffer to wchar_t before writing
    if (writer->mode == WRITE_UNICODE) {
        wchar_t* wBuffer = encoding_utf8_to_wchar((const char*)buffer);
        if (!wBuffer) {
            fprintf(stderr, "Error: Conversion to wchar failed in file_writer_write.\n");
            return 0;
        }
        written = fwrite(wBuffer, sizeof(wchar_t), wcslen(wBuffer), writer->file_writer);
        free(wBuffer);
    } 
    else {
        written = fwrite(buffer, size, count, writer->file_writer);
    }
    #else
    // On non-Windows systems, write the buffer as is
    written = fwrite(buffer, size, count, writer->file_writer);
    #endif

    return written;
}