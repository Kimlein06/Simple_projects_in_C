#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1000
#define STACK_SIZE 100

// Stack structure
char* undoStack[STACK_SIZE];
char* redoStack[STACK_SIZE];
int undoTop = -1, redoTop = -1;

// Push to undo stack
void pushUndo(char* text) {
    if (undoTop < STACK_SIZE - 1) {
        undoStack[++undoTop] = strdup(text);
    }
}

// Pop from undo stack
char* popUndo() {
    if (undoTop >= 0) {
        return undoStack[undoTop--];
    }
    return NULL;
}

// Push to redo stack
void pushRedo(char* text) {
    if (redoTop < STACK_SIZE - 1) {
        redoStack[++redoTop] = strdup(text);
    }
}

// Pop from redo stack
char* popRedo() {
    if (redoTop >= 0) {
        return redoStack[redoTop--];
    }
    return NULL;
}

int main() {
    char text[MAX_LEN] = "";
    char input[MAX_LEN];
    int choice;

    while (1) {
        printf("\n--- Text Editor ---\n");
        printf("1. Type Text\n2. Undo\n3. Redo\n4. Show Text\n5. Exit\n");
        printf("Choose: ");
        scanf("%d", &choice);
        getchar(); // clear newline

        switch (choice) {
            case 1:
                printf("Enter text to add: ");
                fgets(input, MAX_LEN, stdin);
                input[strcspn(input, "\n")] = 0; // remove newline

                char choice;
                printf("Start a new line? (y/n): ");
                scanf(" %c", &choice);
                getchar(); // clear newline again

                pushUndo(text); // Save current state

                if (choice == 'y' || choice == 'Y') {
                    strcat(text, "\n"); // actual newline
                }

                strcat(text, input); // Append new text
                redoTop = -1; // Clear redo stack
                break;


            case 2: {
                char* prev = popUndo();
                if (prev) {
                    pushRedo(text); // Save current state to redo
                    strcpy(text, prev); // Revert to previous
                    free(prev);
                } else {
                    printf("Nothing to undo!\n");
                }
                break;
            }

            case 3: {
                char* redo = popRedo();
                if (redo) {
                    pushUndo(text);
                    strcpy(text, redo);
                    free(redo);
                } else {
                    printf("Nothing to redo!\n");
                }
                break;
            }

            case 4:
                printf("Current Text: %s\n", text);
                break;

            case 5:
                exit(0);

            default:
                printf("Invalid option!\n");
        }
    }

    return 0;
}
