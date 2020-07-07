#include "synchrono/agent/SynTrafficLightAgent.h"

namespace synchrono {
namespace agent {

SynTrafficLightAgent::SynTrafficLightAgent(int id, ChSystem* system) : SynAgent(id, SynAgent::TRAFFIC_LIGHT, system) {
    m_msg = chrono_types::make_shared<SynTrafficLightMessage>(id);

    // Super messy and a lot needs to change about the pipeline of traffic_light creation
    double l = 20;
    double h = 1.0;
    double w = 2.5;
    double b = 2.5;
    double x = 1.5;
    double y = -18;
    AddLane(ChVector<>(x, y, h), ChVector<>(x, y - l, h), w);
    AddLane(ChVector<>(x + w + b, y, h), ChVector<>(x + w + b, y - l, h), w);

    x = 20;
    y = -6;
    b = .75;
    AddLane(ChVector<>(x, y, h), ChVector<>(x + l, y, h), w);
    AddLane(ChVector<>(x, y + w + b, h), ChVector<>(x + l, y + w + b, h), w);
    AddLane(ChVector<>(x, y + (w + b) * 2, h), ChVector<>(x + l, y + (w + b) * 2, h), w);
    b = 1.75;
    AddLane(ChVector<>(x, y + (w + b) * 3, h), ChVector<>(x + l, y + (w + b) * 3, h), w);

    x = -3.5;
    y = 40;
    b = 2.75;
    AddLane(ChVector<>(x, y - l, h), ChVector<>(x, y + 0.5 * l, h), w);
    AddLane(ChVector<>(x - (w + b), y, h), ChVector<>(x - (w + b), y - l, h), w);

    if (m_rank == 1) {
        SetColor(SynSPATMessage::GREEN, {0, 1});
    } else if (m_rank == 2) {
        SetColor(SynSPATMessage::GREEN, {0, 1, 2, 3});
    } else if (m_rank == 3) {
        SetColor(SynSPATMessage::GREEN, {0});
    }
}

SynTrafficLightAgent::~SynTrafficLightAgent() {}

void SynTrafficLightAgent::AddLane(ChVector<> stopPoint, ChVector<> offsetFromStop, double width) {
    std::static_pointer_cast<SynTrafficLightMessage::State>(m_msg->GetState())
        ->map.lanes.push_back(SynMAPMessage::Lane({width, stopPoint, offsetFromStop}));
    std::static_pointer_cast<SynTrafficLightMessage::State>(m_msg->GetState())
        ->spat.lanes.push_back(SynSPATMessage::Lane({static_cast<SynSPATMessage::Color>(SynSPATMessage::RED)}));
}

void SynTrafficLightAgent::ChangeToYellow() {
    if (m_rank == 1) {
        if (currState == 0) {
            SetColor(SynSPATMessage::YELLOW, {0, 1});
        } else if (currState == 1) {
            SetColor(SynSPATMessage::YELLOW, {2, 3, 4, 5});
        } else if (currState == 2) {
            SetColor(SynSPATMessage::YELLOW, {6, 7});
        }
    } else if (m_rank == 2) {
        if (currState == 0) {
            SetColor(SynSPATMessage::YELLOW, {0, 1, 2, 3});
        } else if (currState == 1) {
            SetColor(SynSPATMessage::YELLOW, {4, 5, 6});
        }
    } else if (m_rank == 3) {
        if (currState == 0) {
            SetColor(SynSPATMessage::YELLOW, {0});
        } else if (currState == 1) {
            SetColor(SynSPATMessage::YELLOW, {1});
        } else if (currState == 2) {
            SetColor(SynSPATMessage::YELLOW, {2});
        } else if (currState == 3) {
            SetColor(SynSPATMessage::YELLOW, {3});
        }
    }
}

void SynTrafficLightAgent::SetAllRed() {
    auto lanes = std::static_pointer_cast<SynTrafficLightMessage::State>(m_msg->GetState())->spat.lanes;
    for (auto lane : lanes) {
        lane.color = SynSPATMessage::RED;
    }
}

void SynTrafficLightAgent::UpdateStates() {
    if (m_rank == 1) {
        if (currState == 0) {
            currState = 1;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::RED, {0, 1});
            SetColor(SynSPATMessage::GREEN, {2, 3, 4, 5});
        } else if (currState == 1) {
            currState = 2;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::RED, {2, 3, 4, 5});
            SetColor(SynSPATMessage::GREEN, {6, 7});
        } else if (currState == 2) {
            currState = 0;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::GREEN, {0, 1});
            SetColor(SynSPATMessage::RED, {6, 7});
        }
    } else if (m_rank == 2) {
        if (currState == 0) {
            currState = 1;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::GREEN, {4, 5, 6});
            SetColor(SynSPATMessage::RED, {0, 1, 2, 3});
        } else if (currState == 1) {
            currState = 0;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::GREEN, {0, 1, 2, 3});
            SetColor(SynSPATMessage::RED, {4, 5, 6});
        }
    } else if (m_rank == 3) {
        if (currState == 0) {
            currState = 1;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::GREEN, {1});
            SetColor(SynSPATMessage::RED, {0});
        } else if (currState == 1) {
            currState = 2;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::GREEN, {2});
            SetColor(SynSPATMessage::RED, {1});
        } else if (currState == 2) {
            currState = 3;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::GREEN, {3});
            SetColor(SynSPATMessage::RED, {2});
        } else if (currState == 3) {
            currState = 0;
            time_from_last_state = 0;
            SetColor(SynSPATMessage::GREEN, {0});
            SetColor(SynSPATMessage::RED, {3});
        }
    }
}

