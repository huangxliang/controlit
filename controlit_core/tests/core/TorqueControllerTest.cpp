#include <gtest/gtest.h>

#include <string>
#include <fstream>

#include <controlit/Command.hpp>
#include <controlit/TorqueController.hpp>
#include <controlit/CompoundTask.hpp>
#include <controlit/CompoundTaskFactory.hpp>
#include <controlit/ControlModel.hpp>
#include <controlit/addons/eigen/LinearAlgebra.hpp>
#include <controlit/RobotState.hpp>
#include <controlit/TaskUpdater.hpp>
#include <controlit/utility/ControlItParameters.hpp>
#include <controlit/utility/StatsUtility.hpp>

#include <ros/package.h> // for ros::package::getPath(...)

using controlit::TorqueController;
using controlit::CompoundTask;
using controlit::ControlModel;
using controlit::RobotState;
using controlit::TaskUpdater;
using Vector;

class TorqueControllerTest : public ::testing::Test
{
protected:

    std::string readFileInPackage(std::string packageName, std::string filePath)
    {
        std::string controlitConfigFilesDir = ros::package::getPath(packageName);

        if (controlitConfigFilesDir.compare("") == 0)
        {
            CONTROLIT_ERROR << "Could not find package \"" << packageName << "\"!";
            return "";
        }

        std::stringstream parameters;

        std::string line;
        std::ifstream myfile (controlitConfigFilesDir + filePath);
        if (myfile.is_open())
        {
            while ( myfile.good() )
            {
                std::getline (myfile,line);
                parameters << line << std::endl;
            }
            myfile.close();
        }
        else
        {
            CONTROLIT_ERROR << "Unable to open file " << controlitConfigFilesDir << filePath;
            return "";
        }

        std::string result = parameters.str();

        // CONTROLIT_DEBUG_RT << "Read in the following:\n" << result;

        return result;
    }

    std::string getParameters()
    {
        return readFileInPackage("controlit", "/tests/core/testData/TorqueControllerTest_Parameters_CartPos.yaml");
    }

    std::string getRobotDescription()
    {
        return readFileInPackage("controlit", "/tests/core/testData/TorqueControllerTest_V1_pinned_controlit.urdf");
    }

    virtual void SetUp()
    {
        // ros::NodeHandle nh;

        // Get the parameters
        std::string parameters = getParameters();

        // Get the robot description
        std::string robotDescription = getRobotDescription();

        // Create the TorqueController
        controller.reset(new TorqueController("MyTestController"));

        robotState.reset(new RobotState());

        // Create the ControlModel
        model.reset(ControlModel::createModel(robotDescription, parameters, robotState.get(),
            &controlitParameters));

        EXPECT_TRUE(model.get() != nullptr) << "Control model is NULL!";

        // Update the joint state within the ControlModel
        robotState->init(model->getNActuableDOFs());

        robotState->setRobotBaseState(Vector3d::Zero(),
            drc::common::Quaternion::Identity(), Vector::Zero(6));

        // Update the model
        model->updateJointState();
        model->update();

        // Create and initialize the CompoundTask
        controlit::CompoundTaskFactory compoundTaskFactory;
        compoundTask.reset(compoundTaskFactory.loadFromString(parameters));

        EXPECT_TRUE(compoundTask.get() != nullptr) << "CompoundTask is NULL!";

        EXPECT_TRUE(compoundTask->init(*(model.get()))) << "Failed to initialize ControlModel";

        // Create the TaskUpdater
        taskUpdater.reset(new TaskUpdater());

        EXPECT_TRUE(compoundTask->addTasksToUpdater(taskUpdater.get()))
            << "Failed to add tasks to TaskUpdater";

        taskUpdater->startThread();

        ros::NodeHandle nh;

        // Initialize the controller
        controller->init(nh, *(model.get()), &controlitParameters);

        // Initialize the command
        command.init(model->getNActuableDOFs());
    }

    virtual void TearDown()
    {
    }

    controlit::utility::ControlItParameters controlitParameters;
    std::shared_ptr<TorqueController> controller;
    std::shared_ptr<RobotState> robotState;
    std::shared_ptr<ControlModel> model;
    std::shared_ptr<CompoundTask> compoundTask;
    std::shared_ptr<TaskUpdater> taskUpdater;

    /*!
     * For storing the command generated by TorqueController.
     */
    controlit::Command command;
};

TEST_F(TorqueControllerTest, BenchmarkTest)
{
    int NUM_ROUNDS = 1000;

    std::vector<double> resultCache(NUM_ROUNDS, 0);

    taskUpdater->updateTasks(model.get());

    for (int ii = 0; ii < NUM_ROUNDS; ii++)
    {

        taskUpdater->checkTasksForUpdates();

        ros::Time startTime = ros::Time::now();

        controller->computeCommand(startTime, *model, *compoundTask, command);

        double elapsedPeriod = (ros::Time::now() - startTime).toSec();

        resultCache[ii] = elapsedPeriod;

        //std::cout << "Round " << ii << ", period = " << elapsedPeriod << std::endl;
    }

    double avg, stdev;

    controlit::utility::computeAvgAndStdDev(resultCache, avg, stdev);

    CONTROLIT_INFO << "Latency of calling controlit::TorqueController::computeCommand: "
             << (avg * 1e6) << " ± " << (stdev * 1e6) << "μs";
}
