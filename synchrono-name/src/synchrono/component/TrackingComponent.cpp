
#include "synchrono/component/TrackingComponent.h"




namespace synchrono {
namespace agent {

TrackingComponent::TrackingComponent(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
    : SynVehicleComponent(rank, driver, vehicle) {
    m_nearest_vehicle = 1000;
//std::cout<<"-----!!!!!CONSTRUCT T MODULE!!!!!!!!!----------"<<std::endl;
    m_rank = rank;
}


TrackingComponent::~TrackingComponent() {}

void TrackingComponent::Synchronize(double time) {
    m_driver->Synchronize(time);

}

void TrackingComponent::Advance(double step) {
    m_driver->Advance(step);
}

//Tbrain is going to process the message to extract the location of the sender agent
//and also updates the location of the agent on the current rank
void TrackingComponent::ProcessMessage(SynMessage* msg, int sender_rank) {
    if((std::find(target_rank.begin(), target_rank.end(), sender_rank) != target_rank.end()) 
    || target_rank.empty()) {
        trackLoc(msg,sender_rank,m_vehicle.GetChassisBody()->GetPos());
    } else {
    }
   
}

//Track location function extract the Message state data and also receives
//the location of the agent on the current rank from the SynMPIManager
void TrackingComponent::trackLoc(SynMessage* msg, int sender_rank,chrono::Vector Sen){

    if(enableDis==true){
        //update the location on the current rank everytime when processmessage()function is called.
        myLoc = Sen;
        auto state = std::static_pointer_cast<SynVehicleMessage::State>(msg->GetState());

        //extract location data from the state object
        std::cout<<"----sender rank---"<<sender_rank<<"--------"<<std::endl;
        std::cout<<(state->chassis.pos)<<std::endl;

        //display the location of the agent on the current rank
        std::cout<<"====my rank==="<<m_rank<<"======="<<std::endl;
        std::cout<<"my Loc"<<myLoc<<std::endl;
    
        //create two ChVector for collision detection
        //pos1 stores the sender location, pos2 stores the current rank lcoation
        ChVector<> pos1 = state->chassis.pos;
        ChVector<> pos2 = myLoc;

        //calls collision detect helper function
        checkDistanceCircle(pos1,pos2,5.0,sender_rank);
        checkDistanceRec(pos1,pos2,3.0,7.0,sender_rank);
        std::cout<<std::endl;
    }
    
}

//This function check collision using a circular range
//The extra arguments are the radius of the circle and the sender rank
void TrackingComponent::checkDistanceCircle(ChVector<> pos1,ChVector<> pos2, double radius,int sender_rank){
    double locData[3];
    for(int i = 0; i<3;i++)
    {
        locData[i] = pos1[i] - pos2[i];
    }

    //calculate the distance between two agents
    double res = locData[0]*locData[0]+locData[1]*locData[1]+locData[2]*locData[2];
    std::cout<<"distance data is: "<<res<<std::endl;

    //compare the distance with the given radius parameter of the circle
    if(res<=radius)
    {
        std::cout<<"Circular Detection: rank "<< m_rank<< " collides with rank " << sender_rank
        <<std::endl;
        actionUsrCir();
    }else{
        std::cout<<"Circular Detection: no collision"<<std::endl;
    }

}

//The function checks the collision by drawing a rectangular box
//The extra arguments requried are the length of the rectangular's shorter side
//the length of the rectangular's longer side, and the rank of the sender
void TrackingComponent::checkDistanceRec(ChVector<> pos1,ChVector<> pos2, double shortRecSide,
    double longRecSide,int sender_rank){
    //Calculate X Y Z displacemet components
    double disX = pos1[0] - pos2[0];
    double disY = pos1[1] - pos2[1];
    double disZ = pos1[2] - pos2[2];

    //Obtain absolute distance magnitude
    if(disX<0){
        disX = -disX;
    }
    if(disY<0){
        disY = -disY;
    }
    if(disZ<0){
        disZ = -disZ;
    }

    //Compare with the given lengths of the rectangal
    if(disX < shortRecSide && disY < longRecSide){
        std::cout<<"Rectangular Detection: rank "<< m_rank<< " collides with rank " << sender_rank
        <<std::endl;
        actionUsrRec();
    }else{
        std::cout<<"Rectangular Detection: no collision"<<std::endl;
    }


}


double TrackingComponent::DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2) {
    // https://www.intmath.com/plane-analytic-geometry/perpendicular-distance-point-line.php
    double A = l1.y() - l2.y();
    double B = -(l1.x() - l2.x());
    double C = l1.x() * l2.y() - l1.y() * l2.x();
    double d = abs(A * p.x() + B * p.y() + C) / sqrt(A * A + B * B);
    return d;
}

bool TrackingComponent::IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w) {
    double len = (sp - op).Length();
    // TODO :: Should be w / 2, but paths are too far away from the actual lanes

    return (pos - sp).Length() < len && (pos - op).Length() < len && DistanceToLine(pos, sp, op) < w;
}

void TrackingComponent::updateMyLoc(chrono::Vector Sen){
    std::cout<<"testing Tbrain myLoc"<<std::endl;
    myLoc = Sen;
}

void TrackingComponent::enableLocDisplay(){
    enableDis = true;
}


void TrackingComponent::disableLocDisplay(){
    enableDis = false;
};




}  // namespace agent
}  // namespace synchrono
