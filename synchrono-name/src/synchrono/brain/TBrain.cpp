
#include "synchrono/brain/TBrain.h"




namespace synchrono {
namespace agent {

TBrain::TBrain(int rank, std::shared_ptr<ChDriver> driver, ChVehicle& vehicle)
    : SynVehicleBrain(rank, driver, vehicle) {
    m_nearest_vehicle = 1000;
//std::cout<<"-----!!!!!CONSTRUCT T BTAIN!!!!!!!!!----------"<<std::endl;
    m_rank = rank;
}


TBrain::~TBrain() {}

void TBrain::Synchronize(double time) {
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

void TBrain::Advance(double step) {
    //std::cout<<"-----!!!!!ADVANCE T BRAINN!!!!!!!!!----------"<<std::endl;
    m_driver->Advance(step);
}

void TBrain::ProcessMessage(SynMessage* msg, int sender_rank) {
     //std::cout<<"pos"<<m_vehicle.GetChassisBody()->GetPos()<<std::endl;
    //std::cout<<"-----!!!!!!!!!Pmesg!!!!!!!!!!----------"<<std::endl;
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


void TBrain::trackLoc(SynMessage* msg, int sender_rank,chrono::Vector Sen){
    myLoc = Sen;
    auto state = std::static_pointer_cast<SynVehicleMessage::State>(msg->GetState());
    std::cout<<"----sender rank---"<<sender_rank<<"--------"<<std::endl;
    std::cout<<(state->chassis.pos)<<std::endl;
    std::cout<<"====my rank==="<<m_rank<<"======="<<std::endl;
    std::cout<<"my Loc"<<myLoc<<std::endl;
    
    ChVector<> pos1 = state->chassis.pos;
    ChVector<> pos2 = myLoc;
    checkDistanceCircle(pos1,pos2);
    checkDistanceRecCollision(pos1,pos2);
    std::cout<<std::endl;
}


void TBrain::checkDistanceCircle(ChVector<> pos1,ChVector<> pos2){
    double locData[3];
    for(int i = 0; i<3;i++)
    {
        locData[i] = pos1[i] - pos2[i];
    }

    double res = locData[0]*locData[0]+locData[1]*locData[1]+locData[2]*locData[2];
    std::cout<<"distance data is: "<<res<<std::endl;
    checkDistanceCircleCollision(res);
}

void TBrain::checkDistanceCircleCollision(double distance){
    if(distance <=5.0 )
    {
        std::cout<<"Circular Detection: collision!!!!!!!!!!"<<std::endl;
    }else{
        std::cout<<"Circular Detection: no collision"<<std::endl;
    }
}

void TBrain::checkDistanceRecCollision(ChVector<> pos1,ChVector<> pos2){
    double disX = pos1[0] - pos2[0];
    double disY = pos1[1] - pos2[1];
    double disZ = pos1[2] - pos2[2];
    if(disX<0){
        disX = -disX;
    }
    if(disY<0){
        disY = -disY;
    }
    if(disZ<0){
        disZ = -disZ;
    }

    if(disX<3&&disY<7){
        std::cout<<"Rectangular Detection: collision!!!!!!!!!!"<<std::endl;
    }else{
        std::cout<<"Rectangular Detection: no collision"<<std::endl;
    }
}


double TBrain::DistanceToLine(ChVector<> p, ChVector<> l1, ChVector<> l2) {
    // https://www.intmath.com/plane-analytic-geometry/perpendicular-distance-point-line.php
    double A = l1.y() - l2.y();
    double B = -(l1.x() - l2.x());
    double C = l1.x() * l2.y() - l1.y() * l2.x();
    double d = abs(A * p.x() + B * p.y() + C) / sqrt(A * A + B * B);
    return d;
}

bool TBrain::IsInsideBox(ChVector<> pos, ChVector<> sp, ChVector<> op, double w) {
    double len = (sp - op).Length();
    // TODO :: Should be w / 2, but paths are too far away from the actual lanes

    return (pos - sp).Length() < len && (pos - op).Length() < len && DistanceToLine(pos, sp, op) < w;
}

void TBrain::updateMyLoc(chrono::Vector Sen){
    std::cout<<"testing Tbrain myLoc"<<std::endl;
    myLoc = Sen;
}



}  // namespace agent
}  // namespace synchrono
