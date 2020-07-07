#ifndef SYN_CLI_H
#define SYN_CLI_H

#include "synchrono/SynApi.h"

#include "synchrono/cli/cxxopts.hpp"

namespace synchrono {

class SYN_API SynCLI {
  public:
    /// Constructor
    SynCLI(std::string program, std::string help_string = " - command line options");

    /// Destructor
    ~SynCLI() {}

    /// Add default options for synchrono demos
    void AddDefaultDemoOptions();

    /// Parse messages and display help message, if needed
    void Parse(int argc, char* argv[]);

    /// Check for value in vector
    template <typename T>
    bool HasValueInVector(std::string option, T value) {
        const auto vector = Get(option).as<std::vector<T>>();
        return std::find(vector.begin(), vector.end(), value) != vector.end();
    }

    /// Add an option
    void AddOption(std::string group, cxxopts::Option option);

    /// Get option
    const cxxopts::OptionValue& Get(std::string option);

    /// Get option as type
    /// Recommanded way of accessing
    template <typename T>
    const T GetAsType(std::string option) {
        try {
            return (*m_result)[option].as<T>();
        } catch (std::domain_error e) {
            std::cout << "SynCLI::GetAsType: Could not cast \"" << option << "\" as " << typeid(T).name() << std::endl;
            std::cout << "SynCLI::GetAsType: Exitting..." << std::endl;
            exit(-1);
        }
    }

    /// Set the verbosity of the cli
    void SetVerbosity(bool verbosity) { m_verbose = verbosity; }

  private:
    bool m_verbose;  ///< verbosity of the cli

    cxxopts::Options m_options;                      ///< Command line options
    std::shared_ptr<cxxopts::ParseResult> m_result;  ///< Parsing results
};

}  // namespace synchrono

#endif
