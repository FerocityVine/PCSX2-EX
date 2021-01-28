# ExPATCH Documentation v2.0

---

## What is ExPATCH?
ExPATCH (Short for **Ex**tended **PATCH**) is an extension to the PNACH system used in PCSX2. It adds some extra functions from CodeBreaker, fixes a few bugs, and improves performance a bit.

## How do I use ExPATCH, and how can I make patches for it?
In the same way as a PNACH file.

ExPATCH patches are basically just differently formatted, RAW GameShark Codes.

You should know that:
- ExPATCH, like LuaEngine, has an address range of 0x0000000 to 0x2000000. While emulated EE memory begins at 0x20000000, you needn't shift addresses to this region.
- Codes follow a set number of defined formats.

You can find the cheatsheet for ExPATCH below.

## ExPATCH Cheatsheet
```
0AAAAAAA 000000VV -> Writes value V into address A (8-bit Write)

1AAAAAAA 000000VV -> Writes value V into address A (16-bit Write)

2AAAAAAA 000000VV -> Writes value V into address A (32-bit Write)

30X000VV 0AAAAAAA -> Modifies the value of address A with the operator X by the value V (8-bit)
Operations (X): 0 - Increase (8-bit), 1 - Decrease (8-Bit), 2 - Increase (16-bit), 3 - Decrease (16-bit)

30X00000 0AAAAAAA |-> Modifies the value of address A with the operator X by the value V (32-bit)
VVVVVVVV 00000000 |
Operations (X): 4 - Increase, 5 - Decrease

4AAAAAAA TTTTBBBB |-> Writes value V into address A + (B * (4 * T))
VVVVVVVV 00000000 | in a loop T times.

5FFFFFFF NNNNNNNN |-> Copies the N amount of bytes from address F to address T
0TTTTTTT 00000000 |

6AAAAAAA VVVVVVVV |-> Writes value V into the pointed address of A + O, X
000X0000 OOOOOOOO | determines data depth (0 = 8-bit, 1 = 16-bit 2 = 32-bit)

7AAAAAAA 000XVVVV -> Performs a bitwise operation with the value stored at address
A and value V, and writes it back to address A.
Operations (X): 0 = 8-Bit OR, 1 = 16-Bit OR, 2 = 8-Bit AND, 3 = 16-Bit AND, 4 = 8-Bit XOR
5 = 16-Bit XOR

8AAAAAAA OOOOOOOO |-> Copies the value at the pointed address of A + O into
0TTTTTTT 00000000 |  address T

9AAAAAAA 0TTTTTTT -> Copies the value stored at address A to address T
 
ADDDDDDD 00000000 |-> Copies the value stored at address D to the pointed
0PPPPPPP 0IIIIIII |  address of P + I 

BXXSSSSS 00000000 -> Activates X amount of lines every S milliseconds (Frame-Calculated).

CAAAAAAA 00000000 |-> Compares the value at address A with the value at
0TTTTTTT YZXXXXXX |  address T. If condition Z is true, it activates X amount of lines.
Value Type (Y): 0 = 32-Bit, 1 = 16-Bit, 2 = 8-Bit
Operations (Z): 0 = Equal, 1 = Not Equal, 2 = Lesser Than, 3 = Greater Than, 4 = Lesser or Equal, 5 = Greater or Equal

DAAAAAAA 00XXVVVV -> Checks the value at address A and compares it to value V. If
condition X is true, it activates 1 line of code.
Operations (X): 00 = Equal, 10 = Not Equal, 20 = Lesser Than, 30 = Greater Than, 40 = Lesser or Equal, 50 = Greater or Equal

EZNNVVVV XAAAAAAA -> Checks the value at address A and compares it to value V. If the
condition X is true, it activates N amount of lines.
Value Type (Z): 0 = 16-Bit, 1 = 8-Bit
Operations (X): 0 = Equal , 1 = Not Equal, 2 = Lesser Than, 3 = Greater Than, 4 = Lesser or Equal, 5 = Greater or Equal
```

# Example Code:

As an example, this block of code:

```
patch=1,EE,E0041A04,extended,0032BAE0
patch=1,EE,E0030001,extended,0032BAE4
patch=1,EE,E0020001,extended,0032BAE8
patch=1,EE,01C6C754,extended,00000001
patch=1,EE,01C6C750,extended,00000001
```

Translates to:
IF the value at address 0x032BAE0 is equal to 0x1A04; run the next 4 lines.
Then IF the value at address 0x032BAE4 is equal to 0x0001; run the next 3 lines.
Then IF the value at address 0x032BAE8 is equal to 0x0001; run the next 2 lines.
Then set the byte at 0x1C6C754 to 01.
Then set the byte at 0x1C6C750 to 01.

If at any point one of those conditions is not true the last 2 lines do nothing.

---

Special thanks to [Num](https://github.com/1234567890num) for writing most of this documentation.