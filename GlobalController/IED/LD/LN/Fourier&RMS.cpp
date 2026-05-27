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
      fourierC(std::make_shared<CMV>("Действующий ток С", false, this->getLNRef())),
      mode(std::make_shared<SPS>("Режим Фильтра",this->getLNRef(),false))
      {}

void Fourier::recieveSampledValues(const double& currentA_, const double& currentB_, const double& currentC_){
    currentA->setInstMag(currentA_);
    currentB->setInstMag(currentB_);
    currentC->setInstMag(currentC_);
    unpackSampledValues();
}

void Fourier::unpackSampledValues(){
    masA->push_back(currentA->getInstMag());
    masB->push_back(currentB->getInstMag());
    masC->push_back(currentC->getInstMag());

    if(masA->size() == N){
        counter++;
        if(counter == 10)
        {
            std::shared_ptr<Vector> A = std::make_shared<Vector>("cVal", EnumFunctionalConstraints::MX, TriggerOption(true,false,true), this->getLNRef()); 
            std::shared_ptr<Vector> B = std::make_shared<Vector>("cVal", EnumFunctionalConstraints::MX, TriggerOption(true,false,true), this->getLNRef());
            std::shared_ptr<Vector> C = std::make_shared<Vector>("cVal", EnumFunctionalConstraints::MX, TriggerOption(true,false,true), this->getLNRef());


            calculateFourier(masA, A,    0, PhaseA);
            calculateFourier(masB, B, -120, PhaseB);
            calculateFourier(masC, C,  120, PhaseC);

            fourierA->set_cVal(*A);
            fourierB->set_cVal(*B);
            fourierC->set_cVal(*C);
            counter = 0;
        }
        masA->erase(masA->begin());
        masB->erase(masB->begin());
        masC->erase(masC->begin());
    }
}
    

void Fourier::calculateFourier(std::shared_ptr<std::vector<double>> mas, std::shared_ptr<Vector> vec, double ang, PhaseTag phase){
    if (mode){
        double Fx_ = 0;
        double Fy_ = 0;
        for(int i = 0; i < N; i++){
            Fx_ += mas->at(i)*sin(2*M_PI*(i+1.0)/N);
            Fy_ += mas->at(i)*cos(2*M_PI*(i+1.0)/N);
        }
        Fx_ *= 2.0/N;
        Fy_ *= 2.0/N;
        Fx[phase] = Fx_;
        Fy[phase] = Fy_;
        vec->setMag(sqrt((Fx_*Fx_ + Fy_*Fy_)/2));
        vec->setAng(atan2(Fy_,Fx_));
    } else {
        double mag = 0;
        for(int i = 0; i < N; i++){
            mag += std::pow(mas->at(i),2);
        }
        double rms = sqrt(mag/N);
        double a = ang/180 * M_PI;
        Fx[phase] = rms * cos(a);
        Fy[phase] = rms * sin(a);
        vec->setMag(rms);
        vec->setAng(a);
        
    }

}
