#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
using namespace std;

void cubicSmooth5(double in[], double out[], int N)
{

	int i;
	if (N < 5)
	{
		for (i = 0; i <= N - 1; i++)
			out[i] = in[i];
	}

	else
	{
		out[0] = (69.0 * in[0] + 4.0 * in[1] - 6.0 * in[2] + 4.0 * in[3] - in[4]) / 70.0;
		out[1] = (2.0 * in[0] + 27.0 * in[1] + 12.0 * in[2] - 8.0 * in[3] + 2.0 * in[4]) / 35.0;
		for (i = 2; i <= N - 3; i++)
		{
			out[i] = (-3.0 * (in[i - 2] + in[i + 2]) + 12.0 * (in[i - 1] + in[i + 1]) + 17.0 * in[i]) / 35.0;
		}
		out[N - 2] = (2.0 * in[N - 5] - 8.0 * in[N - 4] + 12.0 * in[N - 3] + 27.0 * in[N - 2] + 2.0 * in[N - 1]) / 35.0;
		out[N - 1] = (-in[N - 5] + 4.0 * in[N - 4] - 6.0 * in[N - 3] + 4.0 * in[N - 2] + 69.0 * in[N - 1]) / 70.0;
	}
	return;
}

void cubicSmooth7(double in[], double out[], int N)
{
	int i;
	if (N < 7)
	{
		for (i = 0; i <= N - 1; i++)
		{
			out[i] = in[i];
		}
	}
	else
	{
		out[0] = (39.0 * in[0] + 8.0 * in[1] - 4.0 * in[2] - 4.0 * in[3] +
			1.0 * in[4] + 4.0 * in[5] - 2.0 * in[6]) / 42.0;
		out[1] = (8.0 * in[0] + 19.0 * in[1] + 16.0 * in[2] + 6.0 * in[3] -
			4.0 * in[4] - 7.0* in[5] + 4.0 * in[6]) / 42.0;
		out[2] = (-4.0 * in[0] + 16.0 * in[1] + 19.0 * in[2] + 12.0 * in[3] +
			2.0 * in[4] - 4.0 * in[5] + 1.0 * in[6]) / 42.0;
		for (i = 3; i <= N - 4; i++)
		{
			out[i] = (-2.0 * (in[i - 3] + in[i + 3]) +
				3.0 * (in[i - 2] + in[i + 2]) +
				6.0 * (in[i - 1] + in[i + 1]) + 7.0 * in[i]) / 21.0;
		}
		out[N - 3] = (-4.0 * in[N - 1] + 16.0 * in[N - 2] + 19.0 * in[N - 3] +
			12.0 * in[N - 4] + 2.0 * in[N - 5] - 4.0 * in[N - 6] + 1.0 * in[N - 7]) / 42.0;
		out[N - 2] = (8.0 * in[N - 1] + 19.0 * in[N - 2] + 16.0 * in[N - 3] +
			6.0 * in[N - 4] - 4.0 * in[N - 5] - 7.0 * in[N - 6] + 4.0 * in[N - 7]) / 42.0;
		out[N - 1] = (39.0 * in[N - 1] + 8.0 * in[N - 2] - 4.0 * in[N - 3] -
			4.0 * in[N - 4] + 1.0 * in[N - 5] + 4.0 * in[N - 6] - 2.0 * in[N - 7]) / 42.0;
	}
}

int main(int argc, char* argv[]) {
	double x_buf[200];
	double y_buf[200];
	double res_buf[200];

	ifstream ifs;
    std::string aa;
	char* read_file = "E:\\March\\project\\160921-changebytime\\tools\\deal-res-truesp\\reduce-for-one-week\\res_target\\final-radtio-update\\final_ratio.txt";
	ifstream fin(read_file);
	int line_num = 0;
	while (fin >> aa) {
		int index = aa.find_first_of(',');
		string x_val = aa.substr(0, index);
		string y_val = aa.substr(index + 1);
		double tmp = 0;
		stringstream sstr(x_val);
		sstr >> tmp;
		x_buf[line_num] = tmp;
		stringstream sstr2(y_val);
		sstr2 >> tmp;
		y_buf[line_num] = tmp;
		line_num += 1;
	}
	line_num = line_num - 1;
	cubicSmooth5(y_buf, res_buf, line_num);

	//cout << line_num - 1;
	char* write_file = "E:\\March\\project\\160921-changebytime\\tools\\deal-res-truesp\\reduce-for-one-week\\res_target\\final-radtio-update\\result.txt";
	ofstream fout(write_file);
	if (fout) {
		for (int i = 0; i < line_num; ++i) {
			fout << x_buf[i] << ',' << res_buf[i] << endl;
		}
	}
	
	system("pause");
	return 0;
}