#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Exi::Unit
{

    using TestFunction = bool(*)();

    /**
     * Class representing a set of unit tests
     */
    class Tests
    {
    public:
        using TestMap = std::unordered_map<std::string, TestFunction>;
        explicit Tests(const TestMap& testMap)
        {
            for (const auto& pair : testMap)
                m_TestMap.emplace(pair);
        }

        /**
         * Execute a unit test by name
         * @param name
         * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
         */
        int Execute(const std::string& name) const
        {
            if (m_TestMap.contains(name))
            {
                TestFunction fn = m_TestMap.at(name);
                return fn ? !fn() : EXIT_FAILURE;
            }

            printf("ERROR: Unknown test name '%s'\n", name.c_str());
            return EXIT_FAILURE;
        }

        /**
         * Execute a unit test from command line arguments
         * @param argc
         * @param argv
         * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
         */
        int Execute(int argc, const char** argv) const
        {
            std::vector<std::string> args(&argv[0], &argv[argc]);
            if (argc < 2)
            {
                puts("ERROR: Test name not specified, available tests:");
                for (const auto& pair : m_TestMap)
                    printf("    %s\n", pair.first.c_str());
                return EXIT_FAILURE;
            }

            return Execute(argv[1]);
        }
    private:
        TestMap m_TestMap;
    };

}
