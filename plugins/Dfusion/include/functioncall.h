#ifndef FUNCTIONCALL__H
#define FUNCTIONCALL__H
#include <vector>
using std::vector;
class FunctionCaller
{
public:
	enum callconv
	{
		STD_CALL, //__stdcall - all in stack
		FAST_CALL, //__fastcall - as much in registers as fits
		THIS_CALL, //__thiscall - eax ptr to this, rest in stack
		CDECL_CALL //__cdecl - same as stdcall but no stack realign
	};

	FunctionCaller(size_t base):base_(base){};

	int CallFunction(size_t func_ptr,callconv conv,const vector<int> &arguments);
private:
	size_t base_;
};

#endif //FUNCTIONCALL__H