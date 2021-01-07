#include "Functions.h"


void TestPrint(MatrixXd M, string title) {
  //Print the title of this matrix
	cout << "___" << title << "___" << endl;
  cout << M << endl;
  cout << endl;
}

double degrees(double deg, double min, double sec){
  return deg + min/60 + sec/3600;
}

vector<double> dms(double degrees){
  vector<double> dms;

  int deg = degrees;

  //truncate degrees
  dms.push_back(deg);

  //truncate arc minutes
  degrees = (degrees - double(deg))*60;
  deg = degrees;
  dms.push_back(deg);

  //arc seconds does not need to be concatenated
  degrees = (degrees - double(deg))*60;
  dms.push_back(degrees);

  return dms;
}

double degrees(double rad){
  double PI =acos(-1);
  
  return rad * (180/PI);
}

void Output(string filename, MatrixXd m) {
	//Output file
	ofstream out(filename);
	if (out.fail())
	{
		cout << "Could not open output file" << endl;
		exit(1);
	}
	out << filename << endl;
	for (int i = 0; i < m.rows(); i++) {
		for (int j = 0; j < m.cols(); j++) {
			out << setw(6) << fixed << setprecision(6) << m(i, j) << "   ";
		}
		out << endl;
	}
	return;
}

double radians(double deg){
  double PI =acos(-1);
  
  return deg * PI / 180;
}