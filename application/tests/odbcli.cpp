#include <iostream>
#include <string>
#include <asio/ts/internet.hpp>

extern "C" {
#include <linenoise.h>
}

using asio::ip::tcp;

int main(int argc, char* argv[])
    {
    try
        {
        if (argc != 3)
            {
            std::cout << "Usage: " << argv[0] << " <server> <path>\n";
            std::cout << "Example:\n";
            std::cout << "  " << argv[0] << " localhost 1025\n";
            return 1;
            }
        std::string sHistoryFilename{".odbcli_history"};
        linenoiseHistoryLoad(sHistoryFilename.c_str());
        for (;;)
            {
            asio::ip::tcp::iostream oStream;
//          oStream.expires_after(std::chrono::seconds(60));
            oStream.connect(argv[1], argv[2]);
            if (!oStream)
                {
                std::cout << "Unable to connect: " << oStream.error().message() << "\n";
                return 1;
                }

//            std::cout << "> ";
//            std::string sInput;
//            std::getline(std::cin, sInput);
//            oStream << sInput + "\n";

            char* line = linenoise("> ");
            if ( !line || line[0] == 'q' ) { free(line); return 0; }
            linenoiseHistoryAdd(line);
            linenoiseHistorySave(sHistoryFilename.c_str());
            oStream << std::string(line) + "\n";
            free(line);

            std::string sOutput;
            do
                {
                std::getline(oStream, sOutput);
                if ( sOutput == " " )
                    {
                    std::cout << "\n";
                    }
                else
                    {
                    std::cout << sOutput << "\n";
                    }
                } while ( (sOutput != ".") && (sOutput != "") );

            // Write the remaining data to output.
//          std::cout << oStream.rdbuf();
            }
        }
    catch (std::exception& e)
        {
        std::cout << "Exception: " << e.what() << "\n";
        }

    return 0;
    }


