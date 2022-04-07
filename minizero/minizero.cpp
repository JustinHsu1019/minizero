#include "actor_group.h"
#include "configuration.h"
#include "console.h"
#include "environment.h"
#include "zero_server.h"
#include <chrono>
#include <sys/time.h>
#include <torch/script.h>
#include <vector>
using namespace std;
using namespace minizero;

void usage()
{
    cout << "./minizero [arguments]" << endl;
    cout << "arguments:" << endl;
    cout << "\t-mode [gtp|sp]" << endl;
    cout << "\t-gen configuration_file" << endl;
    cout << "\t-conf_file configuration_file" << endl;
    cout << "\t-conf_str configuration_string" << endl;
}

void genConfiguration(config::ConfigureLoader& cl, string sConfigFile)
{
    // check configure file is exist
    ifstream f(sConfigFile);
    if (f.good()) {
        char ans = ' ';
        while (ans != 'y' && ans != 'n') {
            cerr << sConfigFile << " already exist, do you want to overwrite it? [y/n]" << endl;
            cin >> ans;
        }
        if (ans == 'y') { cerr << "overwrite " << sConfigFile << endl; }
        if (ans == 'n') {
            cerr << "didn't overwrite " << sConfigFile << endl;
            f.close();
            return;
        }
    }
    f.close();

    ofstream fout(sConfigFile);
    fout << cl.toString();
    fout.close();
}

bool readConfiguration(config::ConfigureLoader& cl, string sConfigFile, string sConfigString)
{
    if (!sConfigFile.empty() && !cl.loadFromFile(sConfigFile)) {
        cerr << "Failed to load configuration file." << endl;
        return false;
    }
    if (!sConfigString.empty() && !cl.loadFromString(sConfigString)) {
        cerr << "Failed to load configuration string." << endl;
        return false;
    }

    cerr << cl.toString();
    return true;
}

void runConsole()
{
    console::Console console;
    string command;
    while (getline(cin, command)) {
        if (command == "quit") { break; }
        console.executeCommand(command);
    }
}

void runSelfPlay()
{
    actor::ActorGroup ag;
    ag.run();
}

void runZeroServer()
{
    server::ZeroServer server;
    server.run();
}

void runTest()
{
    Environment env;
    env.reset();
   // srand(time(0));
    while (!env.isTerminal()) {
        vector<Action> legal_actions = env.getLegalActions();
        int index = rand() % legal_actions.size();
        env.act(legal_actions[index]);
    }
    cout << env.toString() << endl;
    
    EnvironmentLoader env_loader;
    env_loader.loadFromEnvironment(env);
    cout << env_loader.toString() << endl;
}

int main(int argc, char* argv[])
{
    if (argc % 2 != 1) {
        usage();
        return -1;
    }

    env::setUpEnv();

    string sMode = "console";
    string sConfigFile = "";
    string sConfigString = "";
    config::ConfigureLoader cl;
    config::setConfiguration(cl);

    for (int i = 1; i < argc; i += 2) {
        string sCommand = string(argv[i]);

        if (sCommand == "-mode") {
            sMode = argv[i + 1];
        } else if (sCommand == "-gen") {
            genConfiguration(cl, argv[i + 1]);
            return 0;
        } else if (sCommand == "-conf_file") {
            sConfigFile = argv[i + 1];
        } else if (sCommand == "-conf_str") {
            sConfigString = argv[i + 1];
        } else {
            cerr << "unknown argument: " << sCommand << endl;
            usage();
            return -1;
        }
    }

    if (!readConfiguration(cl, sConfigFile, sConfigString)) { return -1; }

    if (sMode == "console") {
        runConsole();
    } else if (sMode == "sp") {
        runSelfPlay();
    } else if (sMode == "zero_server") {
        runZeroServer();
    } else if (sMode == "test") {
        runTest();
    } else {
        cerr << "Error mode: " << sMode << endl;
        return -1;
    }

    return 0;
}
