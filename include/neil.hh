#ifndef neil_h
#define neil_h 1
#include <vector>

class neil{
private:
  std::vector<std::vector<double>> xVal;
  std::vector<std::vector<double>> yVal;
  void Init();
public:
  neil(){Init();};
  ~neil(){};

  double getNEIL(int partType=1,double energy=1);  
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
