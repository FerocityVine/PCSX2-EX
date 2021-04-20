/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PrecompiledHeader.h"

#define _PC_ // disables MIPS opcode macros.

#include "IopCommon.h"
#include "Patch.h"

u32 _frameInt = 0;
u32 _lineSkip = 0;
u32 _writeType = 0;
u32 _cheatType = 0;
u32 _cheatFlag = 0;
u32 _cheatAddress = 0;
u32 _iteration = 0;
u32 _iterationInc = 0;
u32 _valueInc = 0;

void writeCheat()
{
    switch (_writeType) {
        case 0x0:
            memWrite8(_cheatAddress, _iterationInc & 0xFF);
            break;
        case 0x1:
            memWrite16(_cheatAddress, _iterationInc & 0xFFFF);
            break;
        case 0x2:
            memWrite32(_cheatAddress, _iterationInc);
            break;
        default:
            break;
    }
}

void handle_extended_t(IniPatch *_input)
{
    if (_lineSkip > 0)
        _lineSkip--;

    else if (_cheatType > 0) {
        switch (_cheatType) {
            case 0x30: {
                const u32 _value = memRead32(_cheatAddress);

                if (_cheatFlag == 0x40)
                    memWrite32(_cheatAddress, _value + (_input->addr));
                else
                    memWrite32(_cheatAddress, _value - (_input->addr));

                _cheatType = 0;
                _cheatFlag = 0;
            } break;

            case 0x40: {
                for (u32 i = 0; i < _iteration; i++) {
                    const u32 _address = static_cast<u32>(_cheatAddress) + i * _iterationInc;
                    const u32 _value = static_cast<u32>(_input->addr) + _input->data * i;
                    memWrite32(_address, _value);
                }

                _cheatType = 0;
            } break;

            case 0x50: {
                for (u32 i = 0; i < _iteration; i++) {
                    const u8 _value = memRead8(_cheatAddress + i);
                    memWrite8((_input->addr + i) & 0x0FFFFFFF, _value);
                }

                _cheatType = 0;
            } break;

            case 0x60: {
                if (_cheatFlag == 0x10) {
                    u32 _value = memRead32(_cheatAddress & 0x0FFFFFFF);

                    _cheatAddress = _value + static_cast<u32>(_input->addr);
                    _iteration--;

                    if (_iteration == 0) {
                        _cheatType = 0;
                        _cheatFlag = 0;

                        if (((_value & 0x0FFFFFFF) & 0x3FFFFFFC) != 0)
                            writeCheat();
                    }

                    else {
                        _value = memRead32(_cheatAddress);

                        _cheatAddress = _value + static_cast<u32>(_input->data);
                        _iteration--;

                        if (_iteration == 0) {
                            _cheatType = 0;
                            _cheatFlag = 0;

                            if (((_value & 0x0FFFFFFF) & 0x3FFFFFFC) != 0)
                                writeCheat();
                        }
                    }
                }

                else {
                    if (((u32)_input->addr & 0x0000FFFF) == 0)
                        _iteration = 1;

                    else
                        _iteration = (u32)_input->addr & 0x0000FFFF;

                    _writeType = ((u32)_input->addr & 0x000F0000) >> 16;
                    const u32 _value = memRead32(_cheatAddress);

                    _cheatAddress = _value + (u32)_input->data;
                    _iteration--;

                    if (_iteration == 0) {
                        _cheatType = 0;
                        if (((_value & 0x0FFFFFFF) & 0x3FFFFFFC) != 0)
                            writeCheat();
                    }

                    else {
                        if (((_value & 0x0FFFFFFF) & 0x3FFFFFFC) == 0) {
                            _cheatType = 0;
                            _cheatFlag = 0;
                        }

                        else {
                            _cheatType = 0x60;
                            _cheatFlag = 0x10;
                        }
                    }
                }
            } break;

            case 0x80: {
                const u32 _value = memRead32(_cheatAddress);
                memWrite32(static_cast<u32>(_input->addr) & 0x0FFFFFFF, _value);

                _cheatType = 0;
            } break;

            case 0xA0: {
                u32 _address = memRead32(static_cast<u32>(_input->addr) & 0x0FFFFFFF) + static_cast<u32>(_input->data);
                u32 _data = memRead32(_cheatAddress);

                memWrite32(_address, _data);
                _cheatType = 0;
            } break;

            case 0xC0: {
                u32 _typeData = (u32)_input->data & 0xF0000000;
                u32 _typeComp = (u32)_input->data & 0x0F000000;
                u32 _lineNumber = (u32)_input->data & 0x00FFFFFF;

                u32 _value1 = 0;
                u32 _value2 = 0;
                switch (_typeData) {
                    case 0x00000000:
                        _value1 = memRead32(_cheatAddress);
                        _value2 = memRead32(static_cast<u32>(_input->addr) & 0x0FFFFFFF);
                        break;
                    case 0x10000000:
                        _value1 = memRead16(_cheatAddress);
                        _value2 = memRead16(static_cast<u32>(_input->addr) & 0x0FFFFFFF);
                        break;
                    case 0x20000000:
                        _value1 = memRead8(_cheatAddress);
                        _value2 = memRead8(static_cast<u32>(_input->addr) & 0x0FFFFFFF);
                        break;
                    default:
                        break;
                }

                switch (_typeComp) {
                    case 0x0000000:
                        if (_value1 != _value2)
                            _lineSkip = _lineNumber;
                        break;
                    case 0x1000000:
                        if (_value1 == _value2)
                            _lineSkip = _lineNumber;
                        break;
                    case 0x2000000:
                        if (_value1 >= _value2)
                            _lineSkip = _lineNumber;
                        break;
                    case 0x3000000:
                        if (_value1 <= _value2)
                            _lineSkip = _lineNumber;
                        break;
                    case 0x4000000:
                        if (_value1 > _value2)
                            _lineSkip = _lineNumber;
                        break;
                    case 0x5000000:
                        if (_value1 < _value2)
                            _lineSkip = _lineNumber;
                        break;
                    default:
                        break;
                }

                _cheatType = 0;
            } break;
        }
    } else
        switch (_input->addr & 0xF0000000) {
            case 0x00000000:
                memWrite8(_input->addr & 0x0FFFFFFF, static_cast<u8>(_input->data) & 0x000000FF);
                break;

            case 0x10000000:
                memWrite16(_input->addr & 0x0FFFFFFF, static_cast<u16>(_input->data) & 0x0000FFFF);
                break;

            case 0x20000000:
                memWrite32(_input->addr & 0x0FFFFFFF, static_cast<u32>(_input->data));
                break;

            case 0x30000000: {
                u8 _value8 = memRead8(_input->data);
                u16 _value16 = memRead16(_input->data);

                switch (_input->addr & 0x00F00000) {
                    case 0x000000:
                        memWrite8(_input->data & 0x0FFFFFFF, _value8 + (_input->addr & 0x000000FF));
                        break;
                    case 0x100000:
                        memWrite8(_input->data & 0x0FFFFFFF, _value8 - (_input->addr & 0x000000FF));
                        break;
                    case 0x200000:
                        memWrite16(_input->data & 0x0FFFFFFF, _value16 + (_input->addr & 0x0000FFFF));
                        break;
                    case 0x300000:
                        memWrite16(_input->data & 0x0FFFFFFF, _value16 - (_input->addr & 0x0000FFFF));
                        break;
                    case 0x400000:
                        _cheatType = 0x30;
                        _cheatFlag = 0x40;
                        _cheatAddress = _input->data;
                        break;
                    case 0x500000:
                        _cheatType = 0x30;
                        _cheatFlag = 0x50;
                        _cheatAddress = _input->data;
                        break;
                }
            } break;

            case 0x40000000:
                _iteration = (static_cast<u32>(_input->data) & 0xFFFF0000) / 0x10000;
                _iterationInc = (static_cast<u32>(_input->data) & 0x0000FFFF) * 4;
                _cheatAddress = static_cast<u32>(_input->addr) & 0x0FFFFFFF;
                _cheatType = 0x40;
                break;

            case 0x50000000:
                _iteration = _input->data;
                _cheatAddress = static_cast<u32>(_input->addr) & 0x0FFFFFFF;
                _cheatType = 0x50;
                break;

            case 0x60000000:
                _iteration = 0;
                _iterationInc = static_cast<u32>(_input->data);
                _cheatAddress = static_cast<u32>(_input->addr) & 0x0FFFFFFF;
                _cheatType = 0x60;
                break;

            case 0x70000000: {
                u8 _value8 = memRead8(static_cast<u32>(_input->addr) & 0x0FFFFFFF);
                u16 _value16 = memRead16(static_cast<u32>(_input->addr) & 0x0FFFFFFF);

                u32 _address = static_cast<u32>(_input->addr) & 0x0FFFFFFF;

                switch (_input->data & 0x00F00000) {
                    case 0x000000:
                        memWrite8(_address, static_cast<u8>(_value8 | (_input->data & 0x000000FF)));
                        break;
                    case 0x100000:
                        memWrite16(_address, static_cast<u16>(_value16 | (_input->data & 0x0000FFFF)));
                        break;
                    case 0x200000:
                        memWrite8(_address, static_cast<u8>(_value8 & (_input->data & 0x000000FF)));
                        break;
                    case 0x300000:
                        memWrite16(_address, static_cast<u16>(_value16 & (_input->data & 0x0000FFFF)));
                        break;
                    case 0x400000:
                        memWrite8(_address, static_cast<u8>(_value8 ^ (_input->data & 0x000000FF)));
                        break;
                    case 0x500000:
                        memWrite16(_address, static_cast<u16>(_value16 ^ (_input->data & 0x0000FFFF)));
                        break;
                }
            } break;

            case 0x80000000:
                _cheatAddress = memRead32(static_cast<u32>(_input->addr) & 0x0FFFFFFF) + _input->data;
                _cheatType = 0x80;
                break;

            case 0x90000000: {
                u32 _value = memRead32(static_cast<u32>(_input->addr) & 0x0FFFFFFF);
                memWrite32(static_cast<u32>(_input->data) & 0x0FFFFFFF, _value);
            } break;

            case 0xA0000000:
                _cheatAddress = static_cast<u32>(_input->addr) & 0x0FFFFFFF;
                _cheatType = 0xA0;
                break;

            case 0xB0000000: {
                _lineSkip = (static_cast<u32>(_input->addr) & 0x0FF00000) / 0x100000;

                _frameInt++;
                int _time = _frameInt / 60;

                if (_time * 1000 >= static_cast<long long>(_input->addr) & 0x000FFFFF) {
                    _frameInt = 0;
                    _lineSkip = 0;
                }
            } break;

            case 0xC0000000:
                _cheatAddress = static_cast<u32>(_input->addr) & 0x0FFFFFFF;
                _cheatType = 0xC0;
                break;

            case 0xD0000000: {
                u16 _value1 = memRead16(static_cast<u32>(_input->addr) & 0x0FFFFFFF);
                u16 _value2 = static_cast<u16>(_input->data) & 0x0000FFFF;

                switch (_input->data & 0xFFFF0000) {
                    case 0x000000:
                        if (_value1 != _value2)
                            _lineSkip = 1;
                        break;
                    case 0x100000:
                        if (_value1 == _value2)
                            _lineSkip = 1;
                        break;
                    case 0x200000:
                        if (_value1 >= _value2)
                            _lineSkip = 1;
                        break;
                    case 0x300000:
                        if (_value1 <= _value2)
                            _lineSkip = 1;
                        break;
                    case 0x400000:
                        if (_value1 > _value2)
                            _lineSkip = 1;
                        break;
                    case 0x500000:
                        if (_value1 < _value2)
                            _lineSkip = 1;
                        break;
                }
            } break;

            case 0xE0000000: {
                u8 _value8 = memRead8((u32)_input->data & 0x0FFFFFFF);
                u16 _value16 = memRead16((u32)_input->data & 0x0FFFFFFF);

                u32 type = (u32)_input->addr & 0x0F000000;
                u32 comp = (u32)_input->data & 0xF0000000;
                u32 cond8 = (u32)_input->addr & 0x000000FF;
                u32 cond16 = (u32)_input->addr & 0x0000FFFF;
                u32 skip = (u32)_input->addr & 0x00FF0000;

                switch (type) {
                    case 0x00000000: {
                        switch (comp) {
                            case 0x00000000:
                                if (_value16 != cond16)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x10000000:
                                if (_value16 == cond16)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x20000000:
                                if (_value16 >= cond16)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x30000000:
                                if (_value16 <= cond16)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x40000000:
                                if (_value16 > cond16)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x50000000:
                                if (_value16 < cond16)
                                    _lineSkip = skip / 0x10000;
                                break;
                        }
                    } break;

                    case 0x01000000: {
                        switch (comp) {
                            case 0x00000000:
                                if (_value8 != cond8)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x10000000:
                                if (_value8 == cond8)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x20000000:
                                if (_value8 >= cond8)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x30000000:
                                if (_value8 <= cond8)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x40000000:
                                if (_value8 > cond8)
                                    _lineSkip = skip / 0x10000;
                                break;
                            case 0x50000000:
                                if (_value8 < cond8)
                                    _lineSkip = skip / 0x10000;
                                break;
                        }
                    } break;
                }
            } break;
        }
}

