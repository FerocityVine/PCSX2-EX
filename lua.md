## What is LuaEngine?

LuaEngine is a version of Lua running within PCSX2-EX, powered by sol2. It allows for the use of Lua scripts as a method of modifying any game in question.
 
## How do I use LuaEngine mod that I have downloaded?

By default, PCSX2-EX makes a folder called "**lua**" in the PCSX2 folder in the Documents folder. 
To use a LuaEngine script, all you have to do is to drop that script inside that folder.
However, LuaEngine also has I/O capability. So if a mod you have downloaded also has "**io_load**" or "**io_packages**" directories included, 
you would need to drop those folders to that "**lua**" folder as well.

## How do I use LuaEngine to create a mod myself?

First, you should get the CRC32 of the game you want to mod. It should be in the PCSX2 Console's title alongside the game title. 
Create a new file in the aforementioned "**lua**" folder and name it "**XXXXXXXX.lua**". Replace the several X letters with the game's CRC32. 
For example; For Kingdom Hearts 2 Final Mix, it would be "**F266B00B.lua**".

Inside the file, copy and paste this snippet:

```lua
function _OnInit()
end

function _OnFrame()
end
```

The script in question should have at least one of these functions to work. The "**_OnInit**" function is executed when the script is first loaded, 
and the "**_OnFrame**" function is executed every frame. So put the things you want to happen every frame into the "**_OnFrame**" function.

That is the minimum you would need to have a functioning script. The rest is on you. The scripts in LuaEngine function just as if they were regular Lua scripts.
Because... they kind of are! Below you can find the cheatsheet to all PCSX2-EX specific functions.

## How do I use libraries made for Lua with LuaEngine?

The libraries you would need to use must be 5.4 compliant. In addition, if the libraries need to be compiled, they must be compiled
for x86 and with Lua 5.4, and on the platform that you are on, with the copiler that compiled PCSX2-EX
(All release versions of PCSX2-EX are compiled with MSVC2019 for Windows x86)

## Important Notes to using LuaEngine

- Currently, because of a bug within PCSX2 itself, saving a state and toggling the framelimiter resets LuaEngine. This is being worked on.
- LuaEngine, like ExPATCH, has an address range of 0x0000000 to 0x2000000. While emulated EE memory begins at 0x20000000, you needn't shift addresses to this region.
- All values are unsigned.
- There is no limit for the amount of scripts loaded at this moment.
- LuaEngine can work in conjunction with ExPATCH.


## LuaEngine Cheatsheet

## Global Variables

- ``GAME_ID`` => Returns the current game's CRC32 checksum as an integer. Ex: ``0xF266B00B``
- ``SCRIPT_PATH`` => Returns the path which Lua scripts are loaded from as a string. Ex: ``"C:\Users\Topaz\Documents\PCSX2\lua"``
- ``CHEATS_PATH`` => Returns the path which ExPATCH files are loaded from as a string. Ex: ``"C:\Users\Topaz\Documents\PCSX2\cheats"``
- ``ENGINE_VERSION`` => Returns the LuaEngine's version as a float. Ex: ``2.0``


## ReadXXXX(Address)

Reads a certain value from **Address**. Returns the read value. 
The "**XXXX**" is a placeholder for the value type.
List of types available: Byte, Short, Int, Long, Float, String, Boolean

Example:
```lua
    local _tempInt = ReadInt(0xDEADB00) -- Read an integer from address 0xDEADB00
    local _tempByte = ReadByte(0xB007555) -- Read a byte from address 0xB007555
```

## WriteXXXX(Address, Value)

Writes **Value** to **Address**. 
The "**XXXX**" is a placeholder for the value type.
List of types available: Byte, Short, Int, Long, Float, String, Boolean

Example:
```lua
    WriteShort(0xDEADB00, 0xFFFF) -- Write 0xFFFF to 0xDEADB00
    WriteBoolean(0xB007555, true) -- Write true to 0xB007555
```

## ReadArray(Address, Length)

Reads **Length** amount of bytes from memory, starting at **Address**. Returns the read bytes as an array.

Example:
```lua
    local _tempArray = ReadArray(0xBEEFDED, 0x10) -- Reads 16 bytes starting at 0xBEEFDED
```

## WriteArray(Address, Array)

Writes **Array** to the memory, starting at **Address**. The array must be made of bytes (8-bit integers).

Example:
```lua
    local _tempArray = { 0xFE, 0xA5, 0x70 }
    WriteArray(0x6660420, _tempArray) -- Writes _tempArray to memory, starting at 0x6660420
```

## ReadFile(FileName)

Reads the file "**lua/io_load/FileName**" to an array.

Example:
```lua
    local _tempFile = ReadFile("killMePl0x.kek") -- Reads the file at lua/io_load/killMePl0x.kek to _tempFile. Re-reading the file is unnecessary from this point.
```
## ReadFile(FileName, Offset, Size)

Reads the **Size** bytes from "**lua/io_load/FileName**" to an array, starting from **Offset** .

Example:
```lua
    local _tempFile = ReadFile("killMePl0x.kek", 0x0069, 0x0420) -- Reads 0x0420 bytes from the file at lua/io_load/killMePl0x.kek to _tempFile, starting from 0x0069
```
## GetPointer(Address, Offset)

Calculates a pointer with the given values. Basically, reads the value at **Address** and adds **Offset** to it. Returns the resulting value.

Example:
```lua
    local _tempPointer = GetPointer(0xBEEFDAD, 0x15) -- Reads the value at 0xBEEFDAD and adds 0x15 to it.
```

## ConsolePrint(Message, Color = 1)

Prints unto the PCSX2-EX console. The color is normally 1.

The available colors are, starting from 1:
Black, Green, Red, Blue, Magenta, Orange, Gray, Cyan, Yellow, White, StrongBlack, StrongGreen, StrongRed, StrongBlue, StrongMagenta, StrongOrange, StrongGray, StrongCyan, StrongYellow, StrongWhite

Example:
```lua
    ConsolePrint("Howdy partner?", 0x05) -- Prints to the PCSX2-EX Console: "Howdy partner?" in Magenta.
```

### Any functions present in this document signify v2.25.0 compatibility. Any function absent in this document is no longer supported and should be considered obsolete.