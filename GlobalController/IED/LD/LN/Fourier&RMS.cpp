#include "Fourier&RMS.h"


Fourier::Fourier(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      currentA(std::make_shared<SAV>("Мгновенный ток А", this->getLNRef(), false)),
      currentB(std::make_shared<SAV>("Мгновенный ток В", this->getLNRef(), false)),
      currentC(std::make_shared<SAV>("Мгновенный ток С", this->getLNRef(), false)),
      masA(std::make_shared<std::vector<double>>()),
      masB(std::make_shared<std::vector<double>>()),
      masC(std::make_shared<std::vector<double>>()),
      fourierA(std::make_shared<CMV>("Действующий ток А", false, this->getLNRef())),
      fourierB(std::make_shared<CMV>("Действующий ток В", false, this->getLNRef())),
      fourierC(std::make_shared<CMV>("Действующий ток С", false, this->getLNRef()))
      {}

void Fourier::recieveSampledValues(const double& currentA_, const double& currentB_, const double& currentC_){
    currentA->setInstMag(currentA_);
    currentB->setInstMag(currentB_);
    currentC->setInstMag(currentC_);
}

void Fourier::unpackSampledValues(){
    if(masA->size() == N){
        std::shared_ptr<Vector> A = std::shared_ptr<Vector>(); 
        std::shared_ptr<Vector> B = std::shared_ptr<Vector>();
        std::shared_ptr<Vector> C = std::shared_ptr<Vector>();


        calculateFourier(masA, A,    0);
        calculateFourier(masB, B, -120);
        calculateFourier(masC, C,  120);

        fourierA->set_cVal(*A);
        fourierC->set_cVal(*B);
        fourierC->set_cVal(*C);

        masA->clear();
        masB->clear();
        masC->clear();
    }
    masA->push_back(currentA->getInstMag());
    masB->push_back(currentB->getInstMag());
    masC->push_back(currentC->getInstMag());
}

void Fourier::calculateFourier(std::shared_ptr<std::vector<double>> mas, std::shared_ptr<Vector> vec, double ang){
    if (mode){
        double Fx = 0;
        double Fy = 0;
        for(int i = 0; i < N; i++){
            Fx += mas->at(i)*sin(2*M_PI*freq*(i+1)/N);
            Fy += mas->at(i)*cos(2*M_PI*freq*(i+1)/N);
        }
        Fx *= 2.0/N;
        Fy *= 2.0/N;
        vec->setMag(sqrt(Fx*Fx + Fy*Fy));
        vec->setAng(atan2(Fy,Fx));
    } else {
        double mag = 0;
        for(int i = 0; i < N; i++){
            mag += std::pow(mas->at(i),2);
        }
        vec->setMag(sqrt(mag/N));
        vec->setAng(ang);
        
    }

}