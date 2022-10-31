#pragma once

#include "base_actor.h"
#include "network.h"
#include "paralleler.h"
#include <deque>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace minizero::actor {

class ThreadSharedData : public utils::BaseSharedData {
public:
    int getAvailableActorIndex();

    bool do_cpu_job_;
    int actor_index_;
    std::mutex mutex_;
    std::vector<std::shared_ptr<BaseActor>> actors_;
    std::vector<std::shared_ptr<network::Network>> networks_;
    std::vector<std::vector<std::shared_ptr<network::NetworkOutput>>> network_outputs_;
};

class SlaveThread : public utils::BaseSlaveThread {
public:
    SlaveThread(int id, std::shared_ptr<utils::BaseSharedData> shared_data)
        : BaseSlaveThread(id, shared_data) {}

    virtual void initialize() override;
    virtual void runJob() override;
    virtual bool isDone() override { return false; }

protected:
    virtual bool doCPUJob();
    virtual void doGPUJob();
    inline std::shared_ptr<ThreadSharedData> getSharedData() { return std::static_pointer_cast<ThreadSharedData>(shared_data_); }
};

class ActorGroup : public utils::BaseParalleler {
public:
    ActorGroup() {}

    void run();
    virtual void initialize() override;
    virtual void summarize() override {}

protected:
    virtual void createNeuralNetworks();
    virtual void createActors();
    virtual void handleIO();
    virtual void handleFinishedGame();
    virtual void handleCommand();
    virtual void handleCommand(const std::string& command_prefix, const std::string& command);

    virtual void createSharedData() override { shared_data_ = std::make_shared<ThreadSharedData>(); }
    virtual std::shared_ptr<utils::BaseSlaveThread> newSlaveThread(int id) override { return std::make_shared<SlaveThread>(id, shared_data_); }
    inline std::shared_ptr<ThreadSharedData> getSharedData() { return std::static_pointer_cast<ThreadSharedData>(shared_data_); }

    bool running_;
    std::deque<std::string> commands_;
    std::unordered_set<std::string> ignored_commands_;
};

} // namespace minizero::actor
