# Scary VBS Stuff

## Compile resources

```
windres resources.rc -O coff -o resources.res
```

## Compile executable

```
cl runscript.c /DEBUG:NONE /link /subsystem:windows /entry:mainCRTStartup
```

Now all that's left is to change the name of the executable and wait for the bait