// Only used from Patch.cpp and we don't export this in any h file.
// Patch.cpp itself declares this prototype, so make sure to keep in sync.
void _ApplyPatch(IniPatch *p)
{
    if (p->enabled == 0)
        return;

    switch (p->cpu) {
        case CPU_EE:
            switch (p->type) {
                case BYTE_T:
                    if (memRead8(p->addr) != (u8)p->data)
                        memWrite8(p->addr, (u8)p->data);
                    break;

                case SHORT_T:
                    if (memRead16(p->addr) != (u16)p->data)
                        memWrite16(p->addr, (u16)p->data);
                    break;

                case WORD_T:
                    if (memRead32(p->addr) != (u32)p->data)
                        memWrite32(p->addr, (u32)p->data);
                    break;

                case DOUBLE_T:
                    u64 mem;
                    memRead64(p->addr, &mem);
                    if (mem != p->data)
                        memWrite64(p->addr, &p->data);
                    break;

                case EXTENDED_T:
                    handle_extended_t(p);
                    break;

                default:
                    break;
            }
            break;

        case CPU_IOP:
            switch (p->type) {
                case BYTE_T:
                    if (iopMemRead8(p->addr) != (u8)p->data)
                        iopMemWrite8(p->addr, (u8)p->data);
                    break;
                case SHORT_T:
                    if (iopMemRead16(p->addr) != (u16)p->data)
                        iopMemWrite16(p->addr, (u16)p->data);
                    break;
                case WORD_T:
                    if (iopMemRead32(p->addr) != (u32)p->data)
                        iopMemWrite32(p->addr, (u32)p->data);
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}