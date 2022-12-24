## LOGFUG

Az argumentumként megadott logikai függvényt megoldja minden négy biten ábrázolható számra, az elkészült igazságtáblát kiírja.

Példa:

```
$ ./logfug -r a~b
--      DCBA    -
0       0000    0
1       0001    1
2       0010    0
3       0011    0
4       0100    0
5       0101    1
6       0110    0
7       0111    0
8       1000    0
9       1001    1
10      1010    0
11      1011    0
12      1100    0
13      1101    1
14      1110    0
15      1111    0
```

Támogatja a hanyagul megadott logikai függvényeket is, az *ÉS* műveletet nem kell jelölni.

Szintax:
```
~   nem
*   és
+   vagy
```

### Fordítás:
Talán/remélhetőleg a *NIX rendszereken ez megteszi:
```
cc -o logfug logfug.c
```
