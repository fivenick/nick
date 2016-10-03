#include"HShell.h"


int main(int argc, char* argv[]){
	if(argc <= 1)
		return 1;
	vector<string> arg_lst;
	for(int i = 1; i < argc; ++i){
		arg_lst.push_back(string(argv[i]));
	}
	XShell shell_obj(argc, arg_lst);
	string cur_res;
	while(NextArg() == SWITCHARG){
		cur_res = GetSwtArg();
		/*in this example, the arg must be a single letter*/
		if(cur_res.size() != 1) HError("switch must be a single letter");
		switch(cur_res[0]){
			/*arg 'a' has no arg vals*/
			case 'a':
				DealA();
				break;
			/*arg 'o' has an int val*/
			case 'o':
				int ii = GetIntArg();
				DealO(ii);
				break;
		}
	}
}
