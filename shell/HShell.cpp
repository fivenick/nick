#include <sstream>
#include "HShell.h"

int XShell::NumArgs(){
	return arglist_.size(); 
}
ArgKind XShell::NextArg(){
	string cur_arg;
	if(NumArgs() == 0 || (NumArgs() <= next_arg_index_)) return NOARG;
	cur_arg = GetNextArg(false);
	stringstream ss(cur_arg);
	int test_num = 0;
	if(ss >> test_num)
		return INTARG;
	float test_flt = 0.0;
	if(ss >> test_flt)
		return FLOATARG;
	if(cur_arg[0] == '-')
		return SWITCHARG;
	return STRINGARG;
}
string XShell::GetNextArg(bool step){
	string res(" ");
	if(arglist_.size() > next_arg_index_)	
		res = arglist_[next_arg_index_];
	else
		ArgError("out of index");
	if(step){
		++next_arg_index_;
	}
	return res;
}
string XShell::GetStrArg(){
	string res;
	if(NextArg() != STRINGARG)
		ArgError("string");
	res = GetNextArg(true);
	return res;
}
string XShell::GetSwtArg(){
	if(NextArg() != SWITCHARG)
		ArgError("switch");
	return GetNextArg(true).substr(1, string::npos);
}
int XShell::GetIntArg(){
	int res = 0;
	string str_res;
	if(NextArg() != INTARG)
		ArgError("int");
	str_res = GetNextArg(true);
	stringstream ss(str_res);
	if(ss >> res)
		ArgError("int");
	return res;
}
float XShell::GetFltArg(){
	float res = 0.0;
	string str_res;
	int res_type = NextArg();
	/*should focus on the &&*/
	if(res_type != INTARG && res_type != FLOATARG)
		ArgError("float");
	res = GetNextArg(true);
	return res;
}


