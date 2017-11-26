#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 4096
#define LAST_FD 10      // Last supported file descriptor (3 <= x <= LAST_FD)

#define ERR_IMBALANCE   -1
#define ERR_EOF         -2
#define ERR_WRITE       -3
#define ERR_FLUSH       -4

struct Stream {
    FILE          *file;
    struct Stream *next;
    int            index;
    int            status;
};

struct Stream *head;
struct Stream *tail;

/*
    Initializes the linked list with stdout as head
*/
int list_init() {
    struct Stream *item = (struct Stream*)malloc(sizeof(struct Stream));
    
    if (item == NULL) {
        return -1;
    }
    
    item->file = stdout;
    item->next = NULL;
    item->index = 1;
    item->status = 0;
    
    head = item;
    tail = item;
    
    return 0;
}

/*
    Adds an item to the linked list
*/
int list_add(FILE *file, int index) {
    struct Stream *item = (struct Stream*)malloc(sizeof(struct Stream));
    
    if (item == NULL) {
        return -1;
    }
    
    item->file = file;
    item->next = NULL;
    item->index = index;
    item->status = 0;
    
    tail->next = item;
    tail = item;
    
    return 0;
}

/*
    Clears the linked list and frees any allocated memory
*/
void list_clear() {
    struct Stream *item = head;
    
    while (item != NULL) {
        if (item->file != NULL) {
            fclose(item->file);
            item->file = NULL;
        }
        
        free(item);
        
        item = item->next;
    }
}

/*
    Tries to open all streams up to LAST_FD for writing and adds them to the list
*/
int open_streams() {
    int i = 0;
    int added = 0;
    int result = 0;
    FILE *file = NULL;
    
    for (i = 3; i <= LAST_FD; i++) {
        file = fdopen(i, "wb");
        
        if (file == NULL) {
            continue;
        }
        
        added = list_add(file, i);
        
        if (added != 0) {
            continue;
        }
        
        result++;
    }
    
    return result;
}

/*
    Reads from stdin and writes the buffer to the output streams
*/
void amplify(int streams) {
    struct Stream *current = head;
    unsigned char buffer[BUFFER_SIZE];
    size_t read = 0;
    size_t written = 0;
    int active_streams = streams;
    
    while ((read = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, stdin)) > 0 && active_streams > 0) {
        current = head;
        
        while (current != NULL) {
            if (current->status != 0) {
                continue;
            }
            
            written = fwrite(buffer, sizeof(unsigned char), read, current->file);
            
            if (read != written) {
                current->status = ERR_IMBALANCE;
            }
            
            if (feof(current->file) != 0) {
                current->status = ERR_EOF;
                clearerr(current->file);
            }
            
            if (ferror(current->file) != 0) {
                current->status = ERR_WRITE;
                clearerr(current->file);
            }
            
            if (current->status != ERR_WRITE) {
                fflush(current->file);
                
                if (ferror(current->file) != 0) {
                    current->status = ERR_FLUSH;
                    clearerr(current->file);
                }
            }
            
            if (current->status != 0) {
                active_streams--;
            }
            
            current = current->next;
        }
    }
}

int main(int argc, char **argv) {
    int err = 0;
    int streams = 0;
    
    err = list_init();
    
    if (err != 0) {
        goto exit;
    }
    
    streams = open_streams();
    
    amplify(streams);
    
    list_clear();

exit:
    return err;
}