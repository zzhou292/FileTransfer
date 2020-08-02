
#include "synchrono/brain/TrackingBrain.h"




namespace synchrono {
namespace agent {

TrackingBrain::TrackingBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
    : SynVehicleBrain(rank, driver, vehicle) {
    m_nearest_vehicle = 1000;
//std::cout<<"-----!!!!!CONSTRUCT T BTAIN!!!!!!!!!----------"<<std::endl;
    m_rank = rank;
}


TrackingBrain::~TrackingBrain() {}

void TrackingBrain::Synchronize(double time) {
    //std::cout<<"-----!!!!!SYNCHRONIZE!!!!!!!!!----------"<<std::endl;
    double temp_range = 1000;
    if (false && GetRank() == 4) {
        std::string color = m_light_color == 0 ? "green" : (m_light_color == 1 ? "yellow" : "red");
        std::string box = m_inside_box ? "inside box" : "not inside box";
        std::cout << "Light is " << color << " and " << box << std::endl;
    }
    if (m_light_color == SynSPATMessage::RED && m_inside_box) {
        temp_range = temp_range < m_dist ? temp_range : m_dist;
    }
    if (false && GetRank() == 4) {
        std::cout << "Distance is: " << m_dist << std::endl;
    }


    std::static_pointer_cast<ChPathFollowerACCDriver>(m_driver)->SetCurrentDistance(temp_range);

    // std::cout << "Range is: " << temp_range << std::endl;

    m_driver->Synchronize(time);

    // ChDriver::Inputs driver_inputs = m_driver->GetInputs();
    // // driver_inputs.m_braking = 1;
    // // driver_inputs.m_throttle = .05;
    // // driver_inputs.m_steering = 0;
    // std::cout << "\r\nsteering: " << driver_inputs.m_steering;
    // std::cout << "\r\nthrottle: " << driver_inputs.m_throttle;
    // std::cout << "\r\nbraking: " << driver_inputs.m_braking;
    // std::cout << std::endl;
}

void TrackingBrain::Advance(double step) {
    m_driver->Advance(step);
}

//TrackingBrain is going to process the message to extract the location of the sender agent
//and also updates the location of the agent on the current rank
void TrackingBrain::ProcessMessage(SynMessage* msg, int sender_rank) {
    trackLoc(msg,sender_rank,m_vehicle.GetChassisBody()->GetPos());
    switch (msg->GetType()) {
        case SynMessage::TRAFFIC_LIGHT: {
            // jht - Doesn't seem like we ever call delete, is this a memory leak?
            auto spat = std::static_pointer_cast<SynTrafficLightMessage::State>(msg->GetState())->spat;
            SynSPATMessage* spat_msg =
                new SynSPATMessage(m_rank, chrono_types::make_shared<SynSPATMessage::State>(spat));
            ProcessMessage(spat_msg,sender_rank);
            auto map = std::static_pointer_cast<SynTrafficLightMessage::State>(msg->GetState())->map;
            SynMAPMessage* map_msg = new SynMAPMessage(m_rank, chrono_types::make_shared<SynMAPMessage::State>(map));
            ProcessMessage(map_msg,sender_rank);
            break;
        }
        case SynMessage::SPAT: {
            // SPAT Processing
            auto spat_lanes = std::static_pointer_cast<SynSPATMessage::State>(msg->GetState())->lanes;
            // std::cout << "Processing Message ::" << std::endl;
            // for (auto lane : spat_lanes) {
            //     std::cout << lane.color << " ";
            // }
            // std::cout << std::endl;

            // std::cout << "Lane :: " << m_lane << std::endl;
            // std::cout << "Lane Color :: " << spat_lanes[m_lane].color << std::endl;

            m_light_color = spat_lanes[m_lane].color;
            break;
        }
        case SynMessage::MAP: {
            // MAP Processing
            auto map_lanes = std::static_pointer_cast<SynMAPMessage::State>(msg->GetState())->lanes;
            ChVector<> pos = m_vehicle.GetChassisBody()->GetPos();
           

            if (IsInsideBox(pos, map_lanes[m_lane].stopPos, map_lanes[m_lane].offsetPos, map_lanes[m_lane].width)) {
                m_inside_box = true;
            } else {
                m_inside_box = false;
            }
            m_dist = (map_lanes[m_lane].stopPos - pos).Length();

            break;
        }
    }
}

//Track location function extract the Message state data and also receives
//the location of the agent on the current rank from the SynMPIManager
void TrackingBrain::trackLoc(SynMessage* msg, int sender_rank,chrono::Vector Sen){

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
        checkDistanceRecCollision(pos1,pos2,3.0,7.0,sender_rank);
        std::cout<<std::endl;
    }
    
}

//This function check collision using a circular range
//The extra arguments are the radius of the circle and the sender rank
void TrackingBrain::checkDistanceCircle(ChVector<> pos1,ChVector<> pos2, double radius,int sender_rank){
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
void TrackingBrain::checkDistanceRecCollision(ChVector<> pos1,ChVector<> pos2, double shortRecSide,
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


double TrackingBrain::DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2) {
    // https://www.intmath.com/plane-analytic-geometry/perpendicular-distance-point-line.php
    double A = l1.y() - l2.y();
    double B = -(l1.x() - l2.x());
    double C = l1.x() * l2.y() - l1.y() * l2.x();
    double d = abs(A * p.x() + B * p.y() + C) / sqrt(A * A + B * B);
    return d;
}

bool TrackingBrain::IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w) {
    double len = (sp - op).Length();
    // TODO :: Should be w / 2, but paths are too far away from the actual lanes

    return (pos - sp).Length() < len && (pos - op).Length() < len && DistanceToLine(pos, sp, op) < w;
}

void TrackingBrain::updateMyLoc(chrono::Vector Sen){
    std::cout<<"testing TrackingBrain myLoc"<<std::endl;
    myLoc = Sen;
}

void TrackingBrain::enableLocDisplay(){
    enableDis = true;
}


void TrackingBrain::disableLocDisplay(){
    enableDis = false;
};




}  // namespace agent
}  // namespace synchrono
