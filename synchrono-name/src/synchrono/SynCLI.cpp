#include "synchrono/SynCLI.h"

namespace synchrono {

SynCLI::SynCLI(std::string program, std::string help_string) : m_options(program, help_string), m_verbose(false) {}

void SynCLI::AddDefaultDemoOptions() {
    // General simulation options
    m_options.add_option(
        "Sim", cxxopts::Option("verbose", "Simulation verbosity", cxxopts::value<bool>()->default_value("false")));
    m_options.add_option(
        "Sim", cxxopts::Option("step_size", "Time step size", cxxopts::value<double>()->default_value("1e-3")));
    m_options.add_option("Sim", cxxopts::Option("heartbeat", "Heartbeat time step size",
                                                cxxopts::value<double>()->default_value("1e-2")));
    m_options.add_option("Sim", cxxopts::Option("time_end", "End time", cxxopts::value<double>()->default_value("10")));
    // Irrlicht options
    m_options.add_option("Irrlicht", cxxopts::Option("i,irr", "Ranks for irrlicht usage",
                                                     cxxopts::value<std::vector<int>>()->default_value("-1")));
    m_options.add_option("Irrlicht", cxxopts::Option("irr_save", "Toggle irrlicht saving on",
                                                     cxxopts::value<bool>()->default_value("false")));
    m_options.add_option("Irrlicht", cxxopts::Option("irr_vis", "Toggle irrlicht visualization on",
                                                     cxxopts::value<bool>()->default_value("false")));
    // Sensor options
    m_options.add_option("Sensor", cxxopts::Option("s,sens", "Ranks for sensor usage",
                                                   cxxopts::value<std::vector<int>>()->default_value("-1")));
    m_options.add_option("Sensor", cxxopts::Option("sens_save", "Toggle sensor saving on",
                                                   cxxopts::value<bool>()->default_value("false")));
    m_options.add_option("Sensor", cxxopts::Option("sens_vis", "Toggle for sensor visualizing on",
                                                   cxxopts::value<bool>()->default_value("false")));

    m_options.add_option("", cxxopts::Option("h,help", "Print usage"));
}

void SynCLI::Parse(int argc, char* argv[]) {
    m_result = std::make_shared<cxxopts::ParseResult>(m_options.parse(argc, argv));

    if (m_result->count("help")) {
        std::cout << m_options.help() << std::endl;
        exit(0);
    }
}

void SynCLI::AddOption(std::string group, cxxopts::Option option) {
    m_options.add_option(group, option);
}

const cxxopts::OptionValue& SynCLI::Get(std::string option) {
    return (*m_result)[option];
}

}  // namespace synchrono
