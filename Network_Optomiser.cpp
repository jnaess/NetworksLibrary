#include "Network_Optomiser.h"

#include <sstream>

Network_Optomiser::Network_Optomiser() {
  return;
}

Network_Optomiser::Network_Optomiser(string filename) {
  ifstream file(filename);
    if (file.is_open()) {
      string line;

      //these are used to compare the type of input to read
      int type = -1;

      while (getline(file, line)) {
        base = base + line + "\n";
      }
      file.close();
  }
  
  //initialize our base Network
  min = Network(filename);
}

void Network_Optomiser::Combi(vector<string> a, int reqLen, int s, int currLen, bool check[], int l)
{
    if (currLen > reqLen){
      return;
    }

    else if (currLen == reqLen) {
      //create a new combination
      ofstream out("combo_"+to_string(combo)+".txt");

      //check that file opened
      if (out.fail()){
            cout << "Could not open output file" << endl;
            exit(1);
      }

      //create base file
      out << base << "distance";

      for (int i = 0; i < l; i++) {
          if (check[i] == true) {
              out << "\n" << a[i] ;
          }
      }
      combo++;
        
      return;
    }

    if (s == l) {
      return;
    }

  check[s] = true;
  Combi(a, reqLen, s + 1, currLen + 1, check, l);

  check[s] = false;
  Combi(a, reqLen, s + 1, currLen, check, l);
}

void Network_Optomiser::generate_combos(){
  vector<string> a = min.combos();

  int n = a.size();

  int i = 0;

  bool check[n];

  
  cout << "\n";

  //set all checks to false for use in algorithm
  for (i = 0; i < n; i++) {
      check[i] = false;
  }
    
  for (i = 1; i <= n; i++) {
    //one round for each combination length
    Combi(a, i, 0, 0, check, n);
  }


  ofstream out("total_combos.txt");

  if (out.fail())
  {
      cout << "Could not open output file" << endl;
      exit(1);
  }
  //always one extra combo
  combo = combo-1;

  return;
}

void Network_Optomiser::generate_networks(){
  //makes sure that all combos are in order
  generate_combos();

  //generates each network based off of the counted number of combos
  for(int i = 0; i < 30; i++){
    networks.emplace_back("combo_"+to_string(i)+".txt");
  }

  //cout << endl << networks.size() << endl;
}

void Network_Optomiser::best_network(){
  vector<int> indexes;
  vector<double> a;
  double alpha;
  vector<int> legs;

  //get the indexes of all networks that pass
  for(int i = 0; i < networks.size(); i++){
    if(networks[i].passes()){
      indexes.push_back(i);

      alpha = networks[i].average_a();
      a.push_back(alpha);

      legs.push_back(networks[i].models.size());
    }
  }

  //temp lowest index
  int low = lowest(legs);

  //lowest index within the networks vector
  int low_index = indexes[low];
  cout << endl << endl << endl << endl << endl << endl;

  cout << "_____Best Network combo" << low_index << ".txt" << " Details_____" << endl;
  networks[low_index].output();

  cout << "_____Best Network combo" << low_index << ".txt" << " Summary_____" << endl;
  networks[low_index].quick_print(networks[low_index].models);

  cout << "_____Best Network combo" << low_index << ".txt" << " Ellipse Errors_____" << endl;
  networks[low_index].output_error_legs();

  
}

int Network_Optomiser::lowest(vector<int> legs){
  int low = 0;

  for(int i = 0; i < legs.size(); i++){
    //set new index because it was a lower value
    if(legs[i]==11){
      low = i;
    }
  }
  return low;
}