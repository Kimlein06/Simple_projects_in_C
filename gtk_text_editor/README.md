# GTK Text Editor (C)

A simple GTK-based text editor with undo/redo, file open/save, and live word & character count — built in C.

## Features
- Undo/Redo using stack logic
- Manual Commit system
- Open and Save to text files
- Live word and character stats
- Built with GTK 3

## Build Instructions

```bash
gcc main.c -o gtk_editor `pkg-config --cflags --libs gtk+-3.0`
./gtk_editor
