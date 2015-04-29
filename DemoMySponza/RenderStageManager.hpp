#pragma once

#include <memory>
#include <vector>

#include "RenderStage.hpp"

class RenderStageManager
{

    struct Wrapper
    {
        bool enabled;
        int priority;
        std::string name;
        std::shared_ptr<RenderStage> renderStage;
    };

public:

    RenderStageManager();
    ~RenderStageManager();

    void Render();

    void AddRenderStage(const int priority_, const std::string &renderStageName_, std::shared_ptr<RenderStage> stage_);
    void RenderStageActive(const std::string &name_, const bool active_);

    const std::vector<std::shared_ptr<RenderStage>> GetStages() const;

private:

    std::vector<std::shared_ptr<Wrapper>> stages;

};