void SynTrafficLightAgent::Advance(double time_of_next_sync) {
    time_from_last_state += (time_of_next_sync - currentTime);
    currentTime = time_of_next_sync;

    if (time_from_last_state <= RGTIME) {
    } else if (time_from_last_state <= RGTIME + YLTIME) {
        SynTrafficLightAgent::ChangeToYellow();
    } else if (time_from_last_state <= RGTIME + YLTIME + AFTERYLTIME) {
        SynTrafficLightAgent::SetAllRed();
    } else if (time_from_last_state > RGTIME + YLTIME + AFTERYLTIME) {
        SynTrafficLightAgent::UpdateStates();
    }

    m_msg->GetState()->time = currentTime;

    SetAllRed();
}

void SynTrafficLightAgent::SetColor(SynSPATMessage::Color color, const std::list<int>& lanes) {
    auto& spat = std::static_pointer_cast<SynTrafficLightMessage::State>(m_msg->GetState())->spat;
    for (int i : lanes) {
        spat.lanes[i].color = color;
        if (m_box) {
        }
    }
}

void SynTrafficLightAgent::InitializeZombie(ChSystem* system) {
    // if (m_system && system) {
    //     std::cerr << "SynTrafficLightAgent::InitializeZombie: Agent has a ChSystem and a ChSystem was passed. Using
    //     agent's "
    //                  "ChSystem."
    //               << std::endl;
    // } else if (!m_system && system) {
    //     m_system = system;
    // } else if (!m_system && !system) {
    //     std::cerr << "SynTrafficLightAgent::InitializeZombie: Agent does not have a ChSystem and no ChSystem was
    //     passed. A "
    //                  "segfault would occur. Exiting..."
    //               << std::endl;
    //     exit(1);
    // }
    int step_number = 0;
    int render_frame = 0;
    currentTime = 0;
    currState = 0;
    time_from_last_state = 0;

    // Super messy and a lot needs to change about the pipeline of traffic_light creation
    double l = 20;
    double h = 1.0;
    double w = 2.5;
    double b = 2.5;
    double x = 1.5;
    double y = -18;
    AddLane(ChVector<>(x, y, h), ChVector<>(x, y - l, h), w);
    AddLane(ChVector<>(x + w + b, y, h), ChVector<>(x + w + b, y - l, h), w);

    x = 20;
    y = -6;
    b = .75;
    AddLane(ChVector<>(x, y, h), ChVector<>(x + l, y, h), w);
    AddLane(ChVector<>(x, y + w + b, h), ChVector<>(x + l, y + w + b, h), w);
    AddLane(ChVector<>(x, y + (w + b) * 2, h), ChVector<>(x + l, y + (w + b) * 2, h), w);
    b = 1.75;
    AddLane(ChVector<>(x, y + (w + b) * 3, h), ChVector<>(x + l, y + (w + b) * 3, h), w);

    x = -3.5;
    y = 40;
    b = 2.75;
    AddLane(ChVector<>(x, y - l, h), ChVector<>(x, y + 0.5 * l, h), w);
    AddLane(ChVector<>(x - (w + b), y, h), ChVector<>(x - (w + b), y - l, h), w);

    if (m_rank == 1) {
        SetColor(SynSPATMessage::GREEN, {0, 1});
    } else if (m_rank == 2) {
        SetColor(SynSPATMessage::GREEN, {0, 1, 2, 3});
    } else if (m_rank == 3) {
        SetColor(SynSPATMessage::GREEN, {0});
    }

    auto state = std::static_pointer_cast<SynTrafficLightMessage::State>(m_msg->GetState());
    bool visualize = false;
    if (visualize) {
        double t = .5;
        int i = 0;

        for (auto lane : state->map.lanes) {
            ChVector<> sp = lane.stopPos;
            ChVector<> op = lane.offsetPos;
            double w = lane.width;

            // orientation of the box
            if (i <= 1 || i >= 6) {
                m_box = chrono_types::make_shared<ChBodyEasyBox>(w, (sp - op).Length(), t, 2700, false, true);
                m_box->SetPos((op + sp) / 2);
                m_box->SetBodyFixed(true);
                m_system->Add(m_box);
            } else {
                m_box = chrono_types::make_shared<ChBodyEasyBox>((sp - op).Length(), w, t, 2700, false, true);
                m_box->SetPos((op + sp) / 2);
                m_box->SetBodyFixed(true);
                m_system->Add(m_box);

                std::cout << (sp - op).Length() << std::endl;
                std::cout << w << std::endl;
                std::cout << t << std::endl;
                std::cout << sp.x() << std::endl;
                std::cout << sp.y() << std::endl;
                std::cout << sp.z() << std::endl;
            }

            // auto texture = chrono_types::make_shared<ChTexture>();
            // texture->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));  // texture in ../data
            // m_box->AddAsset(texture);

            i++;
        }
    }

    // attach a 'box' shape populated with segments:
    // auto box = chrono_types::make_shared<ChPathShape>();
    // ChLineSegment seg1(ChVector<>(1, 2, 0), ChVector<>(1, 3, 0));
    // box->GetPathGeometry()->AddSubLine(mseg1);
    // ChLineSegment seg2(ChVector<>(1, 3, 0), ChVector<>(2, 3, 0));
    // box->GetPathGeometry()->AddSubLine(mseg2);
    // box->GetPathGeometry()->AddSubLine(marc1);
    // road->AddAsset(box);
}

void SynTrafficLightAgent::SynchronizeZombie(SynMessage* message) {}

std::shared_ptr<SynMessage::State> SynTrafficLightAgent::GetState() {
    return m_msg->GetState();
}

void SynTrafficLightAgent::GenerateMessagesToSend(std::vector<SynMessage*>& messages) {
    messages.push_back(new SynTrafficLightMessage(m_rank, m_msg->GetState()));
}

}  // namespace agent
}  // namespace synchrono
