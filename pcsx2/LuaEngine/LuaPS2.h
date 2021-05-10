#ifndef LUA_PS2
#define LUA_PS2

#include "sol.hpp"
#include "LuaEngine.h"
#include "DCInstance.h"

using namespace sol;
using namespace std;

using LuaState = sol::state;
using LuaFunction = sol::function;

class LuaPS2
{
	static u32 _checksum;
	static wxString _loadPath;
	static wxString _savePath;

	static bool _isSaving;

public:
	LuaState luaState;
	LuaFunction bootFunction;
	LuaFunction initFunction;
	LuaFunction frameFunction;

	#pragma region Reader Functions
	static u8  Read_UInt08(u32 Input) {
		return memRead8(Input);
	}
	static u16 Read_UInt16(u32 Input) {
		return memRead16(Input);
	}
	static u32 Read_UInt32(u32 Input) {
		return memRead32(Input);
	}
	static u64 Read_UInt64(u32 Input) {
		u64 _out = 0;

		memRead64(Input, &_out);
		return _out;
	}
	static bool Read_Boolean(u32 Input) {
		switch (memRead8(Input))
		{
		case 0:
			return false;
		case 1:
			return true;
		default:
			return NULL;
		}
	}
	static float Read_Single(u32 Input) {
		u32 _value = memRead32(Input);
		return *reinterpret_cast<float*>(&_value);
	}
	static string Read_String(u32 Input01, u32 Input02) {
		std::vector<u8> _value(Input02);

		for (size_t i = 0; i < Input02; i++)
		{
			_value.at(i) = memRead8(Input01);
			Input01++;
		}

		return string(_value.begin(), _value.end());
	}
	static vector<u8> Read_UInt08_Array(u32 Input01, u32 Input02) {
		std::vector<u8> _value(Input02);

		for (size_t i = 0; i < Input02; i++)
		{
			_value.at(i) = memRead8(Input01);
			Input01++;
		}

		return _value;
	}
	#pragma endregion

	#pragma region Writer Functions
	static void Write_UInt08(u32 Input01, u8 Input02) {
		const u8 _value = static_cast<u8>(Input02);

		if (memRead8(Input01) != _value)
			memWrite8(Input01, _value);
	}
	static void Write_UInt16(u32 Input01, u16 Input02) {
		const u16 _value = static_cast<u16>(Input02);

		if (memRead16(Input01) != _value)
			memWrite16(Input01, _value);
	}
	static void Write_UInt32(u32 Input01, u32 Input02) {
		if (memRead32(Input01) != Input02)
			memWrite32(Input01, Input02);
	}
	static void Write_UInt64(u32 Input01, u64 Input02)
	{
		u64 _read = 0;
		const u64 _val = Input02;

		memRead64(Input01, &_read);

		if (_read != _val)
			memWrite64(Input01, &_val);
	}
	static void Write_Boolean(u32 Input01, bool Input02) {
		memWrite8(Input01, Input02 ? 1 : 0);
	}
	static void Write_Single(u32 Input01, float Input02)
	{
		float _inp = Input02;
		const u32 _val = *reinterpret_cast<u32*>(&_inp);

		if (memRead32(Input01) != _val)
			memWrite32(Input01, _val);
	}
	static void Write_String(u32 Input01, string Input02)
	{
		vector<uint8_t> _array(Input02.begin(), Input02.end());

		for (size_t i = 0; i < _array.size(); i++)
		{
			const u8 _value = static_cast<u8>(_array[i]);

			memWrite8(Input01, _value);
			Input01++;
		}
	}
	static void Write_UInt08_Array(u32 Input01, vector<u8> Input02)
	{
		for (size_t i = 0; i < Input02.size(); i++)
		{
			const u8 _value = static_cast<u8>(Input02[i]);

			memWrite8(Input01, _value);
			Input01++;
		}
	}
	#pragma endregion

	#pragma region Calculator Functions
	static u32 Calculate_Pointer(u32 Input01, u32 Input02)
	{
		u32 Value = memRead32(Input01);
		return Value + Input02;
	}

    static u32 UnsignedShift32(u32 base, u32 shift)
    {
        string _inBinary = bitset<32>(base).to_string();
        string _outBinary = "00000000000000000000000000000000";

        for (u32 i = 0; i < 32 - shift; i++)
            _outBinary[i] = _inBinary[i + shift];

        u32 _out = bitset<32>(_outBinary).to_ulong();
        return _out;
    }
	#pragma endregion
	
	#pragma region File I/O Functions
	static vector<u8> File_Read(const char* Input02) {
		wxString _location = Path::Combine(_loadPath, Input02);

		ifstream _input(_location.ToStdString(), ios_base::binary);

		_input.seekg(0, std::ios_base::end);
		const size_t _length = _input.tellg();
		_input.seekg(0, std::ios_base::beg);

		vector<u8> _buffer;
		_buffer.reserve(_length);
		copy(istreambuf_iterator<char>(_input), istreambuf_iterator<char>(), back_inserter(_buffer));

		return _buffer;
	}
	static vector<u8> File_ReadRegion(const char* Input01, u32 Input02, u32 Input03) {
		wxString _location = Path::Combine(_loadPath, Input01);

		ifstream _input(_location.ToStdString(), ios_base::binary);

		const size_t _length = Input03;
		_input.seekg(Input02, std::ios_base::beg);

		vector<u8> _buffer;
		_buffer.reserve(_length);
		copy(istreambuf_iterator<char>(_input), istreambuf_iterator<char>(), back_inserter(_buffer));

		return _buffer;
	}
	#pragma endregion

	void SetFunctions();
	LuaPS2(wxString Input01, u32 Input02);
};

#endif