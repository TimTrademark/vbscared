# Scary VBS Stuff

## Compile resources

```
windres resources.rc -O coff -o resources.res
```

## Compile executable

```
gcc runscript.c -o runscript.exe .\resources.res -mwindows
```

Now all that's left is to change the name of the executable and wait for the bait
