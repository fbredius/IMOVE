//
// Created by Wouter Posdijk on 12/05/16.
//

#ifndef IMOVE_UPDATELIGHTTRAILSACTION_H
#define IMOVE_UPDATELIGHTTRAILSACTION_H


#include "../../Action.h"
#include "../LightTrail.h"
#include "../GravityPoint.h"
#include "../Repositories/LightsSceneRepositories.h"
#include "../LightTrailConfiguration.h"


class UpdateLightTrailsAction : public Action {
private:
    LightTrailRepository* lightTrails;
    GravityPointRepository* gravityPoints;
    Vector2 calculateForce(LightTrail trail);
    const LightTrailConfiguration& config;
public:
    UpdateLightTrailsAction(LightTrailRepository* lightTrails, GravityPointRepository* gravityPoints,
    const LightTrailConfiguration& config);

    bool isDone(Action *&followUp) override;
    void execute(float dt) override;
};


#endif //IMOVE_UPDATELIGHTTRAILSACTION_H
