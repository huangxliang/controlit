/*
 * Copyright (C) 2015 The University of Texas at Austin and the
 * Institute of Human Machine Cognition. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 2.1 of
 * the License, or (at your option) any later version. See
 * <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 */

#ifndef __CONTROLIT_CORE_TASK_HPP__
#define __CONTROLIT_CORE_TASK_HPP__

#include <mutex>
#include <string>
#include <yaml-cpp/yaml.h>

#include <controlit/PlanElement.hpp>
#include <controlit/TaskCommand.hpp>
#include <controlit/TaskState.hpp>
#include <controlit/ControlModel.hpp>

#include <controlit/addons/eigen/LinearAlgebra.hpp>

namespace controlit {

class Task : public PlanElement
{
protected:
    /*!
     * The default constructor.
     */
    explicit Task();
  
    /*!
     * A constructor that takes as input an instance name, command type,
     * active state, and inactive state.
     *
     * \param[in] instanceName The instance name of this task, e.g., "MyCartPosTask"
     * \param[in] commandType The type of command generated by this task,
     * e.g., force, acceleration, etc.
     * \param[in] activeState The active state that's used by the MainServo thread.
     * \param[in] inactiveState The inactive state that's updated by the TaskUpdater thread.
     */
    explicit Task(std::string const& instanceName, CommandType commandType,
        TaskState * activeState, TaskState * inactiveState);
  
public:

    /*!
     * The destructor.
     */
    ~Task();

    /*!
     * Initializes this task. This method is called once at startup.
     *
     * Subclasses may override this method but should call it because it 
     * initializes the robot's inactive and active states.
     *
     * \param[in] model The robot's current active control model.
     * Note that this reference should <b>not</b> be stored as a member
     * variable because it may not always be the active one in the future.
     * Note that this cannot be a const reference because subclasses may want
     * to call methods belonging to the RBDL model within the ControlModel,
     * which currently do not support const parameters.
     */
    virtual bool init(ControlModel & model);
  
    /*!
     * Re-initializes this task.  This method gets called when the task is re-enabled
     * after being disabled.  Thus subclasses should expect that this method be
     * called any number of times after startup.
     *
     * \param[in] model The robot's current active control model.
     * Note that this reference should <b>not</b> be stored as a member
     * variable because it may not be the active one.
     */
    virtual bool reinit(ControlModel & model);
  
    /*!
     * Loads a task configuration from a YAML specification.
     *
     * \param node The YAML node containing the specification.
     */
    bool loadConfig(YAML::Node const& node);
  
    /*!
     * Saves the task configuration into a YAML specification.
     *
     * \param node The node in which to save the specification.
     */
    bool saveConfig(YAML::Emitter& node) const;
  
    /*!
     * Whether this task is initialized.
     *
     * \return true if init() was called on this task.
     */
    bool isInitialized() { return initialized; }
  
    /*!
     * Updates the state of this task, which includes its Jacobian matrix.
     * This method should be called by a TaskUpdater thread that runs in
     * parallel with the MainServo thread.
     *
     * \param[in] model The current active control model.
     * Note that this pointer should <b>not</b> be stored as a member
     * variable because it may not be the active one.
     */
    bool updateState(ControlModel * model);

    /*!
     * Computes the control points. This is used for model-based sensing.
     * It can be overriden by child classes.
     */
    virtual bool sense(ControlModel & model) { return true; }
  
    /*!
     * Checks whether this task has updated state and if so switches to it.
     * This method is called by the servo thread.
     *
     * \return whether this Task's state was updated.
     */
    bool checkUpdatedState();
  
    /*!
     * Gets the task's Jacobian matrix.
     * This matrix has the following dimensions:
     *   - # rows = # task space dimensions
     *   - # cols = # DOFs (real + virtual)
     */
    bool getJacobian(Matrix & Jt);
  
    /*!
     * Obtains the task's command.
     *
     * \param[in] model The control model, which contains the robot's model.
     * \param[out] command The task's desired command.
     */
    virtual bool getCommand(ControlModel & model, TaskCommand & command) = 0;
  
    /*!
     * Gets the command type to determine if this is a force or task command.
     */
    virtual CommandType getCommandType() const {return commandType_;}
  
    /*!
     * Prints the task.
     *
     * \param os The output stream to which to print.
     * \param title Text to print at the very top.
     * \param prefix Text to print at the beginning of each section
     */
    virtual void dump(std::ostream& os,
        std::string const& title,
        std::string const& prefix) const;

protected:

    /*!
     * The implementation of the updateState method.  This is defined by
     * subclasses.
     *
     * \param[in] model The current active control model.
     * Note that this pointer should <b>not</b> be stored as a member
     * variable because it may not be the active one in the future.
     * \param[in] taskState The TaskState that should be updated.
     */
    virtual bool updateStateImpl(ControlModel * model, TaskState * taskState) = 0;
  
    /*!
     * The command type.
     */
    CommandType commandType_;

    /*!
     * Whether to perform a tare operation.
     * A value of 1 means this task should do a tare.
     */
    int tare;

private:

    /*!
     * Adds the task-specific parameters to the Parameter Reflection infrastructure.
     */
    void setupParameters();

    /*!
     * Define the possible states of the TaskUpdate thread.
     */
    enum class StateUpdateStatus : int {IDLE, UPDATING_STATE, UPDATED_STATE_READY};
  
    /*!
     * Returns a string representation of a particular StateUpdateStatus.
     *
     * \param[in] state The state update status.
     * \return A string representation of the state update status.
     */
    std::string stateUpdateStatusToString(StateUpdateStatus state);
  
    /*!
     * This is the state of the TaskUpdate thread.
     */
    StateUpdateStatus stateUpdateStatus;
  
    /*!
     * Whether this task is initialized.  This is set to be true
     * when the init() method is called.
     */
    bool initialized;
  
    /*!
     * The task's inactive state, which is written to by the TaskUpdater thread.
     */
    TaskState * inactiveState;
  
    /*!
     * The task's active state, which is read by the MainServo thread.
     */
    TaskState * activeState;
};

} // namespace controlit

#endif // __CONTROLIT_CORE_TASK_HPP__

