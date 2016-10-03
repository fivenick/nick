#ifndef HSHELL_H_
#define HSHELL_H_

#include<string>
#include<vector>
typedef enum{
	SWITCHARG, STRINGARG, INTARG, FLOATARG, NOARG
}ArgKind;
class XShell{
	private:
		vector<string> arglist_;
		int next_arg_index_;
	public:
		XShell(int argc, vector<string> argv){
			next_arg_index_ = 0;
			arglist_ = argv
		}
		int NumArgs();
		/*get the next arg kind*/
		ArgKind NextArg();
		/*get the next arg*/
		string GetNextArg(bool step);

		string GetStrArg();
		string GetSwtArg();
		int GetIntArg();
		long GetLongArg();
		float GetFltArg();
};
