

#include "RenderStageManager.hpp"

RenderStageManager::RenderStageManager()
{

}

RenderStageManager::~RenderStageManager()
{

}

void RenderStageManager::Render()
{
    for (auto i = stages.begin(); i != stages.end(); ++i)
    {
        (*i)->renderStage->Execute();
    }
}

void RenderStageManager::AddRenderStage(const int priority_, const std::string &renderStageName_, std::shared_ptr<RenderStage> stage_)
{
    std::shared_ptr<Wrapper> newStage = std::make_shared<Wrapper>();
    newStage->name = renderStageName_;
    newStage->priority = priority_;
    newStage->renderStage = stage_;
    newStage->enabled = true;

    for (auto i = stages.begin(); i != stages.end(); ++i)
    {
        if ((*i)->priority > priority_)
        {
            stages.insert(i, newStage);
            break;
        }
    }
}

void RenderStageManager::RenderStageActive(const std::string &name_, const bool active_)
{
    for (auto i = stages.begin(); i != stages.end(); ++i)
    {
        if ((*i)->name.compare(name_))
        {
            (*i)->enabled = active_;
            return;
        }
    }
}

const std::vector<std::shared_ptr<RenderStage>> RenderStageManager::GetStages() const
{
    std::vector<std::shared_ptr<RenderStage>> renderStages(stages.size());

    for (unsigned int i = 0; i < stages.size(); ++i)
    {
        renderStages[i] = stages[i]->renderStage;
    }

    return renderStages;
}