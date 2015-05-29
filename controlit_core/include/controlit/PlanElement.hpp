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

#ifndef __CONTROLIT_PLAN_ELEMENT_HPP__
#define __CONTROLIT_PLAN_ELEMENT_HPP__

#include <string>
#include <ros/ros.h>

#include <controlit/ParameterReflection.hpp>

namespace controlit {

/*!
 * This is the super-class of all plan elements, which include controlit::Task
 * and controlit::Constraint.  It defines the "isEnabled" parameter that is used
 * by the controller to determine which tasks and constraints to use.
 */
class PlanElement : public ParameterReflection
{
protected:

    /*!
     * A constructor.
     *
     * \param typeName The type name of the plan element (e.g., "task", "constraint", etc.)
     * \param instanceName The instance name of the plan element (e.g., "JPosTask", "CartPosTask", etc.)
     */
    explicit PlanElement(std::string const& typeName, std::string const& instanceName);

public:
    /*!
     * Returns whether this plan element is enabled.
     *
     * \return true if this plan element is enabled.
     */
    bool isEnabled() const;
  
    /*!
     * Generates a string representation of this class.
     *
     * \param os The output stream to which to print the string.
     * \param prefix The string to add to the beginning of each line.
     */
    void dump(std::ostream& os, std::string const& prefix) const;
  
private:

    /*!
     * Whether this plan element is enabled. Zero means false; non-zero means true.
     */
    int isEnabled_;
};

} // namespace controlit

#endif // __CONTROLIT_PLAN_ELEMENT_HPP__
