# EXIB - Serialization Format
<p style="border-radius: 3px; border-bottom: 4px solid gray"></p>

## <p style="border-radius: 2px; border-bottom: 3px solid gray">Format</p>
### <p style="border-radius: 2px; border-bottom: 3px solid gray">Header</p>
Each EXIB object begins with an 8-byte header consisting of 4 fields. All values in
an EXIB are in network byte order (big-endian). 
```c++
struct EXIBHeader
{
    exib_uint16_t Signature;
    exib_uint8_t  Major;
    exib_uint8_t  Minor;
    exib_uint32_t SymbolTable;
};
```
The first two bytes of the header make up a 16-bit signature with the value 
of `0xE71B`. The third header byte contains 
the major version of the format, and the fourth byte contains the minor version. The 
final four bytes of the header contain an offset to the EXIB symbol table, relative to
the beginning of the header.

### <p style="border-radius: 2px; border-bottom: 3px solid gray">Data</p>
An EXIB object consists of fields and other object definitions. Each field begins with
an 8-bit field prefix.
```c++
struct EXIBFieldPrefix
{
    uint8_t Root       : 1;
    uint8_t Array      : 1;
    uint8_t ClassValue : 1;
    uint8_t ClassRef   : 1;
    uint8_t Named      : 1;
};
```

The first field in every EXIB object is known as the root object. Since the root object 
has a normal field prefix, it can have any type. If the `ClassValue` or `ClassRef` 
bits are set in a field prefix, a 16-bit symbol table index will follow it. Otherwise,
the type field is 8-bits. If the `Named` bit is set, another 16-bit symbol table
index will follow the type field.

```
[EXIBFieldPrefix] [Type|TypeSymbol] (NameSymbol) [ScopeLength|Value]
```

